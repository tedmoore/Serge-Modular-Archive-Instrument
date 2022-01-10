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

enum GuiTypeEnum { KNOB, LED, PUSH };

class SergeGUI {
public:
    ofImage img;
};

class SergeKnob : public SergeGUI {
public:
    
    float x, y;
    float normval = 0;
    GuiTypeEnum guiType;
    
    void setup(float x_, float y_, float radius, ofImage &knobImage_, ofImage &ledImage_, ofImage &pushImage_){
        x = x_;
        y = y_;
        knobImage = knobImage_;
        ledImage = ledImage_;
        pushImage = pushImage_;
        if(radius > 25){
        guiType = KNOB;
        } else {
            guiType = LED;
        }
    }
    
    void draw(float xoff, float yoff, float ratio){
        // rotate around center https://forum.openframeworks.cc/t/how-to-rotate-around-center-of-mass/3942/3
        float size = knobImage.getWidth() * ratio;
        ofPushMatrix();
        
        ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
        ofRotateZDeg(normval * 270);
        
        switch(guiType){
            case KNOB:
                knobImage.draw(0,0,size,size);
                break;
            case LED:
                ledImage.draw(0,0,size,size);
                break;
            case PUSH:
                pushImage.draw(0,0,size,size);
                break;
        }
        
        ofPopMatrix();
    }
    
    bool inside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
//        cout << d << "\n";
        return d < (knobImage.getWidth() * 0.5);
    }
    
    bool increment(float pixels){
        normval = ofClamp( normval + (pixels * 0.005), 0.0, 1.0 );
        cout << normval << endl;
        return (normval == 0) || (normval == 1);
    }
};

#endif /* SergeKnob_hpp */
