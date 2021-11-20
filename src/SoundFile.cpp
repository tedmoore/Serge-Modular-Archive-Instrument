//
//  SoundFile.cpp
//  serge_01
//
//  Created by macprocomputer on 10/31/21.
//

#include "SoundFile.hpp"

void SoundFile::load(string path,int samplerate){
    if(ofFile::doesFileExist(path)){
        audiofile.load(path);
        if (!audiofile.loaded()){
            ofLogError()<<"error loading file, double check the file path";
        }else{
            cout << "successfully loaded " << path << endl;
            init(samplerate);
        }
        
        cout << "loaded: " + path << endl;
    }
}

void SoundFile::init(int samplerate){
    env[0].setup(samplerate * 0.03);
    env[1].setup(samplerate * 0.03);
}
