//
//  ChangedAware.hpp
//  serge_01
//
//  Created by macprocomputer on 3/5/22.
//

#ifndef ChangedAware_hpp
#define ChangedAware_hpp

#include "ofMain.h"
#include <stdio.h>

class ChangedAware {
    
public:
    void setAt(int index, double val){
        vec[index] = val;
        changed = true;
    }
    double getAt(int index){
        return vec[index];
    }
    
    vector<double> get(){
        return vec;
    }
    
    void resize(int size){
        vec.resize(size);
    }
    
    bool changed = false;

private:
    vector<double> vec;
};

#endif /* ChangedAware_hpp */
