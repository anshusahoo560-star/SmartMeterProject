#ifndef ENERGYCALCULATOR_H
#define ENERGYCALCULATOR_H

#include <cstdint>

class EnergyCalculator {
public:
    // meterConstant: number of pulses per 1 kWh (calibrated to 3200 imp/kWh standard)
    // tariffRate: cost per kWh (e.g. 7.50)
    EnergyCalculator(double meterConstant = 3200.0, double tariffRate = 7.50);

    // Calculate cumulative energy consumed in Kilowatt-hours (kWh)
    // 320 pulses with 3200 imp/kWh = exactly 0.1 kWh
    double calculateEnergyKWh(uint64_t pulses) const;

    // Calculate instantaneous active power in Kilowatts (kW) given delta pulses over delta seconds
    double calculatePowerKW(uint64_t deltaPulses, double deltaSeconds) const;

    // Calculate instantaneous active power in Watts (W)
    double calculatePowerWatts(uint64_t deltaPulses, double deltaSeconds) const;

    // Calculate active power in Watts from a fixed pulse-to-pulse interval in seconds
    // At interval = 1.125s with 3200 imp/kWh -> (3600 * 1000) / (1.125 * 3200) = 1000 W
    double calculatePowerFromIntervalSeconds(double intervalSeconds) const;

    // Calculate billing cost based on tariff rate
    double calculateCost(double energyKWh) const;

    // Getters and setters
    double getMeterConstant() const;
    void setMeterConstant(double constant);

    double getTariffRate() const;
    void setTariffRate(double rate);

private:
    double meterConstant;
    double tariffRate;
};

#endif // ENERGYCALCULATOR_H
