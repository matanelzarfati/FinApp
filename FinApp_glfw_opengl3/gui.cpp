// ================================================================
// ================ Section: Includes & Definitions ===============

// GLFW + OpenGL 3

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// something to do with reverse-compatability of VS compilers(c.2015)
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif




#include "gui.h"
#include "stocks.h"
#include "logger.h"

#include <time.h>
#include <algorithm>
#include "implot.h"
#include "implot_internal.h"    // for time picking struct
#include "stdio.h"

// ============================== END =============================
// ================================================================































// ================================================================
// =================== Section: Global Variables ==================

extern bool FinApp_isRunning;
extern StockItem* staticVar_selectedStock;
extern bool POLYGONE_ONE_DAY_LATENCY; // for correction of "last updated"

static GLFWwindow* window; // The OS/Main window.

static float staticVar_FontGlobalScale = 1.0f;

//const ImVec2 tempVar_defaultGraphSize = { 900,480 };

const M_string WINDOW_TITLE = "FinApp - by Matanel Zarfati & Daniel Benshtein";

//M_vec<internalWindow*> windDB;  // Database of the ImGui stand-alone windows.
// ============================== END =============================
// ================================================================































// ================================================================
// ================= Section: Classes' Fucntions ==================


static bool staticVar_stocksWindow = true;      // visibility of the stocks window



// ============================== END =============================
// ================================================================































// ================================================================
// ====================== Section: Functions ======================

// GLFW-related function. used in init_GUI().
static void glfw_error_callback(int error, const char* description){ 
    logger.log("ERR: GLFW Error " + std::to_string(error) + ": " + std::string(description)); 
}


// GLFW, OpenGL, and few ImGui releted init(s)
void init_GUI() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        logger.log("ERR: glfwInit");
        exit(EXIT_FAILURE);
    }

    #if defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
    #endif

    


    // Create window(the OS-window) with graphics context
    window = glfwCreateWindow(1280, 720, WINDOW_TITLE.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        logger.log("ERR: glfwCreateWindow");
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window); // maximize window
    glfwSwapInterval(1);        // Enable vsync





    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;


    

    // Setup Dear ImPlot context
    ImPlot::CreateContext();
    //ImGui::GetIO().Fonts->Build();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();




    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }








    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);




    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);





}



// The GUI main loop.
void loop_GUI() {

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // init internal windows DB
    //windDB.push_back(new internalWindow("news", true));
    //windDB.push_back(new internalWindow("stocks", true));
    //windDB.push_back(new internalWindow("todo", true));


    
    // Main loop
    while (FinApp_isRunning)
        {
        // check if termination request was recieved via close button [x]
        FinApp_isRunning = (!glfwWindowShouldClose(window)) ? true : false;

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        
        
        






        // Need to research & play around. either use IO, or PushFont. Also maybe ScaleAllSizes.
        //ImGui::GetStyle().ScaleAllSizes(staticVar_FontGlobalScale);
        // 
        //io.FontGlobalScale = staticVar_FontGlobalScale;
        ImGui::GetFont()->Scale = staticVar_FontGlobalScale;
        ImGui::PushFont(ImGui::GetFont());

        //ShowDemos(clear_color, io);
        FinApp();

        ImGui::PopFont();

        //ImGui::EndFrame();    //needed? none in example
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }// END: the gui loop

        
}// END: loop_GUI func

// At the end of the program, clears things.
void clean_GUI() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}





void ShowDemos(ImVec4 clear_color, ImGuiIO io) {
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }


    // 4. ImPlot Demo
    ImPlot::ShowDemoWindow();
}




