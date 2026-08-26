# MarkovChip: 16-State Multistep Markov Chain Accelerator
### 35 MHz Physical Design & Sign-Off Report (GF180MCU 5.0V 7-Track Node)

**Authors:** Mushfiq Raiyun and Diganto Saha  
**Team:** BUETCHIP — IEEE SSCS PICO CHIPATHON 2026  
**Technology:** GlobalFoundries 180nm MCU (`gf180mcu_fd_sc_mcu7t5v0` 5.0V 7-Track SC Library)  

---

> [!NOTE]
> - **Selected Voltage for Tape-Out:** 5.0 V
> - **Selected Clock Frequency:** 35 MHz ($T_{clk} = 28.57\text{ ns}$)

---

> [!IMPORTANT]
> ### 🚨 Fan-out & Slew Violation Justification (Sign-Off Clearance)
> 
> **Violation Summary:** The 35 MHz physical sign-off run reports **74 `max_fanout` violations** (against `set_max_fanout 32.0`) and **9 `max_slew` violations** (in the Slowest corner on `bgc_8`–`bgc_11` nodes: 3.87 ns vs. 3.00 ns limit).
> 
> **Formal Technical Rationale & Silicon Safety:**
> 1. **Zero Setup/Hold Timing Violations:** Multi-corner STA (`report_checks -slack_max -0.01`) confirms **ZERO timing violations** across all PVT corners (Fastest, Typical, Slowest). Worst-case setup slack remains positive at **+2.16 ns** ($125^\circ\text{C}$, 4.50V, SS corner) and **+14.40 ns** in the nominal corner.
> 2. **Structural/Architectural Cause:** The 74 fanout violations stem from the 16 parallel `Row_Col_BGC` state blocks driving the shared 16-input `adder_tree` summation network (48 fanout per driver node, limit 32). This is an architectural broadcast property of the 16-state stochastic matrix accelerator.
> 3. **Electrical Capacitance & Slew Verification:**
>    - **Max Capacitance (`max_cap`):** **0 violations** across all 12,293 nets in the design.
>    - **Max Slew (`max_slew`):** **0 violations on the clock tree**. The 9 data-path slew notices occur only at the $125^\circ\text{C}$ Slowest corner (3.87 ns vs. 3.00 ns limit) and carry zero impact on setup/hold slack.
> 4. **Physical Correctness:** KLayout and Magic report **0 DRC violations**, **0 Antenna violations**, and a **100% LVS Match across all 12,293 nets**.
> 5. **CTS Parameter Optimization:** In earlier unoptimized runs, fanout violations exceeded 500. By tuning OpenLane's TritonCTS parameters—setting `CTS_MAX_FANOUT: 6`, `CTS_CLUSTER_SIZE: 6`, and `CTS_CLUSTER_DIAMETER: 60`—adjacent flip-flops were grouped into spatial clusters driven by high-capacity clock buffers (`clkbuf_16`, `buf_8`), eliminating max-slew and max-cap issues on the clock tree.
> 6. **Verdict:** The residual fan-out/slew warnings are purely discrete electrical design-rule (DRC) limits that **do not compromise silicon functionality or 35 MHz timing closure**.

---

## 1. Executive Summary & Frequency Transition Rationale

The MarkovChip accelerator was initially targeted at **20 MHz** ($T_{clk} = 50.00\text{ ns}$). Following multi-corner physical sign-off evaluations in OpenLane across Process-Voltage-Temperature (PVT) corners, we transitioned the target operating frequency to **35 MHz** ($T_{clk} = 28.57\text{ ns}$).

