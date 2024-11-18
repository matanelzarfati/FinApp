#ifndef MACROS_H
#define MACROS_H


// ================================================================
// ================ Section: Includes & Definitions ===============
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <queue>


// comment-out this to turn off debbug messages.
// use DEBBUG_MSG. ex:  DEBBUG_MSG( "ERR" << ": [" << str1 << "]" )
//#define DEBBUG_MODE_ON



// IO
#define ENDL std::endl
#define CIN std::cin
#define COUT std::cout
#define CERR std::cerr

// String
#define TO_STR std::to_string
#define M_string std::string

// STLs
#define M_vec std::vector
#define M_queue std::queue

#define M_thread std::thread
#define M_jthread std::jthread

#define M_fs std::filesystem

#define M_json nlohmann::json







#ifdef DEBBUG_MODE_ON
#define DEBBUG_MSG(x) COUT << x << ENDL;
#else
#define DEBBUG_MSG(x) ;
#endif





//namespace str = std::string;
//namespace fs = std::filesystem;

// ============================== END =============================
// ================================================================



#endif