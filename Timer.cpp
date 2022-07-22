#include "Timer.h"
namespace FinalProj{
    ostream& operator<<(ostream& os, Timer& t_){
        os << t_.past();
        t_.reset(); // Stop time
        return os;
    }
}
