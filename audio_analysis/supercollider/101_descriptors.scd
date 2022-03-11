{
	arg recs_folder, slice_dur_samps = 44100, offset_samps = 0, action;
	var recs_folder_pn = PathName(recs_folder);
	var wavs_pn = recs_folder_pn.files.select{arg path; path.extension == "wav"};
	var params_csv = CSVFileReader.readInterpret(
		recs_folder_pn.files.select{
			arg path; path.fileName.contains("synthesis_params.csv")
		}[0].fullPath
	);
	var master_slice_i = 0;
	var n_frames_arr = wavs_pn.collect{arg path; SoundFile.use(path.fullPath,{
		arg sf;
		if(sf.numChannels > 1,{"WAV files must be mono".throw});
		if(sf.sampleRate != 44100,{"WAV file sample rate must be 44100"});
		sf.numFrames;
	})};
	var n_slices_arr = (n_frames_arr / slice_dur_samps).round;
	var slices_array = Array.newClear(n_slices_arr.sum);
	var loc_ds_dict = Dictionary.newFrom(["cols",3,"data",Dictionary.new]);
	var params_ds_dict = Dictionary.newFrom(["cols",params_csv.shape[1],"data",Dictionary.new]);
	var outfolder = File.realpath(recs_folder+/+"../../analyses")+/+recs_folder_pn.folderName+/+Date.localtime.stamp;
	var server_options = ServerOptions();
	var completed_bools = false.dup(wavs_pn.size);

	var statsFlatComp = {
		arg featuresBuf,statsBuf,flatBuf,masterBuf,masterBufOffset_frames;
		FluidBufStats.processBlocking(featuresBuf.server,featuresBuf,stats:statsBuf,numDerivs:1);
		FluidBufFlatten.processBlocking(featuresBuf.server,statsBuf,destination:flatBuf);
		FluidBufCompose.processBlocking(
			featuresBuf.server,
			flatBuf,
			destination:masterBuf,
			destStartFrame:masterBufOffset_frames
		);
	};

	var analyze = {
		arg id, start_frame, num_frames, buf, featuresBuf,statsBuf, flatBuf,pointBuf, ds;
		FluidBufSpectralShape.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,pointBuf,0);
		FluidBufPitch.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,pointBuf,98); // 98 = 7*7*2
		FluidBufLoudness.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,pointBuf,126); // 126 = 98 + 28; // 28 = (2*7*2)
		FluidBufMFCC.processBlocking(buf.server,buf,start_frame,num_frames,features:featuresBuf,numCoeffs:40);
		statsFlatComp.(featuresBuf,statsBuf,flatBuf,pointBuf,154); // 154=126+28; // 28=(2*7*2)
		ds.addPoint(id,pointBuf);
	};

	outfolder.mkdir;

	"wave files: %".format(wavs_pn).postln;
	"params_csv: %".format(params_csv).postln;
	"slices array len: %".format(slices_array.size).postln;

	n_slices_arr.do{
		arg n_slices, wav_i;
		n_slices.do{
			arg file_slice_i;
			var start_samp = ((file_slice_i * slice_dur_samps) + offset_samps).asInteger;
			var end_samp = min(start_samp + slice_dur_samps,n_frames_arr[wav_i]).asInteger;
			var num_samps = end_samp - start_samp;

			slices_array[master_slice_i] = [master_slice_i,wav_i,start_samp,num_samps];
			loc_ds_dict["data"][master_slice_i] = [wav_i,start_samp,num_samps];
			params_ds_dict["data"][master_slice_i] = params_csv[master_slice_i];

			master_slice_i = master_slice_i + 1;
		};
	};

	"slices array shape: %".format(slices_array.shape).postln;
	"slice array 2000: %".format(slices_array[2000]).postln;

	Server.killAll;

	server_options.inDevice_("MacBook Pro Microphone");
	server_options.outDevice_("MacBook Pro Speakers");
	server_options.sampleRate_(44100);

	wavs_pn.do{
		arg wav_pn, wav_i;
		var file_sub_array = slices_array.select{arg sa; sa[1] == wav_i};

		var server = Server("Server-%-%".format(wav_i,UniqueID.next),NetAddr("localhost",58000 + wav_i),server_options);

		server.waitForBoot{
			var audio_file = Buffer.read(server,wav_pn.fullPath);
			var ds = FluidDataSet(server);
			var featuresBuf = Buffer(server);
			var statsBuf = Buffer(server);
			var flatBuf = Buffer(server);
			var pointBuf = Buffer(server);

			server.sync;

			file_sub_array.do{
				arg arr, file_sub_array_counter;
				var id_int = arr[0];
				var wav_i = arr[1];
				var startFrame = arr[2];
				var numFrames = arr[3];
				"wav %\t% / %\tserver: %\t% / %".format(
					wav_i,
					(file_sub_array_counter+1).asString.padLeft(6),
					file_sub_array.size.asString.padLeft(6),
					wav_i,
					id_int.asString.padLeft(6),
					slices_array.size.asString.padLeft(6)
				).postln;
				analyze.(id_int,startFrame,numFrames,audio_file,featuresBuf,statsBuf,flatBuf,pointBuf,ds);
				server.sync;
			};

			ds.write(outfolder+/+"00_descriptors_wav%.json".format(wav_i));

			server.sync;

			server.quit;

			completed_bools[wav_i] = true;

			if(completed_bools.includes(false).not,{

				s.options.sampleRate_(44100);
				s.options.inDevice_("MacBook Pro Microphone");
				s.options.outDevice_("MacBook Pro Speakers");

				s.waitForBoot{
					var loc_ds = FluidDataSet(s);
					var params_ds = FluidDataSet(s);
					loc_ds.load(loc_ds_dict);
					loc_ds.write(outfolder+/+"01_loc.json");
					params_ds.load(params_ds_dict);
					params_ds.write(outfolder+/+"02_params.json");
					s.sync;
					action.value(outfolder,params_ds,loc_ds);
				}
			})
		};
	};
}