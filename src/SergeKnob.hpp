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

enum SergeGUIType { KNOB, LED, PUSH };

struct SergeGUIEvent {
    int index = -1;
    float val = -1;
    int param = -1;
    int radio = -1;
    SergeGUIType type = KNOB;
};

class SergeGUI {
public:
    ofImage img;
    float x, y;
    float val = 0;
    int index;
    int param;
    int radio;
    function<void(const SergeGUIEvent event)> callback;

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

    void setup(nlohmann::json &json, ofImage &img_){
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        img = img_;
        index = json["index"].get<int>();
        param = json["param"].get<int>();
        radio = json["radio"].get<int>();
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
//        cout << "callback set\n";
    }

    bool inside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
        return d < (img.getWidth() * 0.5);
    }
    
    bool dispatchEvent(){
        SergeGUIEvent event;
        event.index = index;
        event.val = val;
        event.param = param;
        event.radio = radio;
        event.type = KNOB;

        callback(event);
    }

};

class SergeKnob : public SergeGUI {
public:

    void increment(float pixels){
        val = ofClamp( val + (pixels * 0.005), 0.0, 1.0 );
        dispatchEvent();
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
        dispatchEvent();
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
        dispatchEvent();
    }

    void mouseReleased(){
        val = 0;
    }
};

#endif /* SergeKnob_hpp */
