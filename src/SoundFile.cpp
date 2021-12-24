//
//  SoundFile.cpp
//  serge_01
//
//  Created by macprocomputer on 10/31/21.
//

#include "SoundFile.hpp"

void SoundFile::setup(string path_, int samplerate_){
    samplerate = samplerate_;
    path = path_;
    fade_dur_samps = samplerate * 0.05;
    env[0].setup(fade_dur_samps);
    env[1].setup(fade_dur_samps);
    masterEnv.setup(fade_dur_samps);
}

void SoundFile::threadedFunction(){
    if(ofFile::doesFileExist(path)){
        audiofile.load(path);
        if (!audiofile.loaded()){
            ofLogError() << "error loading file, double check the file path";
        }else{
            cout << "successfully loaded " << path << endl;
        }
    } else {
        ofLogError() << "error loading file, file path does not exist";
    }
}

void SoundFile::setPosGate(int sample, int n_frames_, int gate){
    startPoint = sample;
    endPoint = sample + (n_frames_ - fade_dur_samps);
//    masterEnv.setTarget(gate);
    masterEnv.setValue(gate);
    flipPlayer();
}
