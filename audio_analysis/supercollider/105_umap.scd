{
	arg ds_pca, analysis_folder,action,numNeighbours = 50, minDist = 0.9;
	s.waitForBoot{
		var umap = FluidUMAP(s,2,numNeighbours,minDist);
		var ds_umap = FluidDataSet(s);
		if(ds_pca.isNil,{
			ds_pca = FluidDataSet(s).load(analysis_folder+/+"05_analysis_pca.json");
		});
		umap.fitTransform(ds_pca,ds_umap);
		FluidNormalize(s).fitTransform(ds_umap,ds_umap);
		ds_umap.write(analysis_folder+/+"06_umap_norm.json");
		s.sync;
		action.(ds_umap,analysis_folder)
	};
}