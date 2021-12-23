//
//  SergeHandle.cpp
//  serge_01
//
//  Created by macprocomputer on 12/22/21.
//

#include "SergeHandle.hpp"

void SergeHandle::setup(ofxDatGuiSlider* handle_, double min_, double max_){
    min = min_;
    max = max_;
    handle = handle_;
}
