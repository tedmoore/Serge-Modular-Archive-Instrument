(
~path = PathName(thisProcess.nowExecutingPath).pathOnly+/+"../bin/data/images/Serge Gui Layout (2022)/gui_info.json";

~n_in_plot_win = 44;
~n_in_skeu_win = 78;

~json = Dictionary.newFrom([
	"plot",Array.fill(~n_in_plot_win,{
		arg i;
		Dictionary.newFrom([
			"index",i,
			"type",1, // 1 = LED
			"radio",-1,
			"param",-1
		]);
	}),
	"skeuomorph",Array.fill(~n_in_skeu_win,{
		arg i;
		Dictionary.newFrom([
			"index",i,
			"type",0, // 0 = KNOB
			"radio",-1,
			"param",-1
		]);
	});
]);

// the plot window info will always be the same:
~json["plot"][0]["type"] = 2; // 2 = PUSH

[1,2,3,25,42,43].do{
	arg i;
	~json["plot"][i]["radio"] = 0; // 0 = KNOB
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

JSONWriter(~json,~path);
)