//
//  SergeRadio.hpp
//  serge_01
//
//  Created by macprocomputer on 1/10/22.
//

#ifndef SergeRadio_hpp
#define SergeRadio_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SergeGUI.hpp"

class SergeRadio {
public:
    vector<SergeGUI*> leds;
    int current_index = -1;
    
    void addGui(SergeGUI* led){
        leds.push_back(led);
    }
    
    // the int it returns is if the radio index of the image index it was sent was found
    // -1 if it was not found
    int update(int image_index){
        int found = -1;
        for(int i = 0; i < leds.size(); i++){
            if(leds[i]->image_index == image_index){
                found = i;
                current_index = i;
                break;
            }
        }
        
        if(found >= 0){
            updateLEDs();
        }
        
        return found;
    }
    
    void updateLEDs(){
        for(int i = 0; i < leds.size(); i++){
            if(i == current_index){
                leds[i]->setOn(true);
            } else {
                leds[i]->setOn(false);
            }
        }
    }
    
    int getCurrentIndex(){
        return current_index;
    }
    
    void setIndex(int index){
        if(index >=0 and index < leds.size()){
            current_index = index;
            updateLEDs();
        }
    }
    
    int advance(){
        current_index = (current_index + 1) % leds.size();
        updateLEDs();
        return current_index;
    }
};
#endif /* SergeRadio_hpp */
