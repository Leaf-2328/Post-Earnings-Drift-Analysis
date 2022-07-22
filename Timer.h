#ifndef Timer_hpp
#define Timer_hpp

#include <ctime>
#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std;

namespace FinalProj{
    class Timer{
    public:
        Timer(){
            last = time(nullptr);
        }
        void reset(){
            last = time(nullptr);
        }
        double past(){
            return difftime(time(nullptr), last);
        }
        friend ostream& operator<<(ostream& os, Timer& t_);
    private:
        time_t last; 
    };
}

#endif /* Timer_hpp */
