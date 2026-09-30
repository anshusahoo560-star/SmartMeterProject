# Smart Energy Smart Meter – Pulse Counter & Analytics Agents
## Comprehensive Project Report (Stages 1 – 6)

---

### STAGE 1: Project Introduction

#### 1.1 Project Title
**Smart Energy Smart Meter – Pulse Counter and Analytics Agents with Linux System Programming**

#### 1.2 Problem Statement
Traditional analog and standalone electricity meters cannot report instantaneous load characteristics, lack edge intelligence to detect electrical overloads in real time, and offer no visibility into consumption anomalies. Utilities and consumers need a responsive, edge-computing smart metering system capable of processing high-frequency impulse signals, computing active power and energy, enforcing safety thresholds, and generating automated analytics summaries.

#### 1.3 Objectives
- Intercept and count high-frequency optical/electrical impulses ($1000\text{ imp/kWh}$) asynchronously and safely using atomic operations.
- Calculate real-time active power ($\text{kW}$), cumulative energy ($\text{kWh}$), and accrued electricity billing.
- Implement an edge **Alert Manager** that flags power surges ($+2.0\text{ kW}$) and overloads ($>5.0\text{ kW}$).
- Provide an **Analytics Agent** to monitor peak power, average consumption, and generate structured session reports.
- Utilize Linux system programming primitives: multi-threading (`std::thread`), POSIX signal handling (`SIGINT`/`SIGTERM`), file I/O persistence, and device driver architecture concepts.

#### 1.4 Scope
- Real-time pulse simulation and counting.
- Instantaneous power and cumulative energy computation.
- Automated safety alerts and anomaly logging.
- Thread-safe persistent CSV data logging.
- Kernel-to-userspace character device interface design concept (`/dev/smart_meter_pulse`).

#### 1.5 Expected Outcome
A complete, multi-threaded C++17 application compiled with `g++` on Linux/WSL that runs continuous energy metering, handles `Ctrl+C` graceful shutdown, passes automated unit tests, and outputs structured CSV records and visual analytics summaries.

---

### STAGE 2: Project Requirements & PRD

#### 2.1 Functional Requirements
1. **Pulse Acquisition:** Thread-safe incrementing and delta extraction of optical meter pulses.
2. **Energy & Power Math:**
   $$\text{Energy (kWh)} = \frac{\text{Pulses}}{\text{Meter Constant (imp/kWh)}}$$
   $$\text{Power (kW)} = \frac{3600 \times \Delta\text{Pulses}}{\Delta t \times \text{Meter Constant}}$$
3. **Data Persistence:** Automated logging of timestamp, pulse count, energy ($\text{kWh}$), power ($\text{kW}$), bill, and status to `meter_log.csv`.
4. **Alert System:** Real-time triggering of `CRITICAL` overload alerts ($P > 5\text{ kW}$) and `WARNING` surge alerts ($\Delta P \ge 2\text{ kW}$).
5. **Analytics Agent:** Real-time tracking of peak load timestamp, minimum power, session average power, and total cost.

#### 2.2 Non-Functional Requirements
- **Reliability:** No pulse drops or race conditions under concurrent producer-consumer threads.
- **Maintainability:** Modular OOP separation across clean `.h` header and `.cpp` implementation files.
- **Linux Compatibility:** Compiles cleanly with `g++ -std=c++17` and CMake on standard Linux and WSL environments.
- **Graceful Termination:** Immediate interception of `SIGINT` (Ctrl+C) to prevent file corruption and flush logs.

---

### STAGE 3: System Design & Architecture

#### 3.1 Architectural Layers
- **Input Layer:** `PulseSimulator` background thread simulating electrical loads; `pulse_driver.c` character device driver concept.
- **Processing Layer:** `PulseCounter` (lockless atomic), `EnergyCalculator` (conversion math).
- **Intelligence Layer:** `AnalyticsAgent` (statistical tracking), `AlertManager` (threshold evaluation).
- **Persistence & Presentation:** `DataLogger` (`meter_log.csv`), Console Dashboard with graceful shutdown summary.

---

### STAGE 4: Core Implementation & Prototype

The project was constructed with the following modular components:
1. `include/PulseCounter.h` & `src/PulseCounter.cpp`
2. `include/PulseSimulator.h` & `src/PulseSimulator.cpp`
3. `include/EnergyCalculator.h` & `src/EnergyCalculator.cpp`
4. `include/DataLogger.h` & `src/DataLogger.cpp`
5. `include/AlertManager.h` & `src/AlertManager.cpp`
6. `include/Analytics.h` & `src/Analytics.cpp`
7. `src/main.cpp` (Multithreading & POSIX Signals)
8. `driver/pulse_driver.c` (Linux Character Device Concept)

---

### STAGE 5: Testing & Verification

Automated unit tests were implemented in `tests/test_meter.cpp` covering:
- **Test 1: Pulse Counter Verification:** Tested atomic counting, delta extraction, and reset functionality.
- **Test 2: Energy Calculator Verification:** Verified $2500\text{ pulses} = 2.5\text{ kWh}$, $1000\text{ pulses in } 3600\text{s} = 1.0\text{ kW}$, and tariff billing accuracy.
- **Test 3: Alert Manager Verification:** Tested normal operation, surge trigger at $+2.5\text{ kW}$, and critical overload trigger at $6.0\text{ kW}$.
- **Test 4: Analytics Agent Verification:** Validated peak power extraction and average power calculation.

**Result:** All 4 test suites passed with 100% success.

---

### STAGE 6: Presentation & Demo Guide

#### Demo Walkthrough Script
1. **Show Directory Structure & CMake:** Show clean separation of `include/`, `src/`, `driver/`, `tests/`.
2. **Execute Unit Tests:** Run `./bin/TestSmartMeter` to prove mathematical correctness.
3. **Execute Live Demo:** Run `./bin/SmartMeter --demo`:
   - Point out baseline consumption (~$1.97\text{ kW}$).
   - Show simulated heavy load spike ($6.2\text{ kW}$) triggering instantaneous `OVERLOAD` and `SURGE` alerts.
   - Show normalization back to $1.8\text{ kW}$.
   - Show automated generation of the final **Analytics Summary Report**.
4. **Show Persistent File:** Open `meter_log.csv` to demonstrate recorded measurements and logged event alerts.
