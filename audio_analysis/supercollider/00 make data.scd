(
// ============================== MAIN PARAMS TO ADJUST ==========================
var input_chan = [0,1,2,3]; // audio from synth to record
var output_chans = [0,1,2]; // CV output channels
var n_steps_per_dim = [4,4,4];
var ranges_for_dimensions = [
	[-0.5,0.5],
	[-1,1],
	[-1,1]
];
var wait_time = 1; // duration of each parameter setting
var run_analysis = true; // flip to true to do it, keep false to test how long it will take
var trig_dur = 0.001; // duration of impulse output
var trig_out_chan = 3; // separate from the CV output channels
var dest_folder = "/Users/ted/Desktop/sc_es_serge/";
// ================================================================================

var max_data_points = 100000;
var max_time_sec = 60 * 60 * 24;

var timestamp = Date.localtime.stamp;
dest_folder = dest_folder+/+timestamp;
File.mkdir(dest_folder);

s.options.numOutputBusChannels_(output_chans.maxItem + 1);
s.waitForBoot({
	if(output_chans.size != n_steps_per_dim.size,{
		"Number of output channels and number of dimensions must match.".warn;
	},{
		var total_points = n_steps_per_dim.product;
		if(total_points > max_data_points,{
			"This is over % data points. It is % data points. Maybe choose less?".format(max_data_points,total_points).warn;
		},{
			var total_time_sec = total_points * wait_time;
			if(total_time_sec > max_time_sec,{
				"This is over the max time: %\nIt is: %\nMaybe choose less?".format(max_time_sec.asTimeString,total_time_sec.asTimeString).warn;
			},{
				"Total Synthesis Points: %".format(total_points).postln;
				"Total Time:             %".format(total_time_sec.asTimeString).postln;
				if(run_analysis,{
					var dims_points = n_steps_per_dim.collect({
						arg n_steps, i;
						var min = ranges_for_dimensions[i][0];
						var max = ranges_for_dimensions[i][1];
						n_steps.collect({arg i; i.linlin(0,n_steps-1,min,max)});
					});
					var synthesis_params = dims_points.allTuples(max_data_points);
					var synthesis_params_file = File(dest_folder+/+"%_synthesis_params.csv".format(timestamp),"w");
					var log_file = File(dest_folder+/+"%_log_file.txt".format(timestamp),"w");

					synthesis_params.do({
						arg line, line_i;
						if(line_i != 0,{synthesis_params_file.write("\n")});
						synthesis_params_file.write("%".format((line_i * wait_time).asTimeString));
						line.do({
							arg val, i;
							synthesis_params_file.write(",%".format(val.asString));
						});
					});

					synthesis_params_file.close;

					log_file.write("Input Channel:                 %\n".format(input_chan));
					log_file.write("Output Channels:               %\n".format(output_chans));
					log_file.write("Number of Steps per Dimension: %\n".format(n_steps_per_dim));
					log_file.write("Time Between Points:           %\n".format(wait_time));
					log_file.write("Total Data Points:             %\n".format(total_points.asInteger));
					log_file.write("Total Time:                    %\n".format(total_time_sec.asTimeString));
					log_file.write("Ranges for Dimensions:         %\n".format(ranges_for_dimensions));
					log_file.close;

					Buffer.loadCollection(s,synthesis_params.flatten,output_chans.size,{
						arg params_buf;
						Routine{
							var rec_buf = Buffer.alloc(s,s.sampleRate.nextPowerOfTwo,input_chan.size);
							var synth;

							s.sync;

							rec_buf.postln;

							rec_buf.write(dest_folder+/+"%_audio_rec.wav".format(timestamp),"wav","int24", 0, 0, true,{

								defer{params_buf.plot};

								synth = {
									var trig = Impulse.ar(wait_time.reciprocal);
									var idx = PulseCount.ar(trig);
									var sig = BufRd.ar(output_chans.size,params_buf,idx-1,0,1);
									var in_sig = SoundIn.ar(input_chan);

									Out.ar(trig_out_chan,Trig.ar(trig,trig_dur));

									sig.poll;

									sig.do({
										arg val, i;
										Out.ar(output_chans[i],val);
									});

									DiskOut.ar(rec_buf,in_sig);

									FreeSelf.kr(A2K.kr(idx > params_buf.numFrames));
								}.play;

								synth.onFree({
									rec_buf.close;
								});
								// defer{s.meter};
							});
						}.play;
					});
				});
			});
		});
	});
});
)