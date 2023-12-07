#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofxAsync::run([&](){
        ofSleepMillis(500);
        running = true;
        ofSleepMillis(5000);
        running = false;
        has_finished = true;
    });
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    ofxAsync::update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(has_finished){
        ofSetColor(255, 0, 0);
        ofDrawRectangle(0, 0, 400, 400);
    }else if(running){
        ofSetColor(0, 128, 255);
        ofDrawRectangle(0, 0, 400, 400);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'c'){
        ofxAsync::stopAll();
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
