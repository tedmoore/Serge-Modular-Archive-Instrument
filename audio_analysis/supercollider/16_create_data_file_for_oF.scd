{
	arg umap_folder, action;
	s.reboot;
	s.waitForBoot{
		var pn = PathName(umap_folder.withTrailingSlash);
		var dimred2d_ds_path = pn.files.select{arg p; p.fileName.contains("normed")}[0].fullPath;
		var og_ds_path = File.realpath(pn.fullPath+/+"../../")+/+"analysis.json";
		var loc_ds_path = File.realpath(pn.fullPath+/+"../../")+/+"loc.json";
		var params_ds_path = File.realpath(pn.fullPath+/+"../../")+/+"params.json";

		var select_cols = [0,5,98,99,126]; // spec_cent, spec_flatness, pitch, pitch_conf, loudness

		var loc_ds = FluidDataSet(s);
		var dimred2d_ds = FluidDataSet(s);
		var og_ds = FluidDataSet(s);
		var params_ds = FluidDataSet(s);
		var dsq = FluidDataSetQuery(s);
		var normer = FluidNormalize(s);
		var output_ds = FluidDataSet(s);
		var outbuf = Buffer(s);
		var outfolder = umap_folder+/+Date.localtime.stamp;

		// load location of slice dataset
		loc_ds.read(loc_ds_path);
		// load umap results dataset
		dimred2d_ds.read(dimred2d_ds_path);

		// append umap dataset to location dataset
		dsq.addRange(0,2);
		dsq.transformJoin(dimred2d_ds,loc_ds,output_ds);

		s.sync;
		"======== DATASET (5 dims): location :: umap".postln;
		output_ds.print;

		// read from disk the original dataset
		og_ds.read(og_ds_path);

		// pull the columns i want
		dsq.clear();
		select_cols.do{
			arg col;
			dsq.addColumn(col);
		};
		dsq.transform(og_ds,og_ds);

		s.sync;
		"======== DATASET (5 dims): og".postln;
		og_ds.print;

		// normalize og dataset
		normer.fitTransform(og_ds,og_ds);

		// append og dataset to output dataset
		dsq.clear();
		dsq.addRange(0,select_cols.size);
		dsq.transformJoin(og_ds,output_ds,output_ds);

		s.sync;
		"======== DATASET (10 dims): location:: umap :: og (5 desc)".postln;
		output_ds.print;

		// load params ds
		params_ds.read(params_ds_path);

		normer.fitTransform(params_ds,params_ds);

		params_ds.cols({
			arg cols;

			"params ds has % cols".format(cols).postln;

			dsq.clear();
			dsq.addRange(0,cols);
			dsq.transformJoin(params_ds,output_ds,output_ds);

			outfolder.mkdir;

			"======== DATASET (14 dims): umap :: location :: og (5 desc) :: params".postln;
			output_ds.print;

			output_ds.toBuffer(outbuf,action:{
				fork{
					outbuf.write(outfolder+/+"temp_buf.wav","wav","float");
					s.sync;
					SoundFile.use(outfolder+/+"temp_buf.wav",{
						arg sf;
						var array = FloatArray.newClear(sf.numFrames * sf.numChannels);
						sf.readData(array);
						array = array.clump(sf.numChannels);
						array.shape.postln;
						array[0].postln;
						ArrayToCSV(array,outfolder+/+"data_for_oF.csv");
						File.delete(outfolder+/+"temp_buf.wav");
						action.(outfolder);
					});
				};
			});
		});
	};
}