########################################################################
# Build Multi Soapy SDR support module
########################################################################
cmake_minimum_required(VERSION 3.1...3.10)
project(SoapyMultiSDR CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(SoapySDR 0.8.1 CONFIG)
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
