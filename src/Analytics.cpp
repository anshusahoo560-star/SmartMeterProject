#include "Analytics.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

AnalyticsAgent::AnalyticsAgent() 
    : peakPowerKW(0.0), peakTimestamp("N/A"), minPowerKW(1e9), sumPowerKW(0.0) {}

void AnalyticsAgent::recordSample(const std::string& timestamp, double powerKW, double energyKWh) {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    ConsumptionSample sample{timestamp, powerKW, energyKWh};
    samples.push_back(sample);

    sumPowerKW += powerKW;

    if (powerKW > peakPowerKW) {
        peakPowerKW = powerKW;
        peakTimestamp = timestamp;
    }

    if (powerKW < minPowerKW) {
        minPowerKW = powerKW;
    }
}

double AnalyticsAgent::getPeakPowerKW() const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    return peakPowerKW;
}

std::string AnalyticsAgent::getPeakPowerTimestamp() const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    return peakTimestamp;
}

double AnalyticsAgent::getAveragePowerKW() const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    if (samples.empty()) return 0.0;
    return sumPowerKW / static_cast<double>(samples.size());
}

double AnalyticsAgent::getMinPowerKW() const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    if (samples.empty()) return 0.0;
    return minPowerKW;
}

size_t AnalyticsAgent::getTotalSamplesCount() const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    return samples.size();
}

void AnalyticsAgent::reset() {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    samples.clear();
    peakPowerKW = 0.0;
    peakTimestamp = "N/A";
    minPowerKW = 1e9;
    sumPowerKW = 0.0;
}

std::string AnalyticsAgent::generateSummaryReport(double totalCost, const std::string& currencySymbol) const {
    std::lock_guard<std::mutex> lock(analyticsMutex);
    std::stringstream ss;
    
    double lastEnergy = samples.empty() ? 0.0 : samples.back().energyKWh;
    double avgPower = samples.empty() ? 0.0 : (sumPowerKW / samples.size());
    double minPower = samples.empty() ? 0.0 : minPowerKW;

    ss << "\n=======================================================\n";
    ss << "          SMART METER ANALYTICS SUMMARY REPORT         \n";
    ss << "=======================================================\n";
    ss << std::fixed << std::setprecision(3);
    ss << " Total Samples Logged   : " << samples.size() << "\n";
    ss << " Total Energy Consumed  : " << lastEnergy << " kWh\n";
    ss << " Estimated Bill Amount  : " << currencySymbol << totalCost << "\n";
    ss << " Peak Active Power      : " << peakPowerKW << " kW (at " << peakTimestamp << ")\n";
    ss << " Average Active Power   : " << avgPower << " kW\n";
    ss << " Minimum Active Power   : " << minPower << " kW\n";
    ss << "=======================================================\n";

    return ss.str();
}
