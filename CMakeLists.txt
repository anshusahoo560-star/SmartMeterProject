cmake_minimum_required(VERSION 3.10)
project(SmartMeterProject CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pthreads for Linux multithreading
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Put executables in bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Core library source files (excluding main.cpp)
set(LIB_SOURCES
    src/PulseCounter.cpp
    src/PulseSimulator.cpp
    src/EnergyCalculator.cpp
    src/DataLogger.cpp
    src/AlertManager.cpp
    src/Analytics.cpp
)

# Main executable
add_executable(SmartMeter src/main.cpp ${LIB_SOURCES})
target_link_libraries(SmartMeter PRIVATE Threads::Threads)

# Unit test executable
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/test_meter.cpp")
    add_executable(TestSmartMeter tests/test_meter.cpp ${LIB_SOURCES})
    target_link_libraries(TestSmartMeter PRIVATE Threads::Threads)
endif()
