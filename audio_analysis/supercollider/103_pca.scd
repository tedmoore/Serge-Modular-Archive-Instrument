{
	arg ds_analysis, analysis_folder, action;
	var find_variance = "104_find_variance.scd".loadRelative[0];
	s.waitForBoot{

		if(ds_analysis.isNil,{
			ds_analysis = FluidDataSet(s).read(analysis_folder+/+"03_analysis.json");
		});

		ds_analysis.cols({
			arg n_cols;
			var pca;
			var ds_stand = FluidDataSet(s);
			var ds_pca = FluidDataSet(s);
			FluidStandardize(s).fitTransform(ds_analysis,ds_stand);
			pca = FluidPCA(s,n_cols).fitTransform(ds_stand,ds_pca);
			find_variance.(pca,analysis_folder,{
				arg n_pcs, variance_sum;
				var dsq = FluidDataSetQuery(s);
				dsq.addRange(0,n_pcs);
				dsq.transform(ds_pca,ds_pca);
				ds_pca.write(analysis_folder+/+"05_analysis_pca.json",{
					action.(ds_pca,analysis_folder);
				});
			});
		});
	}
}