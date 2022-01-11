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
			"param",-1,
			"x",~plot_locations[i][0],
			"y",~plot_locations[i][1],
			"radius",~plot_locations[i][2],
		]);
	}),
	"skeuomorph",Array.fill(~n_in_skeu_win,{
		arg i;
		Dictionary.newFrom([
			"index",i,
			"type",0, // 0 = KNOB
			"radio",-1,
			"param",-1,
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
};

[(4..10),(11..17),(18..24)].do{
	arg indices, radio_i;
	indices.do{
		arg index;
		// [radio_i,index].postln;
		~json["plot"][index]["radio"] = radio_i;
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