#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <vector>
#include <string>
#include <mutex>
#include <cstdint>

struct ConsumptionSample {
    std::string timestamp;
    double powerKW;
    double energyKWh;
};

class AnalyticsAgent {
public:
    AnalyticsAgent();

    // Record a new periodic measurement sample
    void recordSample(const std::string& timestamp, double powerKW, double energyKWh);

    // Statistical queries
    double getPeakPowerKW() const;
    std::string getPeakPowerTimestamp() const;
    double getAveragePowerKW() const;
    double getMinPowerKW() const;
    size_t getTotalSamplesCount() const;

    // Reset analytics session
    void reset();

    // Generate formatted analytical summary
    std::string generateSummaryReport(double totalCost, const std::string& currencySymbol = "$") const;

private:
    mutable std::mutex analyticsMutex;
    std::vector<ConsumptionSample> samples;
    double peakPowerKW;
    std::string peakTimestamp;
    double minPowerKW;
    double sumPowerKW;
};

#endif // ANALYTICS_H
