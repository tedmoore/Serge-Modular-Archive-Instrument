//
//  SoundFile.hpp
//  serge_01
//
//  Created by macprocomputer on 10/31/21.
//

#ifndef SoundFile_hpp
#define SoundFile_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxAudioFile.h"
#include "SergeEnv.hpp"

class SoundFile : public ofThread {
public:
    
    void setup(string path_, int samplerate_){
        samplerate = samplerate_;
        path = path_;
        fade_dur_samps = samplerate * 0.05;
        for(int i = 0; i < n_players; i++){
            env[i].setup(fade_dur_samps);
        }
        masterEnv.setup(fade_dur_samps);
        startThread();
    }
    float tick();
    void flipPlayer();
    
    void threadedFunction(){
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

    void setPosGate(int sample, int n_frames_, int gate){
        
        if(gate){
            startPoint = sample;
            endPoint = sample + (n_frames_ - fade_dur_samps);
        }
        
        masterEnv.setTarget(gate);
        flipPlayer();
    }
    
    void post(){
        cout << startPoint << " \t" << endPoint << " \t" << masterEnv.target << endl;
    }
    
    ofxAudioFile audiofile;
    
    int n_players = 4;
    unsigned long playIndices[4];
    SergeEnv env[4];

    int samplerate;
    string path;
    int playingIndex = 0;
    int endPoint = 0;
    int fade_dur_samps;
    int startPoint;
    
    SergeEnv masterEnv;
};

inline float SoundFile::tick(){
    float output = 0;
        
    for(int i = 0; i < n_players; i++){
        if(playIndices[i] >= audiofile.length()) playIndices[i] = 0;
        
        output += audiofile.sample(playIndices[i]++,0) * env[i].tick();
    }
    if(playIndices[playingIndex] >= endPoint) flipPlayer();
    
    output *= masterEnv.tick();
    
    return output;
}

inline void SoundFile::flipPlayer(){
    env[playingIndex].setTarget(0.f);
    
    playingIndex = (playingIndex + 1) % n_players;
    
    playIndices[playingIndex] = startPoint;
    env[playingIndex].setTarget(1.f);
}

#endif /* SoundFile_hpp */
