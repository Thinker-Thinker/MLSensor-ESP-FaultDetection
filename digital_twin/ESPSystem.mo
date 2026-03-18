model ESPSystem
  "ESP Digital Twin — Kloss torque curve + equivalent circuit current sensing"

  // ═══════════════════════════════════════════════════════════
  // MOTOR PARAMETERS
  // Equivalent circuit: for I_motor sensing only
  // Kloss formula: for rotor dynamics (realistic torque curve)
  // Design point: omega_ss=183 rad/s, slip=0.025, T_em=43 N.m
  // ═══════════════════════════════════════════════════════════
  parameter Real V_supply   = 1000   "Supply voltage (V)";
  parameter Real f_supply   = 60     "Supply frequency (Hz)";
  parameter Real R_s        = 1.0    "Stator resistance (Ohm)";
  parameter Real R_r        = 0.16   "Rotor resistance (Ohm)";
  parameter Real L_s        = 0.18   "Stator inductance (H)";
  parameter Integer p       = 2      "Pole pairs";
  parameter Real J_motor    = 0.3    "Rotor inertia (kg.m2)";
  parameter Real B_friction = 0.005  "Viscous damping (N.m.s/rad)";

  // Kloss formula parameters
  // T_max = 2 x T_rated = 2 x 43 N.m (typical motor design)
  // s_max = 0.1 (typical breakdown slip for induction motors)
  parameter Real T_max      = 80.0  "Breakdown torque (N.m)";
  parameter Real s_max      = 0.1   "Breakdown slip";

  // ═══════════════════════════════════════════════════════════
  // PUMP PARAMETERS
  // K_pump derived: T_load(183) = T_em(183) - B*183
  //   = 43.4 - 0.92 = 42.5 N.m
  //   K_pump = 42.5 / 183^2 = 0.00127 N.m.s2/rad2
  // ═══════════════════════════════════════════════════════════
  parameter Real omega_rated = 188.5  "Sync speed (rad/s)";
  parameter Real K_pump      = 0.00127 "Pump torque coefficient";
  parameter Real eta_pump    = 0.70   "Pump hydraulic efficiency";
  parameter Real H0          = 80.0   "Shutoff head at rated speed (m)";
  parameter Real Q_max       = 0.022  "Max flow at rated speed (m3/s)";
  parameter Real H_static    = 40.0   "Static well head (m)";
  parameter Real rho         = 850    "Fluid density (kg/m3)";
  parameter Real g           = 9.81   "Gravity (m/s2)";

  // ═══════════════════════════════════════════════════════════
  // FAULT PARAMETERS — zero = healthy normal operation
  // ═══════════════════════════════════════════════════════════
  parameter Real t_gas_lock     = 9999  "Gas lock start time (s); t=100";
  parameter Real gas_severity   = 0   "Gas lock severity 0-1 normal =0; mild = 0.3; moderate = 0.6; severe = 0.85";
  parameter Real erosion        = 0.0   "Impeller erosion 0-1 mild=0.25; moderate=0.5;severe=0.75";
  parameter Real extra_friction = 0.0   "Extra bearing friction (N.m)mild = 5.0; moderate=10.0; severe =15.0";

  // ═══════════════════════════════════════════════════════════
  // STATE VARIABLE
  // ═══════════════════════════════════════════════════════════
  Real omega(start = 5.0)  "Rotor speed (rad/s) 1.0; 15.0; ";

  Real omega_sync;
  Real slip;
  Real T_em          "Electromagnetic torque via Kloss (N.m)";
  Real Z_total;
  Real I_motor       "Phase current from equivalent circuit (A)";
  Real T_load;
  Real gas_ramp;
  Real rho_eff;
  Real K_eff;
  Real speed_ratio;
  Real Q_flow;
  Real H_pump;
  Real P_shaft;
  Real efficiency;
  Real vibration_rms;
  
  Real I_sensor;
  Real omega_sensor;
  Real Q_sensor;
  Real H_sensor;
  Real vib_sensor;
  Real slip_sensor;