// This is essentially the "starter", the "meats & potatoes" of our code/of FinApp code.
// This includes all the logic we students built, using ImGui etc...
void FinApp() {
    // ================ [ Section: vats ] ================
    static bool staticVar_dockingMode = true;
    //COUT << staticVar_dockingMode << ENDL;



    // ================ [ Section: Main Menu ] ================
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File"))
        {
            //ShowExampleMenuFile();    // c. line 7900 at imgui_demo
            if (ImGui::MenuItem(((USE_IMPLOT_LIB) ? "use ImGui Graph" : "use ImPlot Graph"))) { USE_IMPLOT_LIB = !USE_IMPLOT_LIB; COUT << USE_IMPLOT_LIB << ENDL; }
            ImGui::Separator();
            if (ImGui::MenuItem("Docking Mode", NULL, &staticVar_dockingMode)) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) { FinApp_isRunning = false; COUT << "Exit" << ENDL; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Stocks", NULL, &staticVar_stocksWindow);
            ImGui::Separator();
            
            if (ImGui::MenuItem("Font (+)", "+")) { (staticVar_FontGlobalScale < 1.7) ? staticVar_FontGlobalScale += 0.1f : staticVar_FontGlobalScale; }
            if (ImGui::MenuItem("Font (-)", "-")) { (staticVar_FontGlobalScale > 0.3) ? staticVar_FontGlobalScale -= 0.1f : staticVar_FontGlobalScale; }
            
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    

    // ================ [ Section: Key Listener ] ================
    // text zoom-in & zoom-out short-keys
    if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Equal))        // L.Ctrl =
        || (ImGui::IsKeyDown(ImGuiKey_RightCtrl) && ImGui::IsKeyPressed(ImGuiKey_Equal))    // R.Ctrl =
        || ImGui::IsKeyPressed(ImGuiKey_KeypadAdd)) {
        (staticVar_FontGlobalScale < 1.7) ? staticVar_FontGlobalScale += 0.1f : staticVar_FontGlobalScale;
    }
    if ((ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Minus))        // L.Ctrl -
        || (ImGui::IsKeyDown(ImGuiKey_RightCtrl) && ImGui::IsKeyPressed(ImGuiKey_Minus))    // R.Ctrl -
        || ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract)) {
        (staticVar_FontGlobalScale > 0.3) ? staticVar_FontGlobalScale -= 0.1f : staticVar_FontGlobalScale;
    }




    // ================ [ Section: Docking setup ] ================
    if (staticVar_dockingMode) {

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;


        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        window_flags = ImGuiWindowFlags_None
            | ImGuiWindowFlags_NoTitleBar
            | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus
            | ImGuiWindowFlags_NoNavFocus
            //| ImGuiWindowFlags_NoBackground
            ;

        


        if (ImGui::Begin("##DockingWindow", &staticVar_dockingMode, window_flags)) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");     // why? is it a Reserved-ID?

            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None
                //| ImGuiDockNodeFlags_KeepAliveOnly                // Don't display the dockspace node but keep it alive. Windows docked into this dockspace node won't be undocked.
                //| ImGuiDockNodeFlags_NoDockingOverCentralNode     // Disable docking over the Central Node, which will be always kept empty.
                //| ImGuiDockNodeFlags_PassthruCentralNode          // Enable passthru dockspace: 1) DockSpace() will render a ImGuiCol_WindowBg background covering everything excepted the Central Node when empty. Meaning the host window should probably use SetNextWindowBgAlpha(0.0f) prior to Begin() when using this. 2) When Central Node is empty: let inputs pass-through + won't display a DockingEmptyBg background. See demo for details.
                //| ImGuiDockNodeFlags_NoDockingSplit               // Disable other windows/nodes from splitting this node.
                | ImGuiDockNodeFlags_NoResize                     // Disable resizing node using the splitter/separators. Useful with programmatically setup dockspaces.
                //| ImGuiDockNodeFlags_AutoHideTabBar               // Tab bar will automatically hide when there is a single window in the dock node.
                //| ImGuiDockNodeFlags_NoUndocking                 // Disable undocking this node.
                ;

            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            ImGui::End();
        }

    }

    //if (findInternalWindow("news")->_v)     window_news();
    if (staticVar_stocksWindow)   window_stocks();
    //if (findInternalWindow("todo")->_v)     window_todo();

    

}


