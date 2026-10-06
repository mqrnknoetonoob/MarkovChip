#!/usr/bin/env python3
"""
post_routing_patch.py  (v8 -- full-overlap Metal3 plate + dense via arrays)

Reviewer feedback (round 2): v7 landed a single small 1x3 via stack on the
ring. Current still had to squeeze from a wide Metal2 stub, through a tiny
Metal3 patch, back out into the wide Metal4 ring. Requested fix: make
Metal3 the size of the Metal2/Metal4 overlap, and fill it with vias.

What v8 does, per pad (12 pads: 6 VDD, 6 VSS):
  1. Keeps the existing Metal2 stub (die edge -> x=5.04um) and the existing
     Metal1 via at the nearest real FOLLOWPIN row (parallel path, unchanged).
  2. Draws a Metal3 plate = the real Metal2/Metal4 overlap:
        x: the net's own ring wire span (ring centerline +/- 1600 dbu)
        y: the pad's own y_lo..y_hi (same span as the Metal2 stub)
  3. Drops ONE tall Via2 array (Metal2<->Metal3) and ONE tall Via3 array
     (Metal3<->Metal4) at the ring centerline, at the pad's y-center, each
     sized to fill the pad height.

Via arrays are NEW via masters added to the DEF's VIAS section, cloned from
this design's own existing via2_3_* / via3_4_* masters (same VIARULE,
CUTSIZE, LAYERS, ENCLOSURE). Only CUTSPACING and ROWCOL are changed, and
the VIAS count header is updated. Nothing about the via geometry rules is
invented.

DRC rule V2.2b / V3.2b ("via spacing in 4x4 or larger via array is 0.36um"):
the arrays here are only BIG_VIA_COLS (=2) columns wide AND use 0.36um cut
spacing in both directions, so they comply whether or not the KLayout
deck classifies them as "4x4 or larger". This is checked only by the
KLayout signoff deck (Magic does not implement V2.2b/V3.2b), so re-run
KLayout DRC on the resulting GDS and look specifically for V2.2b/V3.2b.

Assumptions (verified against this design's routed DEF, re-verify if the
design/PDK/flow changes):
  - DEF_UNITS_PER_MICRON = 2000
  - CUTSPACING in the DEF VIAS section is edge-to-edge: existing masters
    have CUTSIZE 520 + CUTSPACING 520 = pitch 1040, matching the pitch
    encoded in their names (..._1040_1040).
  - Ring centerlines / widths: VDD 8480 dbu, VSS 1880 dbu, width 3200 dbu
    (read directly off the routed DEF's SHAPE RING wires).
  - FOLLOWPIN rows for both nets start at x=5.04um (10080 dbu).

Called as:  python3 post_routing_patch.py <input_def> <output_def>
"""

import re
import sys

DEF_UNITS_PER_MICRON = 2000

VDD_RANGES_UM = [
    (109.14, 118.64), (95.99, 106.24), (84.14, 94.39),
    (70.61, 80.86), (58.76, 69.01), (46.36, 55.86),
]
VSS_RANGES_UM = [
    (209.14, 218.64), (195.99, 206.24), (184.14, 194.39),
    (170.61, 180.86), (158.76, 169.01), (146.36, 155.86),
]

X_LO_UM = 0.0    # die edge, overlaps the existing FP_DEF_TEMPLATE pin
X_HI_UM = 5.04   # shared FOLLOWPIN row start-x for both nets (Metal1 via)

# Ring centerline x (dbu), read directly off the routed DEF, per net.
RING_X_DBU_BY_NET = {"VDD": 8480, "VSS": 1880}
RING_WIDTH_DBU = 3200            # both rings: 1.6um each side of centerline

# Big via array parameters
BIG_VIA_COLS = 2                 # keep < 4 wide (see docstring)
BIG_VIA_CUT_SPACING_DBU = 720    # 0.36um edge-to-edge, both directions
BIG_VIA_END_MARGIN_DBU = 200     # keep array metal off the exact pad edges
NAME_TAG = "_strong"             # marks masters we added (also re-run guard)

JOG_WIDTH_UM = 0.14


def um_to_dbu(v):
    return int(round(v * DEF_UNITS_PER_MICRON))


# --------------------------------------------------------------------------
# VIAS-section helpers
# --------------------------------------------------------------------------
def _vias_block(def_text):
    m = re.search(r'^VIAS\s+\d+\s*;(.*?)^END VIAS', def_text, re.DOTALL | re.MULTILINE)
    return m.group(1) if m else None


def find_via(def_text, layer_a, layer_b):
    block = _vias_block(def_text)
    if block is None:
        return None
    pat = re.compile(rf'-\s+(\S+)\s+.*LAYERS\s+{layer_a}\s+\S+\s+{layer_b}\b')
    for line in block.splitlines():
        m = pat.match(line.strip())
        if m:
            return m.group(1)
    return None


