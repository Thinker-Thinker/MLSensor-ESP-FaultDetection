# MLSensor: Edge-Deployed ESP Fault Detection

**An edge-deployed machine learning framework for real-time anomaly detection in Electrical Submersible Pumps using a multi-domain Digital Twin.**

> Companion repository for the paper:
> *"MLSensor: An Edge-Deployed Machine Learning Framework for Real-Time Anomaly Detection in Electrical Submersible Pumps Using a Multi-Domain Digital Twin"*
> SPE Nigeria Annual International Conference and Exhibition (NAICE 2026), Lagos, Nigeria, 3–5 August 2026.

---

## Overview

MLSensor is a complete end-to-end pipeline from physics simulation to microcontroller deployment for real-time ESP fault classification.

```
OpenModelica Digital Twin  (ESPSystem.mo)
        │
        ▼
12 labelled simulation CSVs
(3 fault types × 3 severities + 3 normal runs)
        │
        ▼
Python: Gaussian noise injection →
        Sliding-window feature extraction (46 features) →
        Random Forest training
        │
        ▼
micromlgen: Trained model → Self-contained C headers
        │
        ▼
ESP32 DevKit V1: Real-time inference at 427 µs avg latency
```

### Four operating classes detected

| Class | Description |
|---|---|
| **Normal** | Healthy pump operation at design point |
| **Gas Lock** | Gas ingestion reducing effective fluid density |
| **Impeller Erosion** | Progressive degradation of pump impeller |
| **Mechanical Friction** | Bearing wear increasing shaft load |

---

## Repository Structure

```
MLSensor-ESP-FaultDetection/
│
├── README.md
│
├── digital_twin/
│   └── ESPSystem.mo                  # OpenModelica Digital Twin model
│
├── simulation_data/
│   ├── normal_noisy.csv
│   ├── normal_varA.csv
│   ├── normal_varB.csv
│   ├── gas_mild.csv
│   ├── gas_moderate.csv
│   ├── gas_severe.csv
│   ├── erosion_mild.csv
│   ├── erosion_moderate.csv
│   ├── erosion_severe.csv
│   ├── friction_mild.csv
│   ├── friction_moderate.csv
│   └── friction_severe.csv
│
├── training/
│   └── MLSensor_Training.ipynb       # Google Colab training pipeline
│
├── deployment/
│   ├── esp_classifier.h              # Random Forest model (C, 297 KB)
│   ├── esp_scaler.h                  # StandardScaler coefficients (C)
│   ├── esp_features.h                # Feature extraction functions (C)
│   └── MLSensor_ESP32.ino            # Arduino sketch for ESP32
│
└── figures/
    ├── Fig1_Architecture.png
    ├── Fig2_DigitalTwin.png
    ├── Fig3_OMEdit.png
    ├── Fig4_RawSignals.png
    ├── Fig5_KlossCurve.png
    ├── Fig6_FeaturePipeline.png
    ├── Fig7_ConfusionMatrix.png
    ├── Fig8_FeatureImportance.png
    ├── Fig9_LOFO.png
    └── Fig10_InferencePipeline.png
```

---

## Digital Twin — `ESPSystem.mo`

Built in **OpenModelica 1.21+**. Couples three physical domains in a single differential-algebraic system:

| Domain | Model | State Variable |
|---|---|---|
| Electrical | Kloss torque + equivalent circuit | — |
| Mechanical | Newton's rotation (single ODE) | ω (rad/s) |
| Hydraulic | Affinity Laws (explicit) | — |

All hydraulic and electrical quantities are algebraic functions of ω evaluated at each solver timestep.

### Model parameters

| Parameter | Symbol | Value | Unit |
|---|---|---|---|
| Supply voltage | V_supply | 1000 | V |
| Supply frequency | f | 60 | Hz |
| Stator resistance | Rs | 1.0 | Ω |
| Rotor resistance | Rr | 0.16 | Ω |
| Stator inductance | Ls | 0.18 | H |
| Pole pairs | p | 2 | — |
| Rotor inertia | J | 0.3 | kg·m² |
| Breakdown torque | T_max | 80.0 | N·m |
| Breakdown slip | s_max | 0.1 | — |
| Shutoff head | H₀ | 80 | m |
| Max flow rate | Q_max | 0.022 | m³/s |
| Pump torque coefficient | K_pump | 0.00127 | N·m·s²/rad² |
| Static head | H_static | 40 | m |
| Fluid density | ρ | 850 | kg/m³ |

### Governing equations

