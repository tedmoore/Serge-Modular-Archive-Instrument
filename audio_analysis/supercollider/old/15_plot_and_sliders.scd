(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot{
	Routine{
		var dimred2d_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_022227_normed.json";
		var og_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/analysis.json";
		var loc_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/loc.json";
		var headers_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_headers.csv";
		var audio_paths = [
			"/Users/macprocomputer/Desktop/_serge_archive/210322_124317/part1_44k_16b.wav",
			"/Users/macprocomputer/Desktop/_serge_archive/210322_124317/part2_44k_16b.wav",
			"/Users/macprocomputer/Desktop/_serge_archive/210322_124317/part3_44k_16b.wav"
		];
		var params_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/params.json";

		//var audio_buf = Buffer.readChannel(s,audio_path,channels:[0]);
		var dimred2d_ds = FluidDataSet(s);
		var loc_ds = FluidDataSet(s);
		var og_ds = FluidDataSet(s);
		var params_ds = FluidDataSet(s);
		var headers = CSVFileReader.read(headers_path).flop[1];
		var view_cols = (0..6)++[98,99,126]; // spec shape, pitch, loudness
		var append_ds, subset_from_cols;

		s.sync;

		if(~audio_bufs.isNil,{
			~audio_bufs = audio_paths.collect({
				arg path;
				Buffer.read(s,path);
			});
		},{
			~audio_bufs.postln;
		});

		subset_from_cols = {
			arg full_ds,cols,full_headers,action;
			Routine{
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
		append_ds = {
			arg ds0, ds1, action;
			Routine{
				var dsq = FluidDataSetQuery(s);
				var ds2 = FluidDataSet(s);
				s.sync;
				ds1.cols({
					arg cols;
					dsq.addRange(0,cols,{
						dsq.transformJoin(ds1,ds0,ds2,{
							dsq.free;
							action.(ds2);
						});
					});
				});
			}.play;
		};

		s.sync;
		params_ds.read(params_ds_path,{
			dimred2d_ds.read(dimred2d_ds_path,{
				"2d read".postln;
				loc_ds.read(loc_ds_path,{
					"loc ds read".postln;
					append_ds.(loc_ds,params_ds,{
						arg loc_params_ds;
						"loc params ds made".postln;
						// og_ds.read(og_ds_path,{
						// "og ds read".postln;
						/*subset_from_cols.(og_ds,view_cols,headers,{
						arg subset_ds,subset_headers;
						"subset made".postln;*/
						/*append_ds.(dimred2d_ds,subset_ds,{
						arg comp_ds;
						*/
						Routine{
							var normer = FluidNormalize(s);
							"comp made".postln;

							s.sync;

							// normer.fitTransform(comp_ds,comp_ds,{
							normer.fitTransform(dimred2d_ds,dimred2d_ds,{
								Routine{

									var new_ds = dimred2d_ds;
									// var new_ds = comp_ds;
									// var new_headers = ["0","1"] ++ subset_headers;
									var new_headers = ["0","1"];
									var data = List.new;
									var ids = List.new;
									var params_buf = Buffer.alloc(s,4);
									var xy_buf = Buffer.alloc(s,2);
									var loc_params_buf = Buffer.alloc(s,7);
									var params_to_loc_buf = Buffer.read(s,"/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/params_to_loc_buf.wav");
									var xy_tree = FluidKDTree(s);
									"normer done".postln;

									s.sync;

									xy_tree.fit(dimred2d_ds,{
										"tree fit".postln;

										new_ds.dump({
											arg dict;
											Routine{
												var synth;
												var n_audio_bufs = ~audio_bufs.size;
												var scaler = FluidNormalize(s);
												var paramsBuf = Buffer.alloc(s,4);
												var dim_sizes = 15.dup(4);
												var sliders;
												var many_to_one = {
													arg arr;
													var mul_amt = 1;
													var sum = 0;
													arr.reverse.do({
														arg val, i;
														sum = sum + (val * mul_amt);
														mul_amt = mul_amt * dim_sizes[i];
													});
													sum;
												};

												"dump done".postln;

												dict.at("data").keysValuesDo{
													arg key, val;
													ids.add(key);
													data.add(val);
												};

												data = data.asArray;
												new_headers = new_headers.asArray;
												ids = ids.asArray;
												scaler.fit(params_ds,{
													scaler.dump({
														arg scaler_dict;
														Routine{
															var slider_raw_to_int;
															scaler_dict.postln;
															slider_raw_to_int = 4.collect({
																arg i;
																var cs0 = ControlSpec(scaler_dict.at("data_min")[i],scaler_dict.at("data_max")[i]);
																var cs1 = ControlSpec(0,14,\lin,1);
																Dictionary.newFrom([
																	\raw_to_int,{
																		arg raw;
																		var return;
																		"slider raw to int called: %".format(i).postln;
																		cs0.postln;
																		cs1.postln;
																		raw.postln;
																		return = cs1.map(cs0.unmap(raw)).round;
																		return.postln;
																		return;
																	},
																	\slider_cs,cs0
																]);
															});

															SynthDef(\test123,{
																arg nx = 0, ny = 0, which_stream = 0;
																var total_dur = 1, fade_dur = 0.03, start_samps, trig, sig, which_looper, which_rec, trig_lookup, params;
																var rounded_params, idx, loc_from_params, which_rec_muls;

																// ============ LOOK UP FROM XY POSITION =============
																// continuously write the xy position into this buffer
																FluidKrToBuf.kr([nx,ny],xy_buf);

																// if the x or y position has changed then trigger some stuff...
																trig_lookup = Changed.kr(nx) + Changed.kr(ny);

																// trigger the lookup of the nearest point in the 2d space, write that info into loc_params_buf
																xy_tree.kr(trig_lookup,xy_buf,loc_params_buf,1,loc_params_ds);

																// pull the params out of thta buffer and send to the client
																//"loc_params_buf: %".format(loc_params_buf).postln;
																params = FluidBufToKr.kr(loc_params_buf)[3..];
																SendReply.kr(trig_lookup,"/setParams",params);

																// ============ LOOKUP FROM SYNTH PARAMS ==============
																// receive the 4 synth params
																rounded_params = FluidBufToKr.kr(paramsBuf);

																// use that to get the 1D index of the location of the slice
																idx = many_to_one.(rounded_params);

																// use that index to look up (1) which sound file, (2) offset, (3) n samples (always 1 sec for this)
																loc_from_params = BufRd.kr(3,params_to_loc_buf,idx,0,1);
																// loc_from_params now is: [ which_file , start_sample , n_samples ]

																// ============ WHICH ONE TO USE =================
																which_rec = Select.kr(which_stream,[BufRd.kr(1,loc_params_buf,0,0,1),loc_from_params[0]]);
																start_samps = Select.kr(which_stream,[BufRd.kr(1,loc_params_buf,1,0,1),loc_from_params[1]]);

																which_rec_muls = Select.kr(which_rec,Array.fill(3,{
																	arg i;
																	var list = 0.dup(3);
																	list[i] = 1;
																	//list.postln;
																	list;
																}));

																trig = Impulse.kr((total_dur-(fade_dur * 2)).reciprocal) + Changed.kr(start_samps);
																which_looper = PulseCount.kr(trig) % 2;
																which_looper = [which_looper,1-which_looper];

																sig = SinOsc.ar(440,0,0.1);
																sig = 3.collect({
																	arg chan_i;
																	var buf = NamedControl.kr("buf%".format(chan_i).asSymbol,0,0,true);
																	// var buf = ~audio_bufs[0];
																	var si = PlayBuf.ar(1,buf,BufRateScale.kr(buf),which_looper,start_samps,0);
																	si = si * EnvGen.kr(Env.asr(fade_dur,1,fade_dur),which_looper);
																	si = Mix(si) * 0.5;
																	si;
																});
																// which_rec.poll(label:"which_rec");
																sig = sig * which_rec_muls.lag(0.03);
																sig = Mix(sig) * 3.reciprocal;
																Out.ar(0,sig.dup);
															}).add;

															s.sync;

															synth = Synth(\test123,[\buf0,~audio_bufs[0],\buf1,~audio_bufs[1],\buf2,~audio_bufs[2]]);

															s.sync;

															OSCdef(\setParams,{
																arg msg;
																msg[3..].postln;
																defer{
																	sliders.do({
																		arg sl, i;
																		var val = msg[3+i];
																		sl.value_(val);
																		paramsBuf.set(i,slider_raw_to_int[i].at(\raw_to_int).(val));
																	});
																};
															},"/setParams");

															defer{
																data.postln;
																new_headers.postln;
																ids.postln;
																Window.closeAll;
																PlotXYColor(data,{
																	arg nx, ny;
																	synth.set(\nx,nx,\ny,ny,\which_stream,0);
																	/*id.postln;
																	loc_ds.getPoint(id,loc_buf);*/
																},new_headers,ids,justReturnNormXY:true);


																scaler_dict.postln;
																defer{
																	var win = Window();

																	sliders = scaler_dict.at("cols").collect({
																		arg i;
																		// cs.postln;
																		slider_raw_to_int[i].at(\slider_cs).postln;
																		EZSlider(win,Rect(i * 60,0,60,300),"Param %".format(i),slider_raw_to_int[i].at(\slider_cs),{
																			arg sl;
																			"slider action".postln;
																			sl.postln;
																			sl.value.postln;
																			paramsBuf.set(i,slider_raw_to_int[i].at(\raw_to_int).(sl.value));
																			synth.set(\which_stream,1);
																		},layout:'vert');
																	});

																	win.front;
																};

															};
														}.play;
													});
												});
											}.play;
										});
									});
								}.play;
							});
						}.play;
					});
				});
			});
		});
	}.play;
}
)