// ================================================================
// ================ Section: Includes & Definitions ===============
#include <vector>
#include <utility>
#include <string>
#include "stocks.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "imgui.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <time.h>
#include "implot.h"
#include "logger.h"
// ============================== END =============================
// ================================================================





























// ================================================================
// =================== Section: Global Variables ==================

M_vec<StockItem*> stocksDB_nasdaq100, stocksDB_Snp500; // holds the list of stocks to be shown.
//M_queue<class StockItem*> fetchQueue;  // will hold queue for fetching objects. MUST be used with conjunction of Mutex/Locks due to threading
bool USE_IMPLOT_LIB = true; // sets which lib-graph to use by default
bool POLYGONE_ONE_DAY_LATENCY = false;   // whether the fetch requests will be corrected with (-24h)
StockItem* staticVar_selectedStock = NULL;
M_string listOfKeys[] = {
    "eqrA6nHTlTDoaEN9XTZR_qp__YKr7oC2",
    "Z8i1XdTssmJOKPilsEZa4A2VKHCJEW5h",
    "IB7CHyxf3xZbF8aSEmexkHaYsOYvD1SO"
};

// ============================== END =============================
// ================================================================






























// ================================================================
// ================= Section: Classes' Fucntions ==================



StockItem::StockItem(M_string abrv, M_string name, M_string sector, M_string subIndustry) {
	_abrv = abrv;
	_name = name;
	_sector = sector;
	_subIndustry = subIndustry;
	_v = true;


    _activeGraph = CNST_M1;  // default opt.

    for (int i = 0; i < 5; i++) {
        _graphsDB[i]._graphData = NULL; // that the only really important init. this affects if it auto-fetch.
        // the rest of attributes of the struct will be init as needed during fetching.
    }
}

StockItem::~StockItem() {}


GraphData StockItem::getGraphData() { return _graphsDB[_activeGraph]; }



void StockItem::refresh() {
        fetchJson_polygon_io();
        // fetch from other sites
}

