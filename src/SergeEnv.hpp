//
//  SergeEnv.hpp
//  serge_01
//
//  Created by macprocomputer on 11/20/21.
//

#ifndef SergeEnv_hpp
#define SergeEnv_hpp

#include <stdio.h>

enum EnvPosition {STILL, UP, DOWN};

class SergeEnv{
public:
//    SergeEnv();
//    ~SergeEnv();
    void setup(int durSamples);
    float tick();
    void setTarget(float target_);
    
    float target = 0;
    int dur; // in samples (ticks)
    float value;
    float rate;
    EnvPosition mode = STILL;
};

float inline SergeEnv::tick(){
    switch (mode) {
        case STILL:
            break;
        case UP:
            value += rate;
            if(value >= target){
                value = target;
                mode = STILL;
            }
            break;
        case DOWN:
            value -= rate;
            if(value <= target){
                value = target;
                mode = STILL;
            }
            break;
    }
    return value;
}

#endif /* SergeEnv_hpp */
