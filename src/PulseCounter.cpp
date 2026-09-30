#include "PulseCounter.h"

PulseCounter::PulseCounter() : totalPulses(0), deltaPulses(0) {}

void PulseCounter::increment(uint64_t count) {
    totalPulses.fetch_add(count, std::memory_order_relaxed);
    deltaPulses.fetch_add(count, std::memory_order_relaxed);
}

uint64_t PulseCounter::getCount() const {
    return totalPulses.load(std::memory_order_relaxed);
}

void PulseCounter::reset() {
    totalPulses.store(0, std::memory_order_relaxed);
    deltaPulses.store(0, std::memory_order_relaxed);
}

uint64_t PulseCounter::getAndResetDelta() {
    return deltaPulses.exchange(0, std::memory_order_acq_rel);
}