void StockItem::fetchJson_polygon_io() {
    // ================ [ Section: Vars ] ================
    static int Keys_counterOfUsesPerKey = 0, Keys_index = 0;
    static M_string apiKey;


    // ================ [ Section: Key Rotation ] ================
    if (Keys_counterOfUsesPerKey == 5) {
        Keys_counterOfUsesPerKey = 0;
        Keys_index += 1;
        if (Keys_index == listOfKeys->length())
            Keys_index = 0; // loop over to begining
        apiKey = listOfKeys[Keys_index];
        COUT << "Key swapped: " << apiKey << ENDL;
        logger.log("Key swapped: "+ apiKey);
    }
    else {
        apiKey = listOfKeys[Keys_index];
        Keys_counterOfUsesPerKey += 1;
    }



    // ================ [ Section: Prep DateTime for request ] ================
    struct tm past0_present1[2];
    getTimes(_activeGraph, past0_present1);
    M_string past0_present1_str[2] = {"" , ""};
    getTimeToStr(past0_present1, past0_present1_str);
    DEBBUG_MSG(past0_present1_str << ": " << past0_present1_str[0] << ", " << past0_present1_str[1]);



    // ================ [ Section: Try Connect ] ================
    try {
        // try connecting
        httplib::SSLClient cli("api.polygon.io");
        cli.enable_server_certificate_verification(true);
        



        // ================ [ Section: Try Fetch ] ================
        // try fetch & json-parse
        try {
            M_string mult = (_activeGraph == CNST_D5) ? "30" : "1",
                lifespan = (_activeGraph == CNST_D5)? "minute" : "day";
            const auto fetchResult =
                cli.Get("/v2/aggs/ticker/" + _abrv + "/range/"
                    + mult + "/"
                    + lifespan + "/"
                    + past0_present1_str[0] + "/"
                    + past0_present1_str[1] + "?"
                    + "adjusted=true" + "&"
                    + "limit=50000" + "&"
                    + "sort=asc" + "&"
                    + "apiKey=" + apiKey);
            logger.log("=============="+fetchResult->body + "\n==============");  // debug



            // ================ [ Section: Check Statuses ] ================
            // stats codes:     200 - ok        400 - error/bad request     429 - too many requests
            if (fetchResult->status == 400) {
                logger.log("ERR: [polygon.io] query failed with(400)(" + _abrv + ").");
                return;
            }
            else if (fetchResult->status == 429) {
                logger.log("ERR: [polygon.io] reached max query per min (429)(" + _abrv + ").");
                staticVar_selectedStock = NULL; // segnifies status overload(+5/min)
                return;
            }
            else if (fetchResult->status == 200) {
                logger.log("https request (fetchResult->status = 200), Successfully accepted");
                
                const M_json data_body = M_json::parse(fetchResult->body);  // string to JSON
                DEBBUG_MSG(data_body.dump(4) << "\n==============");  // debug

                if (data_body["status"] != "OK") { 
                    logger.log("ERR: [polygon.io] query status is 200 and not 'OK' (" + _abrv + ")."); }




                // ================ [ Section: Data Extraction into GraphData ] ================
                if (_graphsDB[_activeGraph]._graphData != NULL)   // already has data
                    delete[] _graphsDB[_activeGraph]._graphData;  // clear previous data


                _graphsDB[_activeGraph]._graphData_size = data_body["resultsCount"].get<int>(); // stoi(data_body["resultsCount"].dump());// size;     // size
                _graphsDB[_activeGraph]._graphData = new float[_graphsDB[_activeGraph]._graphData_size]();  // malloc block of floats
                _graphsDB[_activeGraph]._graphData_min = INT_MAX;           // min init. will be updated.
                _graphsDB[_activeGraph]._graphData_max = INT_MIN;           // max init. will be updated.
                _graphsDB[_activeGraph]._graphData_startDate = past0_present1[0];   // start time of the request(default: 1M)
                _graphsDB[_activeGraph]._graphData_endDate = past0_present1[1];     // usually present time
                _graphsDB[_activeGraph]._graphData_startDate_str = past0_present1_str[0];     // usually present time
                _graphsDB[_activeGraph]._graphData_endDate_str = past0_present1_str[1];     // usually present time
                
                for (int i = 0; i < _graphsDB[_activeGraph]._graphData_size; i++) {
                    // store graph-data value
                    float tempVar_val = data_body["results"][i]["c"].get<float>();
                    _graphsDB[_activeGraph]._graphData[i] = tempVar_val;

                    // update min val
                    _graphsDB[_activeGraph]._graphData_min =
                        (tempVar_val < _graphsDB[_activeGraph]._graphData_min) ?    // if current min is LARGER than value[i]
                            tempVar_val : _graphsDB[_activeGraph]._graphData_min;   // update min. else, leave-be.

                    // update max val
                    _graphsDB[_activeGraph]._graphData_max =
                        (tempVar_val > _graphsDB[_activeGraph]._graphData_max) ?    // if current max is SMALLER than value[i]
                        tempVar_val : _graphsDB[_activeGraph]._graphData_max;       // update max. else, leave-be.
                }// for
            
            }// if HTTP=200

        }catch (std::exception& e) { logger.log("ERR: exception in fetching[polygon.io] data of : " + _abrv); staticVar_selectedStock = NULL;  return; }
    }catch (std::exception& e) { logger.log("ERR: problem connecting to [polygon.io] during fetch of: " + _abrv); staticVar_selectedStock = NULL; return; }



    // ================ [ Section: Corrections/Debug/msgs ] ================
    // small correction/padding for the graph's Min/Max, so it looks prettier. (doesnt affect the stock-prices values! just cosmetics)
    _graphsDB[_activeGraph]._graphData_min *= 0.97f;
    _graphsDB[_activeGraph]._graphData_max *= 1.03f;


    logger.log("Data fetched and stored successfuly");

#ifdef DEBBUG_MODE_ON
    DEBBUG_MSG("Data fetched and stored successfuly.\ndata['c']: {");
    if(_graphsDB[_activeGraph]._graphData != NULL)
        for (int i = 0; i < _graphsDB[_activeGraph]._graphData_size; i++)
            COUT << _graphsDB[_activeGraph]._graphData[i] << ", ";
    COUT << "}" << ENDL;
#endif
}






M_string StockItem::getFullName() {
    // padding correctiong, since _abrv has diff. lengths and it looks ugly.
    int len = _abrv.length();
    M_string pad =
        (len == 5) ? "" :
        (len == 4) ? " " :
        (len == 3) ? "  " :
        (len == 2) ? "   " : "    ";
    return (_abrv + pad + " -  " + _name); }
M_string StockItem::getName() { return _name; }
M_string StockItem::getAbrv() { return _abrv; }
M_string StockItem::getSector() { return _sector; }
M_string StockItem::getIndustry() { return _subIndustry; }


// ============================== END =============================
// ================================================================





























// ================================================================
// ====================== Section: Functions ======================


//[0] past --> [1] present
void getTimeToStr(struct tm* past0_present1, M_string *past0_present1_str) {

    int tempVar_day = past0_present1[0].tm_mday;   // month day
    int tempVar_month = past0_present1[0].tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
    int tempVar_year = past0_present1[0].tm_year + 1900; // Year is # years since 1900
    past0_present1_str[0] = TO_STR(tempVar_year) + "-"
        + ((tempVar_month < 10) ? "0" : "") + std::to_string(tempVar_month) + "-"
        + ((tempVar_day < 10) ? "0" : "") + std::to_string(tempVar_day);


    tempVar_day = past0_present1[1].tm_mday;
    tempVar_month = past0_present1[1].tm_mon +1;
    tempVar_year = past0_present1[1].tm_year + 1900;
    past0_present1_str[1] = TO_STR(tempVar_year) + "-"
        + ((tempVar_month < 10) ? "0" : "") + std::to_string(tempVar_month) + "-"
        + ((tempVar_day < 10) ? "0" : "") + std::to_string(tempVar_day);

}

