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

class SergeSubView{
public:
    float draw_x;
    float draw_y;
    float draw_w;
    float draw_h;
    
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
            float draw_ratio = win_h / getViewHeight();
            
            draw_w = draw_ratio * getViewWidth();
            draw_h = win_h;

            draw_x = (win_w - draw_w) / 2.f;
            draw_y = 0;
            
            draw();
        } else {
            float draw_ratio = win_w / getViewWidth();
            
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
        float draw_ratio = draw_w / getViewWidth();
        draw_h = getViewHeight() * draw_ratio;
        draw_y = win_h - (draw_h + margin);
    }
    
    void postDims(){
        cout << draw_x << " " << draw_y << " " << draw_w << " " << draw_h << endl;
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
    void load(string path){
        img.load(path);
    }
    void draw(){
        img.draw(draw_x,draw_y,draw_w,draw_h);
    }
    float getViewWidth(){
        return img.getWidth();
    }
    float getViewHeight(){
        return img.getHeight();
    }
};
#endif /* SergeSubView_hpp */
