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
    range = max - min;
    handle = handle_;
}

double SergeHandle::transform(double x){
    return (x - min) / range;
}

double SergeHandle::inverse_transform(double x){
    return (x * range) + min;
}

double SergeHandle::getNormalizedValue(){
    return transform(handle->getValue());
}

void SergeHandle::setValueFromNormalized(double normalized_value){
    handle->setValue(inverse_transform(normalized_value));
}
