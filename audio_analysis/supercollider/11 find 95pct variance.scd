{
	arg pca, pca_folder, action;
	pca.dump({
		arg dict;
		var sum = 0;
		var sum1 = 0;
		var sum2 = 0;
		var sum1_i = 0;
		var vals = List.new;
		var weights = List.new;
		var abssum = 0.dup(dict.at("bases")[0].size);
		// var pn = PathName(ds_path).pathOnly+/+Date.localtime.stamp++"_pca_on_%".format(PathName(ds_path).fileNameWithoutExtension);

		"pca done".postln;

		dict.at("values").do{
			arg val;
			var sqr = pow(val.asFloat,2);
			vals.add(sqr);
			sum = sum + sqr;
		};

		vals.do({
			arg val;
			var weight = (val / sum);
			weights.add(weight);
		});

		while({sum1 < 0.95},{
			var val = weights[sum1_i];
			sum1_i = sum1_i + 1;
			sum1 = sum1 + val;
			"PC %: %\t\t%".format(sum1_i,val,sum1).postln;
		});

		ArrayToCSV(weights.collect({
			arg weight, i;
			sum2 = sum2 + weight;
			["PC %".format(i + 1),weight,sum2]
		}),pca_folder+/+"pca0_explained_variance_ratios.csv");

		action.(sum1_i,sum1);
	});
}