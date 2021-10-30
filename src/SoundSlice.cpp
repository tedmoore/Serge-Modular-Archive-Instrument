//
//  Slice.cpp
//  calgary01
//
//  Created by Ted Moore on 6/8/21.
//

#include "SoundSlice.hpp"
//#include "ofApp.h"

void SoundSlice::setup(vector<ofColor> &rainbow_colors_, ofColor* qualitative_colors_,string csv_line){
    rainbow_colors = rainbow_colors_;
    category_colors = qualitative_colors_;
    parse_csv_line(csv_line);
}

void SoundSlice::parse_csv_line(string csv_line){
    vector<string> tokens = ofSplitString(csv_line,",");
    
    for(int i = 0; i < tokens.size(); i++){
        values.push_back(ofToFloat(tokens[i]));
    }
    
    //cout << values[0] << endl;
}

void SoundSlice::draw(int x_off, int y_off, int width, int height, int x_index, int y_index, int c_index){
    
    ofFill();
    
    ofSetColor(rainbow_colors[int(values[c_index] * 255.f)]);
    current_pos.set(values[x_index], values[y_index]);
    
    int x = x_off + (width * current_pos.x);
    int y = y_off + (height * current_pos.y);
    ofDrawCircle(x,y, 3);
}

void SoundSlice::post(){
    for(int i = 0; i < values.size(); i++){
        cout << values[i] << endl;
    }
}
