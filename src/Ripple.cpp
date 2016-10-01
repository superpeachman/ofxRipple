//
//  Ripple.cpp
//  ripple
//
//  Created by 桃田稔也 on 10/1/16.
//
//

#include "Ripple.h"

Ripple::Ripple(){
    /*--------------------------------------------------------------
     
     
     GUI
     
     
     --------------------------------------------------------------*/
    gui.setup();
    gui.add(minThreathold.setup("MIN DISTANCE", 1000, 1000, 5000));
    gui.add(maxThreathold.setup("MAX DISTANCE", 200, 0, 1000));
    gui.add(highestPos.setup("Highest Pos", 4000.f, 1000.f, 5000.f));
    gui.add(rippleMinThreathold.setup("Ripple Min Val", -100.0f, -1000.0f, 0.0f));
    gui.add(rippleMaxThreathold.setup("Ripple Max Val", 1000.0f, 0.0f, 1000.0f));
    gui.add(rippleDecay.setup("Ripple Decay", .95f, .9f, 1.1f));
    gui.add(rippleRadius.setup("Ripple Radius", 1.f, .9f, 1.1f));
    gui.add(rippleReflectedVal.setup("Ripple Reflected Col", .8f, 0.f, 1.f));
    gui.add(rippleHighlight.setup("Ripple Highlight", 255.f, 0.f, 255.f));
    gui.add(rippleScale.setup("Scale", 2.5f, 1.f, 5.f));
    gui.add(enableLightFlag.setup("Light Enable", false));
    gui.add(lightAutoMoveFlag.setup("Light Auto Move", false));
    gui.add(drawLightFlag.setup("Draw Light", false));
    gui.add(lightDepth.setup("Light Depth", 300, 0, 1000));
    gui.add(debugFlag.setup("Debug", false));

    /*--------------------------------------------------------------
     
     
     KINECT
     
     
     --------------------------------------------------------------*/
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();		// opens first available kinect
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    pixels = wscreen * hscreen;
    wscreen2 = wscreen * .5f;
    hscreen2 = hscreen * .5f;
    pixels2 = wscreen2 * hscreen2;
    
    tempV.resize(pixels2);
    odata.resize(pixels2);
    ndata.resize(pixels2);
    cdata.resize(pixels2);
    ddata.resize(pixels2);

    for (int i=0; i<pixels2; i++) {
        tempV[i] = .0f;
        odata[i] = .0f;
        ndata[i] = .0f;
        ddata[i] = .0f;
    }

    for (int y=0; y<hscreen2; y++) {
        for (int x=0; x<wscreen2; x++) {
            rippleMesh.addVertex(ofVec3f((x - wscreen2 * .5f), (y - hscreen2 * .5f), 0));
            rippleMesh.addTexCoord(ofVec2f(x, y));
            rippleMesh.addColor(ofColor(255, 255, 255));
        }
    }
    
    for (int y=0; y<hscreen2-1; y++) {
        for (int x=0; x<wscreen2-1; x++) {
            int i1 = x + wscreen2 * y;
            int i2 = x + 1 + wscreen2 * y;
            int i3 = x + wscreen2 * (y + 1);
            int i4 = x + 1 + wscreen2 * (y + 1);
            rippleMesh.addTriangle( i1, i2, i3 );
            rippleMesh.addTriangle( i2, i4, i3 );
        }
    }
    setNormals(rippleMesh);
    rippleLight.normalize();
    
    //ofSetSmoothLighting(true);
    //light.setPointLight();
    //light2.setPointLight();
    light.setSpotlight();
    light2.setSpotlight();
    
    light.setAmbientColor(ofColor(100.f, 0.f, 0.f));    //Ambient
    light.setDiffuseColor(ofColor(255.f, 0.f, 0.f));    //Around high light
    light.setSpecularColor(ofColor(255.f, 255.f, 255.f));    //Highlight
    light2.setAmbientColor(ofColor(100.f, 0.f, 0.f));    //Ambient
    light2.setDiffuseColor(ofColor(255.f, 0.f, 0.f));    //Around high light
    light2.setSpecularColor(ofColor(255.f, 255.f, 255.f));    //Highlight
    
    colorHue = ofRandom(0, 250);
    lightColor.setBrightness(255.f);
    lightColor.setSaturation(150.f);
}
//--------------------------------------------------------------
void Ripple::update(){
    kinect.update();
    
    if(kinect.isFrameNewDepth()){
        ofPixels pixData = kinect.getPixels();
        ofPixels depthData = kinect.getDepthPixels();
        
        for (int i = 0; i < pixels2; i++) {
            tempV[i] = odata[i];
            odata[i] = ndata[i];
            ndata[i] = tempV[i];
        }
        
        for (int i = 0; i < pixels2; i++) {
            int x = i % wscreen2;
            int y = (float)i / (float)wscreen2;
            
            /* --------------------
             
             Make new repple from odata
             
             -------------------- */
            //Get latest data.
            auto val = (odata[i - 1]
                         + odata[i + 1]
                         + odata[x + (y - 1) * wscreen2]
                         + odata[x + (y + 1) * wscreen2]) / 2.0f;
            //Old - New
            val = val - ndata[i];
            val = ofClamp(val, rippleMinThreathold, rippleMaxThreathold);
            
            if(val >= 0.0f){
                val *= (rippleDecay * ofMap(val, .0f, rippleMaxThreathold, 1.0f, .0f, true));
            }else{
                val *= (rippleDecay * ofMap(val, rippleMinThreathold, .0f, .0f, 1.0f, true));
            }
            ndata[i] = val;
            
            /* --------------------
             
             Make Current Ripple
             
             -------------------- */
            ofVec3f n = ofVec3f(
                                odata[x - rippleRadius + y * wscreen2] - odata[x + rippleRadius + y * wscreen2],
                                odata[x + (y - rippleRadius) * wscreen2] - odata[x + (y + rippleRadius) * wscreen2],
                                rippleRadius * 2.0f);// -> z
            
            ddata[i] = n.length(); //DEPTH DATA
            n.normalize();
            
            //    ofVec3f rippleLight = ofVec3f(1, 1, 0);
            auto spec = (1.0f - (rippleLight.x + n.x)) + (1.0f - (rippleLight.y + n.y));
            spec *= .5f;
            
            if (spec > rippleReflectedVal){
                spec = (spec - rippleReflectedVal) / (1 - rippleReflectedVal);
            }else{
                spec = .0f;
            }
            
            spec *= rippleHighlight;
            
            int xPos = x * 2 + n.x * 60;
            int yPos = y * 2 + n.y * 60;
            
            if(xPos > wscreen){
                xPos = wscreen - 1;
            }else if(xPos < 0){
                xPos = 1;
            }
            
            if(yPos > hscreen){
                yPos = hscreen - 1;
            }else if(yPos < 0){
                yPos = 1;
            }
            
            utilColor = pixData.getColor(xPos, yPos);
            utilColor += spec;
            cdata[i] = utilColor;
        }

        bool detectFigure = false;
        auto currentRow = 0;

        //cout << "pixel2:" << pixels2 << " depthData:" << depthData.size() << "\n";
        
        //Mask
        for (int k = 0; k < pixels2; k++) {
            int x = k % wscreen2;
            int y = (float)k / (float)wscreen2;
            int kk = x * 2 + y * 2 * wscreen;
            if(kk > pixels) kk = pixels - 1;
            if (currentRow != y) detectFigure = true;//If there in NEW row, try to get edge again.
            ofVec3f pos = rippleMesh.getVertex(k);

            //gui.add(minThreathold.setup("MIN DISTANCE", 1000, 1000, 5000));
            //gui.add(maxThreathold.setup("MAX DISTANCE", 200, 0, 1000));
            if (minThreathold > depthData[kk] && depthData[kk] > maxThreathold) {
                /*-------
                 
                 
                 HIT
                 
                 
                 -------- */
                
                pos.z = ofMap(depthData[kk], maxThreathold, minThreathold, 0.f, highestPos, true);
                rippleMesh.setVertex(k, pos);
                utilColor = pixData.getColor(x * 2, y * 2);
                rippleMesh.setColor(k, utilColor);
                if (detectFigure == true) { detectFigure = false; } // -> Get Edge
            }else{
                /*-------
                 
                 
                 NOT HIT
                 
                 
                 -------- */
                if(!enableLightFlag){
                    pos.z = ddata[k];
                    rippleMesh.setVertex(k, pos);
                    
                    utilColor.set(cdata[k]);
                    rippleMesh.setColor(k, utilColor);
                }else{
                    pos.z = ddata[k];
                    rippleMesh.setVertex(k, pos);
                    
                    utilColor.set(cdata[k]);
                    utilColor.a = ofMap(pos.z, 0.f, highestPos, 80.f, 255.f);
                    rippleMesh.setColor(k, utilColor);
                }
                

                
                //Get Edge
                if (detectFigure == false) {
                    detectFigure = true;
                    auto depthVal = ofMap(depthData[kk], maxThreathold, minThreathold, 0.f, highestPos, true);
                    depthVal = 50.f - depthVal;
                    odata[k] = depthVal;
                }
            }
            currentRow = y;
        }
        
        if(autoMoveFlag){
            rotY += 2;
            if(rotY == 360) rotY = 0;
        }else{
            if(rotY != 0) rotY -= 2;
        }

        if(enableLightFlag){
            if(lightAutoMoveFlag){
                lightX += 2;
                if(lightX > 360) lightX = 0;
                light.setPosition(ofGetWidth()*.5f + sin(ofDegToRad(lightX)) * (wscreen2 * 0.25f) * rippleScale, ofGetHeight()*.5f, lightDepth);
                light2.setPosition(ofGetWidth()*.5f + sin(ofDegToRad(-lightX)) * (wscreen2 * 0.25f) * rippleScale, ofGetHeight()*.5f, lightDepth);
            }else{
                light.setPosition(ofGetWidth()*.5f, ofGetHeight()*.5f, lightDepth);
                light2.setPosition(ofGetWidth()*.5f, ofGetHeight()*.5f, lightDepth);
            }
            
            colorHue += .2f;
            if(colorHue >= 255.f) colorHue = 0.f;
            lightColor.setHue(colorHue);
            light.setDiffuseColor(lightColor);
            lightColor.setHue(255.f - colorHue);
            light2.setDiffuseColor(lightColor);
            light.enable();
            light2.enable();
        }else{
            ofDisableLighting();
            light.disable();
            light2.disable();
        }
    }
}
//--------------------------------------------------------------
void Ripple::draw(){
    ofEnableDepthTest();
    ofPushMatrix();
    
    if(enableLightFlag){ ofEnableLighting(); }

    if(drawLightFlag){
        light.draw();
        light2.draw();
    }
    
    ofTranslate(ofGetWidth()*.5f, ofGetHeight()*.5f, 0);
    ofRotateX(rotX);
    ofRotateY(rotY);
    ofScale(rippleScale, rippleScale);

    rippleMesh.draw();

    if(enableLightFlag) ofDisableLighting();

    ofPopMatrix();
    ofDisableDepthTest();

    //if(debugFlag) kinect.draw(10, ofGetHeight() - 160, 160, 120);
    //if(debugFlag) kinect.drawDepth(170, ofGetHeight() - 160, 160, 120);
    
    if(debugFlag) gui.draw();
    if(debugFlag) ofDrawBitmapString(ofToString(ofGetFrameRate())+ "fps" , 0, 15);
}
//--------------------------------------------------------------
void Ripple::stop(){
    kinect.close();
}
//--------------------------------------------------------------
void Ripple::setRotX(bool flag){
    if(flag){
        rotX += 1;
    }else{
        rotX -= 1;
    }
}
//--------------------------------------------------------------
void Ripple::setRotY(bool flag){
    if(flag){
        rotY += 1;
    }else{
        rotY -= 1;
    }
}
//--------------------------------------------------------------
void Ripple::toggleDebug(){
    debugFlag = !debugFlag;
}
//--------------------------------------------------------------
void Ripple::toggleAutoMove(){
    autoMoveFlag = !autoMoveFlag;
}
//--------------------------------------------------------------
void Ripple::toggleEnableLight(){
    enableLightFlag = !enableLightFlag;
}

