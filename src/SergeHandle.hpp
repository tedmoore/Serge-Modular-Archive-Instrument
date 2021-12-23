//
//  SergeHandle.hpp
//  serge_01
//
//  Created by macprocomputer on 12/22/21.
//

#ifndef SergeHandle_hpp
#define SergeHandle_hpp

#include <stdio.h>
#include "ofxDatGui.h"

class SergeHandle {
public:
    void setup(ofxDatGuiSlider* handle_, double min_, double max_);
    double transform(double x);
    double inverse_transform(double x);
    double getNormalizedValue();
    void setValueFromNormalized(double normalized_value);
    
    double min;
    double max;
    double range;
    ofxDatGuiSlider* handle;
};
#endif /* SergeHandle_hpp */
