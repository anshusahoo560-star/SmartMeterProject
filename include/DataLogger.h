#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <string>
#include <mutex>
#include <cstdint>

class DataLogger {
public:
    DataLogger(const std::string& filename = "meter_log.csv");

    // Basic log method (kept for backward compatibility)
    void log(uint64_t value);

    // Structured CSV logging of smart meter metrics
    void logMeasurement(const std::string& timestamp, 
                        uint64_t totalPulses, 
                        double energyKWh, 
                        double powerKW, 
                        double billAmount, 
                        const std::string& status = "NORMAL");

    // Log explicit alerts/events
    void logEvent(const std::string& timestamp, const std::string& eventType, const std::string& message);

    std::string getFilename() const;

private:
    std::string filename;
    std::mutex fileMutex;
    bool headerWritten;

    void ensureHeader();
};

#endif // DATALOGGER_H
