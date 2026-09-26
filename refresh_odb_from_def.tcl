# refresh_odb_from_def.tcl
# Reloads the patched DEF (written by post_routing_patch.py) and re-saves
# it as the ODB binary snapshot, so downstream flow steps -- which default
# to reading ODB, not DEF, when IO_READ_DEF=0 -- actually see the patch.
#
# Run as a separate OpenROAD subprocess (via exec $::env(OPENROAD_BIN) -exit
# -no_init) from flow.tcl's run_pdn_bridge_patch_step, the same pattern
# OpenLane's own built-in post_run.py hook mechanism already uses. Inherits
# MERGED_LEF / CURRENT_DEF / CURRENT_ODB from the parent process's
# environment -- no separate argument passing needed.

read_lef $::env(MERGED_LEF)
read_def $::env(CURRENT_DEF)
write_db $::env(CURRENT_ODB)