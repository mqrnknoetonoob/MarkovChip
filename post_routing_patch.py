#!/usr/bin/env python3
"""
post_routing_patch.py  (v7 -- direct pad-to-ring via stack, per reviewer feedback)

v5/v6 bridged each pad's Metal2 boundary pin to the shared FOLLOWPIN
row-rail start-x (5.04um), landing a Metal1 via at the nearest real row
for that net. That closed the open circuit and passed DRC/antenna/LVS/
connectivity checks -- but reviewer feedback (Mitch Bailey) correctly
identified the resulting path as electrically weak: current has to
travel pad -> Metal2 -> Metal1 -> along the row rail -> through
whatever general Metal1->Metal4 connection exists -> Metal4 vertical
stripes -> Metal5 horizontal stripes -> and only then reach the ring.
Long, resistive, and dependent on the general internal mesh rather than
a direct low-resistance path to the ring itself.

v7 fix: in addition to the existing Metal1 via (kept -- an extra
parallel path to ground/power is never a problem), add a SECOND, direct
via stack at each pad: Metal2 -> Via2 -> Metal3 -> Via3 -> Metal4,
landing exactly on that net's own power RING wire -- not the general
grid, not the row rail.

Ring centerline x-coordinates below were read directly off this
design's own routed DEF (grep -n "SHAPE RING", scoped per net's own
SPECIALNETS block) -- NOT estimated or derived from a formula. VDD's
ring and VSS's ring sit at genuinely different x (confirmed, see the
full documentation's section on this investigation) -- do not assume
they're interchangeable, and do not assume these values hold for a
different routed DEF without re-verifying:

    grep -n "VDD ( PIN\\|VSS ( PIN" <def>              # find each net's block start
    awk 'NR>=<start> && NR<=<next_net_start> && /SHAPE RING/' <def>

Read the x-coordinate (first number) off the vertical RING segment
nearest the die edge for that net -- that's the ring's centerline; the
wire's half-width (second value in the same "widths"/wire-width field,
here 3200 DBU = 1.6um) tells you the ring's full span around that
centerline, useful for sanity-checking the target lands inside it.
"""

import re
import sys

DEF_UNITS_PER_MICRON = 2000  # verified for THIS design's routed DEF via:
                              #   grep -m1 "UNITS DISTANCE MICRONS" <def>

VDD_RANGES_UM = [
    (109.14, 118.64), (95.99, 106.24), (84.14, 94.39),
    (70.61, 80.86), (58.76, 69.01), (46.36, 55.86),
]
VSS_RANGES_UM = [
    (209.14, 218.64), (195.99, 206.24), (184.14, 194.39),
    (170.61, 180.86), (158.76, 169.01), (146.36, 155.86),
]

X_LO_UM = 0.0    # die edge, overlaps the existing FP_DEF_TEMPLATE pin
X_HI_UM = 5.04   # shared FOLLOWPIN row start-x for BOTH nets (Metal1 via
                 # target -- unchanged from v5/v6, kept as a parallel path)

# Ring centerline x, in DBU, read directly off the routed DEF -- per net,
# NOT shared (see module docstring). This is the NEW via stack's target.
RING_X_DBU_BY_NET = {
    "VDD": 8480,   # ring wire spans 6880-10080 DBU (width 3200), centerline 8480
    "VSS": 1880,   # ring wire spans  280- 3480 DBU (width 3200), centerline 1880
}

JOG_WIDTH_UM = 0.14


def um_to_dbu(v):
    return int(round(v * DEF_UNITS_PER_MICRON))


def find_via(def_text, layer_a, layer_b):
    """
    Scan the DEF's VIAS section for a via master explicitly defined with
    LAYERS <layer_a> <cut> <layer_b> -- reuses a real, already-proven via
    from this design's own VIAS section rather than inventing new via
    geometry. Returns the via name, or None if not found.
    """
    vias_match = re.search(r'^VIAS\s+\d+\s*;(.*?)^END VIAS', def_text, re.DOTALL | re.MULTILINE)
    if not vias_match:
        return None
    vias_block = vias_match.group(1)
    pattern = re.compile(
        rf'-\s+(\S+)\s+.*LAYERS\s+{layer_a}\s+\S+\s+{layer_b}\b'
    )
    for line in vias_block.splitlines():
        m = pattern.match(line.strip())
        if m:
            return m.group(1)
    return None


def parse_followpin_rows(net_block_text, x_dbu):
    rows = [int(y) for y in re.findall(
        rf'FOLLOWPIN\s+\(\s*{x_dbu}\s+(\d+)\s*\)', net_block_text
    )]
    return sorted(set(rows))


def nearest_row(rows_dbu, target_dbu):
    return min(rows_dbu, key=lambda r: abs(r - target_dbu))


