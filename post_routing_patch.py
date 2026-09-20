#!/usr/bin/env python3
"""
post_routing_patch.py

Design-scoped OpenLane post-routing hook for Markov_Chain_Accelerator
(A06_BH_top_wrapper). Called automatically by flow.tcl's pdn_bridge_patch
step (only if this file exists at designs/<DESIGN>/hooks/post_routing_patch.py
-- see flow.tcl's run_pdn_bridge_patch_step proc), right after routing
completes and before parasitics/STA/IR-drop/GDS/LVS/DRC steps consume the
routing DEF.

Called as:
    python3 post_routing_patch.py <input_def> <output_def>

This is the same bridging logic verified in this session (Magic DRC: 0
errors; OpenROAD antenna check: 0 violations; PSM: PSM-0040 connected,
node count +18/net as expected) -- relocated here so it runs automatically
as part of the flow instead of by hand between flow stages.

Root cause recap (see A06_PDN_fix_full_documentation.md for full detail):
the pad-frame VDD/VSS boundary pins (from the reviewer's FP_DEF_TEMPLATE,
A06_BH.def) sit on Metal2 at the die edge, but the design's PDN grid never
generates any geometry on Metal2 -- it only ever exists on Metal1 (rails)
and Metal4/5 (grid/ring). This script bridges that gap post-route by
inserting Metal2 stub wires + Metal1 via drops directly into the routed
DEF's SPECIALNETS section, snapping each via to the nearest real
FOLLOWPIN row for its own net (VDD/VSS rows are not interchangeable).
"""

import re
import sys

DEF_UNITS_PER_MICRON = 2000  # verified for THIS design's routed DEF via:
                              #   grep -m1 "UNITS DISTANCE MICRONS" <def>
                              # NOTE: an earlier floorplan-stage DEF in this
                              # same project used 200 DBU/um -- the scale is
                              # NOT constant across flow stages. If this hook
                              # is ever reused for a different design or a
                              # differently-configured run, re-verify this
                              # value against that run's actual routed DEF
                              # before trusting it.

# y-ranges in microns, taken directly from A06_BH.def VDD/VSS PIN rects
# (the reviewer-supplied FP_DEF_TEMPLATE boundary spec for this design)
VDD_RANGES_UM = [
    (109.14, 118.64), (95.99, 106.24), (84.14, 94.39),
    (70.61, 80.86), (58.76, 69.01), (46.36, 55.86),
]
VSS_RANGES_UM = [
    (209.14, 218.64), (195.99, 206.24), (184.14, 194.39),
    (170.61, 180.86), (158.76, 169.01), (146.36, 155.86),
]

X_LO_UM = 0.0    # die edge, overlaps the existing FP_DEF_TEMPLATE pin
X_HI_UM = 5.04   # shared FOLLOWPIN row start-x for BOTH nets -- verified
                 # via direct grep that VSS's own FOLLOWPIN rows exist ONLY
                 # here, same as VDD's, despite VDD's and VSS's power RINGS
                 # sitting at different x (5.04um vs 1.74um respectively).
                 # The ring-x difference is real but irrelevant to the
                 # bridge target -- the row rail, not the ring surface, is
                 # the actual electrical tie-in point. See documentation
                 # section 4.2 for the full investigation of this point.

JOG_WIDTH_UM = 0.14  # thin Metal2 jog width if a snapped row falls outside
                     # [y_lo, y_hi] -- tune to Metal2 min-width rule if this
                     # ever actually triggers (not observed for this design)


def um_to_dbu(v):
    return int(round(v * DEF_UNITS_PER_MICRON))


def find_m1_m2_via(def_text):
    """
    Scan the DEF's VIAS section for a via master explicitly defined with
    LAYERS Metal1 <cut> Metal2 -- reuses whatever real via master this
    design's own router already proved out, rather than inventing new via
    geometry.
    """
    vias_match = re.search(r'^VIAS\s+\d+\s*;(.*?)^END VIAS', def_text, re.DOTALL | re.MULTILINE)
    if not vias_match:
        return {}
    vias_block = vias_match.group(1)
    candidates = {}
    for line in vias_block.splitlines():
        line = line.strip()
        m = re.match(r'-\s+(\S+)\s+.*LAYERS\s+Metal1\s+\S+\s+Metal2\b', line)
        if m:
            candidates[m.group(1)] = 1
    return candidates


def parse_followpin_rows(net_block_text, x_dbu):
    """Real FOLLOWPIN Metal1 row y-values (dbu) for THIS net's own block."""
    rows = [int(y) for y in re.findall(
        rf'FOLLOWPIN\s+\(\s*{x_dbu}\s+(\d+)\s*\)', net_block_text
    )]
    return sorted(set(rows))


