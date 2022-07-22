#include "CatchData.h"

using namespace std;
namespace FinalProj {
    
    mutex alloc_lock;
    
	struct MemoryStruct {
		char* memory;
		size_t size;
		size_t total_size;
	};

    //TODO: add mutex
	void* myrealloc(void* ptr, size_t size)
	{
        lock_guard<mutex> guard(alloc_lock);
        
        if (ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	}

	int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
	{
		size_t realsize = size * nmemb;
		struct MemoryStruct* mem = (struct MemoryStruct*)data;
		if ((mem->size + realsize) >= mem->total_size)
			mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
		if (mem->memory) {
			memcpy(&(mem->memory[mem->size]), ptr, realsize);
			mem->size += realsize;
			mem->memory[mem->size] = 0;
			if (mem->size > mem->total_size)
				mem->total_size = mem->size;
		}
		return realsize;
	}
    
    // EOD data
	Date strToDate(string date_) {
		string delimeter = "-";
		size_t pos = 0;

		pos = date_.find(delimeter);
		int year = stoi(date_.substr(0, pos));
		date_.erase(0, pos + delimeter.length());

		pos = date_.find(delimeter);
		int month = stoi(date_.substr(0, pos));
		date_.erase(0, pos + delimeter.length());

		pos = date_.find(delimeter);
		int day = stoi(date_);
		date_.erase(0, pos + delimeter.length());

		Date temp(month, day, year);

		return temp;
	}
    int strMonthtoInt(string m_){
        if(m_=="JAN") return 1;
        if(m_=="FEB") return 2;
        if(m_=="MAR") return 3;
        if(m_=="APR") return 4;
        if(m_=="MAY") return 5;
        if(m_=="JUN") return 6;
        if(m_=="JUL") return 7;
        if(m_=="AUG") return 8;
        if(m_=="SEP") return 9;
        if(m_=="OCT") return 10;
        if(m_=="NOV") return 11;
        if(m_=="DEC") return 12;
        return 0;
    }
    
    // Earning file
    Date strToDateEng(string date_){
        // "01-JUN-20"
        string delimeter = "-";
        size_t pos = 0;
        
        pos = date_.find(delimeter);
        int day = stoi(date_.substr(0, pos)); //"01" -> 1
        date_.erase(0, pos + delimeter.length());
        
        pos = date_.find(delimeter);
        string month_str = date_.substr(0, pos);
        int month = strMonthtoInt(month_str);
        date_.erase(0, pos + delimeter.length());
        
        
        pos = date_.find(delimeter);
        int year = stoi("20"+date_);
        date_.erase(0, pos + delimeter.length());
        
        Date temp(month, day, year); // Date constructor //
        
        return temp;
    }
    
    string dateToStr(Date date_) {
        string month = to_string(date_.month/10)+to_string(date_.month%10);
        string day = to_string(date_.day/10)+to_string(date_.day%10);
        return to_string(date_.year)+"-"+month+"-"+day;
    }
    

    
    int getEODdata(string symbol, Date start_date_, Date end_date_, stringstream& sData, bool& ret){
        // declaration of an object CURL
        if(!ret) return 0;
        CURL* handle;
        
        CURLcode result;
        
        // set up the program environment that libcurl needs
        curl_global_init(CURL_GLOBAL_ALL);
        
        // curl_easy_init() returns a CURL easy handle
        handle = curl_easy_init();
        
        // if everything's all right with the easy handle...
        if (handle)
        {
            Timer t;
            string url_common = "https://eodhistoricaldata.com/api/eod/";
            string start_date = dateToStr(start_date_);
            string end_date = dateToStr(end_date_);
            string api_token = "6272b79e92c6a2.28136545";//lyzz
//            string api_token = "6266bf970391e8.47216784";
            
            
            struct MemoryStruct data;
            data.memory = NULL;
            data.size = 0;
            data.total_size = 0;
            
            memset(data.memory, '\0', data.total_size);
            
            string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
            curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());
            
            
            //adding a user agent
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
            
            
            int tries = 3;
            while(tries-->0){
                if(data.memory && data.memory[0]=='D') break;
                if(!ret) return 0;
                result = curl_easy_perform(handle);
            }
            if(tries<=0) ret=false;
            
            if (result != CURLE_OK)
            {
                // if errors have occured, tell us what is wrong with result
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                cout << "Retrying..." << endl;
                return 1;
            }
            
            sData.str(data.memory);
            
            free(data.memory);
            data.size = 0;
        }
        
        else
        {
            fprintf(stderr, "Curl init failed!\n");
            return -1;
        }
        
        // cleanup since you've used curl_easy_init
        curl_easy_cleanup(handle);
        
        // release resources acquired by curl_global_init()
        curl_global_cleanup();
        return 0;
    }
    
    string toUpper(string& s){
        for(char& a: s){ // Inplace change
            if(a>='a' && a<='z') a += 'A'-'a';
        }
        return s;
    }

}
