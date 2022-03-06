//
//  SergeEnv.hpp
//  serge_01
//
//  Created by macprocomputer on 11/20/21.
//

#ifndef SergeEnv_hpp
#define SergeEnv_hpp

#include <stdio.h>
#include "ofMain.h"

enum EnvPosition {STILL, UP, DOWN};

class SergeEnv{
public:
    void setup(float durSamples);
    float tick();
    void setTarget(float target_);
    void setValue(float value_);
    
    float target = 0;
    float dur; // in samples (ticks)
    float value;
    float rate;
    EnvPosition mode = STILL;
};

void inline SergeEnv::setValue(float value_){
    value = value_;
    mode = STILL;
}

void inline SergeEnv::setTarget(float target_){
    target = target_;
    
    rate = fabs(target - value) / dur;
    
    if(target > value) mode = UP;
    if(target < value) mode = DOWN;
}

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