def get_via_def_line(def_text, name):
    block = _vias_block(def_text)
    if block is None:
        return None
    for line in block.splitlines():
        s = line.strip()
        if s.startswith(f"- {name} "):
            return s
    return None


def parse_enclosure(via_line):
    m = re.search(r'ENCLOSURE\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)', via_line)
    if not m:
        raise RuntimeError(f"No ENCLOSURE clause found in via definition: {via_line}")
    return [int(g) for g in m.groups()]


def parse_cutsize(via_line):
    m = re.search(r'CUTSIZE\s+(\d+)\s+(\d+)', via_line)
    if not m:
        raise RuntimeError(f"No CUTSIZE clause found in via definition: {via_line}")
    return int(m.group(1)), int(m.group(2))


def make_big_via(base_line, new_name, rows, cols, cut_spacing):
    line = re.sub(r'^-\s+\S+', f'- {new_name}', base_line, count=1)
    line, n1 = re.subn(r'CUTSPACING\s+\d+\s+\d+', f'CUTSPACING {cut_spacing} {cut_spacing}', line)
    line, n2 = re.subn(r'ROWCOL\s+\d+\s+\d+', f'ROWCOL {rows} {cols}', line)
    if n1 != 1 or n2 != 1:
        raise RuntimeError(f"Could not rewrite CUTSPACING/ROWCOL in: {base_line}")
    return "    " + line