### Key Advantages of the 35 MHz Target
1. **1.75× Execution Speedup:** For a 16-step calculation requiring 66,500 clock cycles, execution latency drops from **3.325 ms** (at 20 MHz) to **1.900 ms** (at 35 MHz).
2. **Iso-Energy Operation:** Dynamic power scales linearly with frequency ($P_{dyn} = \alpha C V_{DD}^2 f$), whereas execution time decreases inversely ($T_{exec} \propto 1/f$). The energy per step remains virtually constant ($\approx \mathbf{209.0\ \mu\text{J}}$ vs. $210.1\ \mu\text{J}$).
3. **Multi-Corner Timing Margin:** Setup slack remains positive under maximum thermal stress ($+2.16\text{ ns}$ at $125^\circ\text{C}$, 4.50V SS corner).

---

## 2. Performance, Power, and Energy Comparison Table

Sign-off metrics extracted directly from OpenLane run reports (`manufacturability.rpt`, `33-rcx_sta.summary.rpt`, `33-rcx_sta.power.rpt`):

| Physical Sign-Off Metric | 20 MHz Target | 35 MHz Target | Delta / Impact |
| :--- | :---: | :---: | :--- |
| **Nominal Supply Voltage ($V_{DD}$)** | 5.00 V | 5.00 V | GF180MCU 5.0V 7-Track SC |
| **Clock Period ($T_{clk}$)** | 50.00 ns | 28.57 ns | $-42.8\%$ Clock Period |
| **Clock Cycles per Step** | 66,500 | 66,500 | Identical |
| **Step Execution Time ($T_{exec}$)** | **3.325 ms** | **1.900 ms** | **1.75× Speedup (75% reduction)** |
| **Total Power ($P_{total}$)** | 63.2 mW | 110.0 mW | Linear Dynamic Power Scaling |
| **Energy per Step ($E_{step}$)** | **210.13 $\mu$J** | **209.00 $\mu$J** | **Iso-Energy ($\sim 0.5\%$ reduction)** |
| **Worst Setup Slack (SS, $125^\circ\text{C}$)** | +22.24 ns | **+2.16 ns** | Clean Timing Closure ($\text{Slack} > 0$) |
| **Worst Hold Slack (FF, $-40^\circ\text{C}$)** | +0.13 ns | **+0.21 ns** | Clean Hold Closure |
| **Magic / KLayout DRC Violations** | 0 | **0** | Clean |
| **LVS Net Match Status** | Clean (12,298) | **Clean (12,293)** | 100% Match |
| **Antenna Violations** | 0 | **0** | Clean (Diode on Input Ports) |

---

## 3. Mathematical Formulation of Energy Savings

Execution time for $N_{cycles} = 66,500$:
$$T_{exec} = N_{cycles} \times T_{clk} = \frac{N_{cycles}}{f}$$

Total energy consumed per calculation step ($E_{step}$):
$$E_{step} = P_{total} \times T_{exec}$$

* **20 MHz Evaluation:**
  $$T_{exec, 20\text{MHz}} = 66,500 \times 50.00\text{ ns} = 3.325\text{ ms}$$
  $$E_{step, 20\text{MHz}} = 0.0632\text{ W} \times 3.325\text{ ms} = \mathbf{210.13\ \mu\text{J}}$$

* **35 MHz Evaluation:**
  $$T_{exec, 35\text{MHz}} = 66,500 \times 28.57\text{ ns} = 1.900\text{ ms}$$
  $$E_{step, 35\text{MHz}} = 0.1100\text{ W} \times 1.900\text{ ms} = \mathbf{209.00\ \mu\text{J}}$$

---

## 4. Key Physical Sign-Off Evaluations & Detailed Reports

Detailed sign-off reports are stored under `Physical_Design/reports_for_35MHz/reports/signoff/`:

### Section 1: DRC, LVS, and Antenna Sign-Off Reports
* **Design Rule Checking (DRC):** **0 DRC violations** confirmed by KLayout (`drc.rpt`, `drc.klayout.xml`) and Magic across all metal layers, diffusion boundaries, and well spacing constraints.
* **Layout vs. Schematic (LVS):** **100% match** across all 12,293 nets in `41-Markov_Chain_Accelerator.lvs.rpt`. Netlist topology perfectly matches extracted GDS layout without shorts or opens.
* **Antenna Protection:** Input ports configured with `DIODE_ON_PORTS: in` to prevent gate-oxide damage during global routing (`43-antenna_violators.rpt`).

