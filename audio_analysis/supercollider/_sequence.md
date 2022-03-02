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
0. file_num (like which audio file it is in)
1. start_sample
2. num_samples
3. umap_x_norm
4. umap_y_norm
5. spec_centroid
6. spec_flatness
7. pitch
8. pitch_confidence
9. loudness
10. param_0_norm
11. param_1_norm
12. param_2_norm
13. param_3_norm
