{
	arg pca_folder, numDimensions = 2, numNeighbors = 30, minDist = 1, iterations = 200, learnRate = 0.1, action;
	s.reboot;
	s.waitForBoot{
		var umap = FluidUMAP(s,numDimensions,numNeighbors,minDist,iterations,learnRate);
		var normer = FluidNormalize(s);
		var ds = FluidDataSet(s);
		var stamp = Date.localtime.stamp;

		var ds_path = PathName(pca_folder).files.select{arg pn; pn.fileName.contains("pca_ds")}[0].fullPath;

		var umap_folder = pca_folder+/+"%_umap".format(stamp);

		// ds_path.postln;

		umap_folder.mkdir;

		s.sync;

		// "init complete".postln;
		ds.read(ds_path,{
			umap.fitTransform(ds,ds,{
				"umap done".postln;

				ds.write(umap_folder+/+"umap_ds_%.json".format(stamp),{
					"umap ds written".postln;

					ArrayToCSV([
						["numDimensions",numDimensions],
						["numNeighbors",numNeighbors],
						["minDist",minDist],
						["iterations",iterations],
						["learnRate",learnRate]
					],umap_folder+/+"umap_%_paramLog.csv".format(stamp));

					normer.fitTransform(ds,ds,{
						ds.write(umap_folder+/+"umap_ds_normed_%.json".format(stamp),{
							"done done".postln;
							ds.free;
							normer.free;
							umap.free;
							action.value(umap_folder);
						});
					});
				});
			});
		});
	};
}