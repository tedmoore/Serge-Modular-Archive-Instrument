~osc = NetAddr("localhost",2884);

~osc.sendMsg("/param1",rrand(0.0,1.0));
~osc.sendMsg("/param2",rrand(0.0,1.0));
~osc.sendMsg("/param3",rrand(0.0,1.0));
~osc.sendMsg("/param4",rrand(0.0,1.0));
~osc.sendMsg("/x",rrand(0.0,1.0));
~osc.sendMsg("/y",rrand(0.0,1.0));
~osc.sendMsg("/x-axis",rrand(0,6));
~osc.sendMsg("/y-axis",rrand(0,6));
~osc.sendMsg("/color-axis",rrand(0,6));
~osc.sendMsg("/step-seq",rrand(1,16));
~osc.sendMsg("/step-seq-advance");

MIDIClient.init;
MIDIClient.destinations

~midi = MIDIOut(0);

~midi.control(1,1,rrand(0,127));
~midi.control(1,2,rrand(0,127));
~midi.control(1,3,rrand(0,127));
~midi.control(1,4,rrand(0,127));

~midi.control(1,5,rrand(0,127));
~midi.control(1,6,rrand(0,127));

~midi.control(1,101,rrand(0,6));
~midi.control(1,102,rrand(0,6));
~midi.control(1,103,rrand(0,6));

~midi.noteOn(1,rrand(60,75),1);
~midi.noteOn(1,100,1);

// modulate xy
(
s.options.sampleRate_(44100);
s.waitForBoot{
	~smai = NetAddr("localhost",2884);
	{
		SendReply.kr(Impulse.kr(30),"/smai",LFDNoise3.kr(1.dup).range(0,1));
	}.play;
	OSCdef(\smai,{
		arg msg;
		"x: ".post;
		{"*".post} ! (msg[3]*40).asInteger;
		"\ny: ".post;
		{"*".post} ! (msg[4]*40).asInteger;
		"\n\n\n\n".postln;
		~smai.sendMsg("/x",msg[3]);
		~smai.sendMsg("/y",msg[4]);
	},"/smai");
}
)

// modulate 4 knobs
(
s.options.sampleRate_(44100);
s.waitForBoot{
	~smai = NetAddr("localhost",2884);
	{
		SendReply.kr(Impulse.kr(30),"/smai",LFDNoise3.kr(1.dup(4)).range(0,1));
	}.play;
	OSCdef(\smai,{
		arg msg;
		var str = "";
		msg[3..].do{
			arg val, i;
			var id = "/param%".format(i+1);
			~smai.sendMsg(id,val);
			str = str + "\n% ".format(id);
			{str = str + "*"} ! (msg[3+i]*40).asInteger;
		};
		{"".postln} ! 100;
		str.postln;
	},"/smai");
}
)

// step the seq
(
~smai = NetAddr("localhost",2884);
fork{
	var beatdur = 0.1;
	inf.do{
		~smai.sendMsg("/step-seq-advance");
		beatdur.wait;
	};
};
)

// random the seq
(
~smai = NetAddr("localhost",2884);
fork{
	var beatdur = 0.18;
	inf.do{
		~smai.sendMsg("/step-seq",rrand(1,16));
		beatdur.wait;
	};
};
)

// modulate xy & axis
(
s.options.sampleRate_(44100);
s.waitForBoot{
	~smai = NetAddr("localhost",2884);
	{
		SendReply.kr(Impulse.kr(30),"/smai",LFDNoise3.kr(1.4.dup).range(0,1));
	}.play;
	OSCdef(\smai,{
		arg msg;
		"x: ".post;
		{"*".post} ! (msg[3]*40).asInteger;
		"\ny: ".post;
		{"*".post} ! (msg[4]*40).asInteger;
		"\n\n\n\n".postln;
		~smai.sendMsg("/x",msg[3]);
		~smai.sendMsg("/y",msg[4]);
	},"/smai");

	fork{
		inf.do{
			var plots = (0..6).scramble;
			~smai.sendMsg("/x-axis",plots[0]);
			~smai.sendMsg("/y-axis",plots[1]);
			~smai.sendMsg("/color-axis",plots[2]);
			rrand(0.1,0.6).wait;
		};
	};
}
)

// light show
(
~smai = NetAddr("localhost",2884);
fork{
	var order = (1..16).scramble[0..7];
	var plots = order.collect{
		(0..6).scramble[0..2];
	};
	var rhythm = order.collect{
		[1,2,4].choose;
	};
	var beatdur = 0.1;
	inf.do{
		order.do{
			arg step, i;
			~smai.sendMsg("/step-seq",step);
			~smai.sendMsg("/x-axis",plots[i][0]);
			~smai.sendMsg("/y-axis",plots[i][1]);
			~smai.sendMsg("/color-axis",plots[i][2]);
			(rhythm[i] * beatdur).wait;
		}
	};
};
)