//[0] past --> [1] present
void getTimes(int TimeFrame, tm* past0_present1) {
    // date in YYYY-MM-DD format
    //struct tm currentTime;
    time_t temp = time(NULL);
    temp -= (POLYGONE_ONE_DAY_LATENCY)? 86400 : 0;    // current time. removed 1D for API.
    localtime_s(&past0_present1[1], &temp);
    /*DEBBUG_MSG(temp << ENDL << TO_STR(past0_present1[1].tm_year + 1900) + " "
        << TO_STR(past0_present1[1].tm_mon + 1) + " "
        << past0_present1[1].tm_mday);*/


    // calculating Data-starting date(/past date)
    // 1D = ~ 86,400 seconds
    if (TimeFrame == CNST_D5)
        temp -= 86400*5;         // 5D = ~ 432,000 seconds
    else if (TimeFrame == CNST_M1)
        temp -= 86400*30;        // 1M = ~ 2,629,746  seconds
    else if (TimeFrame == CNST_M6)
        temp -= 86400*182;       // 6M = ~ 15,724,800 seconds
    else if (TimeFrame == CNST_Y1)
        temp -= 86400*365;       // 1Y = ~ 31,622,400 seconds
    else if (TimeFrame == CNST_Y2)
        temp -= 86400*365*2;     // 2Y = ~ 63,244,800 seconds    
    
    localtime_s(&past0_present1[0], &temp);  // starting date --> struct
}

// utility: builds the stocksDB vector
void loadStocksDB(int listNameID) {
	M_string path = "";
	M_string filedata = "";

    if (listNameID == CNST_NASDAQ100) {
        path = "Resource Files/abrvList_NASDAQ100.json";
    }
    else if (listNameID == CNST_Snp500) {
        path = "Resource Files/abrvList_SP500.json";
    }

	if (path != "") {
		std::ifstream inputFile(path);

        if (!inputFile.is_open()) { logger.log("ERR: loadAbrvList() - NASDAQ100\n"); }
		
		M_string line = "";
		while (std::getline(inputFile, line)) {
			filedata += line;
		}
		//std::cout << filedata << std::endl << std::endl; // debug

		inputFile.close();
    }
    else { logger.log("ERR: loadAbrvList() - unknown list ID"); exit(EXIT_FAILURE); }


	
	nlohmann::json jsondata = nlohmann::json::parse(filedata);

	for (int i = 0; i < jsondata.size(); i++) {
		//std::cout << jsondata[i]["GICS Sub-Industry"].get<std::string>() << std::endl << std::endl;
		StockItem* temp = new StockItem(
			jsondata[i]["Ticker"].get<std::string>(),
			jsondata[i]["Company"].get<std::string>(),
			jsondata[i]["GICS Sector"].get<std::string>(),
			jsondata[i]["GICS Sub-Industry"].get<std::string>());

        if(listNameID == CNST_NASDAQ100)
		    stocksDB_nasdaq100.push_back(temp);
        else if(listNameID == CNST_Snp500)
            stocksDB_Snp500.push_back(temp);
	}
}



//void drawGraph(StockItem* si) {
//
//    fetchQueue.push(si);
//}


// ============================== END =============================
// ================================================================







/*
JSON polygon.io format:
=== aggregate(bars) query ===
    {
        ticker:abrv         // The exchange symbol that this item is traded under.
        queryCount:int      // The number of aggregates (minute or day) used to generate the response.
        resultsCount:int    // The total number of results for this request.
        adjusted:bool       // (?) Whether or not this response was adjusted for splits.
        results:[
            {
                v:float,    // The trading volume of the symbol in the given time period.
                vw:float,   // The volume weighted average price.
                o:float,    // The open price for the symbol in the given time period.
                c:float,    // The close price for the symbol in the given time period.
                h:float,    // The highest price for the symbol in the given time period.
                l:float,    // The lowest price for the symbol in the given time period.
                t:int,      // The Unix Msec timestamp for the start of the aggregate window.
                n:float     // The number of transactions in the aggregate window.
            },
            { ... },
            { ... }
        ],
        status:"OK"/"DELAYED"/"ERROR",  // The status of this request's response.
        request_id:string,              // A request id assigned by the server.
        count:20,                       // (?)
        next_url:url                    // If present, this value can be used to fetch the next page of data.
    }
*/

