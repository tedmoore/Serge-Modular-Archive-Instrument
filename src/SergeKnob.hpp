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
    
    void setup(float x_, float y_, ofImage &knobimg){
        x = x_;
        y = y_;
        img = knobimg;
    }
    
    void draw(float xoff, float yoff, float ratio){
        float size = img.getWidth() * ratio;
        img.draw((x * ratio) + xoff - (size/2.0),(y * ratio) + yoff - (size/2.0),size,size);
    }
    
    bool inside(float mousex, float mousey){
        float d = sqrt(pow(x - mousex,2.0) + pow(y - mousey,2.0));
        cout << d << "\n";
        return d < (img.getWidth() * 0.5);
    }
};

#endif /* SergeKnob_hpp */
