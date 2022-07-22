#ifndef TimeSeries_h
#define TimeSeries_h
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

namespace FinalProj{
    const vector<vector<int>> mdays ={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};
    bool leap(int year);
    
    struct Date{
        
        int month = 0;
        int day = 0;
        int year = 0;
        
        Date(){} // 
        Date(int m, int d, int y) : month(m), day(d), year(y){}
        
        bool unset(){ return year==0; }
        bool operator==(const Date& rhs) const;
        bool operator<(const Date& rhs) const;
        Date operator+(int days) const; // used in add_from_EOD
        Date operator-(int days) const; // used in add_from_EOD
        Date& operator=(const Date& rhs); 
        friend ostream& operator<<(ostream& os, const Date& date); // Does not have to be friend, used in display T cpp 145
        
    };
    
    string dateToStr(Date date_);
    
    class TimeSeries{
    public:
        TimeSeries(){};
        TimeSeries(TimeSeries& rhs);
        void clear(){ts.clear();}
//        T GetValue(Date D){return ts.find(D)->second;}
        double& operator[](Date date){return ts[date];}
        Date start_date(){ return ts.begin()->first; }
        Date end_date(){ return (--(ts.end()))->first; }
        int trim(Date &adate, int depth);
        TimeSeries Return();
        vector<double> AR(TimeSeries& bm);
        TimeSeries CDR();
        bool find(Date d){ return ts.find(d)!=ts.end(); }
        int size(){return ts.size();}
        
        friend void display(TimeSeries Prices, TimeSeries CDR);
    private:
        map<Date, double> ts;
    };
};

#endif /* TimeSeries_h */

