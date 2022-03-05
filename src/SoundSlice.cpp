//
//  Slice.cpp
//  calgary01
//
//  Created by Ted Moore on 6/8/21.
//

#include "SoundSlice.hpp"

void SoundSlice::setup(vector<ofColor> &rainbow_colors_, ofColor* qualitative_colors_,string csv_line){
    rainbow_colors = rainbow_colors_;
    category_colors = qualitative_colors_;
    current_pos_double.resize(2);
    parse_csv_line(csv_line);
}

void SoundSlice::parse_csv_line(string csv_line){
    vector<string> tokens = ofSplitString(csv_line,",");
    
    for(int i = 0; i < tokens.size(); i++){
        values.push_back(ofToDouble(tokens[i]));
    }
}

vector<double> SoundSlice::draw(int x_off, int y_off, int width, int height, int x_index, int y_index, int c_index){
    
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

void SoundSlice::post(){
    for(int i = 0; i < values.size(); i++){
        cout << values[i] << endl;
    }
}
