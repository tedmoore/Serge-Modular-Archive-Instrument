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

struct KeyModifiers {
    bool shift = false;
    bool control = false;
    bool option = false;
    bool command = false;
};

struct SergeGUIItems {
    ofImage knob;
    ofImage push;
    ofImage led;
    ofImage illumination;
    ofImage illuminationBlue;
    ofTrueTypeFont font;
};

enum SergeGUIType { KNOB, LED, PUSH };

enum SergeEventType { MOUSEPRESSED, MOUSERELEASED, MOUSEDRAGGED };

class SergeGUIEvent {
    
public:
    
    SergeGUIType guiType = KNOB;
    SergeEventType eventType = MOUSEPRESSED;
    int image_index = -1;
    float val_f = 0;
    int val_i = 0;
    int parent_id = 0;
    
    void dump() const {
        cout << "\nSergeGUIEvent::dump\n";
        cout << "guiType: " << guiType << endl;
        cout << "eventType: " << eventType << endl;
        cout << "parent_id: " << parent_id << endl;
        cout << "image_index: " << image_index << endl;
        cout << "val_f: " << val_f << endl;
        cout << "val_i: " << val_i << endl;
    }
};

class SergeGUI {
public:
    ofImage img;
    int parent_id = 0;
    float x, y, radius;
    float val_f = 0;
    int val_i = 0;
    int image_index = -1;
    
    function<void(const SergeGUIEvent event)> callback;
    function<void()> proprietaryCallback;
    bool hasProprietaryCallback = false;

    virtual void increment(float pixels) {
        //        cout << "increment in base class\n";
    }
    virtual void draw(float xoff, float yoff, float ratio) {
        //        cout << "draw in base class\n";
    }
    virtual void windowMousePressed(int mousex, int mousey, KeyModifiers &mods){
        //        cout << "mousePressed in base class\n";
    }
    
    virtual void mouseReleased(){
        //        cout << "mouseReleased in base class\n";
    }
    
    virtual void setIlluminate(bool il){
        
    }
    
    virtual void setOn(bool b){
        
    }
    
    virtual bool isOn(){
        
    }

    void setValueF(float v){
        val_f = v;
    }
    
    void setValueI(int i){
        val_i = i;
    }
    
    void setup(nlohmann::json &json, ofImage &img_, int image_i){
        image_index = image_i;
        img = img_;
        parseJson(json);
    }
    
    void parseJson(nlohmann::json &json){
        x = json["x"].get<float>();
        y = json["y"].get<float>();
        radius = json["radius"].get<float>();
        val_f = json["initial_value"].get<float>();
        val_i = json["initial_value"].get<int>();
        parent_id = json["parent_id"].get<int>();
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
    }
        
    void setCallback(function<void(SergeGUIEvent event)> cb){
        callback = cb;
    }

    bool isInside(float* scaledmouse){
        float d = sqrt(pow(x - scaledmouse[0],2.0) + pow(y - scaledmouse[1],2.0));
        return d < (img.getWidth() * 0.5);
    }
    
    bool dispatchEvent(SergeGUIType guiType, SergeEventType eventType){
        SergeGUIEvent event;
        event.guiType = guiType;
        event.eventType = eventType;
        event.image_index = image_index;
        event.val_f = val_f;
        event.val_i = val_i;
        event.parent_id = parent_id;
        callback(event);
    }

};

class SergeKnob : public SergeGUI {
public:
    
    ofImage illuminationRed;
    bool bIlluminate = false;
    ofImage illuminationBlue;
    
    void setIlluminate(bool il){
        bIlluminate = true;
    }
    
    void setup(nlohmann::json &json, SergeGUIItems &guiItems, int image_i){
        image_index = image_i;
        illuminationRed = guiItems.illumination;
        illuminationBlue = guiItems.illuminationBlue;
        img = guiItems.knob;
        parseJson(json);
    }
    
    void setIllumination(SergeGUIItems &guiItems){
        illuminationRed = guiItems.illumination;
        illuminationBlue = guiItems.illuminationBlue;
        bIlluminate = true;
    }
    
    void increment(float pixels){
        val_f = ofClamp(val_f + (pixels * 0.005), 0.0, 1.0 );
        dispatchEvent(KNOB,MOUSEDRAGGED);
    }
    
    void windowMousePressed(int mousex, int mousey, KeyModifiers &mods){
        dispatchEvent(KNOB,MOUSEPRESSED);
    }
    
    void draw(float xoff, float yoff, float ratio){
        float w = img.getWidth() * ratio;
        float h = img.getHeight() * ratio;
        ofPushMatrix();
        ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
        if(bIlluminate){
            float iw = illuminationRed.getWidth() * ratio;
            float ih = illuminationRed.getHeight() * ratio;
            illuminationRed.draw(0,0,iw,ih);
        }
        ofRotateZDeg(val_f * 276);
        img.draw(0,0,w,h);
        ofPopMatrix();
    }
};

class SergeLed : public SergeGUI {
public:

    bool on = false;
    
    bool isOn(){
        return on;
    }
    
    void draw(float xoff, float yoff, float ratio){
        if(on){
            float size = img.getWidth() * ratio;
            ofPushMatrix();
            ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
            img.draw(0,0,size,size);
            ofPopMatrix();
        }
    }
    
    void windowMousePressed(int mousex, int mousey, KeyModifiers &mods){
        dispatchEvent(LED,MOUSEPRESSED);
    }

    void mouseReleased(){

    }
    
    void setOn(bool b){
        on = b;
    }
};

class SergePush : public SergeGUI {
public:
    
    bool pushed = false;
    
    void draw(float xoff, float yoff, float ratio){
        if(pushed){
            float size = img.getWidth() * ratio;
            ofPushMatrix();
            ofTranslate((x * ratio) + xoff,(y * ratio) + yoff);
            img.draw(0,0,size,size);
            ofPopMatrix();
        }
    }

    void windowMousePressed(int mousex, int mousey, KeyModifiers &mods){
        pushed = true;
        dispatchEvent(PUSH,MOUSEPRESSED);
    }

    void mouseReleased(){
        pushed = false;
    }
};

#endif /* SergeKnob_hpp */
