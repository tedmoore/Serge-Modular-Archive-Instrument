#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

    // main window
    ofGLFWWindowSettings settings;
    settings.setSize(1920,1080);
    settings.setPosition(glm::vec2(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    // skeuomorph window
    settings.setSize(1920,1080);
    settings.setPosition(glm::vec2(500,500));
    settings.resizable = false;
    // uncomment next line to share main's OpenGL resources with gui
    //settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    guiWindow->setVerticalSync(false);

    shared_ptr<ofApp> mainApp(new ofApp);
    mainApp->setupSkeuomorph();
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::drawSkeuomorph);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
    
    //    ofSetupOpenGL(1920,1080,OF_WINDOW);            // <-------- setup the GL context
//
//    // this kicks off the running of my app
//    // can be OF_WINDOW or OF_FULLSCREEN
//    // pass in width and height too:
//    ofRunApp(new ofApp());

}
