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

class SoundFile {
public:
    void load(string path, int samplerate);
    float tick();
    void setPosGate(int sample, int n_frames_, int gate);
    void init(int samplerate);
    void flipPlayer();
    
    ofxAudioFile audiofile;
    
    unsigned int playIndices[2] = {0,0};
    
    int playingIndex = 0;
    int endPoint = 0;
    int fade_dur_samps;
    int startPoint;
    
    SergeEnv env[2];
    SergeEnv masterEnv;
};

inline float SoundFile::tick(){
    float output = 0;
    
    for(int i = 0; i < 2; i++){
        if(playIndices[i] >= audiofile.length()) playIndices[i] = 0;
        output += audiofile.sample(playIndices[i]++,0) * env[i].tick();
        
        if(playIndices[playingIndex] >= endPoint) flipPlayer();
    }
    
    output *= masterEnv.tick();
    
    return output;
}

inline void SoundFile::flipPlayer(){
    env[playingIndex].setTarget(0);
    
    playingIndex = 1 - playingIndex;
    
    playIndices[playingIndex] = startPoint;
    env[playingIndex].setTarget(1);
}

#endif /* SoundFile_hpp */
