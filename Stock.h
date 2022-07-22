#ifndef Stock_h
#define Stock_h

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "TimeSeries.h"
#include "Thread.h"
#include "Timer.h"

using namespace std;
namespace FinalProj{
    enum Group {BEAT, MEET, MISS, UNIDENTIFIED};
    class Benchmark;
    Date strToDate(string date_); // EOD
    Date strToDateEng(string date_); // Earnings
    int getEODdata(string symbol, Date start_date_, Date end_date_, stringstream& sData, bool& ret);
    string display(Group g);
    
    class Market;
    
    class Stock{
    public:
        Stock(string symbol_, string name_) : symbol(symbol_), name(name_) {}
        void setN(int N); //set N and reshape timeseries
        void setup_from_earning(string date, string period, string estimate, string reported, string surprise, string surprisePercent);
        virtual void setup_from_EOD(bool& ret); //pricing & returns
        void add_from_EOD(int days, bool& ret);
        void setTS(stringstream& sData);
        void set_group(Group g){ group = g; }
        double getSurprise() { return SurprisePercent; }
        void trim(bool& ret);
        bool getCompleteness() { return completeness; }
        void incomplete(){
            completeness = false;
            group = UNIDENTIFIED; 
        }
        int price_size(){ return Prices->size(); }
        string getSymbol(){ return symbol; }
        //display
        friend ostream& operator<<(ostream& os, Stock& s);
        
        vector<double> getAR(Benchmark* bm);
        string getPeriodEnding(){ return period_ending; }
        void matchBM(Benchmark* bm);
        ~Stock(){
            if(Prices){
                delete Prices;
                Prices = NULL;
            }
        }
        
    protected:
        //from symbols.csv
        string symbol;
        string name;
        
        //pass from Market
        int N;
        Group group = UNIDENTIFIED;
        
        Date announcement_date;
        string period_ending;
        double Estimated = 0, Reported = 0, Surprise = 0, SurprisePercent = 0;
        TimeSeries* Prices = nullptr;
        
        bool completeness = true;
        
    };
    
    class Benchmark : public Stock{
    public:
        Benchmark(string symbol_, string name_):Stock(symbol_,name_){}
        void setup_from_EOD(bool& ret);
        void update_period(Date date_);
        TimeSeries& getReturns(){ return *Returns; }
        ~Benchmark(){
            if(Returns){
                delete Returns;
                Returns = NULL;
            }
        }
    private:
        Date earliest, latest;
        TimeSeries* Returns = nullptr;
    };
}
#endif /* Stock_h */