// Intr.Win: STOCKS. all stuff related to it goes here.
void window_stocks() {

    if (ImGui::Begin("Stocks##stocks-main-window", &staticVar_stocksWindow)) {

        static M_vec<StockItem*> staticVar_pinnedStockItems = {};

        static ImGuiTableFlags AddStock_FormatTable_flags = ImGuiTableFlags_None
            //| ImGuiTableFlags_Resizable
            //| ImGuiTableFlags_Reorderable
            //| ImGuiTableFlags_BordersV
        ;


        if (ImGui::BeginTable("##stocks-table", 2, AddStock_FormatTable_flags)) {
            ImGui::TableSetupColumn("##stocks-table-col0", (ImGuiTableColumnFlags_WidthFixed/* | ImGuiTableColumnFlags_NoResize*/), 300);
            ImGui::TableSetupColumn("##stocks-table-col1", ImGuiTableColumnFlags_WidthStretch);
            
            
            
            
            
            
            
            
            // ================ [ Section: Clocks ] ================
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);

            static int correctionGMT = 0;
            std::time_t currentTime = std::time(NULL) + (correctionGMT - 2)*60*60;  // -2 >> GMT >> +correctiong >> new timezone
            std::tm tempVar_localTime;
            localtime_s(&tempVar_localTime, &currentTime);
            
            std::string clockTime_str = "\t"
                + std::to_string(tempVar_localTime.tm_mday) + "/"
                + std::to_string(tempVar_localTime.tm_mon + 1) + "/"
                + std::to_string(tempVar_localTime.tm_year + 1900) + "  "
                + std::to_string(tempVar_localTime.tm_hour ) + ":"
                + std::to_string(tempVar_localTime.tm_min) + ":"
                + std::to_string(tempVar_localTime.tm_sec)
                + "  (GMT" + ((correctionGMT < 0) ? "" : "+") + TO_STR(correctionGMT) + ")";
            

            if (ImGui::BeginTabBar("##clocks-tabs-bar")) {
                if (ImGui::BeginTabItem("Israel Time")) {
                    correctionGMT = 2;
                    ImGui::Text(clockTime_str.c_str());
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("GMT Time")) {
                    correctionGMT = 0;
                    ImGui::Text(clockTime_str.c_str());
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("NY Time")) {
                    correctionGMT = -7;
                    ImGui::Text(clockTime_str.c_str());
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
            ImGui::Separator();
            





            // ================ [ Section: List Controls ] ================
            if (ImGui::Button("unpin all##unpinAll-button", { 70, 20 })) staticVar_pinnedStockItems = {};


            static ImGuiTextFilter staticVar_filter;
            static int staticVar_filteringShownCounter = 0;

            ImGui::SameLine();
            if (ImGui::Button("x") || ImGui::IsKeyPressed(ImGuiKey_Escape)) staticVar_filter.Clear();

            ImGui::SameLine();
            staticVar_filter.Draw("##filterStocks", 120);

            ImGui::SameLine();
            ImGui::Text((TO_STR(staticVar_filteringShownCounter) + "/" + TO_STR(staticVar_pinnedStockItems.size())).c_str()); // shows counter from previous MainLoop-iter.
            staticVar_filteringShownCounter = 0;    // resets counter. during loop it counts. shows counter in next MainLoop-iter. iteration.
            ImGui::Separator();






            // ================ [ Section: Stock Tabs ] ================
            ImGuiTabBarFlags tab_bar_flags = 0
                | ImGuiTabBarFlags_Reorderable
                | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton
                | ImGuiTabBarFlags_DrawSelectedOverline
                | ImGuiTabBarFlags_FittingPolicyScroll;

            // draw the SELECTABLEs for the favorites, first.
            SE_printList_pinned(staticVar_pinnedStockItems, staticVar_filter, staticVar_filteringShownCounter, staticVar_selectedStock);

            if (ImGui::BeginTabBar("SE & Indecies##tab-bar-StockExchanges-indicies", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("NASDAQ-100##NASDAQ-100-SE&I-tab"))
                {
                    //SE_printList_pinned(staticVar_pinnedStockItems, staticVar_filter, staticVar_filteringShownCounter, staticVar_selectedStock);
                    SE_printList(staticVar_pinnedStockItems, staticVar_filter, staticVar_filteringShownCounter, staticVar_selectedStock, CNST_NASDAQ100);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("S&P-500##S&P-500-SE&I-tab"))
                {
                    SE_printList(staticVar_pinnedStockItems, staticVar_filter, staticVar_filteringShownCounter, staticVar_selectedStock, CNST_Snp500);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }



            // ================ [ Section: Graph Area ] ================
            ImGui::SameLine();
            ImGui::TableSetColumnIndex(1);

            {
                ImGui::BeginChild("##the-graph"/*, { 0, 0 }, (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_Border)*/);
                
                drawGraphArea(staticVar_selectedStock);
                
                ImGui::EndChild();  // "##the-graph"
            }
            

           ImGui::EndTable();  // ##stocks - table
        } // window-table


    }// Stocks##stocks-main-window
    ImGui::End();   // Stocks##stocks-main-window
} // end of Window Stock








// Subfunc. prints at the 'top' the SELECTABLES of stocks that were pinned by the user. i.e. "favorites"
void SE_printList_pinned(
    M_vec<StockItem*>& staticVar_pinnedStockItems,
    ImGuiTextFilter& staticVar_filter,
    int& staticVar_filteringShownCounter,
    StockItem*& staticVar_selectedStock)
{
    for (StockItem* i_SI : staticVar_pinnedStockItems) {

        if (ImGui::Selectable(("[*]" + i_SI->getFullName() + "##selectable").c_str(), staticVar_selectedStock == i_SI)) {
            staticVar_selectedStock = i_SI;
        }

        Pin_Unpin_Stock_Popup(1, staticVar_selectedStock, i_SI, staticVar_pinnedStockItems);
    }
}



// Subfunc. prints the REST(non-pinned) of SELECTABLES stocks.
void SE_printList(
    M_vec<StockItem*>& staticVar_pinnedStockItems,
    ImGuiTextFilter& staticVar_filter,
    int &staticVar_filteringShownCounter,
    StockItem* &staticVar_selectedStock,
    int list)
{
    M_vec<StockItem*> chosenList;
    if (list == CNST_NASDAQ100) {
        chosenList = stocksDB_nasdaq100;
    }
    else if (list == CNST_Snp500) {
        chosenList = stocksDB_Snp500;
    }

    ImGui::BeginChild("##list-of-stocks", { 300, 0 });

    // filtering mode
    if (staticVar_filter.IsActive()) {      


        for (StockItem* i_SI : chosenList) {
            if (staticVar_filter.PassFilter((i_SI->getName()).c_str()) ||
                staticVar_filter.PassFilter((i_SI->getAbrv()).c_str())) {

                // uses <algorithms> lib's find() to check if element exists in pinned-Vector
                auto tempVar_iterator = std::find(staticVar_pinnedStockItems.begin(), staticVar_pinnedStockItems.end(), i_SI);  // iterator
                bool tempVar_foundPin = tempVar_iterator != staticVar_pinnedStockItems.end();   // if iterator == end means not found
                if (tempVar_foundPin) continue; // skips since it was already pinned
                M_string tempVar_strID = /*((tempVar_foundPin) ? "[*]" : "") + */i_SI->getFullName() + "##selectable";

                if (ImGui::Selectable(tempVar_strID.c_str(), staticVar_selectedStock == i_SI)) {
                    staticVar_selectedStock = i_SI;/*
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);*/
                }
                Pin_Unpin_Stock_Popup((tempVar_foundPin)? 1 : 0, staticVar_selectedStock, i_SI, staticVar_pinnedStockItems);
                staticVar_filteringShownCounter += 1;
            }
        }
    }
    else {
        // normal mode
        ImGui::Separator();
        for (StockItem* i_SI : chosenList) {

            // uses <algorithms> lib's find() to check if element exists in pinned-Vector
            auto tempVar_iterator = std::find(staticVar_pinnedStockItems.begin(), staticVar_pinnedStockItems.end(), i_SI);  // iterator
            bool tempVar_foundPin = tempVar_iterator != staticVar_pinnedStockItems.end();   // if iterator == end means not found
            
            if (!tempVar_foundPin) {
                if (ImGui::Selectable((i_SI->getFullName() + "##selectable").c_str(), staticVar_selectedStock == i_SI)) {
                    staticVar_selectedStock = i_SI;
                }
                Pin_Unpin_Stock_Popup(0, staticVar_selectedStock, i_SI, staticVar_pinnedStockItems);
            }


            staticVar_filteringShownCounter += 1;

        }
    }
    ImGui::EndChild();  // ##list-of-stocks
}




// utility. reduces code duplication.
// This function does two things:
// 1- when mouse right-clicked on a SELECTABLE stock, shows PIN/UNPIN popup.
// 2- when mouse hovered over a SELECTABLE stock, shows "Alt-text"/tooltip. right now used for Sector/Indstr. data.
// 
// mode: used to defferentiate between actions of PIN/UNPIN.
//       0: staticVar_pinnedStockItems.push()       - i.e. when "PIN" is chosen. adds the item to favorites.
//       1: staticVar_pinnedStockItems.erase()      - i.e. when "UNPIN" is chosen. removes the item from favorites.
void Pin_Unpin_Stock_Popup(int mode, StockItem*& staticVar_selectedStock, StockItem* i_SI, M_vec<StockItem*> &staticVar_pinnedStockItems) {
    if (ImGui::BeginPopupContextItem(("[*]" + i_SI->getFullName() + "##selectable").c_str()))
    {
        // uses <algorithms> lib's find() to check if element exists in pinned-Vector
        auto tempVar_iterator = std::find(staticVar_pinnedStockItems.begin(), staticVar_pinnedStockItems.end(), i_SI);  // iterator
        bool tempVar_foundPin = tempVar_iterator != staticVar_pinnedStockItems.end();   // if iterator == end means not found
        if (ImGui::MenuItem((((tempVar_foundPin) ? "unpin " : "  pin ") + i_SI->getAbrv()).c_str(), NULL)) {
            if(mode == 1) staticVar_pinnedStockItems.erase(tempVar_iterator);
            else if(mode == 0) staticVar_pinnedStockItems.push_back(i_SI);
        }
        ImGui::EndPopup();
    }

    AltText_ToolTip(("sector: " + i_SI->getSector() + "\n" + "industy: " + i_SI->getIndustry()).c_str());
}

// utility. Adds "Alt-text"/tooltip.
// Using it adds the tooltip to the LAST component we created.
void AltText_ToolTip(const char* desc)
{
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


// plots the right side of the screen, where the Graph will be.
void drawGraphArea(StockItem* staticVar_selectedStock) {
    // ================ [ Section: Vars ] ================
    static int staticVar_graphWidth = 900, staticVar_graphHeight = 520; // keeps graph sizes.
    const float CNST_SLIDER_SPEED = 0.4f;
    ImVec2 MinMax_w = { 0, 1920 }; // keeps graph sizes.
    ImVec2 MinMax_h = { 0, 1080 }; // keeps graph sizes.

    static bool staticVar_recenterGraphOnce = false;    // var to keep track of recentering requests for ImPlot
    GraphData agd;      // Active Graph Data. contains NULL or a GraphData with the values to be plotted.
    bool agdNotNull;    // just a short-hand for (agd._graphData =?= NULL).




    // ================ [ Section: Pre-Checks ] ================
    if (staticVar_selectedStock != NULL) { // check if a stock was selected.
        agd = staticVar_selectedStock->getGraphData();
        agdNotNull = (agd._graphData == NULL) ? false : true;   // selected, but is there GraphData w/ values?

        
        if (!agdNotNull) {
            staticVar_selectedStock->refresh(); // debug
            staticVar_recenterGraphOnce = true;
            agdNotNull = true;  // debug
            agd = staticVar_selectedStock->getGraphData(); // debug
        }
        
    }
    else {  // no stock selected. default graph will be drawn.
        staticVar_recenterGraphOnce = true;
        agdNotNull = false;
    }







    // ================ [ Section: Setup Graph & Ploting ] ================
    // ImGui or ImPlot for the Graph?
    if (USE_IMPLOT_LIB) {
        ImPlotLegendFlags tempVar_legendFlags = ImPlotLegendFlags_None    // default
            | ImPlotLegendFlags_NoButtons                 // legend icons will not function as hide/show buttons
            | ImPlotLegendFlags_NoHighlightItem           // plot items will not be highlighted when their legend entry is hovered
            | ImPlotLegendFlags_NoHighlightAxis           // axes will not be highlighted when legend entries are hovered (only relevant if x/y-axis count > 1)
            //| ImPlotLegendFlags_NoMenus                   // the user will not be able to open context menus with right-click
            ;

        ImPlotFlags tempVar_plotFlags = ImPlotFlags_None    // default
            //| ImPlotFlags_NoInputs                // the user will not be able to interact with the plot
            //| ImPlotFlags_NoMenus                 // the user will not be able to open context menus
            //| ImPlotFlags_NoBoxSelect             // the user will not be able to box-select
            | ImPlotFlags_NoFrame                 // the ImGui frame will not be rendered
            | ImPlotFlags_Crosshairs              // the default mouse cursor will be replaced with a crosshair when hovered
            ;

        ImPlotLineFlags tempVar_plotLineFlags = ImPlotLineFlags_None;


        ImPlotAxisFlags IPA_f = ImPlotAxisFlags_None    // default
            | ImPlotAxisFlags_NoMenus                   // the user will not be able to open context menus with right-click
            | ImPlotAxisFlags_NoSideSwitch              // the user will not be able to switch the axis side by dragging it
            | ImPlotAxisFlags_NoHighlight               // the axis will not have its background highlighted when hovered or held
            | ImPlotAxisFlags_Foreground                // grid lines will be displayed in the foreground (i.e. on top of data) instead of the background
            ;


        // ...
        // ... done with flags, onwards with prep. & plotting...
        // ...


        // check and set some limits/flags etc so the graph will be centered.
        if (staticVar_recenterGraphOnce) {  // was a "one-time auto-fit" requested?
            ImPlot::SetNextAxisToFit(ImAxis_Y1);
            ImPlot::SetNextAxisToFit(ImAxis_X1);
            IPA_f |= ImPlotAxisFlags_AutoFit;
            staticVar_recenterGraphOnce = false;    // next iteration will "free" the graph from annoying autofit.
        }
        else {
            ImPlot::SetNextAxisLimits(ImAxis_Y1, (agdNotNull) ? agd._graphData_min : -5, (agdNotNull) ? agd._graphData_max : 5);
            // start date & end date. mktime to get epoch-format
            ImPlot::SetNextAxisLimits(ImAxis_X1, (agdNotNull) ? mktime(&(agd._graphData_startDate)) : time(NULL) - 86400, (agdNotNull) ? mktime(&(agd._graphData_endDate))   : time(NULL));
        }


        if ( ImPlot::BeginPlot(("[" + ((agdNotNull) ? agd._graphData_startDate_str : "__-__-__") + "] -to- [" + ((agdNotNull) ? agd._graphData_endDate_str : "__-__-__") + "]###ImPlot-graph").c_str(), { (float)staticVar_graphWidth, (float)staticVar_graphHeight}, tempVar_plotFlags)) {

            // Legend & setup of this Plot, in prep. to call PlotLine
            ImPlot::SetupLegend(ImPlotLocation_NorthWest, tempVar_legendFlags);
            ImPlot::SetupAxis(ImAxis_X1, "date", IPA_f);
            ImPlot::SetupAxis(ImAxis_Y1, "price", IPA_f);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            //ImPlot::GetStyle().UseLocalTime = false; // sets the ImPlotScale_Time should be the required ImPlotAxisFlags_Time in description.
            
            
            ImPlot::GetStyle().LineWeight = 2;
            if (agdNotNull) {   // agd is not empty, i.e. there are graph values
                int tempVar_scale_day = (staticVar_selectedStock->_activeGraph == CNST_D5) ? 30 * 60 : 86400;  // 30min steps or 24h

                ImPlot::PlotLine(
                    (staticVar_selectedStock->getAbrv() + "##ImPlot-plotline").c_str(),                    // legend & ID
                    (agd._graphData),                       // values ptr
                    agd._graphData_size,                    // ptr values count
                    tempVar_scale_day,                      // xscale
                    mktime(&(agd._graphData_startDate)),    // start
                    tempVar_plotLineFlags                   // flags
                );
            }
            else {
                float f[] = { 0,0,0,0 };
                int c = 4;
                ImPlot::PlotLine(
                    "<none>##ImPlot-plotline",    // legend & ID
                    f,                            // values ptr
                    c,                            // ptr values count
                    86400,                        // xscale
                    time(NULL) - 86400,           // start
                    tempVar_plotLineFlags         // flags
                );
            }

            ImPlot::EndPlot();

        }
    }// done with ImPlot graph
    else {// using ImGui
        M_string tempVar_title = ((agdNotNull)? staticVar_selectedStock->getAbrv() : "<none>")
            + " : [" + ((agdNotNull) ? agd._graphData_startDate_str : "__ - __ - __") + "] - to - ["
            + ((agdNotNull) ? agd._graphData_endDate_str : "__ - __ - __") + "]";

        if (agdNotNull) {
            ImGui::PlotLines("###ImGui - graph",
                agd._graphData,             // values*
                agd._graphData_size,        // size of values*
                0,                          // offset
                (tempVar_title).c_str(),    // overlay/"legend"/title
                agd._graphData_min,         // min
                agd._graphData_max,         // max
                { (float)staticVar_graphWidth, (float)staticVar_graphHeight },   // size of graph
                4);                         // stride, *DONT* touch!
        }
        else {
            float f[] = { 0,0,0,0 };
            int c = 4;

            ImGui::PlotLines("###ImGui - graph",
                f,                          // values*
                c,                          // size of values*
                0,                          // offset
                (tempVar_title).c_str(),    // overlay/"legend"/title
                -5,                         // min
                5,                          // max
                { (float)staticVar_graphWidth, (float)staticVar_graphHeight },   // size of graph
                4);                         // stride, *DONT* touch!
        }
    }// done whith ImGui graph


    
    
    
    // ================ [ Section: RADIOS ] ================


    // this is the "Controls" block below the chart. contains Radios & Buttons.
    M_string tempVar_str[] = { "5D", "1M", "6M", "1Y", "2Y"};          // two arrays for looping, to help reduce repetition
    int tempVar_cnst[] = { CNST_D5 ,CNST_M1 ,CNST_M6 ,CNST_Y1, CNST_Y2};
    if (staticVar_selectedStock != NULL) {  // some stock was selcted!
        for (int i = 0; i < 5; i++) {
            if (ImGui::RadioButton((tempVar_str[i] + "##" + staticVar_selectedStock->getAbrv()).c_str(),
                staticVar_selectedStock->_activeGraph == tempVar_cnst[i])) {
                staticVar_selectedStock->_activeGraph = tempVar_cnst[i];
                staticVar_recenterGraphOnce = true; // to recenter graph
            }
            ImGui::SameLine();
        }
    }
    else { // no stock selected! draw disabled-place-holders.
        ImGui::BeginDisabled(true);    // no graph selected, so disables the following controls...
        for (int i = 0; i < 5; i++) {
            ImGui::RadioButton((tempVar_str[i] + "##RadioButtonPlaceholder").c_str(), false);
            ImGui::SameLine();
        }
        ImGui::EndDisabled();    // end
    }


        
    ImVec2 tempVar_buttonSize = { 65,20 }; // size for recenter & refresh

    // the recenter is relevant only for ImPlot with values
    if (!USE_IMPLOT_LIB || !agdNotNull) ImGui::BeginDisabled(true);
        if (ImGui::Button("recenter", tempVar_buttonSize)) staticVar_recenterGraphOnce = true;
    if (!USE_IMPLOT_LIB || !agdNotNull) ImGui::EndDisabled(); // end
        
    if (!agdNotNull) ImGui::BeginDisabled(true);    // no graph selected, so disables the following controls...
    ImGui::SameLine();
    if (ImGui::Button("refresh##refresh-graph-button", tempVar_buttonSize)) { staticVar_recenterGraphOnce = true; staticVar_selectedStock->refresh(); }


    // ================ [ Section: Last Updated ] ================
    ImGui::SameLine();
    // print the last time data was fetched for this stock
    std::string tempVar_timeStr;
    if (agdNotNull) {
        tm* tempVar_tm = &(agd._graphData_endDate);
        if (POLYGONE_ONE_DAY_LATENCY) { // correction of values
            time_t tempVar_time = (mktime(tempVar_tm) + 86400);
            localtime_s(tempVar_tm, &tempVar_time);
        }
        tempVar_timeStr = "("
            + std::to_string(tempVar_tm->tm_mday) + "/"
            + std::to_string(tempVar_tm->tm_mon + 1) + "/"
            + std::to_string(tempVar_tm->tm_year + 1900) + " "
            + std::to_string(tempVar_tm->tm_hour) + ":"
            + std::to_string(tempVar_tm->tm_min) + ":"
            + std::to_string(tempVar_tm->tm_sec) + ")";
    }
    else
        tempVar_timeStr = "(--/--/-- --:--:--)";    // space holder for date-time
    ImGui::Text(("Last Updated: " + tempVar_timeStr).c_str()); // "end date" is when the fetch was made.

    if (!agdNotNull) ImGui::EndDisabled(); // end






    // ================ [ Section: SLIDERS ] ================
    if (ImGui::BeginTable("##mini-table-for-sliders", 2)) {
        ImGui::TableSetupColumn("##mini-table-for-sliders-C1", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("##mini-table-for-sliders-C2", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableNextRow();
            
        ImGui::TableSetColumnIndex(0);
        ImGui::DragScalar("Width##WidthOfTables",
            ImGuiDataType_S32,      // DataType(ImGui)
            &staticVar_graphWidth,  // *p_data
            CNST_SLIDER_SPEED,                   // drag_speed
            &MinMax_w.x,            // *p_min
            &MinMax_w.y             // *p_max
        );
        ImGui::TableSetColumnIndex(1);
        ImGui::SameLine();
        ImGui::DragScalar("Height##HeightOfTables",
            ImGuiDataType_S32,      // DataType(ImGui)
            &staticVar_graphHeight,  // *p_data
            CNST_SLIDER_SPEED,                   // drag_speed
            &MinMax_h.x,            // *p_min
            &MinMax_h.y             // *p_max
        );

        ImGui::EndTable();
    }
        

}


// ============================== END =============================
// ================================================================











// [DEPRECATED]

// utility func. to find the StockItem in the stocksDB vector.
//internalWindow* findInternalWindow(std::string name) {
//    for (auto i : windDB)
//        if (i->getName() == name)
//            return i;
//    return NULL;
//}



// constructor
//internalWindow::internalWindow(std::string name, bool v) {
//    _name = name;
//    _v = v;
//    _dock = DOCK_NONE;
//}

//M_string internalWindow::getName() { return _name; }