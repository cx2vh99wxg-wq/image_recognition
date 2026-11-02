#ifndef PERCEPTION_GLES_PRESENTER_H
#define PERCEPTION_GLES_PRESENTER_H

#include <X11/Xlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PERCEPTION_HAVE_GLES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <algorithm>
#include <vector>

/* The caller owns the X Display/Window and calls cleanup before destroying
 * either. All methods, including cleanup, run on the same main thread. */
class GlesPresenter {
public:
    GlesPresenter() = default;
    ~GlesPresenter() { cleanup(); }
    GlesPresenter(const GlesPresenter&) = delete;
    GlesPresenter& operator=(const GlesPresenter&) = delete;

    bool init(Display* display, Window window, VisualID visual_id, int canvas_w, int canvas_h) {
        cleanup();
        error_[0] = '\0';
        if (!display || !window || canvas_w <= 0 || canvas_h <= 0)
            return fail("invalid X11 window or canvas");
        const uint32_t endian = 0x01020304;
        if (*(const unsigned char*)&endian != 4) return fail("ARGB upload requires little endian");
        try {
            if (!open_x11_display(display)) return fail("cannot initialize an X11 EGL display");
            fprintf(stderr, "[GLES] EGL vendor=%s version=%s\n",
                eglQueryString(display_, EGL_VENDOR), eglQueryString(display_, EGL_VERSION));
            if (!eglBindAPI(EGL_OPENGL_ES_API)) return egl_fail("eglBindAPI");
            EGLint count = 0;
            if (!eglGetConfigs(display_, NULL, 0, &count) || count <= 0)
                return egl_fail("no EGL configurations");
            std::vector<EGLConfig> configs((size_t)count);
            if (!eglGetConfigs(display_, configs.data(), count, &count)) return egl_fail("eglGetConfigs");
            int best_score = 0x7fffffff;
            EGLConfig chosen = NULL;
            fprintf(stderr, "[GLES] configs=%d target_visual=0x%lx native_visual_ids=", count, (unsigned long)visual_id);
            for (EGLint n = 0; n < count; ++n) {
                EGLint native = 0, surfaces = 0, renderable = 0, r = 0, g = 0, b = 0;
                eglGetConfigAttrib(display_, configs[n], EGL_NATIVE_VISUAL_ID, &native);
                if (n < 16) fprintf(stderr, "0x%x%s", native, n == count - 1 ? "" : ",");
                eglGetConfigAttrib(display_, configs[n], EGL_SURFACE_TYPE, &surfaces);
                eglGetConfigAttrib(display_, configs[n], EGL_RENDERABLE_TYPE, &renderable);
                eglGetConfigAttrib(display_, configs[n], EGL_RED_SIZE, &r);
                eglGetConfigAttrib(display_, configs[n], EGL_GREEN_SIZE, &g);
                eglGetConfigAttrib(display_, configs[n], EGL_BLUE_SIZE, &b);
                if ((VisualID)native != visual_id || !(surfaces & EGL_WINDOW_BIT) ||
                    !(renderable & EGL_OPENGL_ES2_BIT) || r < 8 || g < 8 || b < 8) continue;
                EGLint depth = 0, stencil = 0, samples = 0;
                eglGetConfigAttrib(display_, configs[n], EGL_DEPTH_SIZE, &depth);
                eglGetConfigAttrib(display_, configs[n], EGL_STENCIL_SIZE, &stencil);
                eglGetConfigAttrib(display_, configs[n], EGL_SAMPLES, &samples);
                int score = samples * 100 + depth + stencil;
                if (score < best_score) { best_score = score; chosen = configs[n]; }
            }
            fprintf(stderr, "%s\n", count > 16 ? "..." : "");
            if (!chosen) return fail("no window ES2 config matches the existing X11 visual");
            surface_ = eglCreateWindowSurface(display_, chosen, (EGLNativeWindowType)window, NULL);
            if (surface_ == EGL_NO_SURFACE) return egl_fail("eglCreateWindowSurface");
            const EGLint context_attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
            context_ = eglCreateContext(display_, chosen, EGL_NO_CONTEXT, context_attributes);
            if (context_ == EGL_NO_CONTEXT) return egl_fail("eglCreateContext");
            if (!eglMakeCurrent(display_, surface_, surface_, context_)) return egl_fail("eglMakeCurrent");
            const char* vertex_source =
                "attribute vec2 a_position; attribute vec2 a_texcoord; varying highp vec2 v_uv;"
                "void main(){gl_Position=vec4(a_position,0.0,1.0);v_uv=a_texcoord;}";
            const char* fragment_source =
                "precision highp float; varying highp vec2 v_uv; uniform sampler2D u_frame;"
                "void main(){gl_FragColor=texture2D(u_frame,v_uv).bgra;}";
            vertex_ = shader(GL_VERTEX_SHADER, vertex_source);
            if (!vertex_) return fail("vertex shader compilation failed");
            fragment_ = shader(GL_FRAGMENT_SHADER, fragment_source);
            if (!fragment_) return fail("fragment shader compilation failed");
            program_ = glCreateProgram();
            glAttachShader(program_, vertex_); glAttachShader(program_, fragment_);
            glBindAttribLocation(program_, 0, "a_position");
            glBindAttribLocation(program_, 1, "a_texcoord");
            glLinkProgram(program_);
            GLint linked = GL_FALSE; glGetProgramiv(program_, GL_LINK_STATUS, &linked);
            if (!linked) {
                char log[256] = {}; glGetProgramInfoLog(program_, sizeof(log), NULL, log);
                fprintf(stderr, "[GLES] link: %s\n", log);
                return fail("shader program link failed");
            }
            // CPU row zero is the top. UV.y=0 is therefore attached to the top vertices.
            const GLfloat vertices[] = {-1,-1,0,1, 1,-1,1,1, -1,1,0,0, 1,1,1,0};
            glGenBuffers(1, &vertices_); glBindBuffer(GL_ARRAY_BUFFER, vertices_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glGenTextures(1, &texture_); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture_);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glUseProgram(program_); glUniform1i(glGetUniformLocation(program_, "u_frame"), 0);
            glDisable(GL_BLEND); glDisable(GL_DITHER); glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST); glDisable(GL_SCISSOR_TEST);
            if (!allocate_texture(canvas_w, canvas_h)) return false;
            int interval = 1;
            const char* option = getenv("VISION_SWAP_INTERVAL");
            if (option && strcmp(option, "0") == 0) interval = 0;
            if (!eglSwapInterval(display_, interval)) return egl_fail("eglSwapInterval");
            fprintf(stderr, "[GLES] renderer=%s GL=%s canvas=%dx%d swap_interval=%d\n",
                (const char*)glGetString(GL_RENDERER), (const char*)glGetString(GL_VERSION), canvas_w, canvas_h, interval);
            if (glGetError() != GL_NO_ERROR) return fail("GL initialization failed");
            ready_ = true;
            return true;
        } catch (...) { return fail("EGL presenter allocation failed"); }
    }

    bool present(const uint32_t* argb, int canvas_w, int canvas_h, int window_w, int window_h) {
        if (!ready_ || !argb || canvas_w <= 0 || canvas_h <= 0) return false;
        if (window_w <= 0 || window_h <= 0) return true; // minimized window
        if ((canvas_w != texture_w_ || canvas_h != texture_h_) && !allocate_texture(canvas_w, canvas_h)) return false;
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, texture_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        // Native ARGB32 is B,G,R,A in little-endian memory; the shader swizzles .bgra.
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, canvas_w, canvas_h, GL_RGBA, GL_UNSIGNED_BYTE, argb);
        glViewport(0, 0, window_w, window_h);
        glClearColor(5.0f/255.0f, 7.0f/255.0f, 21.0f/255.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
        double scale = std::min((double)window_w / canvas_w, (double)window_h / canvas_h);
        int width = (int)(canvas_w * scale + 0.5), height = (int)(canvas_h * scale + 0.5);
        glViewport((window_w - width) / 2, (window_h - height) / 2, width, height);
        glUseProgram(program_); glBindBuffer(GL_ARRAY_BUFFER, vertices_);
        glEnableVertexAttribArray(0); glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (const void*)(2 * sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        if (glGetError() != GL_NO_ERROR) return fail("GL texture upload or draw failed");
        if (!eglSwapBuffers(display_, surface_)) return egl_fail("eglSwapBuffers");
        return true;
    }

    bool ready() const { return ready_; }
    const char* error() const { return error_[0] ? error_ : "GLES presenter unavailable"; }
    void cleanup() {
        ready_ = false;
        if (display_ != EGL_NO_DISPLAY && context_ != EGL_NO_CONTEXT &&
            eglMakeCurrent(display_, surface_, surface_, context_)) {
            if (texture_) glDeleteTextures(1, &texture_);
            if (vertices_) glDeleteBuffers(1, &vertices_);
            if (program_) glDeleteProgram(program_);
            if (vertex_) glDeleteShader(vertex_);
            if (fragment_) glDeleteShader(fragment_);
        }
        texture_ = vertices_ = program_ = vertex_ = fragment_ = 0;
        texture_w_ = texture_h_ = 0;
        if (display_ != EGL_NO_DISPLAY) {
            eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context_ != EGL_NO_CONTEXT) eglDestroyContext(display_, context_);
            if (surface_ != EGL_NO_SURFACE) eglDestroySurface(display_, surface_);
            eglTerminate(display_);
        }
        context_ = EGL_NO_CONTEXT; surface_ = EGL_NO_SURFACE; display_ = EGL_NO_DISPLAY;
    }

