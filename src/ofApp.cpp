#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    //    ofxGuiEnableHiResDisplay();
    ofBackground(100);
    ofEnableAntiAliasing();
    string csv_path = "211030_183738.csv";
    
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
    
//    cout << rainbow_colors.size() << endl;
    
    data.close();
    
    data.open(ofToDataPath(csv_path));
    
    while(!data.eof()){
        getline(data, line);

//        vector<string> tokens = ofSplitString(line," ,");
//        cout << tokens[0] << endl;

        SoundSlice* soundSlice = new SoundSlice;
        soundSlice->setup(rainbow_colors,qualitative_colors,line);
        slices.push_back(soundSlice);
    }
    
    data.close();
    
    labels.push_back("param 1d index");
    labels.push_back("file num");
    labels.push_back("start sample");
    labels.push_back("num samples");
    labels.push_back("umap x norm");
    labels.push_back("umap y norm");
    labels.push_back("grid x norm");
    labels.push_back("grid y norm");
    labels.push_back("spec cent");
    labels.push_back("spec flatness");
    labels.push_back("pitch");
    labels.push_back("pitch conf");
    labels.push_back("loudness");
    labels.push_back("spec cent norm");
    labels.push_back("spec flatness norm");
    labels.push_back("pitch norm");
    labels.push_back("pitch conf norm");
    labels.push_back("loudness norm");
    labels.push_back("num params");
    
    int n_params = int(slices[0]->values[10]);
    for(int i = 0; i < n_params; i++){
        labels.push_back("param " + ofToString(i) + " raw");
    }
    
    for(int i = 0; i < n_params; i++){
        labels.push_back("param " + ofToString(i) + " int");
    }
    
    // instantiate the dropdown //
    menu = new ofxDatGuiDropdown("Dimension", labels);
    
    // and position it in the middle of the screen //
    menu->setPosition(50,50);
    
    cout << menu->size() << endl;
    
    // register to listen for change events //
    menu->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    // finally let's have it open by default //
    //        menu->expand();
    
//    mySlider0 = new ofxDatGuiSlider("test0", 0.f, 1.f, 0.f);
//    mySlider0->setPosition(10,10);
//    mySlider0->onSliderEvent([&](ofxDatGuiSliderEvent e)
//                             {
//        cout << "A slider 0 was moved! " << e.value << endl;
//    });
//
//    mySlider1 = new ofxDatGuiSlider("test1", 0.f, 1.f, 0.f);
//    mySlider1->setPosition(10,50);
//    mySlider1->onSliderEvent([&](ofxDatGuiSliderEvent e)
//                             {
//        cout << "A slider 1 was moved! " << e.value << endl;
//    });
    
    plot_fbo.allocate(1920, 1080);
    windowResized(ofGetWidth(),ofGetHeight());
    drawPlot();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e){
    x_index_sl = e.child;
    
    cout << e.parent << " " << x_index_sl << "\n";
    
    drawPlot();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    menu->update();
//    mySlider0->update();
//    mySlider1->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //drawPlot(200,50,ofGetWidth()-300,ofGetHeight()-100);
    
    plot_fbo.draw(plot_x,plot_y,plot_w,plot_h);
    menu->draw();
}

void ofApp::drawPlot(){
    
    plot_fbo.begin();
    ofClear(255,255,255,255);

    ofFill();
    
    kdTree.clear();
    
    for(SoundSlice *slice : slices){
        kdTree.addPoint(slice->draw(0,0,plot_fbo.getWidth(),plot_fbo.getHeight(),x_index_sl,y_index_sl,c_index_sl));
    }
    
    kdTree.constructKDTree();
    
    plot_fbo.end();
}

void ofApp::find_nearest(int x, int y){
    float scaled_x = ofMap(x,plot_x,plot_x + plot_w,0.f,1.f);
    float scaled_y = ofMap(y,plot_y,plot_y + plot_h,1.f,0.f);
    cout << scaled_x << " " << scaled_y << endl;
    
    vector<double> query_pt = {scaled_x,scaled_y};
    vector<size_t> indexes;
    vector<double> dists;
    kdTree.getKNN(query_pt, 1, indexes, dists);
    
    cout << indexes[0] << endl;
    slices[indexes[0]]->post();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    cout << key << "\n";
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    find_nearest(x,y);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    find_nearest(x,y);
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
    plot_x = 300;
    plot_y = margin;
    plot_w = w - (plot_x + margin);
    plot_h = h - (margin * 2);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
