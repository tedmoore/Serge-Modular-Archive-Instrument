//
//  SergeEnv.cpp
//  serge_01
//
//  Created by macprocomputer on 11/20/21.
//

#include "SergeEnv.hpp"
#include "ofMain.h"

void SergeEnv::setup(int durSamples){
    dur = durSamples;
    value = 0;
}

void SergeEnv::setTarget(float target_){
    target = target_;
    
    rate = fabs((target - value) / dur);
    
    if(target > value) mode = UP;
    if(target < value) mode = DOWN;
}
