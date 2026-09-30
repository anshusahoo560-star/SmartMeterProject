#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include <string>
#include <vector>
#include <mutex>

enum class AlertSeverity {
    INFO,
    WARNING,
    CRITICAL
};

struct AlertRecord {
    std::string timestamp;
    AlertSeverity severity;
    std::string message;
    double currentPowerKW;
};

class AlertManager {
public:
    AlertManager(double maxAllowedPowerKW = 5.0, double surgeThresholdKW = 2.5);

    // Evaluate current metrics and generate alerts if thresholds are breached
    std::vector<AlertRecord> evaluate(double currentPowerKW, double previousPowerKW);

    // Manual tamper alert trigger
    void triggerTamperAlert(const std::string& details);

    // Get all triggered alerts
    std::vector<AlertRecord> getAlertHistory() const;

    // Severity string helper
    static std::string severityToString(AlertSeverity sev);

    // Configure thresholds
    void setMaxAllowedPower(double kw);
    void setSurgeThreshold(double kw);

private:
    double maxAllowedPowerKW;
    double surgeThresholdKW;
    mutable std::mutex alertMutex;
    std::vector<AlertRecord> history;

    std::string getCurrentTimestamp() const;
};

#endif // ALERTMANAGER_H
