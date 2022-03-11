{
	arg ds_analysis, ds_loc, ds_params, ds_umap, analysis_folder, action;
	s.waitForBoot{
		var select_cols = [0,5,98,99,126]; // spec_cent, spec_flatness, pitch, pitch_conf, loudness

		var dsq = FluidDataSetQuery(s);
		var normer = FluidNormalize(s);
		var output_ds = FluidDataSet(s);
		var outbuf = Buffer(s);

		if(ds_loc.isNil,{
			ds_loc = FluidDataSet(s).read(analysis_folder+/+"01_loc.json");
		});

		s.sync;
		"ds_loc".postln;
		ds_loc.print;

		if(ds_umap.isNil,{
			ds_umap = FluidDataSet(s).read(analysis_folder+/+"06_umap_norm.json");
		});

		s.sync;
		"ds_umap".postln;
		ds_umap.print;

		// append umap dataset to location dataset
		dsq.addRange(0,2);
		dsq.transformJoin(ds_umap,ds_loc,output_ds);

		// read from disk the original dataset
		if(ds_analysis.isNil,{
			ds_analysis = FluidDataSet(s).read(analysis_folder+/+"03_analysis.json");
		});

		s.sync;
		"ds_analysis".postln;
		ds_analysis.print;

		// pull the columns i want
		dsq.clear();
		select_cols.do{
			arg col;
			dsq.addColumn(col);
		};
		dsq.transform(ds_analysis,ds_analysis);

		// normalize og dataset
		normer.fitTransform(ds_analysis,ds_analysis);

		// append og dataset to output dataset
		dsq.clear();
		dsq.addRange(0,select_cols.size);
		dsq.transformJoin(ds_analysis,output_ds,output_ds);

		s.sync;
		"output_ds".postln;
		output_ds.print;

		// load params ds
		if(ds_params.isNil,{
			ds_params = FluidDataSet(s).read(analysis_folder+/+"02_params.json");
		});

		s.sync;
		"ds_params".postln;
		ds_params.print;

		normer.fitTransform(ds_params,ds_params);

		ds_params.cols({
			arg cols;

			dsq.clear();
			dsq.addRange(0,cols);
			dsq.transformJoin(ds_params,output_ds,output_ds);

			output_ds.toBuffer(outbuf,action:{
				outbuf.write(analysis_folder+/+"temp_buf.wav","wav","float",completionMessage:{
					SoundFile.use(analysis_folder+/+"temp_buf.wav",{
						arg sf;
						var array = FloatArray.newClear(sf.numFrames * sf.numChannels);
						sf.readData(array);
						array = array.clump(sf.numChannels);
						array.shape.postln;
						array[0].postln;
						ArrayToCSV(array,analysis_folder+/+"07_data_for_oF.csv");
						File.delete(analysis_folder+/+"temp_buf.wav");
						action.(analysis_folder);
					});
				});
			});
		});
	};
}