```
ω_sync  = 2πf / p                                          (1)
s       = (ω_sync − ω) / ω_sync                           (2)
T_em    = T_max × 2 / (s/s_max + s_max/s)   [Kloss]       (3)
Z       = √[(Rs + Rr/s)² + (2πf·Ls)²]                     (4)
I_motor = V_supply / Z                                     (5)
J·dω/dt = T_em − (K_eff·ω² + B·ω + T_fric)               (6)
H_pump  = (1−0.4ε) H₀ r² [1 − Hs/((1−0.4ε)H₀r²)]       (7)
Q_flow  = (1−0.3ε) Qmax r [1 − Hs/((1−0.4ε)H₀r²)]       (8)
K_eff   = K_pump × (ρ_eff/ρ) × (1 − 0.35ε)
```

### Validated steady-state (t = 200 s, normal operation)

| Variable | Design Target | Simulated | Error |
|---|---|---|---|
| ω (rad/s) | 183.0 | 183.5 | 0.27% |
| I (A) | 14.7 | 14.73 | 0.20% |
| Q (m³/s) | 0.010 | 0.010 | <0.5% |
| Efficiency | 0.65–0.70 | 0.648 | 0.3% below lower bound |

### Running the simulations

1. Install OpenModelica: https://openmodelica.org
2. Open **OMEdit** → File → Open → `ESPSystem.mo`
3. Simulation → Simulation Setup: Stop Time = `500`, Solver = `dassl`, Tolerance = `1e-6`
4. Run 12 simulations using the fault parameters below, export each as CSV

**Fault injection parameters:**

| Fault | Modelica Parameter | Mild | Moderate | Severe |
|---|---|---|---|---|
| Gas Lock | `gas_severity` (α) | 0.30 | 0.60 | 0.85 |
| Erosion | `erosion_factor` (ε) | 0.25 | 0.50 | 0.75 |
| Friction | `extra_friction` (N·m) | 5.0 | 10.0 | 15.0 |

For fault runs, set `t_gas_lock = 100.0` (fault onset at t = 100 s).
Normal runs: three runs with start conditions `1.0`, `5.0`, and `15.0`.

---

## Training Pipeline — `MLSensor_Training.ipynb`

Runs on **Google Colab** (free tier). No GPU required.

### Dependencies

```bash
pip install numpy pandas scikit-learn matplotlib seaborn micromlgen
```

### Notebook cells

| Cell | What it does | Output |
|---|---|---|
| **Cell 1** | Load 12 CSVs, assign class labels, trim 30 s startup transients | Labelled DataFrame |
| **Cell 2** | Add Gaussian noise per sensor channel at physical SNR values | Noisy signals (Fig. 4) |
| **Cell 3** | Sliding-window feature extraction: 20-step window, 10-step stride, 46 features | Feature matrix 5,426 × 46 |
| **Cell 4** | Temporal 70/30 split, StandardScaler, Random Forest training | Trained model |
| **Cell 5** | Confusion matrix, classification report, feature importances | Figs. 7, 8 |
| **Cell 6** | LOFO analysis + micromlgen export to C headers | Fig. 9; C files |

### Sensor noise model

Gaussian white noise added in Python. OpenModelica is deterministic and cannot generate stochastic noise independently across runs.

| Sensor | Noise Std Dev | Signal Mean | SNR |
|---|---|---|---|
| I_sensor (CT clamp) | 0.15 A | 14.7 A | 40 dB |
| omega_sensor (tachometer) | 1.0 rad/s | 183.5 rad/s | 45 dB |
| Q_sensor (flowmeter) | 0.0002 m³/s | 0.010 m³/s | 34 dB |
| H_sensor (pressure) | 0.30 m | 40.0 m | 42 dB |
| vib_sensor (MEMS accel.) | 0.05 m/s² | 0.15 m/s² | 10 dB |
| slip_sensor (derived) | 0.001 | ~0.027 | 29 dB |

### Feature set (46 per window)

**Statistical (42):** mean, std, RMS, peak, kurtosis, skewness, crest factor — across 6 sensor channels.

**Cross-modal (4):**

```python
decoupling_IQ    = I_mean  / max(Q_mean,     1e-4)   # rises under erosion, falls under gas lock
decoupling_IH    = I_mean  / max(H_mean,     0.1 )   # separates head-degrading faults
pump_eff_proxy   = (Q_mean * H_mean) / omega_mean**2 # affinity-law hydraulic proxy
speed_load_ratio = omega_mean / max(I_mean,  0.1 )   # rises under gas lock, falls under friction
```

### Dataset

| Class | Severity runs | Raw samples | Windows | Train / Test |
|---|---|---|---|---|
| Normal | 3 | 14,100 | 1,409 | 986 / 423 |
| Gas Lock | 3 | 12,010 | 1,199 | 839 / 360 |
| Erosion | 3 | 14,100 | 1,409 | 986 / 423 |
| Friction | 3 | 14,100 | 1,409 | 986 / 423 |
| **Total** | **12** | **54,310** | **5,426** | **3,797 / 1,629** |

### Classifier

