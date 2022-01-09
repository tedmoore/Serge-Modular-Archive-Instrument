//
//  SergeSubView.hpp
//  serge_01
//
//  Created by macprocomputer on 12/24/21.
//

#ifndef SergeSubView_hpp
#define SergeSubView_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SergeKnob.hpp"

class SergeSubView{
public:
    float draw_x;
    float draw_y;
    float draw_w;
    float draw_h;
    float draw_ratio = 1;
    
    virtual float getViewHeight(){
        cout << "SergeSubView::getViewHeight in super class\n";
    };
    virtual float getViewWidth(){
        cout << "SergeSubView::getViewWidth in super class\n";
    };
    virtual void draw(){
        cout << "SergeSubView::draw in super class\n";

    };
    
    void drawCenteredScaled(int win_w, int win_h){
        float view_ratio = getViewWidth() / getViewHeight();
        float win_ratio = win_w / float(win_h);
        
        if(win_ratio > view_ratio){
            draw_ratio = win_h / getViewHeight();
            
            draw_w = draw_ratio * getViewWidth();
            draw_h = win_h;

            draw_x = (win_w - draw_w) / 2.f;
            draw_y = 0;
            
            draw();
        } else {
            draw_ratio = win_w / getViewWidth();
            
            draw_w = win_w;
            draw_h = draw_ratio * getViewHeight();
            
            draw_x = 0;
            draw_y = (win_h - draw_h) / 2.f;
            draw();
        }

    }
    
    void bottomScaled(int win_w, int win_h, int margin){
        draw_x = margin;
        draw_w = win_w - (margin * 2);
        draw_ratio = draw_w / getViewWidth();
        draw_h = getViewHeight() * draw_ratio;
        draw_y = win_h - (draw_h + margin);
    }
    
    void postDims(){
        cout << draw_x << " " << draw_y << " " << draw_w << " " << draw_h << endl;
    }
    
    void windowMousePressed(float x, float y){
        if(windowPointInFrame(x,y)){
            float offset_x = x - draw_x;
            float offset_y = y - draw_y;
            float scaled_x = x / draw_ratio;
            float scaled_y = y / draw_ratio;
            
            cout << "mouse pressed: " << x << "\t" << offset_x << "\t" << scaled_x << endl;
            cout << "mouse pressed: " << y << "\t" << offset_y << "\t" << scaled_y << "\n\n";
        }
    }
    
    void windowMouseDragged(float x, float y){
        
        if(windowPointInFrame(x,y)){
            float offset_x = x - draw_x;
            float offset_y = y - draw_y;
            float scaled_x = x / draw_ratio;
            float scaled_y = y / draw_ratio;
            
            cout << "mouse dragged: " << x << "\t" << offset_x << "\t" << scaled_x << endl;
            cout << "mouse dragged: " << y << "\t" << offset_y << "\t" << scaled_y << "\n\n";
        }
    }
    
    bool windowPointInFrame(float x, float y){
//        cout << "SergeSubView::windowPointInFrame: " << x << " " << y << endl;
//        cout << draw_x << " " << draw_w << "\n" << draw_y << " " << draw_h << "\n\n";
//        return (x >= draw_x) && (x < (draw_x + draw_w)) && (y >= draw_h) && (y < (draw_y + draw_h));
        bool left = x >= draw_x;
        bool right = x < (draw_x + draw_w);
        bool top = y >= draw_y;
        bool bottom = y < (draw_y + draw_h);
        return left && right && top && bottom;
    }
};

class SergeFBO : public SergeSubView{
public:
    ofFbo fbo;
    void draw(){
        fbo.draw(draw_x,draw_y,draw_w,draw_h);
    }
    float getViewWidth(){
        return fbo.getWidth();
    }
    float getViewHeight(){
        return fbo.getHeight();
    }
};

class SergeImage : public SergeSubView{
public:
    ofImage img;
    
    vector<SergeKnob*> knobs;
    
    void load(string path,ofImage knobImage){
        img.load(path);
        filesystem::path fs_path = filesystem::path(path);
        readKnobPositions(fs_path.parent_path().parent_path().string() + "/Knobs Only/" + fs_path.stem().string() + "_knob_positions.csv",knobImage);
    }
    void readKnobPositions(string path,ofImage &knobImage){
//        cout << path << endl;
//        filesystem::path fspath = filesystem::path(path);
//        cout << "exists: " << filesystem::exists(fspath) << endl;
        string line;
        ifstream data;
        
        data.open(path);
        
        while(!data.eof()){
            getline(data, line);
            vector<string> tokens = ofSplitString(line,",");
            
            cout << "new knob at: " << tokens[0] << "\t " << tokens[1] << endl;
            
            SergeKnob* knob = new SergeKnob;
            knob->setup(ofToFloat(tokens[0]),ofToFloat(tokens[1]),knobImage);
            knobs.push_back(knob);
        }
        
        data.close();
        
        knobs.resize(knobs.size()-1);
    }
    void draw(){
        img.draw(draw_x,draw_y,draw_w,draw_h);
        drawKnobs();
    }
    void drawKnobs(){
        for(int i = 0; i < knobs.size(); i++){
            knobs[i]->draw(draw_x,draw_y,draw_ratio);
        }
    }
    float getViewWidth(){
        return img.getWidth();
    }
    float getViewHeight(){
        return img.getHeight();
    }
};
#endif /* SergeSubView_hpp */
