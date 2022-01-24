{
	arg buf_path, slice_sec = 0.05, n_servers = 1, final_action, time_offset = 0, new_folder;
	var stamp = Date.localtime.stamp;
	var prefix = PathName(buf_path).fileNameWithoutExtension;
	var wav_file_name = PathName(buf_path).fileName;
	var dsID_to_wav_name_dict = Dictionary.new;
	var new_ds_folder = new_folder+/+"ds";
	var new_loc_ds_folder = new_folder+/+"loc_ds";
	var sR = SoundFile.use(buf_path,{arg sf;sf.sampleRate});
	var buf_dur = SoundFile.use(buf_path,{arg sf; sf.duration});
	var n_slices = (buf_dur / slice_sec).floor.asInteger;
	var full_array = Array.fill(n_slices,{
		arg i;
		var ds_id = "%-%".format(prefix,i.asInteger);
		var start_sec = (i * slice_sec) + time_offset;
		dsID_to_wav_name_dict.put(ds_id,wav_file_name);
		[ds_id, start_sec * sR, slice_sec * sR]; // id int, start frames, num frames
	});
	var n_derivs = 2; // it's really one though...
	var statsFlatComp = {
		arg featuresBuf,statsBuf,flatBuf,masterBuf,masterBufOffset_frames,masterBuf_channel,numDerivs=2;
		FluidBufStats.processBlocking(featuresBuf.server,featuresBuf,stats:statsBuf,numDerivs:numDerivs-1);
		FluidBufFlatten.processBlocking(featuresBuf.server,statsBuf,destination:flatBuf);
		FluidBufCompose.processBlocking(featuresBuf.server,flatBuf,destination:masterBuf,destStartFrame:masterBufOffset_frames,destStartChan:masterBuf_channel);
	};
	var analyze = {
		arg id, start_frame, num_frames, buf, featuresBuf,statsBuf, flatBuf,finalBuf,masterBuf_channel;
		FluidBufSpectralShape.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,finalBuf,0,masterBuf_channel,n_derivs);
		FluidBufPitch.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,finalBuf,98,masterBuf_channel,n_derivs); // 98 = 7*7*2
		FluidBufLoudness.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,finalBuf,126,masterBuf_channel,n_derivs); // 126 = 98 + 28; // 28 = (2*7*2)
		FluidBufMFCC.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf,numCoeffs:40);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,finalBuf,154,masterBuf_channel,n_derivs); // 154=126+28; // 28=(2*7*2) //
	};
	var server_options = ServerOptions.new;
	var entries_per_sub = (full_array.size / n_servers).ceil.asInteger;
	// var sub_arrays = full_array.clump(entries_per_sub);
	var sub_arrays = List.new.dup(n_servers);
	var headers_expander = {
		arg header_list;
		var out_headers = List.new;
		n_derivs.do({
			arg deriv_num;
			["mean","stddev","skewness","kurtosis","min","median","max"].do({
				arg stat;
				header_list.do({
					arg desc;
					out_headers.add("%-deriv%-%".format(desc,deriv_num,stat));
				});
			});
		});
		out_headers;
	};
	var finalHeaders = List.new;
	var server_done_booleans = false.dup(n_servers);

	// make sure it's mono
	SoundFile.use(buf_path,{arg sf; if(sf.numChannels != 1,{Error("File must be mono: %".format(buf_path)).throw});});

	full_array.do({
		arg pt, i;
		sub_arrays[i % n_servers].add(pt);
	});

	// server_options.device_("Fireface UC Mac (24006457)");

	"full array size: %".format(full_array.size).postln;
	sub_arrays.do({arg sa; "\tsub array size: %".format(sa.size).postln});

	finalHeaders.addAll(headers_expander.([
		"specCentroid",
		"specSpread",
		"specSkewness",
		"specKurtosis",
		"specRolloff",
		"specFlatness",
		"specCrest"
	]));

	finalHeaders.addAll(headers_expander.(["pitch","pitchConf"]));

	finalHeaders.addAll(headers_expander.(["loudness","truePeak"]));

	finalHeaders.addAll(headers_expander.(Array.fill(40,{
		arg i;
		"mfcc%".format(i.asString.padLeft(2,"0"));
	})));

	finalHeaders = finalHeaders.collect({
		arg head, i;
		[i,head];
	});

	File.mkdir(new_folder);
	File.mkdir(new_ds_folder);
	File.mkdir(new_loc_ds_folder);
	ArrayToCSV(finalHeaders,new_folder+/+"%_%_headers.csv".format(stamp,prefix));
	dsID_to_wav_name_dict.writeArchive(new_folder+/+"dsID_to_wav_name_dict.sco");

	n_servers.do{
		arg server_i;
		var server = Server("Server-%-%".format(server_i,UniqueID.next).asSymbol,NetAddr("localhost",57122 + server_i),server_options);
		server.waitForBoot{
			Routine{
				var buf = Buffer.read(server,buf_path);
				var features_buf = Buffer(server);
				var stats_buf = Buffer(server);
				var flat_buf = Buffer(server);
				var ds_ = FluidDataSet(server);
				var ds_ids = FluidLabelSet(server);
				var start_dur_ds = FluidDataSet(server);
				var start_dur_buf = Buffer.alloc(server,2);
				var array = sub_arrays[server_i];
				var final_buf = Buffer.alloc(server,714,array.size);
				var dspath = new_ds_folder+/+"%_%_server=%.json".format(stamp,prefix,server_i);
				var start_dur_path = new_loc_ds_folder+/+"%_%_server=%_start_dur.json".format(stamp,prefix,server_i);
				var timer = BeatSched.new;
				var prev_time = 0;

				server.sync;

				"features_buf: %".format(features_buf).postln;
				"stats_buf: %".format(stats_buf).postln;
				"flat_buf: %".format(flat_buf).postln;
				"start_dur_buf: %".format(features_buf).postln;
				"final_buf: %".format(final_buf).postln;

				array.do({
					arg pt, i_;
					var pt_label = pt[0];
					var start_frame = pt[1].asInteger;
					var num_frames = pt[2].asInteger;
					var start_time;

					if((start_frame + num_frames) > buf.numFrames,{
						num_frames = -1;
					});

					start_dur_buf.setn(0,[start_frame,num_frames]);
					ds_ids.addLabel(i_,pt_label);
					start_dur_ds.addPoint(pt_label,start_dur_buf);
					analyze.(pt_label,start_frame,num_frames,buf,features_buf,stats_buf,flat_buf,final_buf,i_);
					start_time = timer.time;

					server.sync;

					"id: % / %\tstart: %\tnum: %\tserver: %\t% / %\ttime delta: %".format(pt_label,full_array.size,start_frame,num_frames,server_i,i_+1,array.size,start_time - prev_time).postln;
					prev_time = start_time;
				});

				server.sync;
				"done with anaysis portion".postln;

				ds_.fromBuffer(final_buf,1,ds_ids,{
					ds_.print;
					ds_.write(dspath,{
						start_dur_ds.write(start_dur_path,{
							// Routine{
							server.quit;
							server_done_booleans[server_i] = true;
							if(server_done_booleans.includes(false).not,{
								// 5.wait;
								final_action.value;
							});
							// }.play;
						});
					});
				});
			}.play;
		};
	};
};