(
~umap = {
	arg params, action;
	Routine{
		var umap = FluidUMAP(s,params.numDimensions,params.numNeighbors,params.minDist,params.iterations,params.learnRate);
		var ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95.json";
		var normer = FluidNormalize(s);
		var ds = FluidDataSet(s);

		s.sync;

		"init complete".postln;
		ds.read(ds_path,{
			umap.fitTransform(ds,ds,{
				var stamp = Date.localtime.stamp;
				var path_starter = PathName(ds_path).pathOnly+/+PathName(ds_path).fileNameWithoutExtension;
				"umap done".postln;

				ds.write(path_starter++"_umap_%.json".format(stamp),{
					"umap ds written".postln;

					ArrayToCSV([
						["numDimensions",params.numDimensions],
						["numNeighbors",params.numNeighbors],
						["minDist",params.minDist],
						["iterations",params.iterations],
						["learnRate",params.learnRate]
					],path_starter++"_umap_%_paramConfig.csv".format(stamp));

					normer.fitTransform(ds,ds,{
						ds.write(path_starter++"_umap_%_normed.json".format(stamp),{
							"done done".postln;
							ds.free;
							normer.free;
							umap.free;
							action.value;
						});
					});
				});
			});
		});
	}.play;
};
)

(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot{
	var params = List.new;
	var f_r;
	params.add((numDimensions:2,numNeighbors:15,minDist:0.1,iterations:200,learnRate:0.1));
	params.add((numDimensions:2,numNeighbors:15,minDist:0.7,iterations:200,learnRate:0.1));
	params.add((numDimensions:2,numNeighbors:30,minDist:0.7,iterations:200,learnRate:0.1));
	params.add((numDimensions:2,numNeighbors:30,minDist:0.1,iterations:200,learnRate:0.1));

	f_r = {
		arg ps, i_;
		if(i_ < ps.size,{
			~umap.(ps[i_],{
				f_r.(ps,i_ + 1);
			});
		});
	};

	f_r.(params,0);
};
)