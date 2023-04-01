//
//  Slice.hpp
//  calgary01
//
//  Created by Ted Moore on 6/8/21.
//

#ifndef Slice_hpp
#define Slice_hpp

#include <stdio.h>
#include "ofMain.h"

class SoundSlice {
public:
    void setup(vector<ofColor> &rainbow_colors_, string csv_line){
        rainbow_colors = rainbow_colors_;
        current_pos_double.resize(2);
        parse_csv_line(csv_line);
    }
    void parse_csv_line(string csv_line){
        vector<string> tokens = ofSplitString(csv_line,",");
        
        for(int i = 0; i < tokens.size(); i++){
            values.push_back(ofToDouble(tokens[i]));
        }
    }
    void post(){
        for(int i = 0; i < values.size(); i++){
            cout << values[i] << " ";
        }
        cout << endl;
    }
    vector<double> draw(int x_off, int y_off, int width, int height, int x_index, int y_index, int c_index){
        
        ofFill();
        
        ofSetColor(rainbow_colors[int(values[c_index] * 255.f)]);
        
        current_pos_double[0] = values[x_index];
        current_pos_double[1] = 1.f - values[y_index];
        
        current_pos.set(current_pos_double[0], current_pos_double[1]);
        
        int x = x_off + (width * current_pos.x);
        int y = y_off + (height * current_pos.y);
        ofDrawCircle(x,y, 3);
        
        return current_pos_double;
    }
    
    vector<double> values;
    
    ofVec2f prev_pos;
    ofVec2f target_pos;
    ofVec2f current_pos;
    vector<double> current_pos_double;
    
    vector<ofColor> rainbow_colors;
};

#endif /* Slice_hpp */
