# Padring Power Delivery Network (PDN) Integration & Core Ring Fix

## 1. Overview & Problem Background
In earlier wrapped integration runs, visual inspection in KLayout revealed a physical discontinuity/gap between the padring's digital power pads (**DVDD / DVSS** on the West edge) and the internal core power grid. 

Although internal cell-level rails and stripes were placed, `FP_PDN_CORE_RING` had remained disabled (`0`), preventing the power distribution network from physically bridging across the core-to-pad boundary. Furthermore, the West edge shares both VDD and VSS pins on Metal2 with a corner blockage (`RECT (0 0) (400 4200)`), making coplanar single-layer routing prone to spacing violations or shorts.

---

## 2. Implemented Configuration Changes
To resolve this issue, the core power ring was enabled and configured in `config.tcl` with proper geometry and spacing rules:

```tcl
# Enable Core Power Ring
set ::env(FP_PDN_CORE_RING) "1"

# Ring Dimensions and Spacing
set ::env(FP_PDN_CORE_RING_VWIDTH) "1.6"
set ::env(FP_PDN_CORE_RING_HWIDTH) "1.6"
set ::env(FP_PDN_CORE_RING_VSPACING) "1.7"
set ::env(FP_PDN_CORE_RING_HSPACING) "1.7"

# Ring Offsets from Core Boundary
set ::env(FP_PDN_CORE_RING_VOFFSET) "6"
set ::env(FP_PDN_CORE_RING_HOFFSET) "6"
```

---

## 3. Physical Changes in the Design

1. **Generation of a Dual-Layer Power Ring (Metal4 & Metal5):**
   * Two dedicated power rings were automatically instantiated in the floorplan DEF:
     * **Metal4 (Vertical Ring Legs):** Width of 3.2 um (3200 DBU).
     * **Metal5 (Horizontal Ring Legs):** Width of 3.2 um (3200 DBU).
   * Robust electrical connectivity across the entire power grid is established at the ring corners using standard `via4_5_3200_3200_3_3_1040_1040` via arrays.

2. **Metal Extension to the Padring Boundary (Eliminating the Physical Gap):**
   * The power ring now physically extends past the standard-cell core boundary to reach the padring interface (reaching down to offsets of X = -3.52 um and Y = -5.12 um in the DEF).
   * This ensures the core power grid is physically stitched to the padring's **DVDD / DVSS** pads, eliminating the visible separation/gap previously observed in KLayout.

3. **Safe Multi-Layer Power Hookup (Avoiding Shorts and Blockages):**
   * By routing the power ring and hookups on the upper metal layers (**Metal4 and Metal5**), the power delivery path safely hops over the bottom-left **Metal2 blockage** `RECT (0 0) (400 4200)` and avoids any risk of Metal2 shorts against adjacent VSS ring legs.
