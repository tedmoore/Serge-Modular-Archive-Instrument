#include "ofApp.h"

#define SAMPLERATE 44100
//-------------------------------------------------------------

void ofApp::loadDirectory(string path){

    ofDirectory load_folder(path);
    
    load_folder.listDir();
    load_folder.sort();
    vector<ofFile> files = load_folder.getFiles();
    
    for(int i = 0; i < files.size(); i++){
        if(files[i].getExtension() == "wav"){
//            cout << "loading " << files[i].getAbsolutePath() << endl;
            soundFiles.push_back(move(unique_ptr<SoundFile>(new SoundFile)));
            soundFiles.back()->setup(files[i].getAbsolutePath(),SAMPLERATE);
        } else if(files[i].getExtension() == "csv") {
            readSoundSlicesData(files[i].getAbsolutePath());
        }
    }

    createPointKDTree();
}

void ofApp::setupSkeuomorph(){
    cout.precision(12);
    
    guiItems.font.load(ofToDataPath("OpenSans-Light.ttf"), 16,true,true,true,0.f);
    guiItems.knob.load(ofToDataPath("images/Serge Gui Layout (2022)/DAVIES_KNOB.png"));
    guiItems.illumination.load(ofToDataPath("images/Serge Gui Layout (2022)/KNOB_ILLUMINATION.png"));
    guiItems.push.load(ofToDataPath("images/Serge Gui Layout (2022)/BUTTON_PRESSED.png"));
    guiItems.led.load(ofToDataPath("images/Serge Gui Layout (2022)/LED_ON.png"));

    std::ifstream i(ofToDataPath("images/Serge Gui Layout (2022)/gui_info.json"));
    i >> gui_info_json;
    
    three_panel.load(ofToDataPath("images/Serge GUI Layout (2022)/3-PANELS/3-PANELS.png"),guiItems,gui_info_json["skeuomorph"],true);
    three_panel.setCallback(this, &ofApp::guiCallback);

    skeuomorph_window_width = ofGetScreenWidth() / 2;
    skeuomorph_window_height = 1972; // If this is not hard coded, it displays incorrectly. I had tried ofGetScreenHeight(), and even tried to offset for the menu bar, but it would always display incorrectly.
    
    hid_xy.resize(2);
    params_state.resize(nParams);
}

void ofApp::setup(){
    osc_receiver.setup(2884);
    ofSetFrameRate(30);
    ofBackground(100);
    ofEnableAntiAliasing();
        
    createColors();

    ofFileDialogResult result = ofSystemLoadDialog("Load folder with wav files and data file",true);
    if(result.bSuccess) {
      loadDirectory(result.getPath());
    }

    windowResized(ofGetWidth(),ofGetHeight());
    drawPlot(true);

    soundstream.setup(2, 0, SAMPLERATE, 256, 4);

    // MIDI
    midi_manager.setup();

    // open port by number (you may need to change this)
    midiIn.openPort(0);
    
    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);

    // add ofApp as a listener
    midiIn.addListener(this);

    // =========== GRAPHICS ================
	tkb.setDropdownOptions(midiIn.getInPortList());    
	tkb.load(ofToDataPath("images/Serge GUI Layout (2022)/TAUC/TAUC.png"),guiItems,gui_info_json["plot"],false);
    tkb.setCallback(this,&ofApp::guiCallback);
}

void ofApp::guiCallback(const SergeGUIEvent event){
    cout << "guiType: " << event.guiType;
    cout << "\teventType: " << event.eventType;
    cout << "\tindex: " << event.index;
    cout << "\tval: " << event.val;
    cout << "\tparam: " << event.param;
    cout << "\tradio: " << event.radio;
    cout << "\tdropdown_i: " << event.dropdown_i;
    cout << "\taxis: " << event.axis << endl;
    
    switch (event.guiType) {
        case KNOB:
            knobCallback(event);
            break;
        case LED:
            if(event.axis != -1){
                switch(event.radio){
                    case 0:
                        x_index_i = axis_selection_lookup[event.axis];
                        break;
                    case 1:
                        y_index_i = axis_selection_lookup[event.axis];
                        break;
                    case 2:
                        c_index_i = axis_selection_lookup[event.axis];
                        break;
                }
                drawPlot(true);
            }
            break;
        case PUSH:
            break;
        case DROPDOWN:
            midiIn.closePort();
            midiIn.openPort(event.dropdown_i);
        default:
            break;
    }
}

void ofApp::knobCallback(const SergeGUIEvent event){
    // if command is held, it's a mouse press, and this knob actually controls something:
    
    cout << "key modifier command: " << keyModifiers.command << endl;
    cout << "event type:           " << event.eventType << endl;
    cout << "event param:          " << event.param << endl;
    cout << endl;
    
    if(keyModifiers.command && (event.eventType == MOUSEPRESSED) && (event.param != -1)){
        midi_manager.waitForAssignment(event.param);
    } else if(keyModifiers.shift && (event.eventType == MOUSEPRESSED) && (event.param != -1)){
        midi_manager.removeAssignment(event.param);
    } else {
        if(event.param != -1){
            params_state.setAt(event.param,event.val);
        }
    }
}