def nearest_row(rows_dbu, target_dbu):
    return min(rows_dbu, key=lambda r: abs(r - target_dbu))


def build_stub_wire(via_name, y_lo_um, y_hi_um, via_y_dbu, width_um):
    y_lo_dbu = um_to_dbu(y_lo_um)
    y_hi_dbu = um_to_dbu(y_hi_um)
    y_c_dbu = um_to_dbu((y_lo_um + y_hi_um) / 2.0)
    width_dbu = um_to_dbu(width_um)
    x_lo = um_to_dbu(X_LO_UM)
    x_hi = um_to_dbu(X_HI_UM)

    lines = [
        f"    NEW Metal2 {width_dbu} + SHAPE STRIPE "
        f"( {x_lo} {y_c_dbu} ) ( {x_hi} {y_c_dbu} )"
    ]
    if not (y_lo_dbu <= via_y_dbu <= y_hi_dbu):
        jog_width_dbu = um_to_dbu(JOG_WIDTH_UM)
        lines.append(
            f"    NEW Metal2 {jog_width_dbu} + SHAPE STRIPE "
            f"( {x_hi} {y_c_dbu} ) ( {x_hi} {via_y_dbu} )"
        )
    lines.append(
        f"    NEW Metal1 0 + SHAPE STRIPE ( {x_hi} {via_y_dbu} ) {via_name}"
    )
    return "\n".join(lines)


def patch_net(def_text, net_name, ranges_um, via_name, log):
    sn_match = re.search(r'(SPECIALNETS\s+\d+\s*;.*?)(END SPECIALNETS)', def_text, re.DOTALL)
    if not sn_match:
        raise RuntimeError("Could not find SPECIALNETS section in this DEF")
    sn_start, sn_end = sn_match.start(1), sn_match.end(1)
    sn_block = def_text[sn_start:sn_end]

    pattern = re.compile(rf'(-\s+{re.escape(net_name)}\b.*?)(;)', re.DOTALL)
    match = pattern.search(sn_block)
    if not match:
        raise RuntimeError(f"Could not find SPECIALNETS block for net {net_name}")
    net_block_text = match.group(1)

    x_hi_dbu = um_to_dbu(X_HI_UM)
    rows_dbu = parse_followpin_rows(net_block_text, x_hi_dbu)
    if not rows_dbu:
        raise RuntimeError(
            f"No FOLLOWPIN rows found for net {net_name} at x={x_hi_dbu} dbu "
            f"({X_HI_UM}um). This design's routed DEF may differ from the one "
            f"this hook was verified against -- do not trust the patch output "
            f"until re-verified. See module docstring."
        )

    log(f"{net_name}: {len(rows_dbu)} real FOLLOWPIN rows found at x={X_HI_UM}um")

    new_branches = []
    for (y_lo, y_hi) in ranges_um:
        y_c_dbu = um_to_dbu((y_lo + y_hi) / 2.0)
        row_dbu = nearest_row(rows_dbu, y_c_dbu)
        width_um = y_hi - y_lo
        new_branches.append(build_stub_wire(via_name, y_lo, y_hi, row_dbu, width_um))

    insertion = "\n" + "\n".join(new_branches) + "\n    "
    patched_sn_block = (
        sn_block[:match.start()] + match.group(1) + insertion + match.group(2)
        + sn_block[match.end():]
    )
    return def_text[:sn_start] + patched_sn_block + def_text[sn_end:]


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_def> <output_def>", file=sys.stderr)
        sys.exit(1)

    in_path, out_path = sys.argv[1], sys.argv[2]

    def log(msg):
        print(f"[post_routing_patch] {msg}")

    with open(in_path, "r") as f:
        def_text = f.read()

    log("Scanning for the design's own Metal1<->Metal2 via master ...")
    candidates = find_m1_m2_via(def_text)
    if not candidates:
        print(
            "[post_routing_patch] FATAL: no Metal1<->Metal2 via master found "
            "in this DEF's VIAS section. This hook was verified against a "
            "specific routed DEF for A06_BH_top_wrapper -- if this design's "
            "PDN/via structure has changed, this hook needs re-verification, "
            "not blind re-use. Aborting rather than guessing.",
            file=sys.stderr,
        )
        sys.exit(2)

    via_name = max(candidates, key=candidates.get)
    log(f"Using via master: {via_name}")

    def_text = patch_net(def_text, "VDD", VDD_RANGES_UM, via_name, log)
    def_text = patch_net(def_text, "VSS", VSS_RANGES_UM, via_name, log)

    with open(out_path, "w") as f:
        f.write(def_text)

    log(f"Patched DEF written to {out_path}")


if __name__ == "__main__":
    main()