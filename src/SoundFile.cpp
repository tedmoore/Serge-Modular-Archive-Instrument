//
//  SoundFile.cpp
//  serge_01
//
//  Created by macprocomputer on 10/31/21.
//

#include "SoundFile.hpp"

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
    
//    cout << "audio file sample: " << audiofile.sample(0,0) << endl;
}

void SoundFile::setPosGate(int sample, int n_frames_, int gate){
    startPoint = sample;
    endPoint = sample + (n_frames_ - fade_dur_samps);
    masterEnv.setValue(gate);
    flipPlayer();
}
