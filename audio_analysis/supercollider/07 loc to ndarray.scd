(
s.waitForBoot({
	Routine{
		var params_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210405_152254_master_datasets/params.json";
		var loc_ds_path = "/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_with_jean/210322_124317/210405_152254_master_datasets/loc.json";
		var params_ds = FluidDataSet(s);
		var loc_ds = FluidDataSet(s);
		var dim_sizes = [15,15,15,15];
		var ranges = [
			ControlSpec(0.0,0.9),
			ControlSpec(0.0,1.0),
			ControlSpec(-1.0,0.7),
			ControlSpec(-1.0,0.7)
		];
		var loc_array;
		var loc_buf;
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

		s.sync;

		params_ds.read(params_ds_path);
		loc_ds.read(loc_ds_path);

		s.sync;

		params_ds.size({
			arg size;
			loc_array = Array.newClear(size);
		});
		"array made".postln;

		s.sync;

		params_ds.dump({
			arg params_dict;
			loc_ds.dump({
				arg loc_dict;
				Routine{
					params_dict.at("data").keysValuesDo({
						arg key, val;
						var scaled;
						var idx;
						scaled = val.collect({
							arg va, i;
							(ranges[i].unmap(va) * (dim_sizes[i] - 1)).round;
						});
						idx = many_to_one.(scaled);
						if(loc_array[idx].notNil,{
							"something already at %".format(idx).error;
							val.postln;
							scaled.postln;
							loc_array[idx].postln;
							"".postln;
						});

						loc_array[idx] = loc_dict.at("data").at(key);
						// loc_array[idx].postln;
					});

					loc_buf = Buffer.loadCollection(s,loc_array.flatten,3,{
						arg buf;
						var buf_path = PathName(loc_ds_path).pathOnly+/+"params_to_loc_buf.wav";
						buf.write(buf_path,"wav","float");
						defer{buf.plot};
					});
				}.play;
			});
		});

	}.play;
});
)