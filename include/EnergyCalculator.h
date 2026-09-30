#ifndef ENERGYCALCULATOR_H
#define ENERGYCALCULATOR_H

#include <cstdint>

class EnergyCalculator {
public:
    // meterConstant: number of pulses per 1 kWh (e.g. 1000 imp/kWh or 3200 imp/kWh)
    // tariffRate: cost per kWh (e.g. 7.50)
    EnergyCalculator(double meterConstant = 1000.0, double tariffRate = 7.50);

    // Calculate cumulative energy consumed in Kilowatt-hours (kWh)
    double calculateEnergyKWh(uint64_t pulses) const;

    // Calculate instantaneous active power in Kilowatts (kW) given delta pulses over delta seconds
    double calculatePowerKW(uint64_t deltaPulses, double deltaSeconds) const;

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
