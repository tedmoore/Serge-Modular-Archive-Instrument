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