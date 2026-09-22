########################################################################
# Build Multi Soapy SDR support module
########################################################################
cmake_minimum_required(VERSION 3.1...3.10)
project(SoapyMultiSDR CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(SoapySDR 0.8.1 NO_MODULE REQUIRED)

get_filename_component(_SoapySDR_ROOT "${SoapySDR_INCLUDE_DIRS}/.." ABSOLUTE)
message(STATUS "SoapySDR root directory: ${_SoapySDR_ROOT}")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND NOT "${_SoapySDR_ROOT}" STREQUAL "/usr")
    set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE ${_SoapySDR_ROOT})
endif()

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
