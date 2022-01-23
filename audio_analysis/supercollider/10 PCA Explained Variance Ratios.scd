(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot{
	Routine{
		var ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/analysis.json";

		var ds = FluidDataSet(s);
		var pcaN = 304;
		var pca = FluidPCA(s,pcaN);
		var scaler = FluidStandardize(s);

		s.sync;

		ds.read(ds_path,{
			scaler.fitTransform(ds,ds,{
				pca.fitTransform(ds,ds,{
					pca.dump({
						arg dict;
						var sum = 0;
						var sum1 = 0;
						var vals = List.new;
						var weights = List.new;
						var abssum = 0.dup(dict.at("bases")[0].size);
						var pn = PathName(ds_path).pathOnly+/+Date.localtime.stamp++"_pca_on_%".format(PathName(ds_path).fileNameWithoutExtension);

						"pca done".postln;

						dict.at("values").do{
							arg val;
							var sqr = pow(val.asFloat,2);
							vals.add(sqr);
							sum = sum + sqr;
						};

						vals.do({
							arg val;
							var weight = (val / sum);
							weights.add(weight);
						});

						//weights.postln;

						if(pcaN == 0,{
							sum1 = 1.0;
						},{
							pcaN.do({
								arg i;
								var val = weights[i];
								sum1 = sum1 + val;
								"PCA %: %\t\t%".format(i+1,val,sum1).postln;
							});
						});

						File.mkdir(pn);

						ds.write(pn+/+"pca%_ds_variance=%.json".format(pcaN,sum1.round(0.001)),{
							pca.write(pn+/+"pca_pca.json".format(pcaN),{
								scaler.write(pn+/+"standardScaler.json",{
									var sum2 = 0;
									"files_written".postln;
									ArrayToCSV(weights.collect({
										arg weight, i;
										sum2 = sum2 + weight;
										["PCA %".format(i + 1),weight,sum2]
									}),pn+/+"pca0_explained_variance_ratios.csv");
								});
							});
						});
					});
				});
			});
		});
	}.play;
};
)