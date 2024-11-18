// ================================================================
// ================ Section: Includes & Definitions ===============
#include <iostream>
#include <chrono>
#include "stocks.h"
#include "logger.h"
#include "gui.h"
// ============================== END =============================
// ================================================================



















extern Logger logger;
bool FinApp_isRunning = true;
const unsigned int THREAD_LOGGER_SLEEP_TIME = 5;	// sec







// ================================================================
// ========================== Functions ===========================

int main() {

	logger.log("Running the FinApp program...");

	loadStocksDB(CNST_NASDAQ100);
	loadStocksDB(CNST_Snp500);
	
	
	M_thread thread_gui([]() {
		init_GUI();
		logger.log("Initial settings have been loaded successfully");
		logger.log("GUI is in progress...");
		loop_GUI();
		logger.log("GUI loop finished");
		clean_GUI();
		logger.log("GUI successfully cleared");
		DEBBUG_MSG("loop_GUI() finished.")
		}// END: thread's Lambda-func
	);// END: thread func

	M_thread thread_logger([](){
		while (FinApp_isRunning) {
			logger.flush();
			std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_LOGGER_SLEEP_TIME*1000));
			COUT << "logger flushed..." << ENDL;
		}

		logger.log("Logger thread exiting...");
		logger.flush();
		}
	);
	thread_gui.join();  // awaits gui thread
	thread_logger.join();  // awaits gui thread
	DEBBUG_MSG("loop_GUI thread has finished.");

	



	
	return 0;
}
// ============================== END =============================
// ================================================================