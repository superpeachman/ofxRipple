#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    /*--------------------------------------------------------------
     
     
     BASE
     
     
     --------------------------------------------------------------*/
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0, 0, 0);
    ofEnableAlphaBlending();
    
    ripple = new Ripple();
}

//--------------------------------------------------------------
void ofApp::update(){
    ripple->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ripple->draw();
}
//--------------------------------------------------------------
void ofApp::exit(){
    ripple->stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //FULL SCREEN --------------------------------------------------------------
    if(key == OF_KEY_ALT){
        ofToggleFullscreen();
    }
    switch(key){
        case OF_KEY_LEFT:
            ripple->setRotY(true);
            break;
        case OF_KEY_RIGHT:
            ripple->setRotY(false);
            break;
        case OF_KEY_UP:
            ripple->setRotX(true);
            break;
        case OF_KEY_DOWN:
            ripple->setRotX(false);
            break;
    }
    
    if(key == 'q'){
        ripple->toggleDebug();
    }else if(key == 'w'){
        ripple->toggleAutoMove();
    }else if(key == 'e'){
        ripple->toggleEnableLight();
    }else if(key == 'r'){
    }else if(key == 't'){
    }else if(key == '6'){
    }else if(key == '7'){
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
