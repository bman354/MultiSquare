# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.26
cmake_policy(SET CMP0009 NEW)

# CLIENT_SOURCES at CMakeLists.txt:53 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/*.cpp")
set(OLD_GLOB
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/gameLayer.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/platform/glfwMain.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/platform/otherPlatformFunctions.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/platform/platformInput.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/client/src/renderer.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/out/build/x64-Debug/CMakeFiles/cmake.verify_globs")
endif()

# SHARED_SOURCES at CMakeLists.txt:50 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/common/src/*.cpp")
set(OLD_GLOB
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/common/src/Bullet.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/common/src/MapData.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/common/src/Packet.cpp"
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/common/src/Player.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/out/build/x64-Debug/CMakeFiles/cmake.verify_globs")
endif()

# SERVER_SOURCES at CMakeLists.txt:56 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/server/src/*.cpp")
set(OLD_GLOB
  "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/server/src/server.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "C:/Users/Bryce/Dev Workspace/Cplus/games/MultiSquares/out/build/x64-Debug/CMakeFiles/cmake.verify_globs")
endif()
