//
//  Slice.hpp
//  calgary01
//
//  Created by Ted Moore on 6/8/21.
//

#ifndef Slice_hpp
#define Slice_hpp

#include <stdio.h>
//#include "ofApp.h"
#include "ofMain.h"

class SoundSlice {
public:
    void setup(vector<ofColor> &rainbow_colors_, ofColor* qualitative_colors_, string csv_line);
    void parse_csv_line(string csv_line);
    void post();
    vector<double> draw(int x_off, int y_off, int width, int height, int x_index, int y_index, int c_index);
    
    vector<float> values;
    
    ofVec2f prev_pos;
    ofVec2f target_pos;
    ofVec2f current_pos;
    vector<double> current_pos_double;
    
    vector<ofColor> rainbow_colors;
    ofColor* category_colors;
};

#endif /* Slice_hpp */
