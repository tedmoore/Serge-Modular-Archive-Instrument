#include "ofApp.h"

#define SAMPLERATE 44100
//--------------------------------------------------------------

void ofApp::setup(){
    //cout.precision(17);
    
    osc_receiver.setup(2884);
    
    ofSetFrameRate(60);
    //    ofxGuiEnableHiResDisplay();
    ofBackground(100);
    ofEnableAntiAliasing();
    string csv_path = "211030_183738.csv";
    
    createColors();
    
    double ranges[8];
    for(int i = 0; i < 4; i++){
        ranges[i * 2] = std::numeric_limits<double>::max();
        ranges[(i * 2) + 1] = std::numeric_limits<double>::min();
    }
    
    readSoundSlicesData(csv_path,ranges);
    
//    for(int i = 0; i < 8; i++){
//        cout << ranges[i] << " ";
//    }
//    cout << endl;
    
    createHeadersAndDropdownOptions();
    
    gui = new ofxDatGui(margin,margin);
    gui->setWidth(menu_width);
    
    gui->addLabel("X Dimension");
    x_menu = gui->addDropdown("X",dropdown_options);
    x_menu->onDropdownEvent(this, &ofApp::onDropdownEventX);
    
    gui->addLabel("Y Dimension");
    y_menu = gui->addDropdown("Y",dropdown_options);
    y_menu->onDropdownEvent(this, &ofApp::onDropdownEventY);
    
    gui->addLabel("Color");
    c_menu = gui->addDropdown("C",dropdown_options);
    c_menu->onDropdownEvent(this, &ofApp::onDropdownEventC);
    
    gui->addLabel("MIDI In");
    midi_in_menu = gui->addDropdown("MIDI In",midiIn.getInPortList());
    midi_in_menu->onDropdownEvent(this, &ofApp::onDropdownEventMIDIIN);
    
    for(int i = 0; i < 4; i++){
        handles[i].setup(gui->addSlider("param"+ofToString(i),ranges[i*2],ranges[(i*2)+1]),ranges[i*2],ranges[(i*2)+1]);
        handles[i].handle->onSliderEvent(this, &ofApp::onSliderEvent);
    }
    
    createPointKDTree();

    windowResized(ofGetWidth(),ofGetHeight());
    drawPlot(true);
    
    soundFiles.resize(3);
    for(int i = 0; i < 3; i++){
        soundFiles[i].load(ofToDataPath("audio_files/part"+ofToString(i+1)+"_44k_16b.wav"),SAMPLERATE);
    }
    
    soundstream.setup(2, 0, SAMPLERATE, 256, 4);
    
    // MIDI
    
    midi_manager.setup();
    
    // print input ports to console
    midiIn.listInPorts();

    // open port by number (you may need to change this)
    midiIn.openPort(0);
    //midiIn.openPort("IAC Pure Data In");    // by name
    //midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);

    // add ofApp as a listener
    midiIn.addListener(this);

    // print received messages to the console
    midiIn.setVerbose(true);
}

void ofApp::setupSkeuomorph(){
    mockup.load(ofToDataPath("images/mockup.png"));
    mockup_svg.load(ofToDataPath("images/mockup.svg"));
}

void ofApp::drawSkeuomorph(ofEventArgs &args){
    mockup.draw(0,0);
    mockup_svg.draw();
//    mockup_svg.draw(100,100,300,300);
}

void ofApp::audioOut(float *output, int bufferSize, int nChannels){
    for(int i = 0; i < bufferSize; i++){
        for(int k = 0; k < soundFiles.size(); k++){
            float val = soundFiles[k].tick();
            for(int j = 0; j < nChannels; j++){
                output[(i * nChannels) + j] += val;
            }
        }
    }
    
    //cout << soundFiles[0].env[0].value << " " << soundFiles[0].env[1].value << endl;
}

void ofApp::find_nearest(double x, double y, bool normalized){
    
    double scaled_x = 0, scaled_y = 0;
    
    if(!normalized){
        scaled_x = ofMap(x,plot_x,plot_x + plot_w,0.f,1.f);
        scaled_y = ofMap(y,plot_y,plot_y + plot_h,0.f,1.f);
    } else {
        scaled_x = x;
        scaled_y = y;
    }
    vector<double> query_pt = {scaled_x,scaled_y};
    vector<size_t> indexes;
    vector<double> dists;
    
    kdTree_2d.getKNN(query_pt, 1, indexes, dists);

//    cout << x << " " << y << " " << scaled_x << " " << scaled_y << endl;
//    cout << indexes[0] << endl;
    
    setPlayingIndex(indexes[0],true);
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e){
    if(allow_slider_callback){
        vector<double> point;
        point.resize(4);
        for(int i = 0; i < 4; i++){
            point[i] = handles[i].getNormalizedValue();
        }
        
        vector<size_t> indexes;
        vector<double> distances;
        
        kdTree_params.getKNN(point,1, indexes, distances);
        
//        cout << "slider event, new playing index: " << indexes[0] << endl;
        
        setPlayingIndex(indexes[0],false);
    }
}

