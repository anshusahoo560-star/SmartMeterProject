#include <iostream>
#include <cassert>
#include <cmath>

#include "PulseCounter.h"
#include "EnergyCalculator.h"
#include "AlertManager.h"
#include "Analytics.h"

void testPulseCounter() {
    std::cout << "[TEST] Running PulseCounter tests... ";
    PulseCounter pc;
    assert(pc.getCount() == 0);

    pc.increment(10);
    assert(pc.getCount() == 10);

    uint64_t delta = pc.getAndResetDelta();
    assert(delta == 10);
    assert(pc.getAndResetDelta() == 0);
    assert(pc.getCount() == 10);

    pc.increment(5);
    assert(pc.getCount() == 15);
    pc.reset();
    assert(pc.getCount() == 0);

    std::cout << "PASSED!\n";
}

void testEnergyCalculator() {
    std::cout << "[TEST] Running EnergyCalculator tests... ";
    // Meter constant: 3200 imp/kWh, Tariff: 7.50 / kWh
    EnergyCalculator ec(3200.0, 7.50);

    // Basic calculation check
    double energy = ec.calculateEnergyKWh(3200);
    assert(std::fabs(energy - 1.0) < 0.0001);

    std::cout << "PASSED!\n";
}

// -------------------------------------------------------------
// REQUIRED VERIFICATION 1: Fixed 1.125s interval consistently outputs 1000 W
// -------------------------------------------------------------
void testVerifyActivePowerFixedInterval() {
    std::cout << "[VERIFY 1] Active Power: Fixed 1.125s interval -> 1000 W... ";
    const double meterConstant = 3200.0;
    EnergyCalculator ec(meterConstant, 7.50);

    const double fixedIntervalSeconds = 1.125;
    double powerWatts = ec.calculatePowerFromIntervalSeconds(fixedIntervalSeconds);

    // Expected: (3600 * 1000) / (1.125 * 3200) = 3,600,000 / 3600 = 1000 W
    assert(std::fabs(powerWatts - 1000.0) < 0.001);

    // Also check over multiple pulses: 8 pulses in 9.0 seconds (8 * 1.125 = 9.0s)
    double multiPulseWatts = ec.calculatePowerWatts(8, 9.0);
    assert(std::fabs(multiPulseWatts - 1000.0) < 0.001);

    std::cout << "PASSED! (Consistently reads " << powerWatts << " W)\n";
}

// -------------------------------------------------------------
// REQUIRED VERIFICATION 2: 320 pulses reads exactly 0.1 kWh
// -------------------------------------------------------------
void testTrackEnergyAccumulation320Pulses() {
    std::cout << "[VERIFY 2] Energy Accumulation: 320 pulses -> exactly 0.1 kWh... ";
    const double meterConstant = 3200.0;
    EnergyCalculator ec(meterConstant, 7.50);
    PulseCounter pc;

    // Simulate accumulation to exactly 320 pulses
    pc.increment(320);
    assert(pc.getCount() == 320);

    double totalEnergyKWh = ec.calculateEnergyKWh(pc.getCount());
    // 320 / 3200 = 0.100000 kWh
    assert(std::fabs(totalEnergyKWh - 0.1) < 0.00001);

    std::cout << "PASSED! (Accumulated: " << totalEnergyKWh << " kWh at 320 pulses)\n";
}

// -------------------------------------------------------------
// REQUIRED VERIFICATION 3: Drop interval -> High-power alert fires immediately
// -------------------------------------------------------------
void testAlertTriggerOnIntervalDrop() {
    std::cout << "[VERIFY 3] Alert Triggers: Drop interval -> High-power alert fires... ";
    const double meterConstant = 3200.0;
    EnergyCalculator ec(meterConstant, 7.50);
    AlertManager am(5.0, 2.0); // 5.0 kW overload threshold

    // Normal baseline at 1.125s interval (1000 W = 1.0 kW)
    double normalPowerKW = ec.calculatePowerFromIntervalSeconds(1.125) / 1000.0;
    auto alertsNormal = am.evaluate(normalPowerKW, 0.0);
    assert(alertsNormal.empty()); // No alert at 1000 W

    // Drop interval to 0.200s (200 ms) -> Power surges to (3,600,000 / (0.2 * 3200)) = 5625 W (5.625 kW)
    double droppedIntervalSec = 0.200;
    double highPowerKW = ec.calculatePowerFromIntervalSeconds(droppedIntervalSec) / 1000.0;
    assert(highPowerKW > 5.0); // 5.625 kW > 5.0 kW

    auto alertsTriggered = am.evaluate(highPowerKW, normalPowerKW);
    assert(!alertsTriggered.empty()); // Alert fired immediately!
    assert(alertsTriggered[0].severity == AlertSeverity::CRITICAL);

    std::cout << "PASSED! (Power jumped to " << highPowerKW << " kW -> Immediate Alert Triggered)\n";
}

void testAlertManager() {
    std::cout << "[TEST] Running AlertManager tests... ";
    AlertManager am(5.0, 2.0);

    auto alerts = am.evaluate(2.0, 1.8);
    assert(alerts.empty());

    alerts = am.evaluate(4.5, 2.0);
    assert(alerts.size() == 1);
    assert(alerts[0].severity == AlertSeverity::WARNING);

    alerts = am.evaluate(6.0, 4.5);
    assert(!alerts.empty());
    assert(alerts[0].severity == AlertSeverity::CRITICAL);

    std::cout << "PASSED!\n";
}

void testAnalyticsAgent() {
    std::cout << "[TEST] Running AnalyticsAgent tests... ";
    AnalyticsAgent agent;
    assert(agent.getTotalSamplesCount() == 0);

    agent.recordSample("2026-09-30 10:00:00", 2.0, 0.5);
    agent.recordSample("2026-09-30 10:01:00", 4.0, 1.0);
    agent.recordSample("2026-09-30 10:02:00", 3.0, 1.5);

    assert(agent.getTotalSamplesCount() == 3);
    assert(std::fabs(agent.getPeakPowerKW() - 4.0) < 0.001);
    assert(std::fabs(agent.getAveragePowerKW() - 3.0) < 0.001);

    std::cout << "PASSED!\n";
}

int main() {
    std::cout << "\n=======================================================\n";
    std::cout << "   SMART METER SUITE: UNIT TESTS & PARAMETER VERIFY    \n";
    std::cout << "=======================================================\n";

    testPulseCounter();
    testEnergyCalculator();
    testAlertManager();
    testAnalyticsAgent();

    std::cout << "\n--- PARAMETER VERIFICATIONS (USER CRITERIA) ---\n";
    testVerifyActivePowerFixedInterval();
    testTrackEnergyAccumulation320Pulses();
    testAlertTriggerOnIntervalDrop();

    std::cout << "\nAll Unit Tests & Parameter Verifications PASSED (100% OK)!\n\n";
    return 0;
}
