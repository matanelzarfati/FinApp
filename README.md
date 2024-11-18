# **FinApp** by M&D
A capital market tracking App developed for C.Sci. Project instead of Exam.
Written in CPP. Uses ImGui with GLFW+OpenGL3, as well as httpslib, SSL and ImPlot.

# Project Requirements
## Basic tech. req.
* use of STLs s.a. Vectors and UnorderedMaps
* use of \<filesystem\>, \<fstream\>
* use of Threads & Atomic/Mutex
* 3rd party libs
	* nlohmann for JSON parsing
 	* CPP-Httplib for HTTP/S requests
  	* Dear ImGui for UI/UX

## Operational req.
* sending an HTTP/S request to WWW retirieve JSON data. can use free APIs rotation from the web.
the downloaded data shall be processed and presented somehow to the user.
* User GUI: based on ImGui. shall show the downloaded data somehow, and let the user to interact with it and the UI.

## To-Do
**minimal**
- [x] stl
	- [x] vectors/find/iterators
- [x] files system
	- [x] \<filesystem\>
 	- [x] \<fstream\>
  	- [x] Read
   	- [x] Write
- [x] Multi-threading
	- [x] Threads
	- [x] Atomic/Mutex
- [x] 3rd party libs
	- [x] [nlohmann JSON](https://github.com/nlohmann/json)
 	- [x] [cpp-httplib](https://github.com/yhirose/cpp-httplib)
  	- [x] [Dear ImGui](https://github.com/ocornut/imgui)

# Dev. Team
* [MatanelXMX](https://github.com/MatanelXMX)
* [Grayscale-Nut](https://github.com/Grayscale-Nut)