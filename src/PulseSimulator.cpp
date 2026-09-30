#include "PulseSimulator.h"
#include <iostream>
#include <algorithm>

PulseSimulator::PulseSimulator(PulseCounter& cnt, double constMeter, double accel)
    : counter(cnt), meterConstant(constMeter), timeAcceleration(accel), currentLoadKW(0.0), running(false) {}

PulseSimulator::~PulseSimulator() {
    stop();
}

void PulseSimulator::start(double initialLoadKW) {
    if (running.load()) return;
    currentLoadKW.store(initialLoadKW);
    running.store(true);
    workerThread = std::thread(&PulseSimulator::runSimulation, this);
}

void PulseSimulator::setLoad(double loadKW) {
    currentLoadKW.store(std::max(0.0, loadKW));
}

void PulseSimulator::stop() {
    if (running.load()) {
        running.store(false);
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
}

void PulseSimulator::injectPulses(uint64_t count) {
    counter.increment(count);
}

bool PulseSimulator::isRunning() const {
    return running.load();
}

double PulseSimulator::getCurrentLoadKW() const {
    return currentLoadKW.load();
}

void PulseSimulator::runSimulation() {
    while (running.load()) {
        double load = currentLoadKW.load();
        if (load <= 0.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // Pulses per hour = load (kW) * meterConstant (imp/kWh)
        // Pulses per second = (load * meterConstant) / 3600
        // Interval between pulses in ms = (3600 * 1000) / (load * meterConstant * acceleration)
        double intervalMs = (3600.0 * 1000.0) / (load * meterConstant * timeAcceleration);
        
        // Clamp minimum interval to 10 ms to prevent CPU starvation
        if (intervalMs < 10.0) intervalMs = 10.0;

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(intervalMs)));

        if (running.load()) {
            counter.increment(1);
        }
    }
}
