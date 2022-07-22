#include <stdio.h>
#include "TimeSeries.h"
namespace FinalProj{
    bool leap(int year){
        return year%4==0 && (year%400==0 || year%100!=0);
    }
    
    bool Date::operator==(const Date& rhs) const{
        return month==rhs.month && day==rhs.day && year==rhs.year;
    }
    
    bool Date::operator<(const Date& rhs) const{
        // d.operator<(d1)
        // first const --> d1, second const --> d
        // Without const 
        if(year<rhs.year) return true;
        if(year==rhs.year){
            if(month<rhs.month) return true;
            if(month==rhs.month){
                if(day<rhs.day) return true;
            }
        }
        return false;
    }
    
    Date& Date::operator=(const Date& rhs){
        year = rhs.year;
        month = rhs.month;
        day = rhs.day;
        return *this;
    }
    
    Date Date::operator+(int days) const{
        Date ret = *this;
        ret.day += days;
        int curr_leap = leap(ret.year) ? 1:0; // if leap year 1, else 0
        while(ret.day > mdays[curr_leap][ret.month-1]){
            ret.day  -= mdays[curr_leap][ret.month-1]; // index not value: -1
            ret.month++;
            ret.year+=(ret.month-1)/12;
            ret.month = (ret.month-1)%12+1;
            curr_leap = leap(ret.year);
        }
        return ret;
        
    }
    
    Date Date::operator-(int days) const{
        Date ret = *this;
        while(days >= ret.day){
            days -= ret.day;
            ret.month--;
            if(ret.month==0){
                ret.month=12;
                ret.year--;
            }
            int curr_leap = leap(ret.year) ? 1:0;
            ret.day = mdays[curr_leap][ret.month-1];
        }
        ret.day -= days;
        return ret;
    }
    
    ostream& operator<<(ostream& os, const Date& date){
        os << dateToStr(date);
        return os;
    }
    
    
    
    TimeSeries::TimeSeries(TimeSeries& rhs){
        for(auto iter = rhs.ts.begin(); iter!=rhs.ts.end(); iter++){
            ts[iter->first] = iter->second;
        }
    }
    
    int TimeSeries::trim(Date &adate, int N){
        for(auto iter = ts.begin(); iter != ts.end(); iter++){
            if(adate == iter->first) {
                auto forward = iter;
                auto backward = iter;
                int shift = 0;
                while(N--){
                    shift++;
                    if(backward == ts.begin()) return shift;
                    forward++;
                    backward--; // get one more day data for Price --> 2N + 2
                    if(forward == ts.end()) return -shift;
                }
                
                if(++forward != ts.end()){
                    ts.erase(forward, ts.end());
                }
                if(--backward != ts.begin()){
                    ts.erase(ts.begin(), backward);
                }
            }
        }
        return 0;
    }
    
    vector<double> TimeSeries::AR(TimeSeries& bm){ // bm Timeseries return
        auto iter=ts.begin();
        vector<double> ret;
        double prev_Price= iter->second;
        iter++;
        for(; iter!=ts.end();iter++){
            ret.push_back((iter->second - prev_Price)/prev_Price-bm[iter->first]);
            prev_Price = iter->second;
        }
        return ret;
    }
    
    TimeSeries TimeSeries::Return(){
        auto iter=ts.begin();
        TimeSeries ret;
        double prev_Price= iter->second;
        iter++;
        for(; iter!=ts.end();iter++){
            ret[iter->first] = (iter->second - prev_Price)/prev_Price;
            prev_Price = iter->second;
        }
        return ret;
    }
    
    TimeSeries TimeSeries::CDR(){
        auto iter=ts.begin();
        TimeSeries ret;
        double prev_Price = iter->second;
        double prev_CDR = 0;
        iter++;
        for(; iter!=ts.end();iter++){
            double return_next = (iter->second - prev_Price)/prev_Price;
            prev_Price = iter->second;
            ret[iter->first] = (1+return_next)*(1+prev_CDR)-1;
            prev_CDR = ret[iter->first];
        }
        return ret;
    }
    
    
    void display(TimeSeries Prices, TimeSeries CDR){
        cout << setw(10) << "Date " << setw(10) << "Price" << setw(10) << "CDR" << endl;
        for(auto iter = Prices.ts.begin(); iter != Prices.ts.end(); iter++){
            cout << setw(10) << iter->first << setw(10) << iter->second << setw(13) << (CDR.find(iter->first) ? to_string(CDR[iter->first]) : "-" ) << endl;
        }
    }
}

