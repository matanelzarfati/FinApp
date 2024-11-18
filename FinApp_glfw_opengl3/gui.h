#ifndef GUI_H
#define GUI_H


// ================================================================
// ================ Section: Includes & Definitions ===============
#include "stocks.h"
#include "imgui.h"
#include "macros.h"
#include <GLFW/glfw3.h> // for GLFWwindow var

#define M_WIN_HEIGHT 720
#define M_WIN_WIDTH 1280

// ============================== END =============================
// ================================================================






























// ================================================================
// ========= Section: Global Variables, Classes, Structs =========

extern GLFWwindow* window;
//extern M_vec<class internalWindow*> windDB; // Database of the ImGui stand-alone windows.
//
////extern M_thread thread_gui;
//
//// holds info of ImGui stand-alone windows. was used mainly for checking visibility & mini-docking.
//class internalWindow {
//	private:
//		std::string _name;
//
//	public:
//		bool _v;	// can be public cause passed to Begin() with '&' (i.e. lack of encapsulation)
//		int _dock;	// [DEPRECATED].
//
//		internalWindow(std::string name, bool v);
//		M_string getName();
//
//		// ===== [DEPRECATED] =====
//		//void redock();				// [DEPRECATED].
//		//bool* getVisibility();		// [DEPRECATED].
//		//void setVisibility(bool v);	// [DEPRECATED].
//};

// ============================== END =============================
// ================================================================






























// ================================================================
// ====================== Section: Functions ======================


// division of ImGui Demo main() into sub-functions
void init_GUI();	// init(s) of OpenGL, GLFW, ImGui and the OS/Main window
void loop_GUI();	// The main loop of ImGui + rendering
void clean_GUI();	// Cleanup of ImGui stuff. after this the program ends.




void FinApp();	// Wrapper func. called from within loop_GUI(). calls other sub-funcs

// those called from withing FinApp()
void window_news();
void window_stocks();
void window_todo();

// utility func. to find the StockItem in the stocksDB vector.
//internalWindow* findInternalWindow(std::string name);

void ShowDemos(ImVec4 clear_color, ImGuiIO io);

// draws the right side of the window, where the graph is located
void drawGraphArea(StockItem* staticVar_selectedStock);

// prints on top the favorited stocks
void SE_printList_pinned(M_vec<StockItem*> &staticVar_pinnedStockItems, ImGuiTextFilter &staticVar_filter, int & staticVar_filteringShownCounter, StockItem* &staticVar_selectedStock);
// prints the rest of the stocks
void SE_printList(M_vec<StockItem*> &staticVar_pinnedStockItems, ImGuiTextFilter &staticVar_filter, int & staticVar_filteringShownCounter, StockItem* &staticVar_selectedStock, int listName);

// utility functions. add the right-click menu(for Pin/Unpin) and floating "Alt-text"/tooltip.
void Pin_Unpin_Stock_Popup(int mode, StockItem*& staticVar_selectedStock, StockItem* i_SI, M_vec<StockItem*>& staticVar_pinnedStockItems);
void AltText_ToolTip(const char* desc);
// ============================== END =============================
// ================================================================































// ================================================================
// ================= Section: DEPRECATED/JANK-YARD ================
//// docking constants
//#define DOCK_NONE 0
//#define DOCK_LEFT 1
//#define DOCK_MID 2
//#define DOCK_RIGHT 3
//void util_dockingPopup(internalWindow* iw);
// ============================== END =============================
// ================================================================

#endif