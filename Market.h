#ifndef Market_h
#define Market_h
#include "Matrix.h"
#include "Stock.h"
#include <iterator>
#include <cmath>

namespace FinalProj{
    vector<double> CalculateSTD(vector<vector<double>>& List, vector<double>& Average);
    Group identifyGroup(string g);
    string toUpper(string& s);
    void moving_avg(vector<double>& last_avg, vector<double> new_avg, int idx);

    class Market{
    public:
        Market():results(3,4), N(0){}

        bool action(char op);
        
        ~Market();
        
    private:
        map<string, Stock*> stocks;
        Benchmark* bm = new Benchmark("IWV", "iShares Russell 3000 ETF");
        int N;
        int threads_=10;
        map<Group, map<string,Stock*>> groups;
        Matrix<vector<double>> results; //matrix:3*4; every cell:(2N+1) AAR/AAR_STD/CAAR/CAAR_STD
        const char* cIWV3000SymbolFile = "Russell_3000_component_stocks.csv";
        const char* cIWV3000EarningFile = "Russell3000EarningsAnnouncements.csv";
        Counter counter;
        bool success = true; // All threads can access 
        
        //called by menu
        void init(int N_, int threads_);
        void display_group(string group);
        bool display_stock(string symbol);
        void graph();
        bool is_init(){ return N!=0; }
        void destory();
        
        //helpers
        void setup_from_symbolfile();
        void setup_from_earnings();
        void setup_from_EOD_mt(int threads_);
        void setup_from_EOD_thread(int shift, int length);
        void addStock(string s_symbol, string s_name);
        void setN(int N_);
        void grouping();
        void data_calculation(Market* p);
        int check_data_completeness();
        
        
        class math{
        public:
            math(map<string, Stock*>& ds, int N) : AAR(2*N+1,0), AAR_STD(2*N+1,0), CAAR(2*N+1,0), CAAR_STD(2*N+1,0){// Allocate memory 
                for(auto iter=ds.begin(); iter!=ds.end();iter++){
                    dataset.push_back(iter->second);
                }
            }
            
            // Calculate single Group's AAR,AAR std, CAAR, CAAR std and store values in results matrix
            void export_result(Matrix<vector<double>>& results, Group g, Market* p);
            
        private:
            vector<Stock*> dataset;
            vector<double> AAR, AAR_STD, CAAR, CAAR_STD;
            vector<Stock*> bootstrapping();
            vector<vector<double>> calcAAR_CAAR_Once_op(Market* p);
        };
    };
}

#endif /* Market_h */
