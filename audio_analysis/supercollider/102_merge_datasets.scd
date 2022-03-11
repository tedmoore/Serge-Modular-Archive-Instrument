{
	arg analysis_folder,action;
	var ds_jsons = PathName(analysis_folder).files.select{arg path; path.fileNameWithoutExtension.contains("descriptors")};

	s.waitForBoot{
		var output_dataset = FluidDataSet(s);

		output_dataset.read(ds_jsons[0].fullPath);

		(ds_jsons.size-1).do{
			arg i;
			var ds = FluidDataSet(s);
			ds.read(ds_jsons[i+1].fullPath);
			output_dataset.merge(ds);
		};

		output_dataset.write(analysis_folder+/+"03_analysis.json");

		s.sync;

		action.value(output_dataset,analysis_folder);
	}
}