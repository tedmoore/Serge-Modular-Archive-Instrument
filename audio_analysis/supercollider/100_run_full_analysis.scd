(
var recs_folder = "/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/serge_patch_recordings/210306_181449/";

var descriptors = "101_descriptors.scd".loadRelative[0];
var merge = "102_merge_datasets.scd".loadRelative[0];
var pca = "103_pca.scd".loadRelative[0];
var umap = "105_umap.scd".loadRelative[0];
var format = "106_format_for_oF.scd".loadRelative[0];

descriptors.(recs_folder,action:{
	arg analysis_folder, ds_params, ds_loc;
	"--- done with descriptor analysis ---".postln;
	merge.(analysis_folder,action:{
		arg ds_analysis;
		"--- done with merge ---".postln;
		pca.(ds_analysis,analysis_folder,{
			arg ds_pca;
			"--- done with pca ---".postln;
			umap.(ds_pca,analysis_folder,action:{
				arg ds_umap;
				"--- done with umap ---".postln;
				format.(ds_analysis,ds_params,ds_loc,ds_umap,analysis_folder,{
					"--- done with all ---".postln;
				});
			});
		});
	});
});
)