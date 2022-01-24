{
	arg analysis_folder, audio_folder;
	s.waitForBoot{
		var master_analysis_ds = FluidDataSet(s);
		var master_loc_ds = FluidDataSet(s);
		var master_params_ds = FluidDataSet(s);
		var temp_ds = FluidDataSet(s);
		var dsq = FluidDataSetQuery(s);
		var current_params_offset = 0;
		var cond0, new_folder, stamp = Date.localtime.stamp;

		var audio_paths = SelectFiles(audio_folder,['wav'],recursive:false,verbose:true);
		var synth_params_path = PathName(audio_folder).files.select({
			arg pn;
			pn.fullPath.contains("synthesis_params.csv");
		})[0].fullPath;

		var synth_params = CSVFileReader.read(synth_params_path).collect({
			arg line;
			line[1..].collect(_.interpret);
		});
		var master_params_ds_dict = Dictionary.newFrom(["cols",synth_params[0].size,"data",Dictionary.new]);
		var master_loc_ds_dict = Dictionary.newFrom(["cols",3,"data",Dictionary.new]);

		var analysis_paths = PathName(analysis_folder).folders.collect{
			arg pn;
			pn.postln;
			PathName(pn.fullPath+/+"ds").files.collect{
				arg pn0;
				"ds path:  %".format(pn0.fullPath).postln;
				pn0.fullPath;
			};
		};

		var loc_paths = PathName(analysis_folder).folders.collect{
			arg pn;
			pn.postln;
			PathName(pn.fullPath+/+"loc_ds").files.collect{
				arg pn0;
				"loc path: %".format(pn0.fullPath).postln;
				pn0.fullPath;
			};
		};

		var add_param_pts = {
			arg ds, action;
			// "add param pts called".postln;
			// ds.print;
			dsq.clear({
				dsq.addColumn(0,{
					dsq.transform(ds,temp_ds,{
						temp_ds.dump({
							arg dict;
							//"dump done".postln;
							//dict.postln;
							dict.at("data").keysValuesDo({
								arg key, val;
								var rec_idx = key.split($-)[1].interpret.asInteger;
								var params_idx = current_params_offset + rec_idx;
								var params = synth_params[params_idx];
								//"%: %".format(key,params).postln;
								master_params_ds_dict.at("data").put(key,params);
							});
							action.value;
						});
					});
				});
			});
		};

		new_folder = analysis_folder+/+"%_master_datasets".format(stamp);
		File.mkdir(new_folder);

		// merge analyses and create ds for params along the way;
		analysis_paths.do({
			arg array, rec_i; // array of servers
			var master_cond = Condition.new;
			array.do({
				arg path, server_i; // path to specific server
				var cond = Condition.new;
				var initialize = ((rec_i == 0) && (server_i == 0));
				path.postln;

				"current_params_offset: %".format(current_params_offset).postln;

				if(initialize,{
					//"initialize = true".postln;
					master_analysis_ds.read(path,{
						//"read done".postln;
						add_param_pts.(master_analysis_ds,{cond.unhang});
					});
				},{
					//"initialize = false".postln;
					temp_ds.read(path,{
						//"read done".postln;
						master_analysis_ds.merge(temp_ds,0,{
							//"merge done".postln;
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
		});

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
	}
}