#include "Market.h"
#include <algorithm>
namespace FinalProj{
    bool Market::action(char op){
        switch(op){
            case '1':{
                int N = 0;
                while(N<60){
                    cout << "Please enter N (N>=60): ";
                    cin >> N;
                    cout << endl;
                }
                cout << "================================" << endl;
                cout << "Please wait for the market to set up" << endl;
                Timer t;
                init(N,threads_); // thread is data member of MArket
                while(!success && threads_>1) {
                    cout << "Too many requests. Failed to get data." << endl;
                    this_thread::sleep_for(100ns);
                    cout << "To Retry, Enter R; To Exit, Enter any other key" << endl;
                    cin >> op;
                    
                    if(op!='R' && op!='r') return false;
                    cout << "Trying to reboot the market..."<< endl;
                    threads_-=5;
                    init(N,threads_);
                }
                if(threads_<2) {
                    cout << "Failed to set up market " << endl;
                    destory();
                }
                else cout << "Market all set in " << t << " seconds" << endl;
                cout << "================================" << endl;
                cout << endl;
                return true;
            }
                
            case '2':{
                if(!is_init()){
                    cout << "N not set yet. Please set N to start." << endl;
                    break;
                }
                string symbol;
                bool end = false;
                while(!end){
                    cout << "Please enter the stock symbol: ";
                    cin >> symbol;
                    cout << endl;
                    end = display_stock(symbol);
                    cout << endl;
                }
                return true;
            }
                
            case '3':{
                if(!is_init()){
                    cout << "N not set yet. Please set N to start." << endl;
                    break;
                }
                string group;
                cout << "Please choose a group (BEAT, MEET, MISS): ";
                cin >> group;
                cout << endl;
                display_group(group);
                cout << endl;
                return true;
            }
                
            case '4':{
                if(!is_init()){
                    cout << "N not set yet. Please set N to start." << endl;
                    break;
                }
                graph();
                return true;;
            }
                
            case '5':{
                return false;
            }
        }
        return false;
    }
    
    void Market::init(int N_, int threads_){
        Timer t;
        success = true;
        if(stocks.size()==0){
            setup_from_symbolfile();
            setup_from_earnings();
        }
        setN(N_);
        cout << "Catching Data from EOD (threads = " << threads_ << ")..."<< endl;
        setup_from_EOD_mt(threads_);
        if(!success) return;
        cout << "Finished catching data from EOD in " << t << " seconds"<< endl;
        cout << "Reexamining Data..."<< endl;
        int i;
        if((i = check_data_completeness())!=0){
            cout << "ERROR: "<< i << " stocks missing data" << endl;
            success = false;
            return;
        }
        grouping();
        data_calculation(this);
        cout << "Finished data calculation in " << t << " seconds" << endl;
    }
    
    void Market::setN(int N_){
        N = N_;
        counter.reset();
        for (map<string, Stock*>::iterator itr = stocks.begin(); itr != stocks.end(); itr++) {
            (*itr).second->setN(N);
        }
        bm->setN(N);
    }

    void Market::setup_from_symbolfile()
    {
        ifstream fin;
        fin.open(cIWV3000SymbolFile, ios::in);
        string line, name, symbol;
        while (!fin.eof())
        {
            getline(fin, line);
            stringstream sin(line);
            getline(sin, symbol, ',');
            if(symbol=="") continue; // Skip the blank in file
            getline(sin, name);
            addStock(symbol, name);
        }
    }
    
    void Market::setup_from_earnings(){
        ifstream fin;
        fin.open(cIWV3000EarningFile, ios::in);
        string line, symbol_, date, period, estimate, reported, surprise, surprisePercent;
        getline(fin, line);
        while (!fin.eof())
        {
            getline(fin, line);
            stringstream sin(line);
            getline(sin, symbol_, ',');
            if(symbol_=="") continue;
            getline(sin, date, ',');
            getline(sin, period, ',');
            getline(sin, estimate, ',');
            getline(sin, reported, ',');
            getline(sin, surprise, ',');
            getline(sin, surprisePercent);
            stocks[symbol_]->setup_from_earning(date, period, estimate, reported, surprise, surprisePercent);
            bm->update_period(strToDateEng(date));
        }
    }
    
    void Market::setup_from_EOD_mt(int threads_){
        bm->setup_from_EOD(success);
        if(!success) return;
        vector<string> incomplete_list;
        for(auto iter = stocks.begin(); iter!=stocks.end(); iter++){
            if(iter->second->getPeriodEnding()=="") { // Do not exist in earning file
                incomplete_list.push_back(iter->first);
            }
            else iter->second->matchBM(bm);
        }
        
        for(string s: incomplete_list){
            delete stocks[s];
            stocks[s] = NULL;
            stocks.erase(s);
        }
        
        int total = stocks.size();
        int thread_length = stocks.size()/threads_+1;
        int start = 0;
        vector<thread> threads;

        for(int i = 0; i < threads_; i++){
            int step = min(thread_length,total-start);
            threads.push_back(thread(&Market::setup_from_EOD_thread, this, start, step));
            start+=step;
        }
        for (auto &th : threads) {
            th.join();
        }
    }
    
