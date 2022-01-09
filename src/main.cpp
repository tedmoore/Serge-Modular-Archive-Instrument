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
    settings.setPosition(glm::vec2(1920,0));
    settings.resizable = true;
    // uncomment next line to share main's OpenGL resources with gui
//    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    guiWindow->setVerticalSync(false);

    shared_ptr<ofApp> mainApp(new ofApp);
    mainApp->setupSkeuomorph();
    
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::drawSkeuomorph);
    ofAddListener(guiWindow->events().keyPressed,mainApp.get(),&ofApp::gui_keyPressed);
    ofAddListener(guiWindow->events().mousePressed,mainApp.get(),&ofApp::gui_mousePressed);
    ofAddListener(guiWindow->events().mouseDragged,mainApp.get(),&ofApp::gui_mouseDragged);
    ofAddListener(guiWindow->events().windowResized,mainApp.get(),&ofApp::gui_windowResized);
    ofAddListener(guiWindow->events().mouseReleased,mainApp.get(),&ofApp::gui_mouseReleased);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
}
