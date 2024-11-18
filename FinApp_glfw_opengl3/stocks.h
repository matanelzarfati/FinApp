#ifndef NAME_HOLDER01_H
#define NAME_HOLDER01_H

// ================================================================
// ================ Section: Includes & Definitions ===============
#include <vector>
#include <string>
#include "macros.h"
// ============================== END =============================
// ================================================================




























// ================================================================
// ========= Section: Global Variables, Classes, Structs ==========


// globals
extern M_vec<class StockItem*> stocksDB_nasdaq100, stocksDB_Snp500;  // extern: notify compiler that this var is implmnt. somewhere externally/in other file.
//extern M_queue<class StockItem*> fetchQueue;  // will hold queue for fetching objects. MUST be used with conjunction of Mutex/Locks due to threading
extern bool USE_IMPLOT_LIB;
extern M_thread thread_fetch;

const int CNST_NASDAQ100 = 1, CNST_Snp500 = 2;	// lists to display/load. only CNST_NASDAQ100 implmented as of now.
const int CNST_D5 = 0, CNST_M1 = 1, CNST_M6 = 2, CNST_Y1 = 3, CNST_Y2 = 4;	// constants for _activeGraph. used in determining currently shown/requested graph data





// classes

// Holds data for graph X. Generated via 1-fetch request.
// The data is in the requested format(s.a. Date-Range=1D with R=5min, etc...)
// StockItem holds a vector for those GraphData-s.
struct GraphData {
	float* _graphData;		// graph values, i.e. stocks prices. as of now uses ["c"] data(i.e. closing-prices).
	int _graphData_size;	// the amount of values in _graphData. i.e. "arr.length()"
	float _graphData_min, _graphData_max;	// min/max values in _graphData. used to 'scale' the graph(i.e. to show relevant price range)
	struct tm _graphData_startDate, _graphData_endDate;	// Time structure that holds time. based on Unix-Epoch time. notice: when used needs correction(year +1900, day/month +1, etc)
	M_string _graphData_startDate_str, _graphData_endDate_str; // already corrected data based on startDate & endDate, and in String format. used mainly for fetch-request url.
};


// Holds data related to a stock(s.a. _abrv=ADBE i.e. Adobe)
class StockItem {
	private:
		// fields
		M_string _abrv;	// Abreviation s.a. "ADBE"
		M_string _name;	// Full name s.a. "Adobe Inc."
		M_string _sector;
		M_string _subIndustry;

		GraphData _graphsDB[5];	// graph data. _activeGraph has 5 cases as of now.
								//				[0] 5D graph
								//				[1] 1M graph
								//				[2] 6M graph
								//				[3] 1Y graph
								//				[4] 2Y graph

		bool _v;	// visibility status


		void fetchJson_polygon_io();

	public:
		int _activeGraph;	// the presently chosen/shown graph(5D/1M.../1Y etc)

		// methodes
		StockItem(M_string abrv, M_string name, M_string sector, M_string subIndustry);
		~StockItem();

		void refresh();	// requests/tries to fetch new data
		
		GraphData getGraphData();


		M_string getFullName();
		M_string getName();
		M_string getAbrv();
		M_string getSector();
		M_string getIndustry();
};
// ============================== END =============================
// ================================================================
































// ====================== Section: Functions ======================

void loadStocksDB(int listNameID);	// loads static list of stocks. used mainly for NASDAQ-100 and such sub-lists.
//void printStocksDB(); // utility: prints the stocksDB vector. mainly for debug
//void drawGraph(StockItem* si);	// used to draw a given graph

//StockItem* find(M_string keyword);	// utility func. to find the StockItem in the stocksDB vector.


// util functions. playing around with Time, to get Date as String.
// used mainly for FETCH request date-range
void getTimes(int TimeFrame, struct tm*);
void getTimeToStr(struct tm* past0_present1, M_string *past0_present1_str);
// ============================== END =============================
// ================================================================



#endif