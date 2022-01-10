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
    vector<int> indices;
    
    void setup(vector<SergeGUI*>& guis_, vector<int> indices_){
        guis = guis_;
        indices = indices_;
    }
    
    void update(int active){
        for(int i = 0; i < indices.size(); i++){
            int index = indices[i];
            if(index == active){
                guis[index]->setValue(1);
            } else {
                guis[index]->setValue(0);
            }
        }
    }
};
#endif /* SergeRadio_hpp */
