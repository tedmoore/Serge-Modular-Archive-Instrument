#pragma once

#include "ofMain.h"
#include "SoundSlice.hpp"
#include "ofxGui.h"
#include "ofxDatGui.h"
#include "ofxKDTree.h"
#include "SoundFile.hpp"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void drawPlot(bool buildKDTree);
    void find_nearest(int x, int y);
    bool mouseInPlot(int x, int y);
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void audioOut(float *output, int bufferSize, int nChannels);
    void setPlayingIndex(int index, bool updateSliders);

    ofxDatGui* gui;
    ofxDatGuiDropdown* x_menu;
    ofxDatGuiDropdown* y_menu;
    ofxDatGuiDropdown* c_menu;
    void onDropdownEventX(ofxDatGuiDropdownEvent e);
    void onDropdownEventY(ofxDatGuiDropdownEvent e);
    void onDropdownEventC(ofxDatGuiDropdownEvent e);

    ofxDatGuiSlider* sliders[4];
    
    vector<SoundSlice*> slices;
    vector<ofColor> rainbow_colors;
    
    ofColor qualitative_colors[12];
    
    ofFbo plot_fbo;
    
    bool is_moving = false;
    float interp = 0;
    
    int x_index_sl = 4;
    int y_index_sl = 5;
    int c_index_sl = 4;
    
    vector<string> dropdown_options;
    vector<int> dropdown_index_lookup;
    
    int plot_x, plot_y, plot_w, plot_h;
    int margin = 10;
    int menu_width = 300;
    
    ofxKDTree kdTree_2d;
    
    ofxKDTree kdTree_params;
    
    vector<SoundFile> soundFiles;
    
    int playing_index = -1;
    
    ofSoundStream soundstream;
};