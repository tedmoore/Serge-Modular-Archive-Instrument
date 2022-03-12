#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){

    // main window
    ofGLFWWindowSettings settings;
    settings.setSize(1681,1600);
    settings.setPosition(glm::vec2(0,0));
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    // skeuomorph window
    settings.setSize(1678,1972);
    settings.setPosition(glm::vec2(1678,0));
    settings.resizable = true;
    // uncomment next line to share main's OpenGL resources with gui
//    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    guiWindow->setVerticalSync(false);

    shared_ptr<ofApp> mainApp(new ofApp);
    mainApp->setupSkeuomorph();
    
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::drawSkeuomorph);
    ofAddListener(guiWindow->events().keyPressed,mainApp.get(),&ofApp::skeuomorphKeyPressed);
    ofAddListener(guiWindow->events().keyReleased,mainApp.get(),&ofApp::skeuomorphKeyReleased);
    ofAddListener(guiWindow->events().mousePressed,mainApp.get(),&ofApp::skeuomorphMousePressed);
    ofAddListener(guiWindow->events().mouseDragged,mainApp.get(),&ofApp::skeuomorphMouseDragged);
    ofAddListener(guiWindow->events().windowResized,mainApp.get(),&ofApp::skeuomorphResized);
    ofAddListener(guiWindow->events().mouseReleased,mainApp.get(),&ofApp::skeuomorphMouseReleased);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}