equation



  // ── Synchronous speed ─────────────────────────────────────
  omega_sync = 2 * Modelica.Constants.pi * f_supply / p;
  // = 188.5 rad/s

  // ── Slip ──────────────────────────────────────────────────
  slip = max(0.001, min(0.99, (omega_sync - omega) / omega_sync));

  // ── TORQUE: Kloss formula (drives rotor dynamics) ─────────
  // Gives realistic S-shaped torque-speed curve:
  //   high at standstill, peaks at s_max, falls toward sync
  T_em = T_max * 2.0 / (slip / s_max + s_max / slip);

  // ── CURRENT: Equivalent circuit (ML sensor signal) ────────
  // I varies with slip → varies with load changes and faults
  Z_total = sqrt((R_s + R_r / slip)^2 +
                 (2 * Modelica.Constants.pi * f_supply * L_s)^2);
  I_motor = V_supply / Z_total;

  // ── Gas lock ramp ─────────────────────────────────────────
  gas_ramp = gas_severity * noEvent(
               if time > t_gas_lock
               then min(1.0, (time - t_gas_lock) / 15.0)
               else 0.0);

  // ── Effective fluid density ───────────────────────────────
  rho_eff = rho * (1.0 - 0.85 * gas_ramp);

  // ── Effective pump torque coefficient ────────────────────
  K_eff = K_pump * (rho_eff / rho) * (1.0 - 0.35 * erosion);

  // ── Load torque ───────────────────────────────────────────
  T_load = K_eff * omega^2
           + B_friction * omega
           + extra_friction;

  // ── ROTOR DYNAMICS ────────────────────────────────────────
  J_motor * der(omega) = T_em - T_load;

  // ── Monitoring outputs ────────────────────────────────────
  speed_ratio = omega / omega_rated;

  Q_flow = max(0.0, (1.0 - 0.3 * erosion) * Q_max * speed_ratio *
    (1.0 - H_static / max(0.1, H0 * speed_ratio^2)));

  H_pump = max(0.0, H0 * speed_ratio^2 *
    (1.0 - Q_flow / max(1e-6, (1.0 - 0.3*erosion) * Q_max * speed_ratio)));

  P_shaft = T_em * omega;

  efficiency = (eta_pump * K_eff * omega^2) / max(T_em, 0.001);

  vibration_rms = 0.05 * speed_ratio
                + extra_friction / 20.0
                + 1.2 * gas_ramp
                + 0.6 * erosion;
// ── Sensor noise models ───────────────────────────────────
  // Simulates real sensor imperfections:
  // I_motor: CT clamp has 50Hz interference + harmonics
  // omega: tachometer has mechanical resonance noise
  // Q_flow: turbine flowmeter has low-frequency fluctuation
  // H_pump: pressure transducer has thermal drift noise
  // vibration: accelerometer has broadband + structural resonance

I_sensor = I_motor + 0.08 * sin(2*Modelica.Constants.pi*50*time + 0.3) + 0.03 * sin(2*Modelica.Constants.pi*150*time + 1.1) + 0.01 * sin(2*Modelica.Constants.pi*250*time);

 omega_sensor = omega
    + 0.8  * sin(2*Modelica.Constants.pi*30*time + 0.7)
    + 0.3  * sin(2*Modelica.Constants.pi*90*time);

   Q_sensor = Q_flow
    + 0.00015 * sin(2*Modelica.Constants.pi*20*time + 0.5)
    + 0.00008 * sin(2*Modelica.Constants.pi*45*time);

 H_sensor = H_pump
    + 0.12 * sin(2*Modelica.Constants.pi*25*time + 1.4)
    + 0.05 * sin(2*Modelica.Constants.pi*60*time);

 vib_sensor = vibration_rms
    + 0.012 * sin(2*Modelica.Constants.pi*180*time + 0.2)
    + 0.006 * sin(2*Modelica.Constants.pi*360*time + 0.9)
    + 0.004 * sin(2*Modelica.Constants.pi*540*time);

 slip_sensor = slip
    + 0.0003 * sin(2*Modelica.Constants.pi*100*time);
end ESPSystem;
