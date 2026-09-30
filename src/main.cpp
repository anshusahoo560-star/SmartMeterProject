#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>
#include <sstream>

#include "PulseCounter.h"
#include "PulseSimulator.h"
#include "EnergyCalculator.h"
#include "DataLogger.h"
#include "AlertManager.h"
#include "Analytics.h"

// Global flag for Linux POSIX Signal Handling (SIGINT / SIGTERM)
static std::atomic<bool> g_systemRunning(true);

void signalHandler(int signum) {
    std::cout << "\n[System Signal] Received signal (" << signum 
              << "). Initiating graceful shutdown of Smart Meter...\n";
    g_systemRunning.store(false);
}

std::string getFormattedTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int main(int argc, char* argv[]) {
    // 1. Register Linux POSIX Signal Handlers (Ctrl+C graceful exit)
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::cout << "========================================================\n";
    std::cout << "  SMART ENERGY SMART METER - PULSE COUNTER & ANALYTICS  \n";
    std::cout << "  Linux System Programming Engine (Threads/Signals/Logs)\n";
    std::cout << "========================================================\n";

    // 2. Initialize Smart Meter Components
    const double METER_CONSTANT = 1000.0; // 1000 pulses = 1 kWh
    const double TARIFF_RATE = 7.50;      // Cost per kWh
    const double SIM_ACCELERATION = 20.0;  // 20x accelerated for realistic live demo

    PulseCounter counter;
    PulseSimulator simulator(counter, METER_CONSTANT, SIM_ACCELERATION);
    EnergyCalculator calculator(METER_CONSTANT, TARIFF_RATE);
    DataLogger logger("meter_log.csv");
    AlertManager alertManager(5.0, 2.0); // 5.0 kW overload threshold, 2.0 kW surge
    AnalyticsAgent analytics;

    std::cout << "[Config] Meter Constant : " << METER_CONSTANT << " imp/kWh\n";
    std::cout << "[Config] Tariff Rate    : " << TARIFF_RATE << " / kWh\n";
    std::cout << "[Config] Log File       : " << logger.getFilename() << "\n";
    std::cout << "[Status] Press Ctrl+C at any time to halt and generate report.\n\n";

    // 3. Start Pulse Simulator Thread (simulating 2.0 kW household load)
    simulator.start(2.0);

    double previousPowerKW = 0.0;
    auto lastSampleTime = std::chrono::steady_clock::now();
    int cycleCount = 0;

    // Optional duration limit if run with --demo flag (e.g. 15 cycles)
    bool demoMode = (argc > 1 && std::string(argv[1]) == "--demo");
    const int maxDemoCycles = 15;

    std::cout << std::left 
              << std::setw(20) << "Timestamp"
              << std::setw(12) << "Pulses"
              << std::setw(14) << "Energy(kWh)"
              << std::setw(14) << "Power(kW)"
              << std::setw(12) << "Bill"
              << "Status" << "\n";
    std::cout << std::string(78, '-') << "\n";

    // 4. Real-time Monitoring & Analytics Agent Loop
    while (g_systemRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (!g_systemRunning.load()) break;

        cycleCount++;

        // For dynamic simulation demonstration:
        // At cycle 6, simulate heavy appliance turned on (surge to 6.2 kW - Overload!)
        if (cycleCount == 6) {
            std::cout << "\n>>> [SIMULATION EVENT] Heavy Industrial Appliance Activated (Surge to 6.2 kW) <<<\n\n";
            simulator.setLoad(6.2);
        }
        // At cycle 11, simulate appliance turned off back to normal (1.8 kW)
        else if (cycleCount == 11) {
            std::cout << "\n>>> [SIMULATION EVENT] Load normalized back to 1.8 kW <<<\n\n";
            simulator.setLoad(1.8);
        }

        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastSampleTime;
        double deltaSeconds = elapsed.count();
        lastSampleTime = currentTime;

        // Retrieve thread-safe pulse counts
        uint64_t deltaPulses = counter.getAndResetDelta();
        uint64_t totalPulses = counter.getCount();

        // Calculate Energy and Instantaneous Power
        // Note: adjust for simulation acceleration so displayed kW matches simulated load
        double instantaneousPowerKW = calculator.calculatePowerKW(deltaPulses, deltaSeconds) / SIM_ACCELERATION;
        double totalEnergyKWh = calculator.calculateEnergyKWh(totalPulses) / SIM_ACCELERATION;
        double currentBill = calculator.calculateCost(totalEnergyKWh);

        std::string timestamp = getFormattedTimestamp();

        // Evaluate with AlertManager
        std::vector<AlertRecord> alerts = alertManager.evaluate(instantaneousPowerKW, previousPowerKW);
        previousPowerKW = instantaneousPowerKW;

        std::string status = "NORMAL";
        if (!alerts.empty()) {
            status = AlertManager::severityToString(alerts.back().severity) + " ALERT";
        }

        // Record in Analytics Agent and DataLogger
        analytics.recordSample(timestamp, instantaneousPowerKW, totalEnergyKWh);
        logger.logMeasurement(timestamp, totalPulses, totalEnergyKWh, instantaneousPowerKW, currentBill, status);

        // Display Real-time Dashboard Row
        std::cout << std::left 
                  << std::setw(20) << timestamp
                  << std::setw(12) << totalPulses
                  << std::setw(14) << std::fixed << std::setprecision(4) << totalEnergyKWh
                  << std::setw(14) << std::fixed << std::setprecision(2) << instantaneousPowerKW
                  << std::setw(12) << std::fixed << std::setprecision(2) << currentBill
                  << status << "\n";

        // Print alert messages if any
        for (const auto& alert : alerts) {
            std::cout << "  ==> [ALERT " << AlertManager::severityToString(alert.severity) 
                      << "] " << alert.message << "\n";
            logger.logEvent(timestamp, AlertManager::severityToString(alert.severity), alert.message);
        }

        // In demo mode, terminate automatically after demo cycles
        if (demoMode && cycleCount >= maxDemoCycles) {
            std::cout << "\n[Demo Mode] Completed " << maxDemoCycles << " monitoring cycles.\n";
            break;
        }
    }

    // 5. Graceful Teardown
    std::cout << "\nStopping Pulse Simulator background thread...";
    simulator.stop();
    std::cout << " [DONE]\n";

    // 6. Generate and Print Analytics Summary Report
    double finalEnergyKWh = calculator.calculateEnergyKWh(counter.getCount()) / SIM_ACCELERATION;
    double finalCost = calculator.calculateCost(finalEnergyKWh);
    std::string summary = analytics.generateSummaryReport(finalCost, "$");
    std::cout << summary << "\n";

    std::cout << "Data logged successfully to: " << logger.getFilename() << "\n";
    std::cout << "Alerts logged: " << alertManager.getAlertHistory().size() << "\n";
    std::cout << "Smart Meter Engine stopped safely.\n";

    return 0;
}
