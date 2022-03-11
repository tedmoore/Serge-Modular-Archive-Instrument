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
    void setPosGate(int sample, int n_frames_, float gate);
    void flipPlayer();
    void threadedFunction();
    
    ofxAudioFile audiofile;
    
    int n_players = 6;
    unsigned long playIndices[6];
    SergeEnv env[6];

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
