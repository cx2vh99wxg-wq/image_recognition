#define main renderer_main
#include "../udp_rk_rk_yolo/S/x11/csrc/main.cpp"
#undef main
#include <cassert>
#include <iostream>
static void source_pixels_equal(RenderEngine& e){XImage*image=XGetImage(e.display,e.pixmap,0,0,1600,900,AllPlanes,ZPixmap);assert(image);const uint32_t*p=e.dashboard->pixels();for(int y=0;y<900;++y)for(int x=0;x<1600;++x){if((XGetPixel(image,x,y)&0xffffff)!=(p[y*1600+x]&0xffffff)){fprintf(stderr,"PIXMAP_MISMATCH x=%d y=%d\n",x,y);abort();}}XDestroyImage(image);}
static void region_unit(){
 RenderEngine e={};XImage image={};image.bytes_per_line=1600*4+16;image.bits_per_pixel=32;std::vector<unsigned char>memory(image.bytes_per_line*900,0xcc);image.data=(char*)memory.data();e.presentation_image=&image;
 std::vector<uint32_t>pixels(1600*900),server(1600*900);for(size_t i=0;i<pixels.size();++i)pixels[i]=0xff000000u|(i*0x12345u);std::vector<UploadRegion>regions;
 prepare_xrender_upload(&e,pixels.data(),true,regions);assert(regions.size()==1&&regions[0].width==1600&&regions[0].height==900);e.upload_initialized=1;
 auto apply=[&](){for(auto&r:regions){assert(r.x>=0&&r.y>=0&&r.x+r.width<=1600&&r.y+r.height<=900);for(int y=r.y;y<r.y+r.height;++y)memcpy(server.data()+y*1600+r.x,image.data+y*image.bytes_per_line+r.x*4,r.width*4);}assert(!memcmp(server.data(),pixels.data(),pixels.size()*4));for(int y=0;y<900;++y)for(int x=6400;x<image.bytes_per_line;++x)assert(memory[y*image.bytes_per_line+x]==0xcc);};apply();
 const UploadRegion areas[]={{0,180,1600,44},{72,224,1092,504},{0,728,1600,152},{1188,230,412,498}};
 int fragmented=0,empty=0;
 for(int n=0;n<180;++n){if(n%9){const auto&a=areas[n%4];if(n%7==0){for(auto&r:areas)for(int y=0;y<r.height;y+=2)pixels[(r.y+y)*1600+r.x]^=0x123456;fragmented++;}else{int x=a.x+(n*13)%a.width,y=a.y+(n*17)%a.height;pixels[y*1600+x]^=0x765432;}}
 prepare_xrender_upload(&e,pixels.data(),true,regions);assert(regions.size()<=64);if(n%9==0){assert(regions.empty());empty++;}if(n%9&&n%7==0)assert(regions.size()<=4);apply();}
 std::cout<<"REGION_UNIT_PASS cases180 fragmented="<<fragmented<<" empty="<<empty<<" padded_stride=PASS\n";
}
int main(int argc,char**argv){region_unit();if(argc>1)return 0;start_fullscreen=0;assert(!load_car_png("../car.png"));EmergencyBrakeSharedMemory brake={};brake.valid=1;emergency_brake_shm=&brake;RenderEngine e={};assert(!render_engine_init(&e));assert(e.use_xrender&&e.use_shm&&e.use_shm_completion);bool partial=partial_upload_requested();int multi=0,full=0,empty=0;
for(int n=0;n<100;++n){e.profile_start=display_clock_ns();uint32_t color=0xff000000|((n*13%240+8)<<16)|((n*29%240+8)<<8)|(n*37%240+8);std::fill_n((uint32_t*)e.display_buffer,1280*480,color);brake.person_detected=n%5!=0;brake.box_left=(n*17)%600;brake.box_top=(n*13)%440;brake.box_right=brake.box_left+20;brake.box_bottom=brake.box_top+20;current_curve_type=(CurveType)(n%4);current_curve_confidence=n%2?81:59;e.presented_mode=(DisplayMode)((n%9+1)*0x11);e.transition.is_active=n%3;e.transition.progress=(n%11)/10.f;XSetForeground(e.display,e.gc,n*31+3);XSetBackground(e.display,e.gc,n*73+9);bool warmup=partial&&!e.upload_initialized;uint64_t before_rects=e.profile_upload_rects,before_bytes=e.profile_upload_bytes;assert(!present_dashboard(&e));assert(e.use_shm_completion&&!e.shm_completion_disabled);uint64_t added=e.profile_upload_rects-before_rects,bytes=e.profile_upload_bytes-before_bytes;if(added>1)multi++;if(bytes==1600*900*4)full++;if(!added)empty++;XSync(e.display,False);source_pixels_equal(e);XEvent event;int events=0;while(XCheckTypedEvent(e.display,e.shm_completion_type,&event)){assert(consume_shm_completion(&e,&event));events++;}assert(events==(warmup?0:(added?1:0)));assert(!e.shm_pending);
if(n==30||n==60){XResizeWindow(e.display,e.window,n==30?400:600,n==30?300:340);XSync(e.display,False);usleep(50000);assert(!render_engine_check_exit_event(&e));}
if(n==50){release_presentation(&e);assert(!e.upload_initialized);assert(!resize_presentation(&e,e.window_width,e.window_height));}
}
// An unchanged dashboard must reuse the pixmap with no upload/completion.
wait_for_shared_image(&e);last_blink_time=get_current_time_ms();e.profile_start=display_clock_ns();uint64_t before=e.profile_upload_rects;assert(!present_dashboard(&e));XSync(e.display,False);source_pixels_equal(e);if(partial)assert(e.profile_upload_rects==before);render_engine_cleanup(&e);emergency_brake_shm=nullptr;stbi_image_free(car_png_data);car_png_data=nullptr;if(partial)assert(multi>0&&full==2);std::cout<<"REAL_MULTI_REGION_PASS frames100 partial="<<partial<<" multi="<<multi<<" full="<<full<<" empty="<<empty<<" dirty_GC=PASS final_event=PASS recreate=PASS\n";}
