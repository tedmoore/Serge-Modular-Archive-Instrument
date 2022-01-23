# step numbers correspond to files

00. programmatically step through parameter space to get raw audio
01. extract audio descriptors
02. it will probably be in multiple files, so use the recursive thing
  * use multiple servers
04. (optional) inspect dataset (just one of the files; does it seem to have worked at all?)
06. compile datasets because they were analyzed in chunks and on different servers
20. loudness threshold: remove everything below a certain dB (for the plotter)
07. create a buffer that uses an index (calculated from the 4D of the sliders) to look up:
  * which file the slice is in
  * what sample the file starts at
  * how many samples to play back (this is probably always going to be the same for one analysis; all the slices will be the same length)
09. (optional) inspect the master dataset (created in step 06)
10. PCA to dimensionally reduce it all
11. UMAP it to a 2D space
13. (optional) look at the UMAPs and choose one (I'm not sure I remember how useful this is)
14. gridify
15. plot and sliders: this is the mock up of the program we're looking to make

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
