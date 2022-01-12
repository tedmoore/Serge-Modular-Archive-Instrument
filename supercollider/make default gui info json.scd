(
~outpath = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../bin/data/images/Serge Gui Layout (2022)/gui_info.json";

~plot_locations_path = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../bin/data/images/Serge Gui Layout (2022)/TAUC/TAUC_knob_positions.csv";

~skeuomorph_locations_path = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../bin/data/images/Serge Gui Layout (2022)/3-PANELS/3-PANELS_knob_positions.csv";

~skeuomorph_locations = CSVFileReader.readInterpret(~skeuomorph_locations_path);
~plot_locations = CSVFileReader.readInterpret(~plot_locations_path);

// ~skeuomorph_locations.size.postln;
// ~plot_locations.size.postln;

~json = Dictionary.newFrom([
	"plot",Array.fill(~plot_locations.size,{
		arg i;
		Dictionary.newFrom([
			"index",i,
			"type",1, // 1 = LED
			"radio",-1,
			"radio_index",-1,
			"param",-1,
			"axis",-1,
			"inital_value",0,
			"x",~plot_locations[i][0],
			"y",~plot_locations[i][1],
			"radius",~plot_locations[i][2],
		]);
	}),
	"skeuomorph",Array.fill(~skeuomorph_locations.size,{
		arg i;
		Dictionary.newFrom([
			"index",i,
			"type",0, // 0 = KNOB
			"radio",-1,
			"radio_index",-1,
			"param",-1,
			"axis",-1,
			"inital_value",0.5,
			"x",~skeuomorph_locations[i][0],
			"y",~skeuomorph_locations[i][1],
			"radius",~skeuomorph_locations[i][2],
		]);
	});
]);

// the plot window info will always be the same:
~json["plot"][0]["type"] = 2; // 2 = PUSH

[1,2,3,25,42,43].do{
	arg i;
	~json["plot"][i]["type"] = 0; // 0 = KNOB
	~json["plot"][i]["initial_value"] = 0.5;
};

[(4..10),(11..17),(18..24)].do{
	arg indices, radio_i;
	indices.do{
		arg index;
		// [radio_i,index].postln;
		~json["plot"][index]["radio"] = radio_i;
	}
};

~json["plot"][4]["initial_value"] = 1;
~json["plot"][12]["initial_value"] = 1;
~json["plot"][18]["initial_value"] = 1;

// axes

// axis 0
3.do{
	arg axis;
	7.do{
		arg led;
		~json["plot"][(axis * 7) + led + 4]["axis"] = led;
	}
};

// the skeuomorph window stuff will change:
// which params it should control:
~json["skeuomorph"][65]["param"] = 0;
~json["skeuomorph"][66]["param"] = 1;
~json["skeuomorph"][67]["param"] = 2;
~json["skeuomorph"][77]["param"] = 3;

JSONWriter(~json,~outpath);
)