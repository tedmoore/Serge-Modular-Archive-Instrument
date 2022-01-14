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

enum SergeGUIType { KNOB, LED, PUSH, DROPDOWN};

struct SergeGUIEvent {
    int index = -1;
    float val = -1;
    int param = -1;
    int radio = -1;
    int axis = -1;
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
    int axis;
    float radius;
    
    function<void(const SergeGUIEvent event)> callback;

    virtual void increment(float pixels) {
//        cout << "increment in base class\n";
    }
    virtual void draw(float xoff, float yoff, float ratio) {
//        cout << "draw in base class\n";
    }
    virtual void mousePressed(){
//        cout << "mousePressed in base class\n";
    }

    virtual void mouseReleased(){
//        cout << "mouseReleased in base class\n";
    }

    void setValue(float v){
        val = v;
    }

    void setup(nlohmann::json &json, ofImage &img_){
        
//        cout << json << endl;
        img = img_;
        parseJson(json);
    }
    
    void parseJson(nlohmann::json &json){
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        radius = json["radius"].get<float>();
        index = json["index"].get<int>();
        param = json["param"].get<int>();
        radio = json["radio"].get<int>();
        axis = json["axis"].get<int>();
        val = json["initial_value"].get<float>();
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
    }
    
    void setCallback(function<void(SergeGUIEvent event)> cb){
        callback = cb;
    }

    bool inside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
        return d < (img.getWidth() * 0.5);
    }
    
    bool dispatchEvent(SergeGUIType type){
        SergeGUIEvent event;
        event.index = index;
        event.val = val;
        event.param = param;
        event.radio = radio;
        event.type = type;
        event.axis = axis;
        
        callback(event);
    }

};

class SergeKnob : public SergeGUI {
public:
    
    ofImage illumination;
    bool illuminateKnobs = false;
    
    void setIllumination(ofImage &knob_illumination){
        illumination = knob_illumination;
        illuminateKnobs = true;
    }
    
    void increment(float pixels){
        if(param != -1){
            val = ofClamp( val + (pixels * 0.005), 0.0, 1.0 );
            dispatchEvent(KNOB);
        }
    }
    
    void draw(float xoff, float yoff, float ratio){
        // rotate around center https://forum.openframeworks.cc/t/how-to-rotate-around-center-of-mass/3942/3
        float w = img.getWidth() * ratio;
        float h = img.getHeight() * ratio;
        ofPushMatrix();
        ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
        if(illuminateKnobs && (param != -1)){
            float iw = illumination.getWidth() * ratio;
            float ih = illumination.getHeight() * ratio;
            illumination.draw(0,0,iw,ih);
//            cout << "param: " << param << "\tiw " << iw << "\tih: " << ih << endl;
        }
        ofRotateZDeg(val * 270);
        img.draw(0,0,w,h);
        ofPopMatrix();
    }
};

class SergeLed : public SergeGUI {
public:

    void draw(float xoff, float yoff, float ratio){
        if(val != 0){
            float size = img.getWidth() * ratio;
            ofPushMatrix();
            ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
            img.draw(0,0,size,size);
            ofPopMatrix();
        }
    }

    void mousePressed(){
//        cout << "led mousePressed\n";
        dispatchEvent(LED);
    }

    void mouseReleased(){

    }

};

class SergePush : public SergeGUI {
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
        val = 1;
        dispatchEvent(PUSH);
    }

    void mouseReleased(){
        val = 0;
    }
};

class SergeDropdown : public SergeGUI {
public:
    
    vector<string> options;
    
    void setOptions(vector<string> options_){
        options = options_;
    }
    
    void mousePressed(){
        val = !val;
        cout << "x: " << x << "\ty: " << y << "\tradius: " << radius << endl;
        dispatchEvent(DROPDOWN);
    }
    
    void draw(float xoff, float yoff, float ratio){
        
        float size = img.getWidth() * ratio;
        int text_margin = 5;
        float text_height = 50 * ratio;
        ofPushMatrix();
        ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
        img.draw(0,0,size,size);
        
        if(val > 0.5){
            for(int i = 0; i < options.size(); i++){
                int yoff_off = 20 * i;
                
                ofFill();
                ofSetColor(255,255,255,255);
                ofDrawRectangle(0,yoff_off,radius * ratio * 2,text_height);
                
                ofSetLineWidth(7);
                ofNoFill();
                ofSetColor(96,161,207,255);
                ofDrawRectangle(0,yoff_off,radius * ratio * 2,text_height);

                ofSetColor(0,0,0,255);
                ofDrawBitmapString(options[i], (-1 * radius  * ratio) + text_margin, yoff_off + text_margin);
            }
        }
        ofPopMatrix();
    }
};

#endif /* SergeKnob_hpp */
