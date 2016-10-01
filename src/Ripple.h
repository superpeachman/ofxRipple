//
//  Ripple.hpp
//  ripple
//
//  Created by 桃田稔也 on 10/1/16.
//
//
#pragma once
#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxGui.h"

class Ripple {
public:
    Ripple();
    void reset(float min, float max, float depth, float eps, float brightness);
    void update();
    void draw();
    void stop();
    void setRotY(bool flag);
    void setRotX(bool flag);
    void toggleDebug();
    void toggleAutoMove();
    void toggleEnableLight();
    
private:
    void setNormals( ofMesh &mesh );
    /*--------------------------------------------------------------
     
     
     GUI
     
     
     --------------------------------------------------------------*/
    ofxPanel gui;
    ofxSlider<int> minThreathold;
    ofxSlider<int> maxThreathold;
    ofxSlider<float> highestPos;
    ofxSlider<float> rippleMinThreathold;
    ofxSlider<float> rippleMaxThreathold;
    ofxSlider<float> rippleDecay;
    ofxSlider<float> rippleRadius;
    ofxSlider<float> rippleReflectedVal;
    ofxSlider<float> rippleHighlight;
    ofxSlider<float> rippleScale;
    ofxToggle enableLightFlag;
    ofxToggle lightAutoMoveFlag;
    ofxToggle drawLightFlag;
    ofxSlider<float> lightDepth;
    ofxToggle debugFlag;

    /*--------------------------------------------------------------
     
     
     Kinect
     
     
     --------------------------------------------------------------*/
    ofxKinect kinect;
    int wscreen = 640;
    int hscreen = 480;
    int wscreen2;
    int hscreen2;
    int pixels, pixels2;
    float rotX = ofDegToRad(0);
    float rotY = ofDegToRad(0);
    bool autoMoveFlag = false;
    
    vector<float> odata;//Old DEPTH pixel
    vector<float> ndata;//New pixel
    vector<float> tempV;//For temp data
    vector<ofColor> cdata;//For color data
    vector<float> ddata;//For depth data

    ofMesh rippleMesh;//Final Data
    ofVec3f rippleLight = ofVec3f(1.0f, 1.0f, 0.0f);
    ofColor utilColor;
    
    ofLight light;
    ofLight light2;
    float colorHue;
    ofColor lightColor;
    float lightX = ofDegToRad(0);
};