```python
from sklearn.ensemble import RandomForestClassifier
clf = RandomForestClassifier(n_estimators=100, max_depth=10,
                             class_weight='balanced', random_state=42)
```

### Results (temporal 70/30 split, with Gaussian noise)

| Class | Precision | Recall | F1-Score | Support |
|---|---|---|---|---|
| Normal | 1.00 | 1.00 | 1.00 | 423 |
| Gas Lock | 1.00 | 1.00 | 1.00 | 360 |
| Erosion | 1.00 | 1.00 | 1.00 | 423 |
| Friction | 1.00 | 1.00 | 1.00 | 423 |
| **Macro Avg** | **1.00** | **1.00** | **1.00** | **1,629** |

> **On 100% accuracy:** This validates the discriminative completeness of the 46-feature representation on simulation data. It does not claim equivalent performance on real field measurements. The sim-to-real gap is the primary open research challenge identified in the paper.

### Top feature importances

| Rank | Feature | Importance |
|---|---|---|
| 1 | slip_sensor_rms | 10.6% |
| 2 | **decoupling_IQ** | **10.3%** |
| 3 | slip_sensor_mean | 9.8% |
| 4 | Q_sensor_rms | 8.3% |
| 5 | Q_sensor_mean | 7.1% |

---

## Edge Deployment

### Hardware — ESP32 DevKit V1

| Specification | Value |
|---|---|
| Processor | Xtensa LX6 dual-core @ 240 MHz |
| Total flash | 4 MB |
| Total SRAM | 520 KB (~320 KB available) |
| Program storage used | 309,779 bytes **(23%)** |
| Dynamic memory used | 21,448 bytes **(6%)** |
| MLSensor model size | 325.3 KB |
| Communication | Wi-Fi, Bluetooth, UART |

### Build and flash

1. Install **Arduino IDE 2.x**
2. Add ESP32 board URL in `File → Preferences`:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. `Tools → Board → ESP32 Dev Module`
4. **Critical — include order:**
   ```cpp
   #include "esp_scaler.h"      // must be first — defines N_FEATURES = 46
   #include "esp_features.h"    // uses N_FEATURES
   #include "esp_classifier.h"  // defines the classifier
   ```
5. Upload → Serial Monitor at **115200 baud**

### Inference

```cpp
Eloquent::ML::Port::RandomForest classifier;
float sensor_window[20][6];
float feature_vector[46];

extract_features(sensor_window, feature_vector);
normalize_features(feature_vector);
int pred = classifier.predict(feature_vector);
// 0=Normal  1=Gas Lock  2=Erosion  3=Friction
```

### Measured performance

| Metric | Value |
|---|---|
| Min / Max latency | 343 / 507 µs |
| **Average latency** | **427 µs** |
| Compute utilisation | 0.02% of 2-second window |

---

## Key Design Decisions

**Why ML over analytical fault detection?** Four reasons: (1) fault parameters (ρ_eff, K_eff, T_fric) are unobservable from surface instruments; (2) gas lock and erosion produce overlapping single-channel signatures; (3) differentiating noisy speed measurements amplifies uncertainty; (4) real ESPs deviate from idealised models in unmeasurable ways.

**Why Kloss formula for torque?** The simplified equivalent circuit caused solver stall at ω ≈ 30 rad/s. The Kloss formula correctly models the full torque-speed curve at all slip values.

**Why explicit hydraulic equations?** Implicit coupling created a nonlinear algebraic loop that caused solver divergence. Explicit formulation computes H and Q directly from ω at each timestep.

**Why Python noise, not Modelica noise?** `Modelica.Blocks.Sources.Noise` is deterministic — identical waveform every run. Python `np.random.normal()` with independent seeding produces genuine stochastic noise parameterised by physically grounded SNR values.

**Why decoupling_IQ is critical?** It encodes I/Q simultaneously. Under erosion: I stable, Q falls → ratio rises. Under gas lock: both fall, Q faster → ratio falls. This divergence is invisible to any single-channel alarm.

---

## Citation

```bibtex
@inproceedings{mlsensor_naice2026,
  title     = {MLSensor: An Edge-Deployed Machine Learning Framework for
               Real-Time Anomaly Detection in Electrical Submersible Pumps
               Using a Multi-Domain Digital Twin},
  booktitle = {SPE Nigeria Annual International Conference and Exhibition (NAICE 2026)},
  address   = {Lagos, Nigeria},
  month     = {August},
  year      = {2026},
  note      = {SPE-NAICE-XXXX-MS}
}
```

---

## License

**Code and model:** MIT License.
**Simulation data and figures:** CC BY 4.0 — attribution required.

---

## Contact

Repository: https://github.com/Thinker-Thinker/MLSensor-ESP-FaultDetection

> Author contact details will be added upon paper acceptance.