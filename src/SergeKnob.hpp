//
//  SergeKnob.hpp
//  serge_01
//
//  Created by macprocomputer on 1/8/22.
//

#ifndef SergeKnob_hpp
#define SergeKnob_hpp

#include <stdio.h>
#include "ofMain.h"

class SergeKnob {
public:
    
    ofImage img;
    float x, y;
    float normval = 0;
    
    void setup(float x_, float y_, ofImage &knobimg){
        x = x_;
        y = y_;
        img = knobimg;
    }
    
    void draw(float xoff, float yoff, float ratio){
        float size = img.getWidth() * ratio;
        img.draw((x * ratio) + xoff - (size/2.0),(y * ratio) + yoff - (size/2.0),size,size);
    }
    
    bool inside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
        cout << d << "\n";
        return d < (img.getWidth() * 0.5);
    }
    
    bool increment(float pixels){
        normval = ofClamp( normval + (pixels * 0.0003), 0.0, 1.0 );
        cout << normval << endl;
        return (normval == 0) || (normval == 1);
    }
};

#endif /* SergeKnob_hpp */
