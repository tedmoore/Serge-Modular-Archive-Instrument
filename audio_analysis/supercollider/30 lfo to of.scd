(
s.waitForBoot{
	var serge = NetAddr("localhost",2884);
	4.do{
		arg i;
		var name = "/param%".format(i+1);
		{
			var lfo = LFDNoise3.kr(3).range(0,1);
			SendReply.kr(Impulse.kr(30),name,lfo);
		}.play;

		OSCdef(name[1..].asSymbol,{
			arg msg;

			serge.sendMsg(name,msg[3]);
		},name);
	}
}
)

(
s.waitForBoot{
	var serge = NetAddr("localhost",2884);
	["/x","/y"].do{
		arg name;
		{
			var lfo = LFDNoise3.kr(6).range(0,1);
			SendReply.kr(Impulse.kr(30),name,lfo);
		}.play;

		OSCdef(name[1..].asSymbol,{
			arg msg;
			"%: %".format(name,msg[3]).postln;
			serge.sendMsg(name,msg[3]);
		},name);
	}
}
)