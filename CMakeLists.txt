cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR) # because of CMAKE_CXX_STANDARD
project (K8063 C CXX)

option ( BUILD_SHARED_LIBS "Build a shared object or DLL" off )

### Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "")
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  message(STATUS "")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


# Global setting: build everything position independent
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# by default, force c++11
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# in case we have any c code, use the best c standard ever
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)




### set compile flags
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Warray-bounds -Wstack-protector")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -Wextra -fno-inline-functions -fsanitize=address ") #  -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fstack-check -fstack-protector-all")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fstack-check -fstack-protector-all")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    if(USE_LIBCXX)
        set(STDCXX "libc++")
    else(USE_LIBCXX)
        set(STDCXX "libstdc++")
    endif(USE_LIBCXX)
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -stdlib=${STDCXX}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -Wextra -Wdocumentation -fno-inline-functions ")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g3 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -fstack-protector-all")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fstack-check -fstack-protector-all")
endif() 


add_executable(demo "k8063.cpp" "com.c" "com.h")
