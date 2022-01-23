(
s.options.device_("Fireface UC Mac (24006457)");
s.waitForBoot{
	Routine{
		var dimred2d_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_022227_normed.json";
		var og_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/analysis.json";
		var loc_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/loc.json";
		var headers_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210403_174825_part1_44k_16b/210403_174825_part1_44k_16b_headers.csv";
		var audio_paths = [
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part1_44k_16b.wav",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part2_44k_16b.wav",
			"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part3_44k_16b.wav"
		];
		var params_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/210405_152254_master_datasets/params.json";

		//var audio_buf = Buffer.readChannel(s,audio_path,channels:[0]);
		var dimred2d_ds = FluidDataSet(s);
		var loc_ds = FluidDataSet(s);
		var og_ds = FluidDataSet(s);
		var params_ds = FluidDataSet(s);
		var headers = CSVFileReader.read(headers_path).flop[1];
		var view_cols = (0..6)++[98,126]; // spec shape, pitch, loudness
		var append_ds, plot_ds, subset_from_cols;

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
		plot_ds = {
			arg new_ds,new_headers;

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

					Routine{
						var new_ds = dimred2d_ds;
						var new_headers = ["0","1"];
						var data = List.new;
						var ids = List.new;
						var params_buf = Buffer.alloc(s,4);
						var xy_buf = Buffer.alloc(s,2);
						var loc_buf = Buffer.alloc(s,3);

						var xy_tree = FluidKDTree(s);

						s.sync;

						xy_tree.fit(dimred2d_ds,{
							"tree fit".postln;

							new_ds.dump({
								arg dict;
								Routine{
									var synth;
									var n_audio_bufs = ~audio_bufs.size;
									"dump done".postln;

									dict.at("data").keysValuesDo{
										arg key, val;
										ids.add(key);
										data.add(val);
									};

									data = data.asArray;
									new_headers = new_headers.asArray;
									ids = ids.asArray;

									SynthDef(\test123,{
										arg nx = 0, ny = 0, locBuf;
										var total_dur = 1, fade_dur = 0.03, start_samps, trig, sig, which, which_rec, trig_lookup;

										trig_lookup = Changed.kr(nx) + Changed.kr(ny);
										BufWr.kr(nx,xy_buf,0,0);
										BufWr.kr(ny,xy_buf,1,0);

										xy_tree.kr(trig_lookup,xy_buf,locBuf,1,loc_ds);

										start_samps = BufRd.kr(1,locBuf,1,0,1);
										which_rec = BufRd.kr(1,locBuf,0,0,1);

										which_rec = Select.kr(which_rec,Array.fill(3,{
											arg i;
											var list = 0.dup(3);
											list[i] = 1;
											//list.postln;
											list;
										}));

										trig = Impulse.kr((total_dur-(fade_dur * 2)).reciprocal) + Changed.kr(start_samps);
										which = PulseCount.kr(trig) % 2;
										which = [which,1-which];

										sig = SinOsc.ar(440,0,0.1);
										sig = 3.collect({
											arg chan_i;
											var buf = NamedControl.kr("buf%".format(chan_i).asSymbol,0,0,true);
											// var buf = ~audio_bufs[0];
											var si = PlayBuf.ar(1,buf,BufRateScale.kr(buf),which,start_samps,0);
											si = si * EnvGen.kr(Env.asr(fade_dur,1,fade_dur),which);
											si = Mix(si) * 0.5;
											si;
										});
										sig = sig * which_rec.lag(0.03);
										sig = Mix(sig) * 3.reciprocal;
										Out.ar(0,sig.dup);
									}).add;

									s.sync;

									synth = Synth(\test123,[\buf0,~audio_bufs[0],\buf1,~audio_bufs[1],\buf2,~audio_bufs[2],\locBuf,loc_buf]);

									s.sync;

									defer{
										data.postln;
										new_headers.postln;
										ids.postln;
										Window.closeAll;
										PlotXYColor(data,{
											arg nx, ny;
											synth.set(\nx,nx,\ny,ny);
											/*id.postln;
											loc_ds.getPoint(id,loc_buf);*/
										},new_headers,ids,justReturnNormXY:true);
									};
								}.play;
							});
						});
					}.play;
					/*og_ds.read(og_ds_path,{
					"og ds read".postln;
					subset_from_cols.(og_ds,view_cols,headers,{
					arg subset_ds,subset_headers;
					"subset made".postln;
					append_ds.(dimred2d_ds,subset_ds,{
					arg comp_ds;
					Routine{
					var normer = FluidNormalize(s);
					"comp made".postln;

					s.sync;

					normer.fitTransform(comp_ds,comp_ds,{
					Routine{
					"normer done".postln;

					plot_ds.(comp_ds,["umap0","umap1"] ++ subset_headers);

					}.play;
					});
					}.play;
					});
					});
					});*/
				});
			});
		});
	}.play;
}
)

~audio_bufs