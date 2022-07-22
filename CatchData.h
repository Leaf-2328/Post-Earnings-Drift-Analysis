#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include "curl/curl.h"
#include "Stock.h"
#include "TimeSeries.h"
#include "Market.h"

using namespace std;

namespace FinalProj {
    
	struct MemoryStruct;
    string dateToStr(Date date_);
	void* myrealloc(void* ptr, size_t size);
	int write_data2(void* ptr, size_t size, size_t nmemb, void* data);
    int getEODdata(string symbol, Date start_date_, Date end_date_, stringstream& sData, bool& ret);
    string toUpper(string& s);
}
