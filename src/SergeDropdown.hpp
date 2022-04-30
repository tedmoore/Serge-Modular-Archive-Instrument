//
//  SergeDropdown.hpp
//  serge_01
//
//  Created by macprocomputer on 4/30/22.
//

#ifndef SergeDropdown_hpp
#define SergeDropdown_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SergeGUI.hpp"

struct SergeDropdownOption {
    string name = "default name";
    ofRectangle rect = ofRectangle(0,0,0,0);
    bool highlight = false;
};

class SergeDropdown : public SergeGUI {
public:

    bool showOptions = false;
    vector<SergeDropdownOption> options;
    ofTrueTypeFont font;
    
    void setOptions(vector<string> options_){
        for(int i = 0; i < options_.size(); i++){
            SergeDropdownOption o;
            o.name = options_[i].substr(0,10);
            options.push_back(o);
        }
    }
    
    void setFont(ofTrueTypeFont &font_){
        font = font_;
    }
    
    void windowMousePressed(float mousex, float mousey, KeyModifiers &modifiers){
        
        cout << "SergeDropdown::windowMousePressed " << mousex << " " << mousey << endl;
        
        if(showOptions){
            for(int i = 0; i < options.size(); i++){ // iterate over options
                if(options[i].rect.inside(mousex,mousey)){ // if the mouse is inside one
                    
                    val_i = i; // set the value integer to the index
                    dispatchEvent(DROPDOWN,MOUSEPRESSED);

                    // reset all to not be highlighted
                    for(int i = 0; i < options.size(); i++){
                        options[i].highlight = false;
                    }
                    
                    break;
                }
            }
        } else {
            showOptions = true;
            windowMouseMoved(mousex,mousey);
        }
    }
    
    void windowMouseMoved(int x, int y){
        if(showOptions){
            for(int i = 0; i < options.size(); i++){
                options[i].highlight = options[i].rect.inside(x,y);
            }
        }
    }
        
    void draw(float xoff, float yoff, float ratio){
        
        int text_margin = 5;
        float text_height = 50 * ratio;
        float x_zero = ((x * ratio) + xoff) - (radius * ratio);
        
        // TODO: extract not just "radius" from the Processing script, but also the "width" and "height"
        float y_zero = ((y * ratio) + yoff);

        ofSetRectMode(OF_RECTMODE_CORNER);
        
        img.draw(x_zero,y_zero,img.getWidth() * ratio,img.getHeight() * ratio);
        
        if(showOptions){
            for(int i = 0; i < options.size(); i++){
                int yoff_off = text_height * i;
                
                options[i].rect = ofRectangle(x_zero,y_zero + yoff_off,radius * ratio * 2,text_height);
                
                // draw the background
                ofFill();
                if(options[i].highlight){
                    ofSetColor(202,76,69);
                } else if (i == val_i){
                    ofSetColor(96,161,207,255);
                } else {
                    ofSetColor(255,255,255,255);
                }
                ofDrawRectangle(options[i].rect);
                
                // draw the perimeter of the box
                ofSetLineWidth(3);
                ofNoFill();
                ofSetColor(96,161,207,255);
                ofDrawRectangle(options[i].rect);
                
                // draw the text over the top
                ofFill();
                ofSetColor(0,0,0);
                
                // drawing fonts different sizes: https://forum.openframeworks.cc/t/use-different-fonts/30011/4
                float string_height = font.stringHeight(options[i].name);
                ofPushMatrix();
                ofTranslate(x_zero, y_zero + yoff_off);
                ofScale(ratio);
                font.drawStringAsShapes(options[i].name,text_margin,string_height);
                ofPopMatrix();
            }
        }
    }
};

#endif /* SergeDropdown_hpp */
