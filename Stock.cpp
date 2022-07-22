#include "Stock.h"

namespace FinalProj{
    
    void Stock::setN(int N_){
        N = N_;
        if(Prices){
            delete Prices;
            Prices = NULL;
        }
        completeness = true;
    }
    
    void Stock::setup_from_earning(string date, string period, string estimate, string reported, string surprise, string surprisePercent){
        Date temp = strToDateEng(date);
        announcement_date = temp;
        period_ending = period;
        Estimated = stod(estimate);
        Reported = stod(reported);
        Surprise = stod(surprise);
        SurprisePercent = stod(surprisePercent);
    }
    
    void Stock::setTS(stringstream& sData){
        sData.clear();
        sData.seekg(0); // Set pointer to the beginning of the file
        string sValue, sDate;
        double dValue = 0;
        string line;
        TimeSeries Prices_;
        
        // In case the length of Price change, have to delete it first
        if(Prices) {
            Prices_ = *Prices;
            delete Prices;
            Prices = nullptr;
        }
        
        // Variables storing prices and returns
        double Price_next = 0.0;
        double Price_prev = 0.0;
        
        int i = 0;
        
        while (getline(sData, line)) {
            i++;
            size_t found = line.find('-');
            
            if (found != std::string::npos) //until end of the string
            {
                sDate = line.substr(0, line.find_first_of(','));
                line.erase(line.find_last_of(','));
                sValue = line.substr(line.find_last_of(',') + 1);
                dValue = strtod(sValue.c_str(), NULL);
                
                Price_next = dValue;
                Date temp = strToDate(sDate);
                Prices_[temp] = Price_next;
                Price_prev = Price_next;
            }
        }
        
        Prices = new TimeSeries(Prices_); // Copy constructor
    }
    
    void Stock::setup_from_EOD(bool& ret){
        if(!ret) return;
        Timer t;
        Date start_date_ = announcement_date-3*N/2;
        Date end_date_ = announcement_date+3*N/2;
        stringstream sData;
        
        if(getCompleteness()){
            getEODdata(symbol, start_date_, end_date_, sData, ret);
            if(!ret) return;
            setTS(sData);
        }
        
        if(!ret) return;
        trim(ret);
        if(!getCompleteness()){
            delete Prices;
            Prices = NULL;
        }
    }
    
    void Stock::add_from_EOD(int days, bool& ret){
        if(!ret) return;
        Date curr_bound;
        if(days > 0) curr_bound= Prices->end_date();
        else curr_bound= Prices->start_date();
        
        Date start_date_ = min(curr_bound+2*days,curr_bound)-1; 
        Date end_date_ = max(curr_bound+2*days,curr_bound)+1;;
        stringstream sData;
        if(getCompleteness()){
            getEODdata(symbol, start_date_, end_date_, sData, ret);
            setTS(sData);
        }
    }
    
    void Stock::matchBM(Benchmark* bm){
        if(getCompleteness() && !bm->Prices->find(announcement_date)) {
            incomplete();
            cout << "=" << symbol << " Aborted: Announcement date does not match the benchmark." << endl;
        }
    }
    
    void Benchmark::setup_from_EOD(bool& ret){
        Date start_date_ = earliest-2*N;
        Date end_date_ = latest+2*N;
        stringstream sData;
        
        if(!ret) return;
        getEODdata(symbol, start_date_, end_date_, sData, ret);
        if(!ret) return;
        setTS(sData);
        
        Returns = new TimeSeries(Prices->Return());
    }
    
    
    void Benchmark::update_period(Date date_){
        if(earliest.unset()){
            earliest = date_;
            latest = date_;
            return;
        }
        earliest = min(earliest,date_);
        latest = max(latest,date_);
    }
    
    //display
    ostream& operator<<(ostream& os, Stock& s){
        
        os << "Symbol: " << s.symbol << endl
        << "Name: " << s.name << endl;
        if(!s.getCompleteness()){
            os << "Incomplete stock" << endl;
            return os;
        }
        
        os << "Group: " << display(s.group) << endl
        << "Earning Announcement Date: " << s.announcement_date << endl
        << "Period Ending: " << s.period_ending << endl
        << "Estimated: " << s.Estimated << endl
        << "Reported Earnings: " << s.Reported << endl
        << "Surprise: " << s.Surprise << endl
        << "Surprise %: " << s.SurprisePercent << endl << endl;
        display(*s.Prices, s.Prices->CDR());
        return os;
    }
    
    vector<double> Stock::getAR(Benchmark* bm){
        if(completeness) return Prices->AR(bm->getReturns());
        return {};
    }
    
    void Stock::trim(bool& ret){
        int i = 0;
        int last = 0;
        if(!ret) return;
        while(completeness&&(i = Prices->trim(announcement_date, N))!=0){
            if(i > 0) add_from_EOD(2*(N-i),ret);
            else add_from_EOD(2*(-N-i),ret);
            if(i==last) incomplete();
            if(!getCompleteness()) cout << "=" << symbol << " Aborted: Not enough data." << endl;
            last = i;
        }
    }
}
