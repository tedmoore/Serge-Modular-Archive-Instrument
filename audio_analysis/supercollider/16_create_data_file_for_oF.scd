(
s.waitForBoot{
	Routine{
		var dimred2d_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/210426_203845_pca_on_analysis/pca304_ds_variance=0.95_umap_210427_022227_normed.json";
		var og_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/analysis.json";
		var loc_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/loc.json";
		var params_ds_path = "/Users/macprocomputer/Desktop/_serge_archive/210322_124317/210405_152254_master_datasets/params.json";

		~dimred2d_ds = FluidDataSet(s);
		~og_ds = FluidDataSet(s);
		~loc_ds = FluidDataSet(s);
		~params_ds = FluidDataSet(s);

		s.sync;

		~dimred2d_ds.read(dimred2d_ds_path,{
			"dim red 2d read".postln;
			~loc_ds.read(loc_ds_path,{
				"loc read".postln;
				~params_ds.read(params_ds_path,{
					"params read".postln;
					~og_ds.read(og_ds_path,{
						"og read".postln;
					});
				});
			});
		});
	}.play;
}
)

(
s.waitForBoot{
	Routine{
		var subset_from_cols = {
			arg full_ds,cols,action;
			Routine{
				var dsq = FluidDataSetQuery(s);
				var new_ds = FluidDataSet(s);
				s.sync;
				cols.do({
					arg col;
					dsq.addColumn(col);
					s.sync;
				});
				dsq.transform(full_ds,new_ds,{
					action.(new_ds);
				});
			}.play;
		};

		var append_ds = {
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

		var select_cols = [0,5,98,99,126]; // spec_cent, spec_flatness, pitch, pitch_conf, loudness
		var n_params = 4;
		var dim_sizes = [15,15,15,15];
		var ranges = [
			ControlSpec(0.0,0.9),
			ControlSpec(0.0,1.0),
			ControlSpec(-1.0,0.7),
			ControlSpec(-1.0,0.7)
		];
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
		var params_to_int = {
			arg val;
			val.collect({
				arg va, i;
				(ranges[i].unmap(va) * (dim_sizes[i] - 1)).round;
			});
		};

		var normer = FluidNormalize(s);
		var og_normed = FluidDataSet(s);

		s.sync;

		append_ds.(~loc_ds,~dimred2d_ds,{
			arg loc_2d_ds;
			subset_from_cols.(~og_ds,select_cols,{
				arg sub_og_ds;
				append_ds.(loc_2d_ds,sub_og_ds,{
					arg loc_2d_subog_ds;

					normer.fitTransform(sub_og_ds,og_normed,{

						append_ds.(loc_2d_subog_ds,og_normed,{
							arg loc_2d_subog_norm_ds;

							append_ds.(loc_2d_subog_norm_ds,~params_ds,{
								arg final_ds;
								final_ds.print;
								Routine{
									var buf = Buffer(s);
									var labels = FluidLabelSet(s);
									s.sync;
									final_ds.toBuffer(buf,labelSet:labels,action:{
										// {buf.plot}.defer;
										Routine{
											var buf_path = "/Users/macprocomputer/Desktop/_serge_archive/supercollider/buffer_writes/%".format(Date.localtime.stamp);
											buf.write("%.wav".format(buf_path),"WAV","float");

											s.sync;

											SoundFile.use("%.wav".format(buf_path),{
												arg sf;
												var fa = FloatArray.newClear(sf.numChannels * sf.numFrames);
												var outfile = Array.fill(sf.numFrames,{Array.newClear(sf.numChannels + 4 + n_params)});
												sf.numChannels.postln;
												sf.numFrames.postln;
												// fa.size.postln;
												sf.readData(fa);
												// fa.size.postln;
												fa = fa.clump(sf.numChannels);

												fa.do{
													arg line, line_number;
													var params_raw_start = 15;
													var n_params_index = 18;
													var params_raw = line[params_raw_start..(params_raw_start+(n_params-1))];
													var params_int = params_to_int.(params_raw).asInteger;
													var param_1d_index = many_to_one.(params_int).asInteger;

													outfile[param_1d_index][0] = param_1d_index;
													outfile[param_1d_index][1] = line[0].asInteger; // file num
													outfile[param_1d_index][2] = line[1].asInteger; // start sample
													outfile[param_1d_index][3] = line[2].asInteger; // num samples

													/*outfile[param_1d_index][4] = line[3]; // umap x norm
													outfile[param_1d_index][5] = line[4]; // umap y norm*/
													outfile[param_1d_index].putEach([4,5],line[3..4]);

													outfile[param_1d_index][6] = 0.0; // grid x norm
													outfile[param_1d_index][7] = 0.0; // grid y norm

													/*											outfile[param_1d_index][8] = line[5]; // spec cent
													outfile[param_1d_index][9] = line[6]; // spec flatness
													outfile[param_1d_index][10] = line[7]; // pitch
													outfile[param_1d_index][11] = line[8]; // pitch_conf
													outfile[param_1d_index][12] = line[9]; // loudness*/
													outfile[param_1d_index].putEach((8..17),line[5..14]);

													outfile[param_1d_index][n_params_index] = n_params.asInteger; // n params

													outfile[param_1d_index].putEach(((n_params_index+1)..(n_params_index+n_params)),params_raw);
													outfile[param_1d_index].putEach(((n_params_index+n_params+1)..(n_params_index+(n_params*2))),params_int);

													if((line_number % 10000) == 0,{
														outfile[param_1d_index].do({
															arg val, column_n;
															"%:\t%".format(column_n,val).postln;
														});
													});
												};

												ArrayToCSV(outfile,"%.csv".format(buf_path));
												"done".postln;
											});
										}.play;
									});
								}.play;
							});
						});
					});
				});
			});
		});

	}.play;
}
)