//--------------------------------------------------------------
void Ripple::setNormals( ofMesh &mesh ){
    
    //The number of the vertices
    int nV = mesh.getNumVertices();
    
    //The number of the triangles
    int nT = mesh.getNumIndices() / 3;
    
    vector<ofPoint> norm( nV );			//Array for the normals
    
    //Scan all the triangles. For each triangle add its
    //normal to norm's vectors of triangle's vertices
    for (int t=0; t<nT; t++) {
        
        //Get indices of the triangle t
        int i1 = mesh.getIndex( 3 * t );
        int i2 = mesh.getIndex( 3 * t + 1 );
        int i3 = mesh.getIndex( 3 * t + 2 );
        
        //Get vertices of the triangle
        const ofPoint &v1 = mesh.getVertex( i1 );
        const ofPoint &v2 = mesh.getVertex( i2 );
        const ofPoint &v3 = mesh.getVertex( i3 );
        
        //Compute the triangle's normal
        ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
        
        //Accumulate it to norm array for i1, i2, i3
        norm[ i1 ] += dir;
        norm[ i2 ] += dir;
        norm[ i3 ] += dir;
    }
    
    //Normalize the normal's length
    for (int i=0; i<nV; i++) {
        norm[i].normalize();
    }
    
    //Set the normals to mesh
    mesh.clearNormals();
    mesh.addNormals( norm );
}