void ofApp::setPlayingIndex(size_t index, bool updateSliders){
    if(index != playing_index){
        playing_index = index;
        
        int file = slices[playing_index]->values[1];
        int start_frame = slices[playing_index]->values[2];
        int n_frames = slices[playing_index]->values[3];
        
        cout << playing_index << " " << file << " " << start_frame << " " << n_frames << endl;
        
        for(int i = 0; i < soundFiles.size(); i++){
            soundFiles[i].setPosGate(start_frame,n_frames,file == i);
        }
        
        if(updateSliders){
            allow_slider_callback = false;
            for(int i = 0; i < 4; i++){
                handles[i].handle->setValue(slices[playing_index]->values[19 + i]);
            }
            allow_slider_callback = true;
        }
    }
}

void ofApp::onDropdownEventX(ofxDatGuiDropdownEvent e){
    x_index_sl = dropdown_index_lookup[e.child];
    drawPlot(true);
}

void ofApp::onDropdownEventY(ofxDatGuiDropdownEvent e){
    y_index_sl = dropdown_index_lookup[e.child];
    drawPlot(true);
}

void ofApp::onDropdownEventC(ofxDatGuiDropdownEvent e){
    c_index_sl = dropdown_index_lookup[e.child];
    drawPlot(true);
}

void ofApp::onDropdownEventMIDIIN(ofxDatGuiDropdownEvent e){
    midiIn.closePort();
    midiIn.openPort(e.child);
    cout << midiIn.getPort() << endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    processOSC();
    gui->update();
}

