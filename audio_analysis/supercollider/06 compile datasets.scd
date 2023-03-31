{
	arg analysis_folder, audio_folder, action;
	s.reboot;
	s.waitForBoot{
		var master_analysis_ds = FluidDataSet(s);
		var master_loc_ds = FluidDataSet(s);
		var master_params_ds = FluidDataSet(s);
		var temp_ds = FluidDataSet(s);
		var dsq = FluidDataSetQuery(s);
		var current_params_offset = 0;
		var cond0, new_folder, stamp = Date.localtime.stamp;

		var audio_paths = SelectFiles(audio_folder.withTrailingSlash,['wav'],recursive:false,verbose:true);
		var synth_params_path = PathName(audio_folder.withTrailingSlash).files.select({
			arg pn;
			pn.fullPath.contains("synthesis_params.csv");
		})[0].fullPath;

		var synth_params = CSVFileReader.read(synth_params_path);
		var master_loc_ds_dict = Dictionary.newFrom(["cols",3,"data",Dictionary.new]);

		/*
		find the folders inside 'analysis_folder' that have the folder name 'file_' and a number.
		these are the folders that come from each wav file that was analyzed (becuause the multi-
		hour recording needs to be broken up into multiple wav files). this check is important
		because if is 'compile datasets' function gets run multiple times, there could be some
		old extraneous folders in there which it will search causing errors
		*/
		var wav_file_analysis_folders = PathName(analysis_folder.withTrailingSlash).folders.select{
			arg pn;
			pn.folderName.contains("file_");
		};

		var analysis_paths = wav_file_analysis_folders.collect{
			arg pn;
			PathName(pn.fullPath+/+"ds").files.collect{
				arg pn0;
				pn0.fullPath;
			};
		};

		var loc_paths = wav_file_analysis_folders.collect{
			arg pn;
			PathName(pn.fullPath+/+"loc_ds").files.collect{
				arg pn0;
				pn0.fullPath;
			};
		};

		var add_param_pts, master_params_ds_dict;

		synth_params = synth_params.collect{
			arg row;
			row[1..].collect{arg val; val.asFloat};
		};

		master_params_ds_dict = Dictionary.newFrom(["cols",synth_params[0].size,"data",Dictionary.new]);

		add_param_pts = {
			arg ds, action;

			// we're really only interested in the keys of the dataset 'ds'
			dsq.clear({

				// peel off just the 0th column so that we don't have to dump the whole thing...
				dsq.addColumn(0,{
					dsq.transform(ds,temp_ds,{

						// then dump it so we can have access to the keys in the dict
						temp_ds.dump({
							arg dict;
							dict["data"].keysValuesDo({
								arg key, val;

								/*
								the key is going to be the wav file name (should contain only
								underscores as separators, not hyphens!) then a hyphen then
								the index of the one second sample, so we'll peel that off
								*/
								var rec_idx = key.split($-)[1].asInteger;

								/*
								because each wav file starts over the index counting of the
								one second slices, we need to know what the current count is!
								and offset _this_ file's index by that amount
								*/
								var params_idx = current_params_offset + rec_idx;
								var params = synth_params[params_idx];
								// "synthesis params for key '%': \t% \ttype: %".format(key,params,params[0].class).postln;
								master_params_ds_dict["data"][key] = params;
							});
							action.value;
						});
					});
				});
			});
		};

		new_folder = analysis_folder+/+"%_master_datasets".format(stamp);
		File.mkdir(new_folder);

		"analysis folder:    %".format(analysis_folder).postln;
		"audio folder:       %".format(audio_folder).postln;
		"audio_paths:".postln;
		audio_paths.do{arg p; "\t%".format(p).postln;};
		"analysis_paths:".postln;
		analysis_paths.do{arg p; "\t%".format(p).postln;};
		"loc_paths:".postln;
		loc_paths.do{arg p; "\t%".format(p).postln;};
		"synth_params_path:  %".format(synth_params_path).postln;
		"synth params shape: %".format(synth_params.shape).postln;
		"synth params:       %".format(synth_params).postln;
		"new folder:         %".format(new_folder).postln;

		// merge analyses and create ds for params along the way;
		analysis_paths.do{
			arg array, rec_i; // array of servers
			var master_cond = Condition.new;
			array.do({
				arg path, server_i; // path to specific server
				var cond = Condition.new;
				var initialize = ((rec_i == 0) && (server_i == 0));
				"wav file: % \tserver: % \tpath: %".format(rec_i,server_i,path).postln;

				if(initialize,{
					/*
					if this is the very first dataset we're loading from disk, read it straight
					into this master dataset
					*/
					master_analysis_ds.read(path,{

						add_param_pts.(master_analysis_ds,{cond.unhang});
					});
				},{
					temp_ds.read(path,{
						master_analysis_ds.merge(temp_ds,0,{
							add_param_pts.(temp_ds,{cond.unhang});
						});
					});

				});
				cond.hang;
			});

			// offset needs to become, how many datapoints are in the dataset
			master_analysis_ds.size({
				arg size;
				current_params_offset = size;
				master_cond.unhang;
			});

			master_cond.hang;
			s.sync;
		};

		loc_paths.do({
			arg array, rec_i; // array of servers
			array.do({
				arg path, server_i; // path to specific server
				var cond = Condition.new;
				path.postln;
				temp_ds.read(path,{
					temp_ds.dump({
						arg dict;
						dict.at("data").keysValuesDo({
							arg key, val;
							master_loc_ds_dict.at("data").put(key,[rec_i] ++ val);
						});
						cond.unhang;
					});
				});
				cond.hang;
			});
		});

		master_params_ds.load(master_params_ds_dict);
		master_loc_ds.load(master_loc_ds_dict);

		s.sync;

		cond0 = Condition.new;

		dsq.clear({
			dsq.addColumn(0,{
				dsq.transform(master_analysis_ds,temp_ds,{
					temp_ds.dump({
						arg dict;
						dict.at("data").keysDo({
							arg key;
							if(master_params_ds_dict.at("data").at(key).isNil,{
								key.postln;
								master_analysis_ds.deletePoint(key);
								master_loc_ds.deletePoint(key);
							});
						});
						cond0.unhang
					});
				});
			});
		});

		cond0.hang;

		s.sync;

		"master analysis ds:".postln;
		master_analysis_ds.print;
		master_analysis_ds.write(new_folder+/+"analysis.json");
		s.sync;

		"master params ds:".postln;
		master_params_ds.print;
		master_params_ds.write(new_folder+/+"params.json");
		s.sync;

		"master loc ds:".postln;
		master_loc_ds.print;
		master_loc_ds.write(new_folder+/+"loc.json");
		s.sync;

		"size of params csv: %".format(synth_params.size).postln;

		s.sync;

		action.value(new_folder);
	}
}