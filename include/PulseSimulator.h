#ifndef PULSESIMULATOR_H
#define PULSESIMULATOR_H

#include "PulseCounter.h"
#include <thread>
#include <atomic>
#include <chrono>

class PulseSimulator {
public:
    // meterConstant: typically 1000 or 3200 pulses (imp) per kWh
    // timeAcceleration: speeds up simulation for demonstration (e.g. 10x or 60x faster)
    PulseSimulator(PulseCounter& counter, double meterConstant = 1000.0, double timeAcceleration = 1.0);
    ~PulseSimulator();

    // Start background simulation thread at a given load (in kW)
    void start(double initialLoadKW = 2.0);

    // Change current simulated electrical load
    void setLoad(double loadKW);

    // Stop background simulation thread
    void stop();

    // Inject manual pulses directly (for unit testing or event trigger)
    void injectPulses(uint64_t count);

    bool isRunning() const;
    double getCurrentLoadKW() const;

private:
    void runSimulation();

    PulseCounter& counter;
    double meterConstant;
    double timeAcceleration;
    std::atomic<double> currentLoadKW;
    std::atomic<bool> running;
    std::thread workerThread;
};

#endif // PULSESIMULATOR_H
