//
//  SergeStepSequencer.hpp
//  serge_01
//
//  Created by macprocomputer on 4/30/22.
//

#ifndef SergeStepSequencer_hpp
#define SergeStepSequencer_hpp

#include <stdio.h>
#include "ofMain.h"

class SergeStepSequencer {
    
private:
    const int n_steps = 16;
    const char qwerty_order[16] = {'a','w','s','e','d','f','t','g','y','h','u','j','k','o','l','p'};
    int saved_slice_ds[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    
    const int midi_note_step1 = 60;
    
public:
    void setStep(int step, int slice_id){
        saved_slice_ds[step] = slice_id;
    }
    
    // returns a -1 if it's not in our qwerty options or there's nothing saved there
    // returns the slice id there if there is something saved there
    int receiveQWERTY(char qwerty){
        for (int i = 0; i < n_steps; i++){
            if(qwerty_order[i] == qwerty){
                return saved_slice_ds[i];
            }
        }
        return -1;
    }
    
    // should only respond to note on messages
    // returns a -1 if the midi note is not one we care about
    int receiveMIDI(int note){
        if(note >= midi_note_step1 && note < midi_note_step1 + n_steps){
            return saved_slice_ds[note - midi_note_step1];
        }
        return -1;
    }
    
    void randomize(int n_slices){
        for (int i = 0; i < n_steps; i++){
            saved_slice_ds[i] = ofRandom(n_slices);
        }
    }
};

#endif /* SergeStepSequencer_hpp */
