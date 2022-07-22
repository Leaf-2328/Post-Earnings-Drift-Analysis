#include "Thread.h"
namespace FinalProj{
    int Counter::increment(){
        unique_lock<shared_mutex> write(lock);
        return ++cnt;
    }
    
    void Counter::reset(){
        unique_lock<shared_mutex> write(lock);
        cnt = 0;
    }
    
    void process_display_xcode(int curr, int total){
        if(curr == 1) cout <<"Processing... 0%"<<endl;
        int percentage = curr*100/total;
        int last_perc = (curr-1)*100/total;
        if(percentage % 10 == 0 && last_perc%10!=0)
            cout << "Processing... " << percentage << "%" << endl;
    }
}
