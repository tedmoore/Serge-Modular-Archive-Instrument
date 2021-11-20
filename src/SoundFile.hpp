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
#undef PI
#undef TWO_PI
#include "ADSR.h"

class SoundFile {
public:
    void load(string path);
    float tick();
    void init();
    
    ofxAudioFile audiofile;
    
    unsigned int playIndices[2] = {0,0};
    
    stk::ADSR adsr[2];
};

inline float SoundFile::tick(){
    
    if(playIndices[0] >= audiofile.length()) playIndices[0] = 0;
    
    return audiofile.sample(playIndices[0]++,0) * adsr[0].tick();
}

#endif /* SoundFile_hpp */
