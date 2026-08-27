# Fanout Violation Waiver — Justification README

## Summary

STA sign-off checks report **70 max-fanout DRC violations** (limit = 35) across
all three analyzed PVT corners (min, typ/nom, max). This document records the
analysis performed and the justification for accepting these as a **non-blocking
waiver** for the current tapeout.

---

## 1. Scope of the Violation

- **Violation type:** `max_fanout` (structural/connectivity DRC), not a timing
  path failure.
- **Violation count:** 70, identical across `rcx_min_sta_checks.rpt`,
  `rcx_nom_sta_checks.rpt`, and `rcx_max_sta_checks.rpt`.
- **Location:** Exclusively on the **reset (`rst_n`) distribution buffer tree**
  (`fanout20`, `fanout21`, ... `fanout75`) and related global control nets
  (`bgc_0._08_` ... `bgc_15._08_`).
- **Fanout range observed:** 36–70 against a limit of 35.

---

## 2. Electrical / Timing Analysis

| Corner | Max Slew Violations | Max Cap Violations | Reset Path Slack |
|--------|---------------------|---------------------|-------------------|
| Min    | 0                   | 0                   | 21.60 ns (MET)    |
| Nom    | 0                   | 0                   | 21.56 ns (MET)    |
| Max    | 0                   | 0                   | 21.05 ns (MET)    |

Key observations:

- **Zero slew and zero capacitance violations** in all three corners, including
  the worst-case **max** corner (slowest process, highest temp, lowest
  voltage) — confirming existing buffers adequately drive the actual load
  even under worst-case conditions.
- The affected net sits on the **asynchronous reset recovery/removal check**
  (`Path Group: asynchronous`), which shows **21+ ns of positive slack** in
  every corner — far from failing.
- Reset is a **static, low-switching-activity signal**, not part of any
  functional clock or data timing path, so the elevated fanout carries no
  measurable performance risk.
- The tightest slack anywhere in the design (0.48 ns, max corner) occurs on
  an unrelated `adder_tree` logic path and has no connection to the
  fanout-violated nets.

**Conclusion:** The fanout count exceeds the DRC limit, but this is a
structural/rule-of-thumb margin violation — it does not translate into an
actual electrical or timing failure in any analyzed corner.

---

## 3. Disposition

- **Status:** Accepted as a **known, low-risk, non-blocking waiver**.
- **Rationale:** No slew/cap violations, 21+ ns positive slack on the
  affected async path, and the signal is low-activity/non-critical (not on
  any functional clock/data timing path).

---

*Reports referenced: `33-rcx_sta_checks.rpt`, `rcx_min_sta_checks.rpt`,
`rcx_nom_sta_checks.rpt`, `rcx_max_sta_checks.rpt`*
