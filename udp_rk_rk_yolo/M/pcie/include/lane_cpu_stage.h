#ifndef LANE_CPU_STAGE_H
#define LANE_CPU_STAGE_H
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <stdexcept>
namespace m_lane {
// One admitted job, no waiting-frame queue. Owner must collect its future
// before submitting again. Exceptions travel back to that owner.
class LaneCpuStage {
    std::mutex mutex_;std::condition_variable wake_;
    std::packaged_task<void()> job_;
    bool busy_=false,stopping_=false;
    std::thread worker_;
    void loop(){
        for(;;){
            std::packaged_task<void()> job;
            {std::unique_lock<std::mutex> lock(mutex_);
             wake_.wait(lock,[&]{return stopping_||job_.valid();});
             if(!job_.valid())return;
             job=std::move(job_);}
            job();
            {std::lock_guard<std::mutex> lock(mutex_);busy_=false;}
            wake_.notify_all();
        }
    }
public:
    LaneCpuStage():worker_(&LaneCpuStage::loop,this){}
    ~LaneCpuStage(){stop();}
    LaneCpuStage(const LaneCpuStage&)=delete;
    LaneCpuStage&operator=(const LaneCpuStage&)=delete;
    template<class F> std::future<void> submit(F function){
        std::unique_lock<std::mutex> lock(mutex_);
        wake_.wait(lock,[&]{return stopping_||!busy_;});
        if(stopping_)throw std::runtime_error("CPU stage stopped");
        job_=std::packaged_task<void()>(function);auto result=job_.get_future();
        busy_=true;wake_.notify_one();return result;
    }
    void stop(){
        {std::lock_guard<std::mutex> lock(mutex_);stopping_=true;}
        wake_.notify_all();if(worker_.joinable())worker_.join();
    }
};
}
#endif
