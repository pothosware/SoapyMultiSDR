########################################################################
 # Build Soapy SDR support module for HackRF
 ########################################################################

cmake_minimum_required(VERSION 2.8.7)
project(SoapyMultiSDR CXX)

find_package(SoapySDR "0.4.0" NO_MODULE)
if (NOT SoapySDR_FOUND)
    message(FATAL_ERROR "Soapy SDR development files not found...")
endif ()

#enable c++11 features
if(CMAKE_COMPILER_IS_GNUCXX)

    #C++11 is a required language feature for this project
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" HAS_STD_CXX11)
    if(HAS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else(HAS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    endif()

    #disable warnings for unused parameters
    add_definitions(-Wno-unused-parameter)

endif(CMAKE_COMPILER_IS_GNUCXX)

if (APPLE)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wc++11-extensions")
endif(APPLE)

SOAPY_SDR_MODULE_UTIL(
    TARGET MultiSDRSupport
    SOURCES
        Registration.cpp
        Settings.cpp
        Streaming.cpp
)

#unit test for string utils
enable_testing()
include_directories(${SoapySDR_INCLUDE_DIRS})
add_executable(TestMultiNameUtils TestMultiNameUtils.cpp)
target_link_libraries(TestMultiNameUtils ${SoapySDR_LIBRARIES})
add_test(TestMultiNameUtils TestMultiNameUtils)
