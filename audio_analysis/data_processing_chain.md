# Serge Archive

1. Create dataset by stepping over all the parameter combinations
2. Audio Analysis
    1. Save datasetS
3. Compile datasets
4. Create plot view
    1. Remove any slices < -55dB
        1. Save dataset
    2. Standardize Data and PCA down to fewer dimensions (>95% of variance)
        1. Save dataset
        2. Save scaler
        3. Save pca
        4. Save explained variance ratios
    3. UMAP to two dimensions
        1. Save Umap dataset
    4. Auction algorithm to grid
        1. Save grid positions as dataset
5. Create slider view
    1. Put (a) soundfile number (b) start sample and (c) number of frames into 1D buffer data structure
