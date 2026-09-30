#include "PulseSimulator.h"
#include <iostream>
#include <algorithm>

PulseSimulator::PulseSimulator(PulseCounter& cnt, double constMeter, double accel)
    : counter(cnt), meterConstant(constMeter), timeAcceleration(accel), currentIntervalMs(1125.0), running(false) {}

PulseSimulator::~PulseSimulator() {
    stop();
}

void PulseSimulator::start(double initialLoadKW) {
    setLoad(initialLoadKW);
    if (running.load()) return;
    running.store(true);
    workerThread = std::thread(&PulseSimulator::runSimulation, this);
}

void PulseSimulator::startFixedInterval(double intervalSeconds) {
    setIntervalSeconds(intervalSeconds);
    if (running.load()) return;
    running.store(true);
    workerThread = std::thread(&PulseSimulator::runSimulation, this);
}

void PulseSimulator::setLoad(double loadKW) {
    double safeLoad = std::max(0.001, loadKW);
    // interval (ms) = (3600 * 1000) / (loadKW * meterConstant)
    double intervalMs = (3600.0 * 1000.0) / (safeLoad * meterConstant);
    currentIntervalMs.store(intervalMs);
}

void PulseSimulator::setIntervalSeconds(double intervalSec) {
    double safeSec = std::max(0.005, intervalSec);
    currentIntervalMs.store(safeSec * 1000.0);
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
    double intervalMs = currentIntervalMs.load();
    if (intervalMs <= 0.0) return 0.0;
    return (3600.0 * 1000.0) / (intervalMs * meterConstant);
}

double PulseSimulator::getCurrentIntervalSeconds() const {
    return currentIntervalMs.load() / 1000.0;
}

void PulseSimulator::runSimulation() {
    while (running.load()) {
        double intervalMs = currentIntervalMs.load() / timeAcceleration;
        if (intervalMs < 5.0) intervalMs = 5.0; // clamp to 5ms min

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>(intervalMs)));

        if (running.load()) {
            counter.increment(1);
        }
    }
}
