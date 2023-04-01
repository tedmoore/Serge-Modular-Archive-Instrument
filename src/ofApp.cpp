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
            soundFiles.push_back(move(unique_ptr<SoundFile>(new SoundFile)));
            soundFiles.back()->setup(files[i].getAbsolutePath(),SAMPLERATE);
        } else if(files[i].getExtension() == "csv") {
            readSoundSlicesData(files[i].getAbsolutePath());
        } else if(files[i].getExtension() == "json") {
            nlohmann::json patch_info_json;
            std::ifstream pi(files[i].getAbsolutePath());
            pi >> patch_info_json;
            skeuomorph_knob_assignments = patch_info_json["skeuomorph_knob_assignments"].get<vector<int>>();
        }
    }

    createPointKDTree();
}

void ofApp::setupSkeuomorph(){
    
    ofSetDataPathRoot("../Resources/data/");
    
    guiItems.font.load(ofToDataPath("LTe50236.ttf"),20,true,true,true,0.f,182);
    guiItems.knob.load(ofToDataPath("images/Serge Gui Layout (2022)/DAVIES_KNOB.png"));
    guiItems.illumination.load(ofToDataPath("images/Serge Gui Layout (2022)/KNOB_ILLUMINATION.png"));
    guiItems.illuminationBlue.load(ofToDataPath("images/Serge Gui Layout (2022)/KNOB_ILLUMINATION_BLUE.png"));
    guiItems.push.load(ofToDataPath("images/Serge Gui Layout (2022)/BUTTON_PRESSED.png"));
    guiItems.led.load(ofToDataPath("images/Serge Gui Layout (2022)/LED_ON.png"));

    std::ifstream i(ofToDataPath("images/Serge Gui Layout (2022)/gui_info.json"));
    i >> gui_info_json;
    
    skeuomorph.load(ofToDataPath("images/Serge GUI Layout (2022)/3-PANELS/3-PANELS.png"),guiItems,gui_info_json["skeuomorph"]);
    skeuomorph.setCallback(this, &ofApp::guiCallback);

    skeuomorph_window_width = ofGetScreenWidth() / 2;
    skeuomorph_window_height = 1972; // If this is not hard coded, it displays incorrectly. I had tried ofGetScreenHeight(), and even tried to offset for the menu bar, but it would always display incorrectly.
    
    hid_xy.resize(2);
    hid_xy.setAll(0.5);
    params_state.resize(nParams);
    params_state.setAll(0.5);
}

void ofApp::setup(){
    osc_receiver.setup(2884);
    ofSetFrameRate(30);
    ofBackground(background);
    ofEnableAntiAliasing();
        
    createColors();

    ofFileDialogResult result = ofSystemLoadDialog("Load folder with wav files and data file",true);
    if(result.bSuccess) {
      loadDirectory(result.getPath());
    }

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
	plot_controls.load(ofToDataPath("images/Serge GUI Layout (2022)/TAUC/TAUC.png"),guiItems,gui_info_json["plot"]);
    plot_controls.setCallback(this,&ofApp::guiCallback);
    
    for(int i = 0; i < nParams; i++){
        skeuomorph.guis[skeuomorph_knob_assignments[i]]->setIlluminate(true);
    }
    
    // radios
    for(int i = 4; i <= 10; i++){
        x_radio.addGui(plot_controls.guis[i]);
    }
    
    x_radio.setIndex(0);
  
    for(int i = 11; i <= 17; i++){
        y_radio.addGui(plot_controls.guis[i]);
    }
  
    y_radio.setIndex(1);
    
    for(int i = 18; i <= 24; i++){
        c_radio.addGui(plot_controls.guis[i]);
    }

    c_radio.setIndex(2);
    
    for(int i = 26; i <= 41; i++){
        step_sequencer.radio.addGui(plot_controls.guis[i]);
    }
    
    // override the default call back function for the x and y knobs
    
    // 'x' knob
    plot_controls.guis[plot_x_knob_index]->setCallback([&](SergeGUIEvent event){
        hid_xy.setAt(0,event.val_f);
    });
    
    // 'x' knob
    plot_controls.guis[plot_y_knob_index]->setCallback([&](SergeGUIEvent event){
        hid_xy.setAt(1,event.val_f);
    });
    
    // setup step squencer
    step_sequencer.randomize(slices.size());
    step_sequencer.updateIndex(0);
    step_sequencer.setPushButtonPtr(plot_controls.guis[0]);
    
    midi_ports = midiIn.getInPortList();
    
    windowResized(ofGetWidth(),ofGetHeight());
    drawPlot(true,false);
}