void ofApp::audioOut(float *output, int bufferSize, int nChannels){
    if(loaded){
        for(int i = 0; i < bufferSize; i++){
            for(int k = 0; k < soundFiles.size(); k++){
                float val = soundFiles[k]->tick();
                for(int j = 0; j < nChannels; j++){
                    output[(i * nChannels) + j] += val;
                }
            }
        }
    }
}

void ofApp::find_nearest_xy(){
    
    vector<size_t> indexes;
    vector<double> dists;

    kdTree_2d.getKNN(hid_xy.get(), 1, indexes, dists);

    setPlayingIndex(indexes[0],true);
}

void ofApp::find_nearest_param(const vector<double> &p){
    vector<size_t> indexes;
    vector<double> distances;
    kdTree_params.getKNN(params_state.get(),1, indexes, distances);
    setPlayingIndex(indexes[0],false);
}

void ofApp::setPlayingIndex(size_t index, bool updateSliders){
    if(index != playing_index){
        playing_index = index;

        int file = slices[playing_index]->values[0];
        int start_frame = slices[playing_index]->values[1];
        int n_frames = slices[playing_index]->values[2];
                        
        for(int i = 0; i < soundFiles.size(); i++){
            int gate = 0;
            if(i == file) gate = 1;
            soundFiles[i]->setPosGate(start_frame,n_frames,gate);
        }
        
        for(int i = 0; i < 4; i++){
            params_state.setAt(i,slices[playing_index]->values[10 + i]);
        }
        
        tkb.updateParamGuis(params_state.get());
        three_panel.updateParamGuis(params_state.get());
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if(loaded){
        processOSC();
        
        if (hid_xy.changed){
            find_nearest_xy();
            hid_xy.changed = false;
        } else if(params_state.changed){
            find_nearest_param(params_state.get());
            params_state.changed = false;
        }
        
    } else {
        int sum = 0;
        for(int i = 0; i < soundFiles.size(); i++){
            sum += soundFiles[i]->isThreadRunning();
        }
        if(sum == 0){
            loaded = true;
            windowResized(ofGetWidth(),ofGetHeight());
        }
    }
}

void ofApp::processOSC(){
    // ==================== OSC ================================
    while(osc_receiver.hasWaitingMessages()){
        ofxOscMessage oscMsg;
        osc_receiver.getNextMessage(oscMsg);

        string address = oscMsg.getAddress();

        if(address == "/x"){
            hid_xy.setAt(0,oscMsg.getArgAsFloat(0));
        } else if (address == "/y"){
            hid_xy.setAt(1,1.f - oscMsg.getArgAsFloat(0));
        } else if (address == "/param1"){
            params_state.setAt(0,oscMsg.getArgAsFloat(0));
        } else if (address == "/param2"){
            params_state.setAt(1,oscMsg.getArgAsFloat(0));
        } else if (address == "/param3"){
            params_state.setAt(2,oscMsg.getArgAsFloat(0));
        } else if (address == "/param4"){
            params_state.setAt(3,oscMsg.getArgAsFloat(0));
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(loaded){
        drawPlotWindow();
    } else {
        loadingScreen();
    }
}

void ofApp::drawPlotWindow(){
    ofSetColor(255,255,255,255);
    tkb.draw();
    ofSetColor(255,255,255,255);
    ofSetRectMode(OF_RECTMODE_CORNER);
    plot_fbo.draw(plot_x,plot_y,plot_w,plot_h);
    
    if(playing_index >= 0){
        ofVec2f highlighted_pos = slices[playing_index]->current_pos;
        highlighted_pos.operator*=(ofVec2f(plot_w,plot_h));
        highlighted_pos.operator+=(ofVec2f(plot_x,plot_y));
        ofFill();
        ofSetColor(0,0,0,255);
        ofDrawCircle(highlighted_pos,6);
    }
}

void ofApp::loadingScreen(){
    ofSetColor(0);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    ofSetColor(255);
    ofDrawBitmapString("Loading...", 600,300);
}

void ofApp::drawSkeuomorph(ofEventArgs &args){
    if(loaded){
        three_panel.drawCenteredScaled(skeuomorph_window_width,skeuomorph_window_height);
    } else {
        loadingScreen();
    }
}

void ofApp::drawPlot(bool buildKDTree){
    if(buildKDTree) kdTree_2d.clear();

    plot_fbo.allocate(plot_w, plot_h);
    plot_fbo.begin();
    ofClear(255,255,255,255);
    ofFill();
    for(SoundSlice *slice : slices){
        kdTree_2d.addPoint(slice->draw(0,0,plot_fbo.getWidth(),plot_fbo.getHeight(),x_index_i,y_index_i,c_index_i));
    }
    plot_fbo.end();
    if(buildKDTree) kdTree_2d.constructKDTree();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    masterKeyPressed(key);
}

void ofApp::skeuomorphKeyPressed(ofKeyEventArgs& args){
    masterKeyPressed(args.key);
}

void ofApp::masterKeyPressed(int key){
//    cout << "master key pressed " << key << endl;
    switch(key){
        case 3680:
            keyModifiers.shift = true;
            break;
        case 3681:
            keyModifiers.shift = true;
            break;
        case 3682:
            keyModifiers.control = true;
            break;
        case 3684:
            keyModifiers.option = true;
            break;
        case 3685:
            keyModifiers.option = true;
            break;
        case 3686:
            keyModifiers.command = true;
            break;
        case 3687:
            keyModifiers.command = true;
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    masterKeyReleased(key);
}

void ofApp::skeuomorphKeyReleased(ofKeyEventArgs& args){
    masterKeyReleased(args.key);
}

void ofApp::masterKeyReleased(int key){
    switch(key){
        case 3680:
            keyModifiers.shift = false;
            break;
        case 3681:
            keyModifiers.shift = false;
            break;
        case 3682:
            keyModifiers.control = false;
            break;
        case 3684:
            keyModifiers.option = false;
            break;
        case 3685:
            keyModifiers.option = false;
            break;
        case 3686:
            keyModifiers.command = false;
            break;
        case 3687:
            keyModifiers.command = false;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    tkb.windowMouseMoved(x,y);
}

void ofApp::skeuomorphMousePressed(ofMouseEventArgs& args){
    three_panel.windowMousePressed(args.x,args.y,keyModifiers);
}

bool ofApp::mouseInPlot(int x, int y){
    bool left = x >= plot_x;
    bool right = x <= (plot_x + plot_w);
    bool top = y >= plot_y;
    bool bottom = y <= (plot_y + plot_h);
    return left && right && top && bottom;
}

void ofApp::skeuomorphMouseDragged(ofMouseEventArgs& args){
    three_panel.windowMouseDragged(args.x,args.y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    processIncomingMouseXY(x,y);
    tkb.windowMousePressed(x,y,keyModifiers);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    processIncomingMouseXY(x,y);
    tkb.windowMouseDragged(x, y);
}

void ofApp::processIncomingMouseXY(int x, int y){
    if(mouseInPlot(x,y) && !tkb.hasMouseCaptured()){
        hid_xy.setAt(0,(double(x) - plot_x) / plot_w);
        hid_xy.setAt(1,(double(y) - plot_y) / plot_h);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    tkb.windowMouseReleased(x,y);
}

void ofApp::skeuomorphMouseReleased(ofMouseEventArgs& args){
    three_panel.windowMouseReleased(args.x,args.y);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

    tkb.bottomScaled(w,h,margin);

    tkb.postDims();

    plot_x = margin;
    plot_y = margin;
    plot_w = w - (plot_x + margin);
    plot_h = h - ((margin * 3) + tkb.draw_h);

    drawPlot(false);
}

void ofApp::skeuomorphResized(ofResizeEventArgs& args){
    skeuomorph_window_width = args.width;
    skeuomorph_window_height = args.height;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

//    cout << "new midi message " << msg.channel << " " << msg.control << " " << msg.value << " " << endl;
    int learned_midi = midi_manager.processIncomingMIDI(msg.channel,msg.control);
    
//    cout << "learned midi " << learned_midi << endl;
    
    if(learned_midi == 4){
        hid_xy.setAt(0,msg.value / 127.f);
    } else if (learned_midi == 5){
        hid_xy.setAt(1,1.f - (msg.value / 127.f));
    } else if(learned_midi < 4 && learned_midi >= 0){
        params_state.setAt(learned_midi,msg.value / 127.f);
    }
}

void ofApp::createColors(){
    // "bold" from https://carto.com/carto-colors/
    int hexes[12] = {0x7F3C8D,0x11A579,0x3969AC,0xF2B701,0xE73F74,0x80BA5A,0xE68310,0x008695,0xCF1C90,0xf97b72,0x4b4b8f,0xA5AA99};

    for(int i = 0; i < 12; i++){
        qualitative_colors[i] = ofColor::fromHex(hexes[i]);
    }

    ofPixels rainbow_pixels;
    ofLoadImage(rainbow_pixels, ofToDataPath("colormaps/color options-01.png"));
    int hop = rainbow_pixels.getWidth() / 255.f;

    int y = rainbow_pixels.getHeight() - 1;
    for(int x = 0; x < rainbow_pixels.getWidth(); x += hop){
        ofColor c = rainbow_pixels.getColor(x,y);
        c.a = 255;
        rainbow_colors.push_back(c);
    }
}

void ofApp::readSoundSlicesData(string csv_path){
    string line;
    ifstream data;

    data.open(ofToDataPath(csv_path));

    while(!data.eof()){
        getline(data, line);

        SoundSlice* soundSlice = new SoundSlice;
        soundSlice->setup(rainbow_colors,qualitative_colors,line);

        slices.push_back(soundSlice);
    }

    data.close();
}

void ofApp::createPointKDTree(){
    vector<double> point;
    point.resize(4);

    for(int i = 0; i < slices.size(); i++){
        for(int j = 0; j < nParams; j++){
            point[j] = slices[i]->values[10 + j];
        }

        kdTree_params.addPoint(point);
    }

    kdTree_params.constructKDTree();
}
