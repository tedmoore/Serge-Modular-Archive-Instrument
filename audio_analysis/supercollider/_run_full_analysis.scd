(
var source_folder = "/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/serge_patch_recordings/210306_181449/";

var save_folder = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../analyses/%/%".format(PathName(source_folder).folderName,Date.localtime.stamp);
var files = SelectFiles(source_folder,['wav'],recursive:false,verbose:true);
var analyze_file = "01 descriptors_extraction_function_buffer.scd".loadRelative[0];
var compile_datasets = "06 compile datasets.scd".loadRelative[0];
var pca = "10 PCA Explained Variance Ratios.scd".loadRelative[0];
var umap = "12 umap.scd".loadRelative[0];
var toOF = "16_create_data_file_for_oF.scd".loadRelative[0];
var f_r;

save_folder.mkdir;

s.options.sampleRate_(44100);
s.options.inDevice_("MacBook Pro Microphone");
s.options.outDevice_("MacBook Pro Speakers");

f_r = {
	arg array, idx = 0;
	if(idx < array.size,{
		var file_save_folder = save_folder+/+"file_%".format(idx);
		file_save_folder.mkdir;
		analyze_file.(array[idx],1,6,{
			f_r.(array,idx+1);
		},0.0,file_save_folder);
	},{
		"done with analyses".postln;
		compile_datasets.(save_folder,source_folder,{
			arg compiled_dataset_folder;
			pca.(compiled_dataset_folder,{
				arg pca_folder;
				umap.(pca_folder,action:{
					arg umap_folder;
					toOF.(umap_folder,{
						"---- DONE WITH ALL ----".postln;
					});
				});
			});
		});
	});
};

f_r.(files);
)

/*(
var toOF = "16_create_data_file_for_oF.scd".loadRelative[0];
toOF.("/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/analyses/210306_181449/220306_011542/220306_012623_master_datasets/220306_012744_pca/220306_013155_umap/",{
	"=============== done ===============".postln;
})
)*/

(
var compile_datasets = "06 compile datasets.scd".loadRelative[0];
compile_datasets.(
	"/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/analyses/210306_181449/220306_011542/",
	"/Applications/of_v0.11.2_osx_release/apps/myApps/serge_01/audio_analysis/serge_patch_recordings/210306_181449/",
	{"=============== done ===============".postln;}
);
)