def rows_that_fit(height_dbu, cut_h, cut_spacing, enc_max, margin):
    pitch = cut_h + cut_spacing
    avail = height_dbu - 2 * margin - 2 * enc_max - cut_h
    if avail < 0:
        return 1
    return int(avail // pitch) + 1


def add_via_masters(def_text, new_masters):
    if not new_masters:
        return def_text
    m = re.search(r'^(VIAS\s+)(\d+)(\s*;)', def_text, re.MULTILINE)
    if not m:
        raise RuntimeError("Could not find VIAS section header")
    new_count = int(m.group(2)) + len(new_masters)
    def_text = def_text[:m.start()] + f"{m.group(1)}{new_count}{m.group(3)}" + def_text[m.end():]
    idx = def_text.index("END VIAS")
    line_start = def_text.rfind("\n", 0, idx) + 1
    insert = "\n".join(new_masters.values()) + "\n"
    return def_text[:line_start] + insert + def_text[line_start:]


# --------------------------------------------------------------------------
# SPECIALNETS helpers
# --------------------------------------------------------------------------
def parse_followpin_rows(net_block_text, x_dbu):
    rows = [int(y) for y in re.findall(
        rf'FOLLOWPIN\s+\(\s*{x_dbu}\s+(\d+)\s*\)', net_block_text)]
    return sorted(set(rows))


def nearest_row(rows_dbu, target_dbu):
    return min(rows_dbu, key=lambda r: abs(r - target_dbu))


def build_stub_wire(m1_via_name, via2_name, via3_name,
                    y_lo_um, y_hi_um, via_y_dbu, width_um, ring_x_dbu):
    y_lo_dbu = um_to_dbu(y_lo_um)
    y_hi_dbu = um_to_dbu(y_hi_um)
    y_c_dbu = um_to_dbu((y_lo_um + y_hi_um) / 2.0)
    width_dbu = um_to_dbu(width_um)
    x_lo = um_to_dbu(X_LO_UM)
    x_hi = um_to_dbu(X_HI_UM)

    lines = [
        # Metal2 stub (unchanged from v5-v7)
        f"    NEW Metal2 {width_dbu} + SHAPE STRIPE "
        f"( {x_lo} {y_c_dbu} ) ( {x_hi} {y_c_dbu} )"
    ]

    # Existing Metal1 row-rail via (unchanged, parallel path)
    if not (y_lo_dbu <= via_y_dbu <= y_hi_dbu):
        jog_width_dbu = um_to_dbu(JOG_WIDTH_UM)
        lines.append(
            f"    NEW Metal2 {jog_width_dbu} + SHAPE STRIPE "
            f"( {x_hi} {y_c_dbu} ) ( {x_hi} {via_y_dbu} )"
        )
    lines.append(
        f"    NEW Metal1 0 + SHAPE STRIPE ( {x_hi} {via_y_dbu} ) {m1_via_name}"
    )

    # NEW (v8): Metal3 plate = the Metal2/Metal4 overlap. Vertical stripe on
    # the ring centerline, RING_WIDTH wide, spanning the pad's full y range.
    lines.append(
        f"    NEW Metal3 {RING_WIDTH_DBU} + SHAPE STRIPE "
        f"( {ring_x_dbu} {y_lo_dbu} ) ( {ring_x_dbu} {y_hi_dbu} )"
    )
    # NEW (v8): tall via arrays, same (x, y) so the two arrays stack exactly
    lines.append(
        f"    NEW Metal2 0 + SHAPE STRIPE ( {ring_x_dbu} {y_c_dbu} ) {via2_name}"
    )
    lines.append(
        f"    NEW Metal3 0 + SHAPE STRIPE ( {ring_x_dbu} {y_c_dbu} ) {via3_name}"
    )
    return "\n".join(lines)


def patch_net(def_text, net_name, ranges_um, m1_via_name,
              base2_line, base3_line, new_masters, log):
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
            f"({X_HI_UM}um). This DEF may differ from the one this hook was "
            f"verified against -- do not trust the patch until re-verified."
        )

    cut_w, cut_h = parse_cutsize(base2_line)
    enc_max = max(parse_enclosure(base2_line) + parse_enclosure(base3_line))
    pitch = cut_h + BIG_VIA_CUT_SPACING_DBU

    log(f"{net_name}: {len(rows_dbu)} FOLLOWPIN rows at x={X_HI_UM}um; "
        f"ring centerline x={ring_x_dbu} dbu, ring width {RING_WIDTH_DBU} dbu")

    new_branches = []
    for (y_lo, y_hi) in ranges_um:
        y_lo_dbu, y_hi_dbu = um_to_dbu(y_lo), um_to_dbu(y_hi)
        height = y_hi_dbu - y_lo_dbu
        y_c_dbu = um_to_dbu((y_lo + y_hi) / 2.0)
        row_dbu = nearest_row(rows_dbu, y_c_dbu)

        rows = rows_that_fit(height, cut_h, BIG_VIA_CUT_SPACING_DBU, enc_max,
                             BIG_VIA_END_MARGIN_DBU)
        via2_name = f"via2_3_{RING_WIDTH_DBU}_{height}_{rows}_{BIG_VIA_COLS}_{pitch}_{pitch}{NAME_TAG}"
        via3_name = f"via3_4_{RING_WIDTH_DBU}_{height}_{rows}_{BIG_VIA_COLS}_{pitch}_{pitch}{NAME_TAG}"
        if via2_name not in new_masters:
            new_masters[via2_name] = make_big_via(
                base2_line, via2_name, rows, BIG_VIA_COLS, BIG_VIA_CUT_SPACING_DBU)
        if via3_name not in new_masters:
            new_masters[via3_name] = make_big_via(
                base3_line, via3_name, rows, BIG_VIA_COLS, BIG_VIA_CUT_SPACING_DBU)

        footprint_y = (rows - 1) * pitch + cut_h + 2 * enc_max
        log(f"  pad y {y_lo:.2f}-{y_hi:.2f}um: height {height} dbu -> "
            f"{rows} rows x {BIG_VIA_COLS} cols per array "
            f"({rows * BIG_VIA_COLS} cuts/layer), worst-case array height "
            f"{footprint_y} dbu (limit {height})")

        new_branches.append(build_stub_wire(
            m1_via_name, via2_name, via3_name,
            y_lo, y_hi, row_dbu, y_hi - y_lo, ring_x_dbu))

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
        print(f"[post_routing_patch v8] {msg}")

    with open(in_path, "r") as f:
        def_text = f.read()

    if NAME_TAG in def_text:
        print("[post_routing_patch v8] FATAL: this DEF already contains v8 "
              "via masters -- refusing to patch twice.", file=sys.stderr)
        sys.exit(4)

    m1_via = find_via(def_text, "Metal1", "Metal2")
    v2_base = find_via(def_text, "Metal2", "Metal3")
    v3_base = find_via(def_text, "Metal3", "Metal4")
    missing = [n for n, v in [("Metal1-Metal2", m1_via), ("Metal2-Metal3", v2_base),
                              ("Metal3-Metal4", v3_base)] if v is None]
    if missing:
        print(f"[post_routing_patch v8] FATAL: no via master found for: "
              f"{', '.join(missing)}. Aborting rather than guessing.", file=sys.stderr)
        sys.exit(2)

    base2_line = get_via_def_line(def_text, v2_base)
    base3_line = get_via_def_line(def_text, v3_base)
    log(f"Base masters: {m1_via} (M1-M2), {v2_base} (M2-M3), {v3_base} (M3-M4)")

    new_masters = {}
    def_text = patch_net(def_text, "VDD", VDD_RANGES_UM, m1_via, base2_line, base3_line, new_masters, log)
    def_text = patch_net(def_text, "VSS", VSS_RANGES_UM, m1_via, base2_line, base3_line, new_masters, log)
    def_text = add_via_masters(def_text, new_masters)
    log(f"Added {len(new_masters)} new via masters to the VIAS section")

    with open(out_path, "w") as f:
        f.write(def_text)
    log(f"Patched DEF written to {out_path}")


if __name__ == "__main__":
    main()