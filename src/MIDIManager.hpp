//
//  MIDIManager2.hpp
//  serge_01
//
//  Created by macprocomputer on 12/21/21.
//

#ifndef MIDIManager2_hpp
#define MIDIManager2_hpp

#pragma once

#include <stdio.h>

class MIDIManager{
public:
    
    void setup(){
        for(int chan = 0; chan < 16; chan++){
            for(int cc = 0; cc < 128; cc++){
                midi_learn[chan][cc] = -1;
            }
        }
    }

    void waitForAssignment(int which_handle){
        currently_assigning_handle = which_handle;
    }
    
    void removeAssignment(int which_handle){
        for(int chan = 0; chan < 16; chan++){
            for(int cc = 0; cc < 128; cc++){
                if(midi_learn[chan][cc] == which_handle){
                    midi_learn[chan][cc] = -1;
                    return;
                }
            }
        }
    }

    int processIncomingMIDI(int chan, int cc){
        if(currently_assigning_handle != -1){
            midi_learn[chan][cc] = currently_assigning_handle;
            currently_assigning_handle = -1;
        }
        return midi_learn[chan][cc];
    }
    
    void print(){
        cout << "MIDIManager::print()\n";
        for(int chan = 0; chan < 16; chan++){
            for(int cc = 0; cc < 128; cc++){
                if(midi_learn[chan][cc] != -1){
                    cout << "\tChan: " << chan << " \tControl: " << cc << " \tValue: " << midi_learn[chan][cc] << endl;
                }
            }
        }
        cout << "currently_assigning_handle: " << currently_assigning_handle << endl;
    }

    int midi_learn[16][128];
    int currently_assigning_handle = -1;
};

#endif /* MIDIManager2_hpp */
