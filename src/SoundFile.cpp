//
//  SoundFile.cpp
//  serge_01
//
//  Created by macprocomputer on 10/31/21.
//

#include "SoundFile.hpp"

void SoundFile::load(string path){
    const char *filePath = path.c_str();// convert from fancy string to C string
    
    int ifd = -1;
    float* frame = NULL;
    
    ifd = psf_sndOpen(filePath, &soundFileProperties, 0);//open the sound file
    
    long framesRead, totalFramesRead;
    
    if (ifd < 0) {//this can happen if the file does not exist or the format is not supported
        ofSystemAlertDialog("Error loading file: " + path);
    } else {
        frame = (float*)malloc(soundFileProperties.chans * sizeof(float));//set the size of frame to be the nSamples per Frame
        framesRead = psf_sndReadFloatFrames(ifd, frame, 1);//grab the first frame
        totalFramesRead = 0;
        soundFile.clear();//clear the vector that the soundFile will be stored in
        while (framesRead == 1) {//while there are still frames to be read
            totalFramesRead++;//keep track of number of frames;
            for (int i = 0; i < soundFileProperties.chans; i++) {//for every sample in the frame
                soundFile.push_back(frame[i]);//add frame to the soundFile vector
            }
            framesRead = psf_sndReadFloatFrames(ifd, frame, 1);//returns 1 if there are still more frames
            //cout << framesRead << " " << endl;
        }
        
        cout << "loaded: " + path << endl;
    }
}
