//
//  SergeSubView.hpp
//  serge_01
//
//  Created by macprocomputer on 12/24/21.
//

#ifndef SergeSubView_hpp
#define SergeSubView_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SergeKnob.hpp"
#include "SergeRadio.hpp"

class SergeSubView{
public:
    float draw_x;
    float draw_y;
    float draw_w;
    float draw_h;
    float draw_ratio = 1;

    bool mouseIsCaptured = false;
    int grabbed_knob = -1;
    int grabbed_knob_y = 0;

    vector<SergeGUI*> guis;
    
    vector<SergeDropdown*> dropdowns;
    
    vector<SergeRadio*> radios;
    function<void(SergeGUIEvent event)> callback;


    virtual float getViewHeight(){
        cout << "SergeSubView::getViewHeight in super class\n";
    };
    virtual float getViewWidth(){
        cout << "SergeSubView::getViewWidth in super class\n";
    };
    virtual void draw(){
        cout << "SergeSubView::draw in super class\n";
    };

    void drawCenteredScaled(int win_w, int win_h){
        float view_ratio = getViewWidth() / getViewHeight();
        float win_ratio = win_w / float(win_h);

        if(win_ratio > view_ratio){
            draw_ratio = win_h / getViewHeight();

            draw_w = draw_ratio * getViewWidth();
            draw_h = win_h;

            draw_x = (win_w - draw_w) / 2.f;
            draw_y = 0;

            draw();
        } else {
            draw_ratio = win_w / getViewWidth();

            draw_w = win_w;
            draw_h = draw_ratio * getViewHeight();

            draw_x = 0;
            draw_y = (win_h - draw_h) / 2.f;
            draw();
        }
    }

    void bottomScaled(int win_w, int win_h, int margin){
        draw_x = margin;
        draw_w = win_w - (margin * 2);
        draw_ratio = draw_w / getViewWidth();
        draw_h = getViewHeight() * draw_ratio;
        draw_y = win_h - (draw_h + margin);
    }

    void postDims(){
        cout << draw_x << " " << draw_y << " " << draw_w << " " << draw_h << endl;
    }
    
    void windowMouseMoved(int x, int y){
        for(int i = 0; i < dropdowns.size(); i++){
            dropdowns[i]->windowMouseMoved(x,y);
        }
    }

    void windowMousePressed(float x, float y, KeyModifiers &modifiers){
        cout << "SergeSubView::windowMousePressed " << x << " " << y << endl;
        float mouse[2] = {x,y};
        if(windowPointInFrame(mouse)){
            bool active_dropdowns = false;
            
            for(int i = 0; i < dropdowns.size(); i++){
                if(dropdowns[i]->val > 0.5){
                    active_dropdowns = true;
                    dropdowns[i]->windowMousePressed(x,y, modifiers);
                }
            }
            
            if(!active_dropdowns){
                mouseIsCaptured = true;
                scaleWindowPosToImage(mouse);
                for(int i = 0; i < guis.size(); i++){
                    if(guis[i]->inside(mouse)){
                        grabbed_knob = i;
                        grabbed_knob_y = y;
                        guis[i]->mousePressed(x,y,modifiers);
                        break;
                    }
                }
            }
        }
    }

    void windowMouseReleased(float x, float y){
        grabbed_knob = -1;
        mouseIsCaptured = false;
        for(int i = 0; i < guis.size(); i++){
            guis[i]->mouseReleased();
        }
    }

    void scaleWindowPosToImage(float* mouse){
        mouse[0] = (mouse[0] - draw_x) / draw_ratio;
        mouse[1] = (mouse[1] - draw_y) / draw_ratio;
    }

    bool windowPointInFrame(float* mouse){
        bool left = mouse[0] >= draw_x;
        bool right = mouse[0] < (draw_x + draw_w);
        bool top = mouse[1] >= draw_y;
        bool bottom = mouse[1] < (draw_y + draw_h);
        return left && right && top && bottom;
    }

    bool hasMouseCaptured() {
        bool active_dropdowns = false;
        
        for(int i = 0; i < dropdowns.size(); i++){
            active_dropdowns = dropdowns[i]->val || active_dropdowns;
        }
        
        return mouseIsCaptured || active_dropdowns;
    }
    
