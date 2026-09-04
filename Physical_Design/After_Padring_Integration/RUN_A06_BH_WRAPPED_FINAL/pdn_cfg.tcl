# Custom PDN configuration for A06_BH_top_wrapper -- REV 3
#
# REV 1 (ring + -connect_to_pads) worked electrically: DRC clean, LVS
# exact match, no routing congestion regression, ring geometry
# genuinely reached the die boundary. Its ONLY problem was overshoot:
# VOFFSET/HOFFSET=6 + width=1.6 already exceeds the 5um margin between
# CORE_AREA (5,5)-(1105,545) and DIE_AREA (0,0)-(1110,550), guaranteeing
# ring geometry past x=0/x=1110/y=0/y=550.
#
# REV 2 (Metal2 stripe + explicit connect to Metal4) hit a structural
# OpenROAD PDN limitation instead: declaring add_pdn_connect between a
# full-coverage layer (Metal4) and a single boundary-only stripe
# (Metal2) makes the channel-repair engine try to bridge the entire
# core-sized coverage gap, which it can't -- failed identically twice,
# independent of the -pins list, so removing Metal2 from -pins was NOT
# the fix. Abandoning that approach.
#
# REV 3 goes back to REV 1's proven mechanism, just with the ring
# shrunk to fit inside the real 5um margin:
#   VOFFSET/HOFFSET: 6 -> 1
#   width: unchanged at 1.6 (this value didn't cause the overshoot by
#     itself, so no reason to shrink it further -- 1(offset)+1.6(width)
#     = 2.6um, leaving ~2.4um of buffer inside the 5um margin)
#
# MANDATORY: after running, re-check the boundary script (grep/awk
# against results/final/def/<DESIGN_NAME>.def) BEFORE trusting this --
# via arrays at ring corners can add a small amount of extra overhang
# beyond the drawn rectangle, so confirm the actual final geometry,
# don't just trust this arithmetic.

source $::env(SCRIPTS_DIR)/openroad/common/set_global_connections.tcl
set_global_connections

set secondary {}
foreach vdd $::env(VDD_NETS) gnd $::env(GND_NETS) {
    if { $vdd != $::env(VDD_NET)} {
        lappend secondary $vdd

        set db_net [[ord::get_db_block] findNet $vdd]
        if {$db_net == "NULL"} {
            set net [odb::dbNet_create [ord::get_db_block] $vdd]
            $net setSpecial
            $net setSigType "POWER"
        }
    }

    if { $gnd != $::env(GND_NET)} {
        lappend secondary $gnd

        set db_net [[ord::get_db_block] findNet $gnd]
        if {$db_net == "NULL"} {
            set net [odb::dbNet_create [ord::get_db_block] $gnd]
            $net setSpecial
            $net setSigType "GROUND"
        }
    }
}

set_voltage_domain -name CORE -power $::env(VDD_NET) -ground $::env(GND_NET) \
    -secondary_power $secondary

# --- Main stdcell grid (proven-working, unchanged from REV 1) ---
if { $::env(FP_PDN_MULTILAYER) == 1 } {
    define_pdn_grid \
        -name stdcell_grid \
        -starts_with POWER \
        -voltage_domain CORE \
        -pins "$::env(FP_PDN_VERTICAL_LAYER) $::env(FP_PDN_HORIZONTAL_LAYER)"

    add_pdn_stripe \
        -grid stdcell_grid \
        -layer $::env(FP_PDN_VERTICAL_LAYER) \
        -width $::env(FP_PDN_VWIDTH) \
        -pitch $::env(FP_PDN_VPITCH) \
        -offset $::env(FP_PDN_VOFFSET) \
        -spacing $::env(FP_PDN_VSPACING) \
        -starts_with POWER -extend_to_core_ring

    add_pdn_stripe \
        -grid stdcell_grid \
        -layer $::env(FP_PDN_HORIZONTAL_LAYER) \
        -width $::env(FP_PDN_HWIDTH) \
        -pitch $::env(FP_PDN_HPITCH) \
        -offset $::env(FP_PDN_HOFFSET) \
        -spacing $::env(FP_PDN_HSPACING) \
        -starts_with POWER -extend_to_core_ring

    add_pdn_connect \
        -grid stdcell_grid \
        -layers "$::env(FP_PDN_VERTICAL_LAYER) $::env(FP_PDN_HORIZONTAL_LAYER)"
} else {
    define_pdn_grid \
        -name stdcell_grid \
        -starts_with POWER \
        -voltage_domain CORE \
        -pins $::env(FP_PDN_VERTICAL_LAYER)

    add_pdn_stripe \
        -grid stdcell_grid \
        -layer $::env(FP_PDN_VERTICAL_LAYER) \
        -width $::env(FP_PDN_VWIDTH) \
        -pitch $::env(FP_PDN_VPITCH) \
        -offset $::env(FP_PDN_VOFFSET) \
        -starts_with POWER -extend_to_core_ring
}

# Standard cell rails (unchanged)
if { $::env(FP_PDN_ENABLE_RAILS) == 1 } {
    add_pdn_stripe \
        -grid stdcell_grid \
        -layer $::env(FP_PDN_RAIL_LAYER) \
        -width $::env(FP_PDN_RAIL_WIDTH) \
        -followpins \
        -starts_with POWER

    add_pdn_connect \
        -grid stdcell_grid \
        -layers "$::env(FP_PDN_RAIL_LAYER) $::env(FP_PDN_VERTICAL_LAYER)"
}

# --- Core ring on Metal4/Metal5, WITH -connect_to_pads (proven to
#     genuinely reach the pad boundary electrically). Ring shrunk to
#     fit inside the real 5um CORE_AREA-to-DIE_AREA margin: use
#     FP_PDN_CORE_RING_VOFFSET/HOFFSET = 1 in config.json this time,
#     not the old value of 6. ---
if { $::env(FP_PDN_CORE_RING) == 1 } {
    add_pdn_ring \
        -grid stdcell_grid \
        -layers "$::env(FP_PDN_VERTICAL_LAYER) $::env(FP_PDN_HORIZONTAL_LAYER)" \
        -widths "$::env(FP_PDN_CORE_RING_VWIDTH) $::env(FP_PDN_CORE_RING_HWIDTH)" \
        -spacings "$::env(FP_PDN_CORE_RING_VSPACING) $::env(FP_PDN_CORE_RING_HSPACING)" \
        -core_offset "$::env(FP_PDN_CORE_RING_VOFFSET) $::env(FP_PDN_CORE_RING_HOFFSET)" \
        -connect_to_pads
}

define_pdn_grid \
    -macro \
    -default \
    -name macro \
    -starts_with POWER \
    -halo "$::env(FP_PDN_HORIZONTAL_HALO) $::env(FP_PDN_VERTICAL_HALO)"

add_pdn_connect \
    -grid macro \
    -layers "$::env(FP_PDN_VERTICAL_LAYER) $::env(FP_PDN_HORIZONTAL_LAYER)"
