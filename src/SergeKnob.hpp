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

//enum GuiType { KNOB, LED, PUSH };

class SergeGUI {
public:
    ofImage img;
    float x, y;
    float val = 0;
    
    function<void()> callback;

    void setCallback(function<void()> cb){
        callback = cb;
    }
    
    virtual void increment(float pixels) {
        cout << "increment in base class\n";
    }
    virtual void draw(float xoff, float yoff, float ratio) {
        cout << "draw in base class\n";
    }
    virtual void mousePressed(){
        cout << "mousePressed in base class\n";
    }
    
    virtual void mouseReleased(){
        cout << "mouseReleased in base class\n";
    }
    
    void setValue(float v){
        val = v;
    }
    
    void setup(float x_, float y_, ofImage &img_){
        x = x_;
        y = y_;
        img = img_;
    }
    
    bool inside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
        return d < (img.getWidth() * 0.5);
    }

};

class SergeKnob : public SergeGUI {
public:
        
    void increment(float pixels){
        val = ofClamp( val + (pixels * 0.005), 0.0, 1.0 );
        cout << val << endl;
    }
    
    void draw(float xoff, float yoff, float ratio){
        // rotate around center https://forum.openframeworks.cc/t/how-to-rotate-around-center-of-mass/3942/3
        float size = img.getWidth() * ratio;
        ofPushMatrix();
        ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
        ofRotateZDeg(val * 270);
        img.draw(0,0,size,size);
        ofPopMatrix();
    }
};

class SergeLed : public SergeGUI {
public:
    
    void draw(float xoff, float yoff, float ratio){
        if(val > 0.5){
            float size = img.getWidth() * ratio;
            ofPushMatrix();
            ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
            img.draw(0,0,size,size);
            ofPopMatrix();
        }
    }
    
    void mousePressed(){
        cout << "led mousePressed\n";
        val = !val;
    }
    
    void mouseReleased(){
        
    }
    
};

class SergePush : public SergeGUI {
    
    void draw(float xoff, float yoff, float ratio){
        if(val > 0.5){
            float size = img.getWidth() * ratio;
            ofPushMatrix();
            ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
            img.draw(0,0,size,size);
            ofPopMatrix();
        }
    }
    
    void mousePressed(){
        val = 1;
        callback();
    }
    
    void mouseReleased(){
        val = 0;
    }
};

#endif /* SergeKnob_hpp */
