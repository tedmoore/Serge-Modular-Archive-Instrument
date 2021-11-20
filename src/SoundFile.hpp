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
    void init(int samplerate);
    
    ofxAudioFile audiofile;
    
    unsigned int playIndices[2] = {0,0};
    
    SergeEnv env[2];
};

inline float SoundFile::tick(){
    
    if(playIndices[0] >= audiofile.length()) playIndices[0] = 0;
    
    return audiofile.sample(playIndices[0]++,0);
}

#endif /* SoundFile_hpp */
