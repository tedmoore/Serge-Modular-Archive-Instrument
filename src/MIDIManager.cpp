//
//  MIDIManager2.cpp
//  serge_01
//
//  Created by macprocomputer on 12/21/21.
//

#include "MIDIManager.hpp"
#include "ofMain.h"

void MIDIManager::setup(){
    for(int chan = 0; chan < 16; chan++){
        for(int cc = 0; cc < 128; cc++){
            midi_learn[chan][cc] = -1;
        }
    }
}

void MIDIManager::waitForAssignment(int which_handle){
    currently_assigning_handle = which_handle;
}

int MIDIManager::processIncomingMIDI(int chan, int cc){
    if(currently_assigning_handle != -1){
        midi_learn[chan][cc] = currently_assigning_handle;
        currently_assigning_handle = -1;
    }
    cout << chan << " " << cc << " " << midi_learn[chan][cc] << endl;
    return midi_learn[chan][cc];
}
