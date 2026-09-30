#ifndef PULSESIMULATOR_H
#define PULSESIMULATOR_H

#include "PulseCounter.h"
#include <thread>
#include <atomic>
#include <chrono>

class PulseSimulator {
public:
    // meterConstant: 3200 imp/kWh
    // timeAcceleration: multiplier (1.0 for real-time 1.125s, >1.0 for accelerated test)
    PulseSimulator(PulseCounter& counter, double meterConstant = 3200.0, double timeAcceleration = 1.0);
    ~PulseSimulator();

    // Start background simulation thread at a given load (in kW)
    void start(double initialLoadKW = 1.0);

    // Start background simulation thread with a fixed pulse interval (e.g. 1.125s)
    void startFixedInterval(double intervalSeconds);

    // Change current simulated electrical load in kW
    void setLoad(double loadKW);

    // Set a fixed pulse-to-pulse interval in seconds (e.g. 1.125s -> 1000 W)
    void setIntervalSeconds(double intervalSec);

    // Stop background simulation thread
    void stop();

    // Inject manual pulses directly (for unit testing or instant accumulation)
    void injectPulses(uint64_t count);

    bool isRunning() const;
    double getCurrentLoadKW() const;
    double getCurrentIntervalSeconds() const;

private:
    void runSimulation();

    PulseCounter& counter;
    double meterConstant;
    double timeAcceleration;
    std::atomic<double> currentIntervalMs;
    std::atomic<bool> running;
    std::thread workerThread;
};

#endif // PULSESIMULATOR_H