def build_stub_wire(net_name, m1_via_name, m2_m3_via_name, m3_m4_via_name,
                     y_lo_um, y_hi_um, via_y_dbu, width_um, ring_x_dbu):
    """
    One Metal2 stripe spanning the full pin height (unchanged from v5),
    PLUS:
      - the existing Metal1 via at the nearest real FOLLOWPIN row for
        this net (unchanged from v5 -- kept as a parallel path)
      - NEW: a direct Metal2->Via2->Metal3->Via3->Metal4 stack landing
        on this net's own ring centerline, at the pad's own y-center
        (the ring is a continuous wire spanning the full die height, so
        no row-snapping is needed here, unlike the periodic Metal1 rows)
    """
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

    # -- Existing Metal1 row-rail via (v5/v6, kept as a parallel path) --
    if not (y_lo_dbu <= via_y_dbu <= y_hi_dbu):
        jog_width_dbu = um_to_dbu(JOG_WIDTH_UM)
        lines.append(
            f"    NEW Metal2 {jog_width_dbu} + SHAPE STRIPE "
            f"( {x_hi} {y_c_dbu} ) ( {x_hi} {via_y_dbu} )"
        )
    lines.append(
        f"    NEW Metal1 0 + SHAPE STRIPE ( {x_hi} {via_y_dbu} ) {m1_via_name}"
    )

    # -- NEW: direct Metal2 -> Metal3 -> Metal4(ring) stack --
    # Both via drops sit at the SAME (x, y) point so their footprints
    # stack directly on top of one another (Metal2 -> Via2 -> Metal3 ->
    # Via3 -> Metal4), landing on the ring at the pad's own y-center.
    lines.append(
        f"    NEW Metal2 0 + SHAPE STRIPE ( {ring_x_dbu} {y_c_dbu} ) {m2_m3_via_name}"
    )
    lines.append(
        f"    NEW Metal3 0 + SHAPE STRIPE ( {ring_x_dbu} {y_c_dbu} ) {m3_m4_via_name}"
    )

    return "\n".join(lines)


def patch_net(def_text, net_name, ranges_um, m1_via_name, m2_m3_via_name,
              m3_m4_via_name, log):
    ring_x_dbu = RING_X_DBU_BY_NET[net_name]

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
            f"until re-verified."
        )

    log(f"{net_name}: {len(rows_dbu)} real FOLLOWPIN rows found at x={X_HI_UM}um; "
        f"ring centerline x={ring_x_dbu} dbu ({ring_x_dbu/DEF_UNITS_PER_MICRON}um)")

    new_branches = []
    for (y_lo, y_hi) in ranges_um:
        y_c_dbu = um_to_dbu((y_lo + y_hi) / 2.0)
        row_dbu = nearest_row(rows_dbu, y_c_dbu)
        width_um = y_hi - y_lo
        new_branches.append(build_stub_wire(
            net_name, m1_via_name, m2_m3_via_name, m3_m4_via_name,
            y_lo, y_hi, row_dbu, width_um, ring_x_dbu
        ))

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
        print(f"[post_routing_patch v7] {msg}")

    with open(in_path, "r") as f:
        def_text = f.read()

    log("Scanning for this design's own via masters (Metal1-Metal2, Metal2-Metal3, Metal3-Metal4) ...")
    m1_via = find_via(def_text, "Metal1", "Metal2")
    m2_m3_via = find_via(def_text, "Metal2", "Metal3")
    m3_m4_via = find_via(def_text, "Metal3", "Metal4")

    missing = [name for name, v in [
        ("Metal1-Metal2", m1_via), ("Metal2-Metal3", m2_m3_via), ("Metal3-Metal4", m3_m4_via)
    ] if v is None]
    if missing:
        print(
            f"[post_routing_patch v7] FATAL: could not find a real via master for: "
            f"{', '.join(missing)} in this DEF's VIAS section. This hook was "
            f"verified against a specific routed DEF -- if this design's via "
            f"structure has changed, this hook needs re-verification, not blind "
            f"re-use. Aborting rather than guessing.",
            file=sys.stderr,
        )
        sys.exit(2)

    log(f"Metal1-Metal2 via: {m1_via}")
    log(f"Metal2-Metal3 via: {m2_m3_via}")
    log(f"Metal3-Metal4 via: {m3_m4_via}")

    def_text = patch_net(def_text, "VDD", VDD_RANGES_UM, m1_via, m2_m3_via, m3_m4_via, log)
    def_text = patch_net(def_text, "VSS", VSS_RANGES_UM, m1_via, m2_m3_via, m3_m4_via, log)

    with open(out_path, "w") as f:
        f.write(def_text)

    log(f"Patched DEF written to {out_path}")


if __name__ == "__main__":
    main()
