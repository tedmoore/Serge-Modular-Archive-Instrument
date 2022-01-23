(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot{
	Routine{
		var ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_server=0.json";
		var play_info_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210404_163827_part1_44k_16b_server=0_start_dur.json";
		var headers_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_headers.csv";
		var audio_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/part1_44k_16b.wav";
		var input_params_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210322_124317_synthesis_params.csv";

		var audio_buf = Buffer.readChannel(s,audio_path,channels:[0]);
		var ds = FluidDataSet(s);
		var headers = CSVFileReader.read(headers_path).flop[1];
		//              spec      pitch, pitchConf, loudness, mfccs
		var view_cols = (0..6) ++ [98,99,126] ++             (155..167);
		var with_stddev_cols = (0..20) ++ (98..101) ++ (155..167) ++ (195..207);
		// var mfcc_cols = (155..167);
		var ds_query = FluidDataSetQuery(s);
		var subset_from_cols = {
			arg full_ds,cols,full_headers,action;
			Routine{˚
				var dsq = FluidDataSetQuery(s);
				var new_ds = FluidDataSet(s);
				var new_headers = List.new;
				s.sync;
				cols.do({
					arg col;
					dsq.addColumn(col);
					new_headers.add(headers[col]);
					s.sync;
				});
				dsq.transform(full_ds,new_ds,{
					action.(new_ds,new_headers);
				});
			}.play;
		};
		var playID = {
			arg id, play_info_ds, play_info_buf;
			play_info_ds.getPoint(id,play_info_buf,{
				{
					var start_samps = BufRd.kr(1,play_info_buf,0,0,1);
					var num_samps = BufRd.kr(1,play_info_buf,1,0,1);
					var sig = PlayBuf.ar(1,audio_buf,BufRateScale.kr(audio_buf),0,start_samps);
					var dur_sec = num_samps / BufSampleRate.kr(audio_buf);
					var env = EnvGen.kr(Env([0,1,1,0],[0.03,dur_sec-0.06,0.03]),doneAction:2);
					sig = sig * env;
					sig.dup;
				}.play;
			});
		};
		var plot_ds = {
			arg new_ds,new_headers;
			Routine{
				var normer = FluidNormalize(s);
				var data = List.new;
				var ids = List.new;
				var play_info = FluidDataSet(s);
				//var play_info_dict = Dictionary.newFrom(["cols",2,"data",Dictionary.new]);
				var play_info_buf = Buffer.alloc(s,2);
				s.sync;
				normer.fitTransform(new_ds,new_ds);
				// play_info_buf.set(1,audio_buf.sampleRate);
				s.sync;
				new_ds.print;
				new_ds.dump({
					arg dict;
					Routine{
						var synth;
						var get_buf = Buffer.alloc(s,714);

						play_info.read(play_info_path);

						s.sync;

						dict.at("data").keysValuesDo{
							arg key, val;
							ids.add(key);
							data.add(val);
						};

						data = data.asArray;
						new_headers = new_headers.asArray;
						ids = ids.asArray;

						synth = {
							var total_dur = 1;
							var fade_dur = 0.03;
							var start_samps = BufRd.kr(1,play_info_buf,0,0,1);
							var trig;
							var sig;
							var which;
							start_samps = Latch.kr(start_samps,Impulse.kr(100));
							trig = Impulse.kr((total_dur-(fade_dur * 2)).reciprocal) + Changed.kr(start_samps);
							which = PulseCount.kr(trig) % 2;
							which = [which,1-which];
							sig = PlayBuf.ar(1,audio_buf,BufRateScale.kr(audio_buf),which,start_samps,0);
							//sig = sig * EnvGen.kr(Env([0,1,1,0],[fade_dur,total_dur-(fade_dur*2),fade_dur]),which);
							sig = sig * EnvGen.kr(Env.asr(fade_dur,1,fade_dur),which);
							sig = Mix(sig) * 0.5;
							SendReply.kr(trig,"/loop_trig");
							sig.dup;
							// sig;
						}.play;

						OSCdef(\loop_trig,{
							arg msg;
							"loop trig".postln;
						},"/loop_trig");

						"synth running".postln;

						defer{
							data.postln;
							new_headers.postln;
							ids.postln;
							Window.closeAll;
							PlotXYColor(data,{
								arg id;
								id.postln;
								play_info.getPoint(id,play_info_buf);
								ds.getPoint(id,get_buf,{
									get_buf.loadToFloatArray(action:{
										arg fa;
										"loudness: %".format(fa[126]).postln;
									});
								});
								//playID.(id,play_info,play_info_buf);
							},new_headers,ids);
						};
					}.play;
				});
			}.play;
		};
		var append_ds = {
			arg ds0, ds1, action;
			ds0.dump({
				arg dict0;
				ds1.dump({
					arg dict1;

					dict1.at("data").keysValuesDo({
						arg key, val;
						if(dict0.at("data").at(key).notNil,{
							dict0.at("data").put(key,dict0.at("data").at(key).addAll(val));
						},{
							"Key % not found in FluidDataSet 0".format(key).error;
						});
					});

					dict0.at("data").keysDo({
						arg key;
						if(dict1.at("data").at(key).isNil,{
							"Key % not found in FluidDataSet 1".format(key).error;
						});
					});

					dict0.put("cols",dict0.at("cols") + dict1.at("cols"));

					Routine{
						var new_ds = FluidDataSet(s);
						s.sync;
						new_ds.load(dict0);
						s.sync;
						action.(new_ds);
					}.play;
				});
			});
		};
		var filter_ds = {
			arg ds, filter_trios, action;
			Routine{
				var dsq = FluidDataSetQuery(s);
				var new_ds = FluidDataSet(s);
				s.sync;
				ds.cols({
					arg cols;
					dsq.addRange(0,cols,{
						Routine{
							filter_trios.do({
								arg arr;
								var col = arr[0];
								var op = arr[1];
								var val = arr[2];
								dsq.filter(col,op,val);
								s.sync;
							});
							dsq.transform(ds,new_ds,{
								action.(new_ds);
							});
						}.play;
					});
				});
			}.play;
		};

		s.sync;
		ds.read(ds_path);
		s.sync;
		ds.print;

		/*		subset_from_cols.(ds,view_cols,headers,{
		arg new_ds, new_headers;
		plot_ds.(new_ds,new_headers);
		});*/

		// subset_from_cols.(ds,mfcc_cols,headers,{
		// 	arg new_ds, new_headers;
		// 	Routine{
		// 		var dimred = FluidUMAP(s,2,30,1);
		// 		//var dimred = FluidPCA(s);
		// 		var dimred_ds = FluidDataSet(s);
		// 		s.sync;
		// 		dimred.fitTransform(new_ds,dimred_ds,{
		// 			append_ds.(dimred_ds,new_ds,{
		// 				arg comb_ds;
		// 				plot_ds.(comb_ds,["dimred0","dimred1"] ++ new_headers);
		// 			});
		// 		});
		// 	}.play;
		// });

		// filter vol, subset, pca, umap
		/*		filter_ds.(ds,[[126,">",-55]],{
		arg new_ds0;
		subset_from_cols.(new_ds0,with_stddev_cols,headers,{
		arg new_ds, new_headers;
		Routine{
		var scaler = FluidStandardize(s);
		var dimred = FluidUMAP(s,2,50,0.9);
		var pca = FluidPCA(s,11);
		var dimred_ds = FluidDataSet(s);
		s.sync;
		scaler.fitTransform(new_ds,dimred_ds,{
		pca.fitTransform(dimred_ds,dimred_ds,{
		dimred.fitTransform(dimred_ds,dimred_ds,{
		append_ds.(dimred_ds,new_ds,{
		arg comb_ds;
		plot_ds.(comb_ds,["dimred0","dimred1"] ++ new_headers);
		});
		});
		});
		});
		}.play;
		});
		});*/

		// filter vol, pca, umap
		filter_ds.(ds,[[126,">",-55]],{
			arg new_ds0;
			Routine{
				var scaler = FluidStandardize(s);
				var pca = FluidPCA(s,35);
				var dimred = FluidUMAP(s,2,50,0.9);
				var dimred_ds = FluidDataSet(s);
				s.sync;
				scaler.fitTransform(new_ds0,dimred_ds,{
					pca.fitTransform(dimred_ds,dimred_ds,{
						dimred.fitTransform(dimred_ds,dimred_ds,{
							plot_ds.(dimred_ds,["dimred0","dimred1"]);
						});
					});
				});
			}.play;
		});
	}.play;
};
)