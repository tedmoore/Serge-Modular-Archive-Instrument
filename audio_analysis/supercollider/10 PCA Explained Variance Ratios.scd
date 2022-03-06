{
	arg compiled_dataset_folder, action;
	var find_variance = "11 find 95pct variance.scd".loadRelative[0];
	s.reboot;
	s.waitForBoot{
		fork{
			var ds_path = compiled_dataset_folder+/+"analysis.json";

			var pca_folder = compiled_dataset_folder+/+"%_pca".format(Date.localtime.stamp);

			var ds = FluidDataSet(s);
			var pca = FluidPCA(s);
			var scaler = FluidStandardize(s);

			pca_folder.mkdir;

			s.sync;

			ds.read(ds_path,{
				ds.print;
				scaler.fitTransform(ds,ds,{
					ds.cols({
						arg cols;
						"num cols: %".format(cols).postln;
						pca.numDimensions_(cols);
						pca.fitTransform(ds,ds,{
							find_variance.(pca,pca_folder,{
								arg n_pcs, variance;
								fork{
									var dsq = FluidDataSetQuery(s);
									s.sync;
									dsq.addRange(0,n_pcs,{
										dsq.transform(ds,ds,{
											ds.write(pca_folder+/+"pca_ds_pcs=%_variance=%.json".format(n_pcs,variance.round(0.001)),{
												pca.write(pca_folder+/+"pca_pca.json",{
													scaler.write(pca_folder+/+"standardScaler.json",{
														"pca files_written".postln;
														action.(pca_folder);
													});
												});
											});
										});
									});
								};
							});
						});
					});
				});
			});
		};
	}
}