### Section 2: Power, Ground, and Current Paths
* **Power Distribution Network (PDN):** Metal4 and Metal5 grid straps deliver nominal 5.0V supply across the core area.
* **IR Drop:** Maximum dynamic IR drop on $V_{DD}$ (5.0V) and $V_{SS}$ (`34-irdrop-VDD.rpt`, `34-irdrop-VSS.rpt`) is **56 mV** ($\approx 1.12\%$), well below the 10% foundry threshold.
* **Electromigration (EM):** Power tap cells (`TAP_STORAGE_DENSITY: 0.14`, `FP_WELLTAP_CELL`, `FP_ENDCAP_CELL`) ensure safe current density across standard cell power rails.

### Section 3: Analog Matching, Symmetry, and Noise Isolation
* **Noise Isolation:** Continuous welltap and endcap cell grids isolate digital switching noise.
* **Clock Tree Symmetry:** H-Tree clock network synthesis yields a global clock skew of **490 ps** (35 MHz run) and **540 ps** (20 MHz run) in `33-rcx_sta.skew.rpt`, indicating well-balanced clock arrival across all 16 `Row_Col_BGC` blocks.

### Section 4: Capacitance & Slew Verification on Fanout Nets
| Verification Metric | Result / Measurement | Sign-Off Location | Compliance Status |
| :--- | :---: | :--- | :--- |
| **Max Capacitance Violations (`max_cap`)** | **0 Violations** | `33-rcx_sta.checks.rpt` (Line 807) | 100% Clean |
| **Clock Tree Slew Violations** | **0 Violations** | `33-rcx_sta.checks.rpt` | 100% Clean |
| **Data Path Max Slew (`max_slew`)** | **9 Notices** (3.87 ns vs. 3.00 ns limit) | `30-mca/rcx_max_sta.checks.rpt` (Line 625) | Clean Setup (+2.16 ns Slack) |
| **Max Fanout Violations (`max_fanout`)** | **74 Notices** (48 fanout vs 32 limit) | `33-rcx_sta.checks.rpt` (Line 806) | DRC-only; 0 timing impact |

### Section 5: Top-Level Integration and Connectivity
* **Die Dimensions:** `DIE_AREA = "0 0 1100 500"` ($1100\ \mu\text{m} \times 500\ \mu\text{m} = 0.55\text{ mm}^2$).
* **Core Area Dimensions:** `CORE_AREA = "10.08 10.08 1089.92 489.92"`.
  $$\text{Core Width} = 1089.92\ \mu\text{m} - 10.08\ \mu\text{m} = \mathbf{1079.84\ \mu\text{m}}$$
  $$\text{Core Height} = 489.92\ \mu\text{m} - 10.08\ \mu\text{m} = \mathbf{479.84\ \mu\text{m}}$$
  $$\text{Core Surface Area} = 1079.84\ \mu\text{m} \times 479.84\ \mu\text{m} = \mathbf{0.518\text{ mm}^2} \quad (\text{Core Utilization Density} = 0.56)$$
* **I/O Connectivity:** All top-level signals (`clk`, `rst_n`, `spi_in0..3`, `spi_out0..3`, `input_ready`, `output_ready`, `output_valid`, `load_row_or_col`) are routed to the core boundary, ready for final pad ring wrapper integration.

---

## 👥 Authors & Credits

| Name | Contribution |
| :--- | :--- |
| **Mushfiq Raiyun** | Architecture & Verification |
| **Diganto Saha** | Physical Design & STA Sign-Off |
| **Arnob Pal** | Physical Design & Report arrangements |
