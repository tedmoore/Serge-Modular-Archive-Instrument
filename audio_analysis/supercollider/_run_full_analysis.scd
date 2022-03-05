(
var source_folder = "/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/serge_patch_recordings/210301/";

var save_folder = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../analyses/%/%".format(PathName(source_folder).folderName,Date.localtime.stamp);
var files = SelectFiles(source_folder,['wav'],recursive:false,verbose:true);
var analyze_file = "01 descriptors_extraction_function_buffer.scd".loadRelative[0];
var f_r;

save_folder.mkdir;

f_r = {
	arg array, idx = 0;
	if(idx < array.size,{
		var file_save_folder = save_folder+/+"file_%".format(idx);
		file_save_folder.mkdir;
		analyze_file.(array[idx],1,4,{
			f_r.(array,idx+1);
		},0.0,file_save_folder);
	},{
		"done with analyses".postln;
		"06 compile datasets.scd".loadRelative[0].(save_folder,source_folder,{
			arg compiled_dataset_folder;
			"10 PCA Explained Variance Ratios.scd".loadRelative[0].(compiled_dataset_folder,{
				arg pca_folder;
				"12 umap.scd".loadRelative[0].(pca_folder,action:{
					arg umap_folder;
					"16_create_data_file_for_oF.scd".loadRelative[0].(umap_folder,{
						"---- DONE WITH ALL ----".postln;
					});
				});
			});
		});
	});
};

f_r.(files);
)