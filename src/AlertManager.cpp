#include "AlertManager.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

AlertManager::AlertManager(double maxAllowed, double surge)
    : maxAllowedPowerKW(maxAllowed), surgeThresholdKW(surge) {}

std::string AlertManager::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AlertManager::severityToString(AlertSeverity sev) {
    switch (sev) {
        case AlertSeverity::INFO: return "INFO";
        case AlertSeverity::WARNING: return "WARNING";
        case AlertSeverity::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::vector<AlertRecord> AlertManager::evaluate(double currentPowerKW, double previousPowerKW) {
    std::lock_guard<std::mutex> lock(alertMutex);
    std::vector<AlertRecord> newAlerts;

    // Check for Over-Consumption / Overload
    if (currentPowerKW > maxAllowedPowerKW) {
        AlertRecord rec;
        rec.timestamp = getCurrentTimestamp();
        rec.severity = AlertSeverity::CRITICAL;
        rec.currentPowerKW = currentPowerKW;
        std::stringstream ss;
        ss << "OVERLOAD DETECTED: Instantaneous load (" << currentPowerKW 
           << " kW) exceeds maximum threshold (" << maxAllowedPowerKW << " kW)!";
        rec.message = ss.str();
        newAlerts.push_back(rec);
        history.push_back(rec);
    }

    // Check for Sudden Surge
    if (previousPowerKW >= 0.0 && (currentPowerKW - previousPowerKW) >= surgeThresholdKW) {
        AlertRecord rec;
        rec.timestamp = getCurrentTimestamp();
        rec.severity = AlertSeverity::WARNING;
        rec.currentPowerKW = currentPowerKW;
        std::stringstream ss;
        ss << "SURGE DETECTED: Sudden power jump of +" << (currentPowerKW - previousPowerKW) 
           << " kW detected!";
        rec.message = ss.str();
        newAlerts.push_back(rec);
        history.push_back(rec);
    }

    return newAlerts;
}

void AlertManager::triggerTamperAlert(const std::string& details) {
    std::lock_guard<std::mutex> lock(alertMutex);
    AlertRecord rec;
    rec.timestamp = getCurrentTimestamp();
    rec.severity = AlertSeverity::CRITICAL;
    rec.currentPowerKW = 0.0;
    rec.message = "TAMPER ALERT: " + details;
    history.push_back(rec);
    std::cerr << "\n[!] " << rec.message << "\n" << std::endl;
}

std::vector<AlertRecord> AlertManager::getAlertHistory() const {
    std::lock_guard<std::mutex> lock(alertMutex);
    return history;
}

void AlertManager::setMaxAllowedPower(double kw) {
    std::lock_guard<std::mutex> lock(alertMutex);
    maxAllowedPowerKW = kw;
}

void AlertManager::setSurgeThreshold(double kw) {
    std::lock_guard<std::mutex> lock(alertMutex);
    surgeThresholdKW = kw;
}
