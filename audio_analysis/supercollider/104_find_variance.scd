{
	arg pca, pca_folder, action, target_variance = 0.95;
	pca.dump({
		arg dict;
		var sum = 0;
		var variance_sum_0 = 0;
		var variance_sum_1 = 0;
		var pc_counter = 0;
		var vals = List.new;
		var weights = List.new;
		var abssum = 0.dup(dict.at("bases")[0].size);

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

		while({variance_sum_0 < target_variance},{
			var val = weights[pc_counter];
			pc_counter = pc_counter + 1;
			variance_sum_0 = variance_sum_0 + val;
			// "PC %: %\t\t%".format(pc_counter,val,variance_sum_0).postln;
		});

		ArrayToCSV(weights.collect({
			arg weight, i;
			variance_sum_1 = variance_sum_1 + weight;
			["PC %".format(i + 1),weight,variance_sum_1]
		}),pca_folder+/+"04_pca_explained_variance_ratios.csv");

		action.(pc_counter,variance_sum_0);
	});
}