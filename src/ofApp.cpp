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
    
    step_sequencer.randomize(slices.size());
    step_sequencer.updateIndex(0);
    
    midi_ports = midiIn.getInPortList();
    
    windowResized(ofGetWidth(),ofGetHeight());
    drawPlot(true);
}

void ofApp::guiCallback(const SergeGUIEvent event){
    
    cout << "ofApp::guiCallback\n";
    event.dump();
    
    switch (event.guiType) {
        case KNOB:
            knobCallback(event);
            break;
        case LED:
            ledCallback(event);
            break;
        case PUSH:
            if(playing_index >= 0) step_sequencer.assignCurrentStep(playing_index);
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
        drawPlot(true);
        return;
    }
    
    i = y_radio.update(event.image_index);

    if(i >= 0) {
        drawPlot(true);
        return;
    }

    i = c_radio.update(event.image_index);

    if(i >= 0) {
        drawPlot(true);
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

void ofApp::setPlayingIndex(size_t index, bool updateSliders){
    if(index != playing_index){
        playing_index = index;

        int file = slices[playing_index]->values[0];
        int start_frame = slices[playing_index]->values[1];
        int n_frames = slices[playing_index]->values[2];
                        
        for(int i = 0; i < soundFiles.size(); i++){
            soundFiles[i]->setPosGate(start_frame,n_frames,i == file);
        }
        
        for(int i = 0; i < 4; i++){
            params_state.setAt(i,slices[playing_index]->values[10 + i],false);
            if(updateSliders){
                plot_controls.updateGuiValue(plot_control_param_knobs[i], params_state.getAt(i));
                skeuomorph.updateGuiValue(skeuomorph_knob_assignments[i], params_state.getAt(i));
            }
        }        
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

void ofApp::drawPlot(bool buildKDTree){
    if(buildKDTree) kdTree_2d.clear();

    plot_fbo.allocate(plot_w, plot_h);
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
        plot_controls.windowMouseDragged(x, y);
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
    
    switch(msg.status){
        case MIDI_CONTROL_CHANGE:
        {
            int learned_midi = midi_manager.processIncomingMIDI(msg.channel,msg.control);
            if(learned_midi == 4){
                hid_xy.setAt(0,msg.value / 127.f);
            } else if (learned_midi == 5){
                hid_xy.setAt(1,1.f - (msg.value / 127.f));
            } else if(learned_midi < 4 && learned_midi >= 0){
                params_state.setAt(learned_midi,msg.value / 127.f);
            }
        }
            break;
        case MIDI_NOTE_ON:
        {
            int slice_id = step_sequencer.receiveMIDI(msg.pitch);
            if(slice_id >= 0) setPlayingIndex(slice_id,true);
        }
            break;
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
