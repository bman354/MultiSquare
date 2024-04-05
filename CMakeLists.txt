#https://github.com/meemknight/cmakeSetup
#Version 1.0.0

cmake_minimum_required(VERSION 3.16)


#set this to true to ship the game!
#! ! ! ! ! ! !
#DELETE THE OUT FOLDER AFTER CHANGING THIS BECAUSE VISUAL STUDIO DOESN'T SEEM TO RECOGNIZE THIS CHANGE AND REBUILD!
option(PRODUCTION_BUILD "Make this a production build" FALSE)
#DELETE THE OUT FOLDER AFTER CHANGING THIS BECAUSE VISUAL STUDIO DOESN'T SEEM TO RECOGNIZE THIS CHANGE AND REBUILD!

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
else()
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()



#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(multiSquares)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading immaged
add_subdirectory(thirdparty/stb_truetype)		#loading ttf files
add_subdirectory(thirdparty/enet-1.3.17)		#networking
add_subdirectory(thirdparty/raudio)				#audio
add_subdirectory(thirdparty/glm)				#math
add_subdirectory(thirdparty/imgui-docking)		#ui
#----------V-----------------------V------------#my libraries												
add_subdirectory(thirdparty/safeSave)			#saving data, designed for gamedev. It also makes backups so the player won't loose saves.
add_subdirectory(thirdparty/profilerLib)		#profiling, just a simpe library for measuring elapsed time
add_subdirectory(thirdparty/gl2d)				#2D rendering library
add_subdirectory(thirdparty/glui)				#ui library, usefull for making game menus



# Define shared sources
file(GLOB_RECURSE SHARED_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/common/src/*.cpp")

# Define client sources
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/client/src/*.cpp")

# Define server sources
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/server/src/*.cpp")

# Client executable
add_executable(MultiSquaresClient)
target_sources(MultiSquaresClient PRIVATE ${CLIENT_SOURCES} ${SHARED_SOURCES})
if(PRODUCTION_BUILD)
	# setup the ASSETS_PATH macro to be in the root folder of your exe
	target_compile_definitions(MultiSquaresClient PUBLIC RESOURCES_PATH="/client/resources/") 

	# remove the option to debug asserts.
	target_compile_definitions(MultiSquaresClient PUBLIC PRODUCTION_BUILD=1) 

else()
	# This is useful to get an ASSETS_PATH in your IDE during development
	target_compile_definitions(MultiSquaresClient PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/client/resources/")
	target_compile_definitions(MultiSquaresClient PUBLIC PRODUCTION_BUILD=0)
endif()

target_include_directories(MultiSquaresClient PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/client/include/" "${CMAKE_CURRENT_SOURCE_DIR}/client/platform/include/")
target_link_libraries(MultiSquaresClient PRIVATE glm glfw glad stb_image stb_truetype gl2d raudio imgui safeSave enet profilerLib glui)



# Server executable
add_executable(MultiSquaresServer)
target_sources(MultiSquaresServer PRIVATE ${SERVER_SOURCES} ${SHARED_SOURCES})
if(PRODUCTION_BUILD)
	# remove the option to debug asserts.
	target_compile_definitions(MultiSquaresServer PUBLIC PRODUCTION_BUILD=1) 

else()
	target_compile_definitions(MultiSquaresServer PUBLIC PRODUCTION_BUILD=0) 
endif()

target_include_directories(MultiSquaresServer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server/include/")
target_link_libraries(MultiSquaresServer PRIVATE enet)



set_property(TARGET MultiSquaresClient PROPERTY CXX_STANDARD 17)
set_property(TARGET MultiSquaresServer PROPERTY CXX_STANDARD 17)


if(MSVC) # If using the VS compiler...

	target_compile_definitions(MultiSquaresClient PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_compile_definitions(MultiSquaresServer PUBLIC _CRT_SECURE_NO_WARNINGS)

	set_target_properties(MultiSquaresClient PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console
	set_target_properties(MultiSquaresServer PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

	#set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
	#set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

endif()