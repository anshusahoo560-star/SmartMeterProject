#include "DataLogger.h"
#include <fstream>
#include <iostream>
#include <iomanip>

DataLogger::DataLogger(const std::string& fname) : filename(fname), headerWritten(false) {
    ensureHeader();
}

void DataLogger::ensureHeader() {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ifstream checkFile(filename);
    bool existsAndNotEmpty = checkFile.is_open() && (checkFile.peek() != std::ifstream::traits_type::eof());
    checkFile.close();

    if (!existsAndNotEmpty) {
        std::ofstream outFile(filename, std::ios::out);
        if (outFile.is_open()) {
            outFile << "Timestamp,TotalPulses,Energy_kWh,Power_kW,Bill_Currency,Status\n";
            outFile.close();
            headerWritten = true;
        }
    } else {
        headerWritten = true;
    }
}

void DataLogger::log(uint64_t value) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        outFile << "Pulse count: " << value << std::endl;
        outFile.close();
    }
}

void DataLogger::logMeasurement(const std::string& timestamp, 
                                uint64_t totalPulses, 
                                double energyKWh, 
                                double powerKW, 
                                double billAmount, 
                                const std::string& status) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        outFile << std::fixed << std::setprecision(4);
        outFile << timestamp << ","
                << totalPulses << ","
                << energyKWh << ","
                << powerKW << ","
                << billAmount << ","
                << status << "\n";
        outFile.close();
    }
}

void DataLogger::logEvent(const std::string& timestamp, const std::string& eventType, const std::string& message) {
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream outFile(filename, std::ios::app);
    if (outFile.is_open()) {
        outFile << "# EVENT [" << timestamp << "] " << eventType << ": " << message << "\n";
        outFile.close();
    }
}

std::string DataLogger::getFilename() const {
    return filename;
}
