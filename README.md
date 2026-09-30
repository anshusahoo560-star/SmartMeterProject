# Smart Energy Smart Meter – Pulse Counter & Analytics Agents

[![Build & Tests](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20WSL-orange.svg)]()
[![License](https://img.shields.io/badge/license-MIT-green.svg)]()

A Linux-based smart energy meter simulation and real-time energy analytics system developed in modern C++ (C++17). The project integrates multi-threading, POSIX signal handling, real-time pulse processing, energy consumption calculations, automated alert management, and analytics reporting.

---

## 📋 Table of Contents
1. [Project Overview & Problem Statement](#project-overview--problem-statement)
2. [Objectives & Scope](#objectives--scope)
3. [Key Features](#key-features)
4. [System Architecture](#system-architecture)
5. [UML Diagrams](#uml-diagrams)
   - [Class Diagram](#class-diagram)
   - [Sequence Diagram](#sequence-diagram)
   - [State Machine Diagram](#state-machine-diagram)
6. [Linux System Programming Concepts](#linux-system-programming-concepts)
7. [Directory Structure](#directory-structure)
8. [Build and Execution Guide](#build-and-execution-guide)
9. [Test Suite & Verification](#test-suite--verification)
10. [Deliverables Checklist](#deliverables-checklist)

---

## ⚡ Project Overview & Problem Statement

### Problem Statement
Traditional energy meters require manual meter readings, lack real-time visibility into instantaneous power surges, provide no immediate warning mechanisms during dangerous overload conditions, and cannot detect anomalies or power tampering at the edge.

### Solution
This project implements a software-defined **Smart Energy Smart Meter** running on Linux that:
1. Simulates and counts optical high-frequency pulses from an electric meter LED (e.g., $1000\text{ imp/kWh}$).
2. Calculates instantaneous active power ($\text{kW}$), cumulative energy ($\text{kWh}$), and real-time utility bills.
3. Evaluates power thresholds using an **Alert Manager** to flag overload surges and tampering.
4. Executes continuous statistical tracking through an **Analytics Agent** (peak load, average load, anomaly timestamps).
5. Persists time-stamped metrics to a persistent CSV log (`meter_log.csv`).
6. Uses Linux system programming (multi-threading, POSIX signals for graceful teardown, and character device concepts).

---

## 🎯 Objectives & Scope

- **Real-Time Pulse Acquisition:** Capture hardware-generated or simulated pulses concurrently without losing count using thread-safe primitives.
- **Accurate Energy Metering:** Convert raw pulses to kilowatt-hours ($\text{kWh}$) and active power ($\text{kW}$) using configurable meter constants.
- **Edge Analytics & Safety Alerts:** Detect and warn about over-consumption ($>5.0\text{ kW}$) or sudden surges within seconds.
- **Robust Linux Architecture:** Clean multi-threaded architecture with POSIX signal handlers (`SIGINT`, `SIGTERM`) for safe shutdown and log flushing.

---

## 🛠️ Key Features

- **Thread-Safe Pulse Counter:** Lockless atomic counter (`std::atomic<uint64_t>`) with delta tracking for high-frequency pulse inputs.
- **Configurable Pulse Simulator:** Simulates real-world household and industrial loads with speedup acceleration for rapid demonstration.
- **Energy & Billing Calculator:** Real-time computation of power, energy consumption, and tiered billing costs.
- **Real-time Alert Manager:** Threshold monitoring detecting overload conditions and abnormal load jumps.
- **Analytics Agent:** Computes minimum, maximum/peak, and session averages, generating structured summary reports.
- **Structured File Logger:** Appends formatted CSV metrics and event logs with microsecond/second precision.
- **Linux Character Device Driver Concept:** Includes `driver/pulse_driver.c` demonstrating kernel-to-userspace character device architecture (`/dev/smart_meter_pulse`).

---

## 🏗️ System Architecture

```mermaid
flowchart TD
    subgraph Hardware_or_Simulation ["Input Layer"]
        PS["Pulse Simulator Thread\n(Simulates load in kW)"]
        KD["Linux Kernel Driver Concept\n(/dev/smart_meter_pulse)"]
    end

    subgraph Core_Engine ["Smart Meter Core Engine"]
        PC["Pulse Counter\n(std::atomic<uint64_t>)"]
        EC["Energy Calculator\n(kWh, kW, Cost)"]
    end

    subgraph Analytics_Layer ["Analytics & Safety Layer"]
        AA["Analytics Agent\n(Peak, Average, Trends)"]
        AM["Alert Manager\n(Threshold & Surge Check)"]
    end

    subgraph Output_Layer ["Output & Persistence"]
        DL["Data Logger\n(meter_log.csv)"]
        CLI["Terminal Dashboard\n& Summary Report"]
    end

    PS -->|Increments| PC
    KD -.->|Character read| PC
    PC -->|Delta Pulses| EC
    EC -->|Power & Energy| AA
    EC -->|Power & Energy| AM
    AM -->|Alert Events| DL
    AA -->|Metrics| DL
    AA -->|Summary| CLI
    AM -->|Live Warnings| CLI
```

---

## 📊 UML Diagrams

### Class Diagram
```mermaid
classDiagram
    class PulseCounter {
        -std::atomic~uint64_t~ totalPulses
        -std::atomic~uint64_t~ deltaPulses
        +PulseCounter()
        +increment(uint64_t count) void
        +getCount() uint64_t
        +reset() void
        +getAndResetDelta() uint64_t
    }

    class PulseSimulator {
        -PulseCounter& counter
        -double meterConstant
        -double timeAcceleration
        -std::atomic~double~ currentLoadKW
        -std::atomic~bool~ running
        -std::thread workerThread
        +PulseSimulator(counter, meterConstant, accel)
        +start(double initialLoadKW) void
        +setLoad(double loadKW) void
        +stop() void
        +injectPulses(uint64_t count) void
    }

    class EnergyCalculator {
        -double meterConstant
        -double tariffRate
        +EnergyCalculator(meterConstant, tariffRate)
        +calculateEnergyKWh(uint64_t pulses) double
        +calculatePowerKW(uint64_t deltaPulses, double deltaSeconds) double
        +calculateCost(double energyKWh) double
    }

    class AlertManager {
        -double maxAllowedPowerKW
        -double surgeThresholdKW
        -std::vector~AlertRecord~ history
        +evaluate(double currentPower, double prevPower) vector~AlertRecord~
        +triggerTamperAlert(string details) void
        +getAlertHistory() vector~AlertRecord~
    }

    class AnalyticsAgent {
        -std::vector~ConsumptionSample~ samples
        -double peakPowerKW
        -double minPowerKW
        -double sumPowerKW
        +recordSample(timestamp, powerKW, energyKWh) void
        +getPeakPowerKW() double
        +getAveragePowerKW() double
        +generateSummaryReport(cost, currency) string
    }

    class DataLogger {
        -std::string filename
        -std::mutex fileMutex
        +logMeasurement(timestamp, pulses, kWh, kW, bill, status) void
        +logEvent(timestamp, eventType, message) void
    }

    PulseSimulator --> PulseCounter : updates
    PulseCounter <.. EnergyCalculator : queries pulses
    EnergyCalculator --> AnalyticsAgent : supplies metrics
    EnergyCalculator --> AlertManager : supplies power data
    AnalyticsAgent --> DataLogger : logs samples
    AlertManager --> DataLogger : logs alerts
```

### Sequence Diagram
```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Main as Main Program
    participant Sim as PulseSimulator
    participant Ctr as PulseCounter
    participant Calc as EnergyCalculator
    participant Alert as AlertManager
    participant Agent as AnalyticsAgent
    participant Log as DataLogger

    User->>Main: Launch (./bin/SmartMeter)
    Main->>Sim: start(2.0 kW)
    loop Background Pulse Generation
        Sim->>Ctr: increment(1)
    end

    loop Every 1 Second Monitoring
        Main->>Ctr: getAndResetDelta()
        Ctr-->>Main: deltaPulses
        Main->>Calc: calculatePowerKW(deltaPulses, dt)
        Calc-->>Main: currentPowerKW
        Main->>Calc: calculateEnergyKWh(totalPulses)
        Calc-->>Main: energyKWh
        Main->>Alert: evaluate(currentPower, prevPower)
        alt Threshold Exceeded
            Alert-->>Main: AlertRecord (CRITICAL/WARNING)
            Main->>Log: logEvent(Alert)
            Main->>User: Display Console Warning
        end
        Main->>Agent: recordSample(power, energy)
        Main->>Log: logMeasurement(...)
        Main->>User: Print Dashboard Row
    end

    User->>Main: Press Ctrl+C (SIGINT)
    Main->>Sim: stop()
    Main->>Agent: generateSummaryReport()
    Agent-->>User: Print Final Analytics Report
```

### State Machine Diagram
```mermaid
stateDiagram-v2
    [*] --> INITIALIZING : System Boot
    INITIALIZING --> MONITORING : Threads & Devices Started
    
    state MONITORING {
        [*] --> NORMAL_CONSUMPTION
        NORMAL_CONSUMPTION --> SURGE_DETECTED : Delta Power >= Surge Threshold
        SURGE_DETECTED --> NORMAL_CONSUMPTION : Power Stabilizes
        NORMAL_CONSUMPTION --> OVERLOAD_ALERT : Power > Max Allowed Threshold
        OVERLOAD_ALERT --> NORMAL_CONSUMPTION : Load Reduced Below Threshold
    }

    MONITORING --> SHUTTING_DOWN : SIGINT (Ctrl+C) / SIGTERM
    SHUTTING_DOWN --> FINAL_REPORT : Flush Logs & Stop Threads
    FINAL_REPORT --> [*] : Safe Exit (Code 0)
```

---

## 🐧 Linux System Programming Concepts

1. **Multithreading (`std::thread`, `pthreads`):** Concurrent producer-consumer pattern. The simulation thread generates pulses asynchronously while the main thread performs periodic calculations and logging.
2. **POSIX Signals (`signal(SIGINT)`, `signal(SIGTERM)`):** Safe signal interception ensures file handles are flushed and background threads are joined before process exit.
3. **Atomic Operations (`std::atomic`):** Lockless, atomic pulse accumulation avoiding race conditions across threads.
4. **Linux Character Device Driver Concept (`driver/pulse_driver.c`):** Demonstrates registering `/dev/smart_meter_pulse` using `alloc_chrdev_region`, `cdev_add`, and standard `file_operations` (`open`, `read`, `write`, `release`).

---

## 📁 Directory Structure

```text
SmartMeterProject/
├── .gitignore               # Ignores build/, bin/, logs
├── CMakeLists.txt           # Modern CMake configuration (builds app & tests)
├── README.md                # Comprehensive documentation and UML
├── bin/                     # Generated executables (SmartMeter, TestSmartMeter)
├── build/                   # CMake build directory
├── driver/                  # Linux Kernel Device Driver concept
│   ├── Kbuild               # Kernel build descriptor
│   ├── Makefile             # Kernel module compilation Makefile
│   └── pulse_driver.c       # Character device driver implementation
├── include/                 # Header files
│   ├── AlertManager.h
│   ├── Analytics.h
│   ├── DataLogger.h
│   ├── EnergyCalculator.h
│   ├── PulseCounter.h
│   └── PulseSimulator.h
├── src/                     # C++ Source implementations
│   ├── AlertManager.cpp
│   ├── Analytics.cpp
│   ├── DataLogger.cpp
│   ├── EnergyCalculator.cpp
│   ├── PulseCounter.cpp
│   ├── PulseSimulator.cpp
│   └── main.cpp
└── tests/                   # Automated Unit Tests
    └── test_meter.cpp
```

---

## 🚀 Build and Execution Guide

### Prerequisites
- GCC / G++ (v8.0+ supporting C++17)
- CMake (v3.10+)
- Linux (Ubuntu/Debian) or WSL 2 (Windows Subsystem for Linux)
- Make

### 1. Build the Project
In your Linux / WSL terminal:
```bash
# Configure build with CMake
cmake -B build

# Compile all executables
cmake --build build
```

### 2. Run the Main Smart Meter
To run the interactive continuous monitoring (press `Ctrl+C` to stop):
```bash
./bin/SmartMeter
```

To run the automated **15-cycle demo mode** (includes load surge & overload events):
```bash
./bin/SmartMeter --demo
```

### 3. Inspect Logged Data
Review real-time CSV logged measurements:
```bash
cat meter_log.csv
```

---

## 🧪 Test Suite & Verification

Run the automated test runner:
```bash
./bin/TestSmartMeter
```

### Output:
```text
=====================================
   SMART METER SUITE: UNIT TESTS     
=====================================
[TEST] Running PulseCounter tests... PASSED!
[TEST] Running EnergyCalculator tests... PASSED!
[TEST] Running AlertManager tests... PASSED!
[TEST] Running AnalyticsAgent tests... PASSED!

All 4 Test Suites Passed Successfully! (100% OK)
```

---

## ✅ Deliverables Checklist (Stages 1 – 6)

- [x] **Working C++ Project:** Clean C++17 modular architecture.
- [x] **Linux Implementation:** Runs natively on Linux and WSL.
- [x] **g++ Compilation:** Successfully builds via CMake + g++.
- [x] **Pulse Simulator:** Simulates real-time electrical pulses with configurable power levels.
- [x] **Pulse Counter:** Thread-safe atomic counter with delta acquisition.
- [x] **Energy Calculator:** Computes active power ($\text{kW}$), energy ($\text{kWh}$), and tariffs.
- [x] **Data Logger:** Persists formatted CSV logs and event histories.
- [x] **Analytics Agent:** Computes peak power, average consumption, and analytical summary.
- [x] **Alert System:** Detects overloads, surges, and alerts.
- [x] **Linux System Programming:** Multithreading, POSIX signals, atomic memory ordering, and character driver concept.
- [x] **UML Diagrams:** Class, Sequence, and State Machine diagrams included.
- [x] **Unit Tests:** Comprehensive automated test suite passing 100%.
- [x] **Clean GitHub Repository:** Configured `.gitignore` and clean tree.
