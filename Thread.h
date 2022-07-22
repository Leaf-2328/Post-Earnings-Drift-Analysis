#ifndef Thread_hpp
#define Thread_hpp

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <string>
#include <iostream>
#include <cstdio>
#include <condition_variable>

using namespace std;
namespace FinalProj{
    class Counter{
    public:
        int increment();
        void reset();
        
    private:
        int cnt = 0;
        shared_mutex lock;
    };
    
    void process_display_xcode(int curr, int total);
}
#endif /* Thread_hpp */