    void guiCallback(SergeGUIEvent event){
        
        if(event.radio != -1){
            radios[event.radio]->update(event.index);
        }
        
        callback(event);
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
        for(int i = 0; i < guis.size(); i++){
            guis[i]->setCallback(this,&SergeSubView::guiCallback);
        }
    }
    
    void makeRadios(nlohmann::json json){
        int max_radio = 0;
        for(int i = 0; i < json.size(); i++){
            if(json[i]["radio"].get<int>() > max_radio) max_radio = json[i]["radio"].get<int>();
        }
        
        int n_radios = max_radio + 1;
        
        for(int i = 0; i < n_radios; i++){
            SergeRadio* radio = new SergeRadio;
            radios.push_back(radio);
        }
    }
    
    void updateParamGuis(const vector<double> &params){
        for(int i = 0; i < guis.size(); i++){
            if(guis[i]->param >= 0){
                
                guis[i]->setValue(params[guis[i]->param]);
            }
        }
    }
};

class SergeFBO : public SergeSubView{
    // TODO: long term this should be used for the plot window
public:
    ofFbo fbo;
    void draw(){
        fbo.draw(draw_x,draw_y,draw_w,draw_h);
    }
    float getViewWidth(){
        return fbo.getWidth();
    }
    float getViewHeight(){
        return fbo.getHeight();
    }
};

class SergeImage : public SergeSubView{
public:
    ofImage img;
    
    vector<string> dropdownOptions;
    
    // TODO: allow a SergeSubView to have more than 1 drop down
    void setDropdownOptions(const vector<string> &o){
        dropdownOptions = o;
    }

    void load(string path,SergeGUIItems &guiItems,nlohmann::json &json,bool illuminateKnobs){
        img.load(path);
        readKnobPositions(guiItems,json,illuminateKnobs);
    }

    void readKnobPositions(SergeGUIItems &guiItems,nlohmann::json &json,bool illuminateKnobs){
        
        makeRadios(json);
        
        for(int i = 0; i < json.size(); i++){

            switch(json[i]["type"].get<int>()){
                case KNOB:
                {
                    SergeKnob* knob = new SergeKnob;
                    knob->setup(json[i],guiItems.knob);
                    if(illuminateKnobs) knob->setIllumination(guiItems.illumination);
                    guis.push_back(knob);
                }
                    break;
                case LED:
                {
                    SergeLed* led = new SergeLed;
                    led->setup(json[i],guiItems.led);
                    guis.push_back(led);
                    if(json[i]["radio"].get<int>() != -1) radios[json[i]["radio"].get<int>()]->addGui(led);
                }
                    break;
                case PUSH:
                {
                    SergePush* push = new SergePush;
                    push->setup(json[i],guiItems.push);
                    guis.push_back(push);
                }
                    break;
                case DROPDOWN:
                {
                    SergeDropdown* dd = new SergeDropdown;
                    dd->setup(json[i],guiItems.led);
                    dd->setOptions(dropdownOptions);
                    dd->setFont(guiItems.font);
                    dropdowns.push_back(dd);
                    guis.push_back(dd);
                }
            }
        }
    }
    
    void draw(){
        ofFill();
        ofSetColor(255,255,255,255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofDrawRectangle(draw_x, draw_y, draw_w, draw_h);
        img.draw(draw_x,draw_y,draw_w,draw_h);
        drawKnobs();
    }
    
    void drawKnobs(){
        ofSetRectMode(OF_RECTMODE_CENTER);
        for(int i = 0; i < guis.size(); i++){
            guis[i]->draw(draw_x,draw_y,draw_ratio);
        }
    }
    
    float getViewWidth(){
        return img.getWidth();
    }
    
    float getViewHeight(){
        return img.getHeight();
    }

    void windowMouseDragged(float x, float y){
        if(grabbed_knob != -1){
            guis[grabbed_knob]->increment(grabbed_knob_y - y);
            grabbed_knob_y = y;
        }
    }
};
#endif /* SergeSubView_hpp */