private:
    bool open_x11_display(Display* native) {
        // A Mali installation may default to GBM; explicitly select X11 first.
        typedef EGLDisplay (*PlatformDisplay)(EGLenum, void*, const EGLint*);
        PlatformDisplay platform = (PlatformDisplay)eglGetProcAddress("eglGetPlatformDisplayEXT");
        if (platform) {
            display_ = platform(0x31D5 /* EGL_PLATFORM_X11_EXT/KHR */, native, NULL);
            if (display_ != EGL_NO_DISPLAY && eglInitialize(display_, NULL, NULL)) return true;
        }
        display_ = eglGetDisplay((EGLNativeDisplayType)native);
        return display_ != EGL_NO_DISPLAY && eglInitialize(display_, NULL, NULL);
    }
    GLuint shader(GLenum type, const char* source) {
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &source, NULL); glCompileShader(id);
        GLint compiled = GL_FALSE; glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            char log[256] = {}; glGetShaderInfoLog(id, sizeof(log), NULL, log);
            fprintf(stderr, "[GLES] shader: %s\n", log); glDeleteShader(id); return 0;
        }
        return id;
    }
    bool allocate_texture(int width, int height) {
        GLint maximum = 0; glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maximum);
        if (width <= 0 || height <= 0 || width > maximum || height > maximum) return fail("canvas exceeds GL texture limit");
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        if (glGetError() != GL_NO_ERROR) return fail("GL texture allocation failed");
        texture_w_ = width; texture_h_ = height; return true;
    }
    bool egl_fail(const char* operation) {
        snprintf(error_, sizeof(error_), "%s (EGL 0x%x)", operation, eglGetError());
        cleanup(); return false;
    }
    bool fail(const char* message) {
        snprintf(error_, sizeof(error_), "%s", message); cleanup(); return false;
    }
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    GLuint texture_ = 0, vertices_ = 0, program_ = 0, vertex_ = 0, fragment_ = 0;
    int texture_w_ = 0, texture_h_ = 0;
    bool ready_ = false;
    char error_[192] = {};
};
#else
class GlesPresenter {
public:
    bool init(Display*, Window, VisualID, int, int) { return false; }
    bool present(const uint32_t*, int, int, int, int) { return false; }
    bool ready() const { return false; }
    const char* error() const { return "GLES support was not compiled"; }
    void cleanup() {}
};
#endif
#endif
