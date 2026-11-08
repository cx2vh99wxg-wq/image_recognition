#ifndef VISION_HDMI_MODE_GUARD_H
#define VISION_HDMI_MODE_GUARD_H
#include <cerrno>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <chrono>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
namespace vision_display {
class HdmiModeGuard {
    pid_t child_=0;
    bool pending_=false;
    uint64_t last_=0;
    std::string script_;
    static uint64_t now(){return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();}
    bool enabled() const {const char *keep=std::getenv("VISION_KEEP_DISPLAY_MODE");return !keep||std::strcmp(keep,"1");}
    void locate(){
        if(!script_.empty())return;
        char exe[PATH_MAX];const ssize_t n=readlink("/proc/self/exe",exe,sizeof(exe)-1);
        if(n<=0)return;
        exe[n]=0;std::string dir=exe;
        for(int i=0;i<6;++i){const auto at=dir.find_last_of('/');if(at==std::string::npos)break;
            dir.resize(at);const auto candidate=dir+"/common/display/prepare_display.py";
            if(!access(candidate.c_str(),R_OK)){script_=candidate;break;}}
    }
    pid_t start(){
        locate();if(script_.empty()||!enabled())return 0;
        const char *display=std::getenv("DISPLAY");
        const std::string display_arg=std::string("DISPLAY=")+(display?display:":0");
        const pid_t p=fork();
        if(!p){
            const char *args[]={"timeout","6s","/usr/bin/env",display_arg.c_str(),
                                "/usr/bin/python3",script_.c_str(),nullptr};
            execv("/usr/bin/timeout",const_cast<char *const *>(args));_exit(127);
        }
        return p>0?p:0;
    }
  public:
    void prepare(){
        if(!enabled())return;
        const pid_t p=start();if(p)while(waitpid(p,nullptr,0)<0&&errno==EINTR){}
        last_=now();
    }
    void changed(){pending_=true;}
    void poll(){
        if(child_){const auto p=waitpid(child_,nullptr,WNOHANG);if(p==child_||p<0)child_=0;}
        if(!pending_||child_||!enabled()||now()-last_<5000)return;
        last_=now();pending_=false;child_=start();
    }
};
inline HdmiModeGuard &mode_guard(){static HdmiModeGuard guard;return guard;}
}
#endif
