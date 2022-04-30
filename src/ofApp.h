#pragma once

#include "ofMain.h"
#include "MIDIManager.hpp"
#include "SoundSlice.hpp"
#include "ofxGui.h"
#include "ofxKDTree.h"
#include "SoundFile.hpp"
#include "ofxSvg.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "SergeSubView.hpp"
#include "thirdparty/nlohmann/json.hpp"
#include "ChangedAware.hpp"
#include "SergeStepSequencer.hpp"

class ofApp : public ofBaseApp, public ofxMidiListener {

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void masterKeyPressed(int key);
    void keyReleased(int key);
    void skeuomorphKeyReleased(ofKeyEventArgs& args);
    void masterKeyReleased(int key);
    
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
    void find_nearest_xy();
    void find_nearest_param(const vector<double> &p);
    bool mouseInPlot(int x, int y);
    void audioOut(float *output, int bufferSize, int nChannels);
    void setPlayingIndex(size_t index, bool updateSliders);
    void createColors();
    void readSoundSlicesData(string csv_path);
    void createPointKDTree();
    void processIncomingMouseXY(int x, int y);
    void loadingScreen();
    void drawPlotWindow();
    void loadDirectory(string path);
    void jsonToGuiTypes(nlohmann::json json, string key);

    void drawSkeuomorph(ofEventArgs & args);
    void setupSkeuomorph();
    void skeuomorphKeyPressed(ofKeyEventArgs& args);
    void skeuomorphMousePressed(ofMouseEventArgs& args);
    void skeuomorphMouseDragged(ofMouseEventArgs& args);
    void skeuomorphResized(ofResizeEventArgs& args);
    void skeuomorphMouseReleased(ofMouseEventArgs& args);

    void processMIDI();
    void processOSC();

    void guiCallback(const SergeGUIEvent event);
    void knobCallback(const SergeGUIEvent event);

    bool loaded = false;

    vector<SoundSlice*> slices;
    vector<ofColor> rainbow_colors;

    ofColor qualitative_colors[12];

    ofFbo plot_fbo;

    bool is_moving = false;
    float interp = 0;

    int x_index_i = 3;
    int y_index_i = 4;
    int c_index_i = 3;

    int axis_selection_lookup[7] = {3,4,7,8,9,5,6};

    int plot_x, plot_y, plot_w, plot_h;
    int margin = 10;

    ofxKDTree kdTree_2d;

    ofxKDTree kdTree_params;

    vector<unique_ptr<SoundFile>> soundFiles;

    int playing_index = -1;

    ofSoundStream soundstream;

    SergeImage three_panel;
    SergeImage tkb;

    ofxOscReceiver osc_receiver;

    ChangedAware hid_xy;
    ChangedAware params_state;
    
    int tkb_param_knobs[4] = {1,25,42,43};

    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    ofxMidiIn midiIn;
    MIDIManager midi_manager;

    int skeuomorph_window_width;
    int skeuomorph_window_height;

    nlohmann::json gui_info_json;
        
    SergeGUIItems guiItems;

	KeyModifiers keyModifiers;
    
    int nParams = 4;
    
    SergeStepSequencer step_sequencer;
};