void ofApp::guiCallback(const SergeGUIEvent event){
    
    switch (event.guiType) {
        case KNOB:
            knobCallback(event);
            break;
        case LED:
            ledCallback(event);
            break;
        case TOGGLE:
            step_sequencer.idWaitingToAssign = playing_index;
            step_sequencer.bAwaitingAssignment = true;
            break;
        default:
            break;
    }
}

void ofApp::ledCallback(const SergeGUIEvent event){
    
    int i = step_sequencer.mousePressed(event.image_index);
    
    if(i >= 0) {
        setPlayingIndex(i, true);
        return;
    }
    
    i = x_radio.update(event.image_index);
    
    if(i >= 0) {
        drawPlot(true,true);
        return;
    }
    
    i = y_radio.update(event.image_index);

    if(i >= 0) {
        drawPlot(true,true);
        return;
    }

    i = c_radio.update(event.image_index);

    if(i >= 0) {
        drawPlot(true,false);
        return;
    }
}

void ofApp::knobCallback(const SergeGUIEvent event){
    switch(event.parent_id){
        case 0: // plot controls
            for(int i = 0; i < nParams; i++){
                if(event.image_index == plot_control_param_knobs[i]){
                    individualKnobCallback(event.eventType,i,event.val_f);
                    break;
                }
            }
            break;
        case 1: // skeuomorph
            for(int i = 0; i < nParams; i++){
                if(event.image_index == skeuomorph_knob_assignments[i]){
                    individualKnobCallback(event.eventType,i,event.val_f);
                    break;
                }
            }
            break;
        default:
            ofLogError() << "parent_id not recognized";
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

// x and y are normalized 0 to 1
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
    setPlayingIndex(indexes[0],true);
}

void ofApp::setPlayingIndex(int index, bool updateHandles){
    if(index != playing_index and index >= 0 and index < slices.size()){
        playing_index = index;

        int file = slices[playing_index]->values[0];
        int start_frame = slices[playing_index]->values[1];
        int n_frames = slices[playing_index]->values[2];
                        
        for(int i = 0; i < soundFiles.size(); i++){
            soundFiles[i]->setPosGate(start_frame,n_frames,i == file);
        }
        
        for(int i = 0; i < 4; i++){
            params_state.setAt(i,slices[playing_index]->values[10 + i],false);
            if(updateHandles){
                plot_controls.updateGuiValue(plot_control_param_knobs[i], params_state.getAt(i));
                skeuomorph.updateGuiValue(skeuomorph_knob_assignments[i], params_state.getAt(i));
            }
        }
        
        // update the x and y knobs (if the knobs are being used, this is redundant, but it's ok.
        // if the mouse is interacting with the plot, this is not redundant, which is probably
        // the more common use case)
        updateXYKnobs();
    }
}

void ofApp::updateXYKnobs(){
    plot_controls.guis[plot_x_knob_index]->setValueF(slices[playing_index]->current_pos.x);
    plot_controls.guis[plot_y_knob_index]->setValueF(slices[playing_index]->current_pos.y);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(loaded){
        processOSC();
        
        if(redrawPlotNextUpdate){
            drawPlot(true,true);
            redrawPlotNextUpdate = false;
        }
        
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

        if(address == "/param1"){
            params_state.setAt(0,oscMsg.getArgAsFloat(0));
        } else if (address == "/param2"){
            params_state.setAt(1,oscMsg.getArgAsFloat(0));
        } else if (address == "/param3"){
            params_state.setAt(2,oscMsg.getArgAsFloat(0));
        } else if (address == "/param4"){
            params_state.setAt(3,oscMsg.getArgAsFloat(0));
        } else if (address == "/x"){
            hid_xy.setAt(0,oscMsg.getArgAsFloat(0));
        } else if (address == "/y"){
            hid_xy.setAt(1,1.f - oscMsg.getArgAsFloat(0));
        } else if (address == "/x-axis"){
            x_radio.setIndex(oscMsg.getArgAsInt(0));
            redrawPlotNextUpdate = true;
        } else if (address == "/y-axis"){
            y_radio.setIndex(oscMsg.getArgAsInt(0));
            redrawPlotNextUpdate = true;
        } else if (address == "/color-axis"){
            c_radio.setIndex(oscMsg.getArgAsInt(0));
            redrawPlotNextUpdate = true;
        } else if (address == "/step-seq"){
            setPlayingIndex(step_sequencer.goToStep(oscMsg.getArgAsInt(0) - 1), true);
        } else if (address == "/step-seq-advance"){
            setPlayingIndex(step_sequencer.advance(), true);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(loaded){
        drawPlotWindow();
        if(show_options_menu){
            drawOptionsMenu();
        }
    } else {
        loadingScreen();
    }
}

void ofApp::drawOptionsMenu(){
    
    ofSetColor(background,128);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    
    int w = ofGetWidth() * 0.8;
    int h = ofGetHeight() * 0.8;
    int x = (ofGetWidth() - w) / 2;
    int y =(ofGetHeight() - h) / 2;
    int margin = 40;
    ofFill();
    ofSetColor(255);
    ofDrawRectRounded(x,y,w,h,25);
    ofNoFill();
    ofSetLineWidth(10);
    ofSetColor(sergeBlue);
    ofDrawRectRounded(x,y,w,h,25);
    
    ofFill();
    int text_height = guiItems.font.getLineHeight();
    ofSetColor(0);
    guiItems.font.drawStringAsShapes("Press a number to select a MIDI Port:", x + margin, y + margin + text_height);
    for(int i = 0; i < midi_ports.size(); i++){
        if(selected_midi_port == i){
            ofSetColor(sergeRed);
        } else {
            ofSetColor(100);
        }
        guiItems.font.drawStringAsShapes(ofToString(i) + ": " + midi_ports[i],x + margin, y + margin + text_height + ((i+1)*text_height * 1.5));
    }
    guiItems.font.drawStringAsShapes("Press 'M' to close", x + margin, (y + h) - margin);
}

void ofApp::drawPlotWindow(){
    ofSetColor(255,255,255,255);
    plot_controls.draw();
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
        skeuomorph.drawCenteredScaled(skeuomorph_window_width,skeuomorph_window_height);
        
        if(show_options_menu){
            ofSetColor(background,128);
            ofSetRectMode(OF_RECTMODE_CORNER);
            ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        }

    } else {
        loadingScreen();
    }
}

void ofApp::drawPlot(bool buildKDTree, bool update_xy_knobs){
    if(buildKDTree) kdTree_2d.clear();

    plot_fbo.begin();
    ofClear(255,255,255,255);
    ofFill();
    
    int x_index_i = axis_selection_lookup[x_radio.getCurrentIndex()];
    int y_index_i = axis_selection_lookup[y_radio.getCurrentIndex()];
    int c_index_i = axis_selection_lookup[c_radio.getCurrentIndex()];
    
    for(SoundSlice *slice : slices){
        kdTree_2d.addPoint(slice->draw(0,0,plot_fbo.getWidth(),plot_fbo.getHeight(),x_index_i,y_index_i,c_index_i));
    }
    plot_fbo.end();
    if(buildKDTree) kdTree_2d.constructKDTree();
    if(update_xy_knobs) updateXYKnobs();
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
        default:
        {
            if(key == 'm' or key == 'M'){
                show_options_menu = !show_options_menu;
            } else {
                if(show_options_menu){
                    int number = key - '0';
                    if(number >= 0 && number < midi_ports.size()){
                        selected_midi_port = number;
                        midiIn.closePort();
                        midiIn.openPort(selected_midi_port);
                    }
                } else {
                    int slice_id = step_sequencer.receiveQWERTY(key);
                    if(slice_id >=0) setPlayingIndex(slice_id,true);
                }
            }
        }
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

}

void ofApp::skeuomorphMousePressed(ofMouseEventArgs& args){
    if(!show_options_menu) skeuomorph.windowMousePressed(args.x,args.y,keyModifiers);
}

bool ofApp::mouseInPlot(int x, int y){
    bool left = x >= plot_x;
    bool right = x <= (plot_x + plot_w);
    bool top = y >= plot_y;
    bool bottom = y <= (plot_y + plot_h);
    return left && right && top && bottom;
}

void ofApp::skeuomorphMouseDragged(ofMouseEventArgs& args){
    if(!show_options_menu) skeuomorph.windowMouseDragged(args.x,args.y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(!show_options_menu){
        processIncomingMouseXY(x,y);
        plot_controls.windowMousePressed(x,y,keyModifiers);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(!show_options_menu){
        processIncomingMouseXY(x,y);
        plot_controls.windowMouseDragged(x,y);
    }
}

void ofApp::processIncomingMouseXY(int x, int y){
    if(mouseInPlot(x,y) && !plot_controls.hasMouseCaptured()){
        hid_xy.setAt(0,(double(x) - plot_x) / plot_w);
        hid_xy.setAt(1,(double(y) - plot_y) / plot_h);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    plot_controls.windowMouseReleased(x,y);
}

void ofApp::skeuomorphMouseReleased(ofMouseEventArgs& args){
    skeuomorph.windowMouseReleased(args.x,args.y);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

    plot_controls.bottomScaled(w,h,margin);

    plot_controls.postDims();

    plot_x = margin;
    plot_y = margin;
    plot_w = w - (plot_x + margin);
    plot_h = h - ((margin * 3) + plot_controls.draw_h);

    plot_fbo.allocate(plot_w, plot_h);
    
    drawPlot(false,false);
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
    
    switch(msg.status){
        case MIDI_CONTROL_CHANGE:
            newMidiControlChange(msg.control, msg.value, msg.channel);
            break;
        case MIDI_NOTE_ON:
            newMidiNoteOn(msg.pitch, msg.velocity);
            break;
    }
}

void ofApp::newMidiControlChange(int cc, int val, int channel){
    
    switch(cc){
        case 1:
            params_state.setAt(0,val / 127.f);
            break;
        case 2:
            params_state.setAt(1,val / 127.f);
            break;
        case 3:
            params_state.setAt(2,val / 127.f);
            break;
        case 4:
            params_state.setAt(3,val / 127.f);
            break;
        case 5: // set x position on plot
            hid_xy.setAt(0,val / 127.f);
            break;
        case 6: // set y postiion on plot
            hid_xy.setAt(1,1.f - (val / 127.f));
            break;
        case 101: // set x axis
            if(val >= 0 and val < 7){
                x_radio.setIndex(val);
                redrawPlotNextUpdate = true;
            }
            break;
        case 102: // set y axis
            if(val >= 0 and val < 7){
                y_radio.setIndex(val);
                redrawPlotNextUpdate = true;
            }
            break;
        case 103: // set color axis
            if(val >= 0 and val < 7){
                c_radio.setIndex(val);
                redrawPlotNextUpdate = true;
            }
            break;
        default: // if it is not one of the reserved cc
            // this will return a -1 if there is nothing assigned at this channel & cc
            int learned_midi = midi_manager.processIncomingMIDI(channel-1,cc);
            if(learned_midi < 4 && learned_midi >= 0){
                params_state.setAt(learned_midi, val / 127.f);
            }
            break;
    }
}

void ofApp::newMidiNoteOn(int note, int vel){
    
    switch(note){
        case 100:
            setPlayingIndex(step_sequencer.advance(), true);
            break;
        default:
        {
            // this will return -1 if this midi note is not one
            // that the step sequencer is listening for
            int slice_id = step_sequencer.receiveMIDI(note);
            
            if(slice_id >= 0 and slice_id < slices.size()) setPlayingIndex(slice_id,true);
        }
            break;
    }
}

void ofApp::createColors(){

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
        soundSlice->setup(rainbow_colors,line);

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
