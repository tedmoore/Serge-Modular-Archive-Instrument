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
    void setup();
    int processIncomingMIDI(int chan, int cc);
    void waitForAssignment(int which_handle);
    
    int midi_learn[16][128];
    int currently_assigning_handle = -1;
};

#endif /* MIDIManager2_hpp */
