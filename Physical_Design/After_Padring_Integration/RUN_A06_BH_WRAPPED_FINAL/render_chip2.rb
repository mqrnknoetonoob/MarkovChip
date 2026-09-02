view = RBA::LayoutView::new
view.load_layout("results/final/gds/A06_BH_top_wrapper.gds", 0)
view.load_layer_props("<path_to_the_.lyp_file_found_above>")
view.max_hier
view.zoom_fit
view.save_image("chip_view2.png", 2000, 2000)
