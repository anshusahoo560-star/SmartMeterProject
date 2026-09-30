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
    // Meter constant: 1000 imp/kWh, Tariff: 8.00 / kWh
    EnergyCalculator ec(1000.0, 8.00);

    // 2500 pulses = 2.5 kWh
    double energy = ec.calculateEnergyKWh(2500);
    assert(std::fabs(energy - 2.5) < 0.0001);

    // Power test: 1000 pulses in 3600 seconds = 1.0 kW
    double power = ec.calculatePowerKW(1000, 3600.0);
    assert(std::fabs(power - 1.0) < 0.0001);

    // Cost test: 2.5 kWh * 8.00 = 20.00
    double cost = ec.calculateCost(energy);
    assert(std::fabs(cost - 20.0) < 0.0001);

    std::cout << "PASSED!\n";
}

void testAlertManager() {
    std::cout << "[TEST] Running AlertManager tests... ";
    // Overload threshold: 5.0 kW, Surge threshold: 2.0 kW
    AlertManager am(5.0, 2.0);

    // Normal power, no surge
    auto alerts = am.evaluate(2.0, 1.8);
    assert(alerts.empty());

    // Power surge from 2.0 kW to 4.5 kW (+2.5 kW surge)
    alerts = am.evaluate(4.5, 2.0);
    assert(alerts.size() == 1);
    assert(alerts[0].severity == AlertSeverity::WARNING);

    // Overload: 6.0 kW (exceeds 5.0 kW threshold)
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
    std::cout << "\n=====================================\n";
    std::cout << "   SMART METER SUITE: UNIT TESTS     \n";
    std::cout << "=====================================\n";

    testPulseCounter();
    testEnergyCalculator();
    testAlertManager();
    testAnalyticsAgent();

    std::cout << "\nAll 4 Test Suites Passed Successfully! (100% OK)\n\n";
    return 0;
}
