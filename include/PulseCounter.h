#ifndef PULSECOUNTER_H
#define PULSECOUNTER_H

#include <cstdint>
#include <atomic>
#include <mutex>

class PulseCounter {
public:
    PulseCounter();

    // Increment pulse count (thread-safe)
    void increment(uint64_t count = 1);

    // Get cumulative pulse count
    uint64_t getCount() const;

    // Reset the counter
    void reset();

    // Get pulses accumulated since last call to this function
    uint64_t getAndResetDelta();

private:
    std::atomic<uint64_t> totalPulses;
    std::atomic<uint64_t> deltaPulses;
};

#endif // PULSECOUNTER_H
