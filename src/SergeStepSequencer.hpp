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
#include "SergeRadio.hpp"

class SergeStepSequencer {
    
private:
    const int n_steps = 16;
    const char qwerty_order[16] = {'a','w','s','e','d','f','t','g','y','h','u','j','k','o','l','p'};
    int saved_slice_ids[16] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    
    const int midi_note_step1 = 60;
    SergeGUI* pushButton;
    
public:
    
    bool bAwaitingAssignment = false;
    int idWaitingToAssign = -1;
    SergeRadio radio;
    
    void setPushButtonPtr(SergeGUI* pb){
        pushButton = pb;
    }
    
    void associateStep(int step, int slice_id){
        saved_slice_ids[step] = slice_id;
    }
    
    void assignCurrentStep(int slice_id){
        int current_step = radio.getCurrentIndex();
        if(current_step >= 0) saved_slice_ids[current_step] = slice_id;
    }
    
    // returns a -1 if it's not in our qwerty options or there's nothing saved there
    // returns the slice id there if there is something saved there
    int receiveQWERTY(char qwerty){
        for (int i = 0; i < n_steps; i++){
            if(qwerty_order[i] == qwerty){
                radio.setIndex(i);
                return saved_slice_ids[i];
            }
        }
        return -1;
    }
    
    // should only respond to note on messages
    // returns a -1 if the midi note is not one we care about
    int receiveMIDI(int note){
        if(note >= midi_note_step1 && note < midi_note_step1 + n_steps){
            int index = note - midi_note_step1;
            return goToStep(index);
        }
        return -1;
    }
    
    int updateIndex(int index){
        radio.setIndex(index);
        return saved_slice_ids[index];
    }
    
    void randomize(int n_slices){
        for (int i = 0; i < n_steps; i++){
            saved_slice_ids[i] = ofRandom(n_slices);
        }
    }
    
    int mousePressed(int image_index){
        int i = radio.update(image_index);
        if(i >= 0){
            if(bAwaitingAssignment){
                associateStep(i, idWaitingToAssign);
                pushButton->setValueI(0);
                bAwaitingAssignment = false;
            }else{
                return saved_slice_ids[i];
            }
        }
        return -1;
    }
    
    int goToStep(int n){
        if(n < n_steps and n >= 0){
            radio.setIndex(n);
            return saved_slice_ids[n];
        }
        return -1;
    }
    
    int advance(){
        int n = radio.advance();
        return saved_slice_ids[n];
    }
};

#endif /* SergeStepSequencer_hpp */
