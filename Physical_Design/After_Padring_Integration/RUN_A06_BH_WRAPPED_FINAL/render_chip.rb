view = RBA::LayoutView::new
view.load_layout("results/final/gds/A06_BH_top_wrapper.gds", 0)
view.max_hier
view.zoom_fit
view.save_image("chip_view.png", 2000, 2000)
