(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot({
	var ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/analysis.json";
	var loudness_thresh = -55;

	var loudness_column = 126;
	var ds = FluidDataSet(s);
	var dsq = FluidDataSetQuery(s);

	s.sync;

	"init complete".postln;

	ds.read(ds_path,{
		"ds read".postln;
		dsq.clear({
			"dsq cleared".postln;
			ds.cols({
				arg cols;
				"cols: %".format(cols).postln;
				dsq.addRange(0,cols,{
					"add range done".postln;
					dsq.filter(loudness_column,">",loudness_thresh,{
						"filter complete".postln;
						dsq.transform(ds,ds,{
							"transform complete".postln;
							ds.size({
								arg size;
								var pn = PathName(ds_path);
								"new size: %".format(size).postln;
								ds.write(pn.pathOnly+/+pn.fileNameWithoutExtension++"_loudnessThresh=%_size=%.json".format(loudness_thresh,size),{
									"done".postln;
									"new size: %".format(size).postln;
								});
							});
						});
					});
				});
			});
		});
	});
});
)