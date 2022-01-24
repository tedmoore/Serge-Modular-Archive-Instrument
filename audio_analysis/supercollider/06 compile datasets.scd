{
	arg folder;
	var master_analysis_ds = FluidDataSet(s);
	var master_loc_ds = FluidDataSet(s);
	var master_params_ds = FluidDataSet(s);
	var temp_ds = FluidDataSet(s);
	var dsq = FluidDataSetQuery(s);
	var current_params_offset = 0;
	var cond0, new_folder, stamp = Date.localtime.stamp;

	var audio_paths = [
		"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/part1_44k_16b.wav",
		"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/part2_44k_16b.wav",
		"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/part3_44k_16b.wav"
	];
	var synth_params_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210322_124317_synthesis_params.csv";

	var synth_params = CSVFileReader.read(synth_params_path).collect({
		arg line;
		line[1..].collect(_.interpret);
	});
	var master_params_ds_dict = Dictionary.newFrom(["cols",synth_params[0].size,"data",Dictionary.new]);
	var master_loc_ds_dict = Dictionary.newFrom(["cols",3,"data",Dictionary.new]);
	var analysis_paths = [
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_server=0.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_server=1.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_server=2.json"
		],
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=0.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=1.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=2.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=3.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=4.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_175246_part2_44k_16b_server=5.json"
		],
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=0.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=1.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=2.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=3.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=4.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=5.json"
		]
	];

	var loc_paths = [
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210404_163827_part1_44k_16b_server=0_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210404_163827_part1_44k_16b_server=1_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210404_163827_part1_44k_16b_server=2_start_dur.json"
		],
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=0_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=1_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=2_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=3_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=4_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_175246_part2_44k_16b/210403_193557_part2_44k_16b_server=5_start_dur.json"
		],
		[
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=0_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=1_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=2_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=3_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=4_start_dur.json",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_194240_part3_44k_16b/210403_194240_part3_44k_16b_server=5_start_dur.json"
		]
	];
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

	new_folder = PathName(synth_params_path).pathOnly+/+"%_master_datasets".format(stamp);
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