void ofApp::processOSC(){
    // ==================== OSC ================================
    while(osc_receiver.hasWaitingMessages()){
        ofxOscMessage oscMsg;
        osc_receiver.getNextMessage(oscMsg);

        string address = oscMsg.getAddress();
        
        cout << "ofApp::processOSC() address: " << address << endl;
        
        if(address == "/x"){
            hid_x = oscMsg.getArgAsFloat(0);
            find_nearest(hid_x, hid_y,true);
        } else if (address == "/y"){
            hid_y = oscMsg.getArgAsFloat(0);
            find_nearest(hid_x, hid_y,true);
        } else if (address == "/param1"){
            handles[0].handle->setValue(oscMsg.getArgAsFloat(0));
        } else if (address == "/param2"){
            handles[1].handle->setValue(oscMsg.getArgAsFloat(0));
        } else if (address == "/param3"){
            handles[2].handle->setValue(oscMsg.getArgAsFloat(0));
        } else if (address == "/param4"){
            handles[3].handle->setValue(oscMsg.getArgAsFloat(0));
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    plot_fbo.draw(plot_x,plot_y,plot_w,plot_h);
    gui->draw();
    
    if(playing_index >= 0){
        ofSetColor(0,0,0);
        ofVec2f highlighted_pos = slices[playing_index]->current_pos;
        highlighted_pos.operator*=(ofVec2f(plot_w,plot_h));
        highlighted_pos.operator+=(ofVec2f(plot_x,plot_y));
        ofDrawCircle(highlighted_pos,6);
    }
}

void ofApp::drawPlot(bool buildKDTree){
    if(buildKDTree) kdTree_2d.clear();
    
    plot_fbo.allocate(plot_w, plot_h);
    plot_fbo.begin();
    ofClear(255,255,255,255);
    ofFill();
    for(SoundSlice *slice : slices){
        kdTree_2d.addPoint(slice->draw(0,0,plot_fbo.getWidth(),plot_fbo.getHeight(),x_index_sl,y_index_sl,c_index_sl));
    }
    plot_fbo.end();
    if(buildKDTree) kdTree_2d.constructKDTree();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    cout << "key " << key << "\n";
    int hid = key - 48;
    cout << "hid number: " << hid << endl;
    if(hid >= 0 && hid < 6){
        midi_manager.waitForAssignment(hid);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

bool ofApp::mouseInPlot(int x, int y){
    bool left = x >= plot_x;
    bool right = x <= (plot_x + plot_w);
    bool top = y >= plot_y;
    bool bottom = y <= (plot_y + plot_h);
    return left && right && top && bottom;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(mouseInPlot(x,y)) find_nearest(x,y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if(mouseInPlot(x,y)) find_nearest(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    plot_x = menu_width + margin + margin;
    plot_y = margin;
    plot_w = w - (plot_x + margin);
    plot_h = h - (margin * 2);
    
    drawPlot(false);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

    cout << "midi msg: " << msg.channel << " " << msg.control << " " << msg.value << endl;
    
    int learned_midi = midi_manager.processIncomingMIDI(msg.channel,msg.control);
    
    cout << "learned midi: " << learned_midi << endl;
    
    if(learned_midi == 0){
        hid_x = msg.value / 127.f;
        cout << "hid x: " << hid_x << endl;
        find_nearest(hid_x, hid_y,true);
    } else if (learned_midi == 1){
        hid_y = 1.f - (msg.value / 127.f);
        cout << "hid y: " << hid_y << endl;
        find_nearest(hid_x, hid_y,true);
    } else if(learned_midi < 6 && learned_midi > 1){
        cout << "msg val / 127: " << msg.value / 127.f << endl;
        handles[learned_midi - 2].setValueFromNormalized(msg.value / 127.f);
    }
}

void ofApp::createColors(){
    // "bold" from https://carto.com/carto-colors/
    int hexes[12] = {0x7F3C8D,0x11A579,0x3969AC,0xF2B701,0xE73F74,0x80BA5A,0xE68310,0x008695,0xCF1C90,0xf97b72,0x4b4b8f,0xA5AA99};
    
    for(int i = 0; i < 12; i++){
        qualitative_colors[i] = ofColor::fromHex(hexes[i]);
    }
    
    string line;
    ifstream data;
    
    data.open(ofToDataPath("colormaps/CET-R2.csv"));
    while(!data.eof()){
        getline(data,line);
        vector<string> vals = ofSplitString(line,",");
        rainbow_colors.push_back(ofColor(ofToInt(vals[0]),ofToInt(vals[1]),ofToInt(vals[2])));
    }
    
    data.close();
}

void ofApp::readSoundSlicesData(string csv_path, double* ranges){
    string line;
    ifstream data;
    
    data.open(ofToDataPath(csv_path));
    
    while(!data.eof()){
        getline(data, line);
        
        SoundSlice* soundSlice = new SoundSlice;
        soundSlice->setup(rainbow_colors,qualitative_colors,line);
        
        for(int i = 0; i < 4; i++){
            if(soundSlice->values[19 + i] < ranges[i * 2]){
                ranges[i * 2] = soundSlice->values[19 + i];
            }
            if(soundSlice->values[19 + i] > ranges[(i * 2) + 1]){
                ranges[(i * 2) + 1] = soundSlice->values[19 + i];
            }
        }
        
        slices.push_back(soundSlice);
    }
    
    data.close();
}

void ofApp::createPointKDTree(){
    vector<double> point;
    point.resize(4);
    
    for(int i = 0; i < slices.size(); i++){
        for(int j = 0; j < 4; j++){
            point[j] = handles[j].transform(slices[i]->values[19 + j]);
        }

        kdTree_params.addPoint(point);
    }
    
    kdTree_params.constructKDTree();
}

void ofApp::createHeadersAndDropdownOptions(){
    vector<string> values_headers;

    values_headers.push_back("param 1d index");     // 0
    values_headers.push_back("file num");           // 1
    values_headers.push_back("start sample");       // 2
    values_headers.push_back("num samples");        // 3
    values_headers.push_back("umap x norm");        // 4
    
    dropdown_options.push_back("UMAP 1");
    dropdown_index_lookup.push_back(4);
    
    values_headers.push_back("umap y norm");        // 5
    
    dropdown_options.push_back("UMAP 2");
    dropdown_index_lookup.push_back(5);
    
    values_headers.push_back("grid x norm");        // 6
    values_headers.push_back("grid y norm");        // 7
    values_headers.push_back("spec cent");          // 8
    values_headers.push_back("spec flatness");      // 9
    values_headers.push_back("pitch");              // 10
    values_headers.push_back("pitch conf");         // 11
    values_headers.push_back("loudness");           // 12
    values_headers.push_back("spec cent norm");     // 13
    
    dropdown_options.push_back("Spectral Centroid");
    dropdown_index_lookup.push_back(13);
    
    values_headers.push_back("spec flatness norm"); // 14
    
    dropdown_options.push_back("Spectral Flatness");
    dropdown_index_lookup.push_back(14);
    
    values_headers.push_back("pitch norm");         // 15
    
    dropdown_options.push_back("Pitch");
    dropdown_index_lookup.push_back(15);
    
    values_headers.push_back("pitch conf norm");    // 16
    
    dropdown_options.push_back("Pitch Confidence");
    dropdown_index_lookup.push_back(16);
    
    values_headers.push_back("loudness norm");      // 17
    
    dropdown_options.push_back("Loudness");
    dropdown_index_lookup.push_back(17);
    
    values_headers.push_back("num params");         // 18
    
    int n_params = int(slices[0]->values[18]);
    for(int i = 0; i < n_params; i++){
        values_headers.push_back("param " + ofToString(i) + " raw");
    }
    
    for(int i = 0; i < n_params; i++){
        values_headers.push_back("param " + ofToString(i) + " int");
    }
}
