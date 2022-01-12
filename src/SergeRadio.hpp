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
#include "SergeKnob.hpp"

class SergeRadio {
public:
    vector<SergeGUI*> guis;
    
    void addGui(SergeGUI* gui){
        guis.push_back(gui);
    }
    
    void update(int active){
        for(int i = 0; i < guis.size(); i++){
            if(guis[i]->index == active){
                guis[i]->setValue(1);
            } else {
                guis[i]->setValue(0);
            }
        }
    }
};
#endif /* SergeRadio_hpp */
