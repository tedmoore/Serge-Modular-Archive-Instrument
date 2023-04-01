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
    void drawPlot(bool buildKDTree, bool update_xy_knobs);
    void updateXYKnobs();
    void find_nearest_xy();
    void find_nearest_param(const vector<double> &p);
    bool mouseInPlot(int x, int y);
    void audioOut(float *output, int bufferSize, int nChannels);
    void setPlayingIndex(int index, bool updateSliders);
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
    void drawOptionsMenu();

    void guiCallback(const SergeGUIEvent event);
    void knobCallback(const SergeGUIEvent event);
    void ledCallback(const SergeGUIEvent event);
    void individualKnobCallback(SergeEventType eventType, int param_control, float val){
        
        // if it's a mouse drag, we're just setting the value of that param
        if(eventType == MOUSEDRAGGED){
            params_state.setAt(param_control, val);
            
        // if it's a mouse pressed we can either
        } else if(eventType == MOUSEPRESSED){
            
            if(keyModifiers.command){
                midi_manager.waitForAssignment(param_control);
            } else if (keyModifiers.shift){
                midi_manager.removeAssignment(param_control);
            }
            
        }
    }

    bool loaded = false;

    vector<SoundSlice*> slices;
    vector<ofColor> rainbow_colors;

    ofColor qualitative_colors[12];

    ofFbo plot_fbo;

    // the dimension in the data to acess for the seven leds
    // umap 1, umap 2, pitch, pitch conf, loudness, centroid, flatness
    int axis_selection_lookup[7] = {3,4,7,8,9,5,6};

    int plot_x, plot_y, plot_w, plot_h;
    const int margin = 10;

    ofxKDTree kdTree_2d;
    ofxKDTree kdTree_params;
    vector<unique_ptr<SoundFile>> soundFiles;

    int playing_index = -1;

    ofSoundStream soundstream;

    SergeImage skeuomorph;
    SergeImage plot_controls;

    ofxOscReceiver osc_receiver;

    ChangedAware hid_xy;
    ChangedAware params_state;
    
    // the four 'param' knobs on the right side of the tkb controller
    int plot_control_param_knobs[4] = {1,25,42,43};
    int plot_x_knob_index = 2;
    int plot_y_knob_index = 3;

    // MIDI
    void newMidiMessage(ofxMidiMessage& eventArgs);
    void newMidiControlChange(int cc, int val, int channel);
    void newMidiNoteOn(int note, int vel);
    ofxMidiIn midiIn;
    MIDIManager midi_manager;
    bool redrawPlotNextUpdate = false;

    int skeuomorph_window_width;
    int skeuomorph_window_height;

    nlohmann::json gui_info_json;
        
    SergeGUIItems guiItems;
	KeyModifiers keyModifiers;
    
    const int nParams = 4;
    vector<int> skeuomorph_knob_assignments;
    
    SergeStepSequencer step_sequencer;
    
    SergeRadio x_radio, y_radio, c_radio;
    
    bool show_options_menu = true;
    int selected_midi_port = -1;
    
    vector<string> midi_ports;
    
    ofColor sergeRed = ofColor(202,76,69);
    ofColor sergeBlue = ofColor(96,161,207);
    ofColor background = ofColor(100);
};