    void Market::setup_from_EOD_thread(int shift, int length){
        int total = stocks.size();
        auto iter = stocks.begin();
        advance(iter, shift);
        for(int i = 0; i < length; i++){
            if(!success) return;
            iter->second->setup_from_EOD(success);
            iter++;
            process_display_xcode(counter.increment(),total);
        }
    }
    
    void Market::addStock(string s_symbol, string s_name){
        stocks[s_symbol] = new Stock(s_symbol, s_name);
    }
    
    void Market::grouping(){
        //1. sort the stocks
        //2. record the grouping result in groups
        //3. set group labels for each stock
        vector<Stock*> vec;

        for (map<std::string, Stock*>::iterator itr = stocks.begin(); itr != stocks.end(); itr++) {
            if((*itr).second->getCompleteness()){
                vec.push_back((*itr).second);
            }
        }

        // Sort the vector
        // Descending order
        std::sort(vec.begin(), vec.end(), [](Stock*& l, Stock*& r) {return l->getSurprise() > r->getSurprise(); });

        long len = vec.size();
        long beat = round(len / 3);
        long meet = round((len - beat) / 2);
        long miss = len - beat - meet;

        vector<Stock*>::iterator itr = vec.begin();
        for (long i = 0; i < beat; i++) {
            (*itr)->set_group(BEAT);
            groups[BEAT][(*itr)->getSymbol()] = (*itr);
            itr++;
        }

        for (long i = 0; i < meet; i++) {
            (*itr)->set_group(MEET);
            groups[MEET][(*itr)->getSymbol()] = (*itr);
            itr++;
        }

        for (long i = 0; i < miss; i++) {
            (*itr)->set_group(MISS);
            groups[MISS][(*itr)->getSymbol()] = (*itr);
            itr++;
        }
    }
    
    void Market::data_calculation(Market* p){
        
        for(Group g : {BEAT, MEET, MISS}){
            math calculation(groups[g],N); // Initialize with stock pointers
            calculation.export_result(results,g,p);
        }
    }
    
    void Market::display_group(string group) {
        
        Group g = identifyGroup(toUpper(group));
        int len = results[g][0].size();
        
        cout << setw(4) << "Index" << setw(14) << "AAR" << setw(14) << "AAR_STD" << setw(14) << "CAAR" << setw(14) << "CAAR_STD" << endl;
        for(int i = 0; i < len; i++){
            cout << setw(4) << i-N
            << setw(14) << results[g][0][i]
            << setw(14) << results[g][1][i]
            << setw(14) << results[g][2][i]
            << setw(14) << results[g][3][i] << endl;
        }
        
    }
    
    bool Market::display_stock(string symbol) {
        toUpper(symbol);
        if(stocks.find(symbol)!=stocks.end()) cout<<(*stocks[symbol])<<endl;
        else{
            cout << "Symbol does not exist." << endl;
            return false;
        }
        return true;
    }
    
    void Market::graph(){
        FILE *pipe, *tempDataFile1, *tempDataFile2, *tempDataFile3;
        
        const char *tempDataFileName1, *tempDataFileName2, *tempDataFileName3;
        double x,y;
        int i;
        tempDataFileName1 = "BEAT";
        tempDataFileName2 = "MEET";
        tempDataFileName3 = "MISS";
        pipe = popen("/usr/local/bin/gnuplot --persist", "w");
        if (pipe != NULL)
        {
            tempDataFile1 = fopen(tempDataFileName1,"w");
            for (i=0; i < 2*N; i++)
            {
                x= i-N;
                y = results[BEAT][2][i];
                fprintf(tempDataFile1, "%lf %lf\n", x,y);
            }
            fclose(tempDataFile1);
            
            tempDataFile2 = fopen(tempDataFileName2,"w");
            for (i=0; i < 2*N; i++)
            {
                x= i-N;
                y = results[MEET][2][i];
                fprintf(tempDataFile2, "%lf %lf\n", x,y);
            }
            fclose(tempDataFile2);
            
            tempDataFile3 = fopen(tempDataFileName3,"w");
            for (i=0; i < 2*N; i++)
            {
                x= i-N;
                y = results[MISS][2][i];
                fprintf(tempDataFile3, "%lf %lf\n", x,y);
            }
            fclose(tempDataFile3);

            // Send it to gnuplot
            fprintf(pipe,"plot \"%s\" with lines, \"%s\" with lines, \"%s\" with lines\n", tempDataFileName1, tempDataFileName2, tempDataFileName3);
            fflush(pipe);
            fprintf(pipe,"exit \n");
            pclose(pipe);

            // Clean up the temp file
            remove(tempDataFileName1);
            remove(tempDataFileName2);
            remove(tempDataFileName3);
            pclose(pipe);
        }
    }
        
    int Market::check_data_completeness(){
        int incomplete = 0;
        int complete = 0;
        int total = stocks.size();
        for(auto iter=stocks.begin(); iter!=stocks.end(); iter++){
            if(!iter->second->getCompleteness()) incomplete++;
            else if(iter->second->price_size()==2*N+2) complete++;
        }
        cout << incomplete << " of " << total << " stocks aborted." << endl;
        cout << "Analyzing " << complete << " of " << total << " stocks..." << endl;
        return total-incomplete-complete;
    }
    
