0. programmatically step through parameter space to get raw audio
0. extract audio descriptors
0. it will probably be in multiple files, so use the recursive thing
  * use multiple servers
0. (optional) inspect dataset (just one of the files; does it seem to have worked at all?)
0. compile datasets because they were analysed in chunks and on different servers
0. dim redux
  * Standardize
  * PCA (keep 95% of variance)
  * save data set
  * save scaler (i might not need this)
  * save explained variance ratios
0. (optional) look at the UMAPs and choose one (I'm not sure I remember how useful this is)
0. gridify
0. plot and sliders: this is the mock up of the program we're looking to make

# the final data file needs to include

### these are columns:
<!-- 0. param_1d_index -->
0. file_num
0. start_sample
0. num_samples
0. umap_x_norm
0. umap_y_norm
<!-- 0. grid_x_norm
0. grid_y_norm -->
0. spec_centroid
0. spec_flatness
0. pitch
0. pitch_confidence
0. loudness
<!-- 0. num_params -->
0. param_0_raw
0. param_1_raw
0. param_2_raw
0. param_3_raw
0. param_0_int
0. param_1_int
0. param_2_int
0. param_3_int
