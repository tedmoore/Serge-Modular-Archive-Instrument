(
~paths = [
	"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210426_213323_normed.json",
	"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_012907_normed.json",
	"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_022227_normed.json",
	"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_033730_normed.json",
	"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_045301_normed.json"
];
s.waitForBoot{
	//Window.closeAll;
	Routine{
		var data = List.new;
		var ids = List.new;
		var ds = FluidDataSet(s);

		s.sync;

		ds.read(~paths[4],{
			ds.dump({
				arg dict;
				"dump done".postln;

				dict.at("data").keysValuesDo{
					arg key, val;
					ids.add(key);
					data.add(val);
				};

				data = data.asArray;
				ids = ids.asArray;

				defer{
					data.postln;
					ids.postln;
					Window.closeAll;
					PlotXYColor(data,{
						arg id;
						id.postln;
					},["umap0","umap1"],ids);
				};
			});
		});
	}.play;
};
)