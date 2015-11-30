cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
set (TARGET_NAME "AddInNative" CACHE INTERNAL "")
project(${TARGET_NAME})

# setup environment
add_definitions(-DUNICODE -D_UNICODE)
if (NOT MSVC) 
  set(PTHREAD_LIBRARY pthread)
  add_definitions(-finput-charset=WINDOWS-1251)
endif()

# define sources

set(sources
  ./ComponentBaseImpl.cpp
  ./Component.cpp
  ./dllmain.cpp
  ./ComponentBaseImpl.def
)

set(headers
  ./ComponentBaseImpl.h
  ./Map.h
  ./register.h
  ./include/AddInDefBase.h
  ./include/com.h
  ./include/ComponentBase.h
  ./include/IMemoryManager.h
  ./include/types.h  
)

# define include directories
include_directories(./include)

# set target
add_library(${TARGET_NAME} SHARED ${sources} ${headers})

target_compile_features(${TARGET_NAME}
  PRIVATE
  cxx_final
  cxx_variadic_templates
)