    void Market::destory(){
        for(auto iter = stocks.begin(); iter != stocks.end(); iter++){
            delete iter->second;
            iter->second = NULL;
        }
        delete bm;
        bm = new Benchmark("IWV", "iShares Russell 3000 ETF");
        N = 0;
        stocks.clear();
        results.clear();
    }
    
    Market::~Market(){
        for(auto iter = stocks.begin(); iter != stocks.end(); iter++){
            delete iter->second;
            iter->second = NULL;
        }
        delete bm;
        bm = NULL;
    }
    
    void Market::math::export_result(Matrix<vector<double>>& results, Group g, Market* p){
//        for 40 times:
//            for every Group:
//              //bootstrap
//              //calc AAR
//              //calc AAR std
//              //calc CAAR
//              //calc CAAR std
//              //Update results matrix

        const int times = 40;
        vector<vector<double>> AAR_rec;
        vector<vector<double>> CAAR_rec;
        
        for (int i = 0; i < times; i++)
        {
            // Calculate AAR, AAR_STD, CAAR, CAAR_STD one time
            vector<vector<double>> Once = calcAAR_CAAR_Once_op(p);
            vector<double> AAROnce = Once[0];
            vector<double> CAAROnce = Once[1];
            AAR_rec.push_back(AAROnce);
            CAAR_rec.push_back(CAAROnce);
            
            moving_avg(AAR, AAROnce, i);
            moving_avg(CAAR, CAAROnce, i);
            
        }
        AAR_STD = CalculateSTD(AAR_rec, AAR);
        CAAR_STD = CalculateSTD(CAAR_rec, CAAR);
        
        // Update results matrix
        results[g][0] = AAR;
        results[g][1] = AAR_STD;
        results[g][2] = CAAR;
        results[g][3] = CAAR_STD;
        
    }
    
    vector<Stock*> Market::math::bootstrapping(){ // pass a group parameter
        // Randomly choose 80 stock pointers from dataset; Store in ret;
        vector<Stock*> ret; // 80 different stock pointers

        // Use Map to store if a stock was chosen or not
        map<Stock*,bool> CheckMap;
        
        // Initialize the CheckMap
        for (int i=0; i < dataset.size(); i++)
        {
            CheckMap[dataset[i]] = 0;
        }
        
        // Randomly choose 80 different stock pointers from dataset
        long length = dataset.size();
        
        for (int n = 0; n < 80; n++)
        {
            while (true)
            {
                int random = rand() % length; // random in range 0 to (length-1)
                if (CheckMap[dataset[random]] != 1)
                {
                    CheckMap[dataset[random]] = 1;
                    ret.push_back(dataset[random]);
                    break;
                }
            }
        }
        return ret;
    }
    
    void moving_avg(vector<double>& last_avg, vector<double> new_avg, int idx){
        if(last_avg.size()!=new_avg.size()) return;
        for(int i = 0; i< new_avg.size(); i++){
            last_avg[i] = (last_avg[i]*idx+new_avg[i])/(idx+1);
        }
    }
    
    vector<vector<double>> Market::math::calcAAR_CAAR_Once_op(Market* p){
        //for one bootstrap sample set
        int N = p->N;
        vector<double> AAROnce(2*N+1, 0);
        vector<double> CAAROnce(2*N+1, 0);
        
        Benchmark* bm = p->bm;
        vector<Stock*> StockPrtList = bootstrapping(); // 80 stock pointers
        
        for (int i = 0; i < StockPrtList.size(); i++)
        {
            // AAR calculation
            vector<double> AR = StockPrtList[i]->getAR(bm);
            moving_avg(AAROnce, AR, i);
        }
        
        
        CAAROnce[0] = AAROnce[0];
        for(int DateIndex = -N+1; DateIndex <= N; DateIndex++){
            CAAROnce[DateIndex+N] = CAAROnce[DateIndex+N-1]+AAROnce[DateIndex+N];
        }
        
        return {AAROnce, CAAROnce};
    }
    

    vector<double> CalculateSTD(vector<vector<double>>& List, vector<double>& Average)
    {
        vector<double> ret;
        for (int i=0; i < List[0].size(); i++)
        {
            double var = 0;
            for(int j = 0; j<List.size(); j++){
                var +=pow(List[j][i]-Average[i], 2);
            }
            var /= List[0].size();
            ret.push_back(sqrt(var));
        }
        return ret;
    }
    
    Group identifyGroup(string g){
        if(g == "BEAT") return BEAT;
        if(g == "MEET") return MEET;
        if(g == "MISS") return MISS;
        cerr << "Group Label Unidentified. Please check your input." << endl;
        return UNIDENTIFIED;
    }
    
    string display(Group g){
        if(g == BEAT) return "BEAT";
        if(g == MEET) return "MEET";
        if(g == MISS) return "MISS";
        cerr << "Group Label Unidentified" << endl;
        return "UNIDENTIFIED";
    }
    
}

