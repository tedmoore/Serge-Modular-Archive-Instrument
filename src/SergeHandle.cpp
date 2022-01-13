//
//  SergeHandle.cpp
//  serge_01
//
//  Created by macprocomputer on 12/22/21.
//

#include "SergeHandle.hpp"

void SergeHandle::setup(double min_, double max_){
    min = min_;
    max = max_;
    range = max - min;
}

double SergeHandle::transform(double x){ // aka normalize
    return (x - min) / range;
}

double SergeHandle::inverse_transform(double x){ // aka un-normalize
    return (x * range) + min;
}

double SergeHandle::getNormalizedValue(){
    return transform(value);
}

void SergeHandle::setValueFromNormalized(double normalized_value){
    value = inverse_transform(normalized_value);
}
