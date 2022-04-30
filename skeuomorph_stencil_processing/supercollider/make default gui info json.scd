(
~outpath = File.realpath(thisProcess.nowExecutingPath.dirname+/+"../../bin/data/images/Serge Gui Layout (2022)/gui_info.json");

~plot_locations_path = File.realpath(thisProcess.nowExecutingPath.dirname+/+"../../bin/data/images/Serge Gui Layout (2022)/TAUC/TAUC_knob_positions.csv");

~skeuomorph_locations_path = File.realpath(thisProcess.nowExecutingPath.dirname+/+"../../bin/data/images/Serge Gui Layout (2022)/3-PANELS/3-PANELS_knob_positions.csv");

~skeuomorph_locations = CSVFileReader.readInterpret(~skeuomorph_locations_path);
~plot_locations = CSVFileReader.readInterpret(~plot_locations_path);

// ~skeuomorph_locations.size.postln;
// ~plot_locations.size.postln;

~default_plot = {
	arg i;
	Dictionary.newFrom([
		"parent_id",0,
		"gui_type",1, // enum = { KNOB , LED , GUI , DROPDOWN }
		"x",~plot_locations[i][0],
		"y",~plot_locations[i][1],
		"radius",~plot_locations[i][2],
		"initial_value",0.5
	]);
};

~json = Dictionary.newFrom([
	"plot",Array.fill(~plot_locations.size + 1,{ // + 1 for the drop down menu...this will probably need to change
		arg i;
		~default_plot.(i);
	}),
	"skeuomorph",Array.fill(~skeuomorph_locations.size,{
		arg i;
		Dictionary.newFrom([
			"parent_id",1,
			"gui_type",0,
			"x",~skeuomorph_locations[i][0],
			"y",~skeuomorph_locations[i][1],
			"radius",~skeuomorph_locations[i][2],
			"initial_value",0.5
		]);
	});
]);

// the plot window info will always be the same:
~json["plot"][0]["gui_type"] = 2; // 2 = PUSH

[1,2,3,25,42,43].do{
	arg index;
	~json["plot"][index]["gui_type"] = 0; // 0 = KNOB
	~json["plot"][index]["initial_value"] = 0.5;
};

// drop down menu
~json["plot"][~plot_locations.size]["x"] = 2366;
~json["plot"][~plot_locations.size]["y"] = 135;
~json["plot"][~plot_locations.size]["radius"] = 171;
~json["plot"][~plot_locations.size]["gui_type"] = 3; // 3 = DROPDOWN
~json["plot"][~plot_locations.size]["initial_value"] = -1;

JSONWriter(~json,~outpath);

~json.postln;
)