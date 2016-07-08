########################################################################
 # Build Soapy SDR support module for HackRF
 ########################################################################

cmake_minimum_required(VERSION 2.8.7)
project(SoapyMultiSDR CXX)

find_package(SoapySDR "0.4.0" NO_MODULE)
if (NOT SoapySDR_FOUND)
    message(FATAL_ERROR "Soapy SDR development files not found...")
endif ()

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
