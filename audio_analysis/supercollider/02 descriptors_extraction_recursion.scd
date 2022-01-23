(
~files = [
"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/test_audio/1.wav",
"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/test_audio/2.wav"
];

/*~files = [
"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part1_44k_16b.wav",
"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part2_44k_16b.wav",
"/Volumes/Ted's 10TB My Book (June 2020)/Research/serge_archive/210322_124317/part3_44k_16b.wav"
];*/

~f_r = {
	arg array, idx = 0;
	if(idx < array.size,{
		~extract_from_buf.(array[idx],1,3,{
			~f_r.(array,idx+1);
		},0.014);
	});
};

~f_r.(~files);
)