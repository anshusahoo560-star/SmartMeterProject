#include "EnergyCalculator.h"

EnergyCalculator::EnergyCalculator(double constMeter, double rate)
    : meterConstant(constMeter), tariffRate(rate) {}

double EnergyCalculator::calculateEnergyKWh(uint64_t pulses) const {
    if (meterConstant <= 0.0) return 0.0;
    return static_cast<double>(pulses) / meterConstant;
}

double EnergyCalculator::calculatePowerKW(uint64_t deltaPulses, double deltaSeconds) const {
    if (meterConstant <= 0.0 || deltaSeconds <= 0.0) return 0.0;
    // Power (kW) = (3600 * deltaPulses) / (deltaSeconds * meterConstant)
    return (3600.0 * static_cast<double>(deltaPulses)) / (deltaSeconds * meterConstant);
}

double EnergyCalculator::calculateCost(double energyKWh) const {
    return energyKWh * tariffRate;
}

double EnergyCalculator::getMeterConstant() const {
    return meterConstant;
}

void EnergyCalculator::setMeterConstant(double constant) {
    if (constant > 0.0) meterConstant = constant;
}

double EnergyCalculator::getTariffRate() const {
    return tariffRate;
}

void EnergyCalculator::setTariffRate(double rate) {
    if (rate >= 0.0) tariffRate = rate;
}
