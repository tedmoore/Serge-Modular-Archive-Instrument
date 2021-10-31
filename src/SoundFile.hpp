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
#include "ofxMaxim.h"
#include "ofxPortSF.h"

class SoundFile {
public:
    void load(string path);
    
    vector<float> soundFile;
    unsigned int playIndices[2] = {0,0};
    
    PSF_PROPS soundFileProperties;
};

#endif /* SoundFile_hpp */
