//
//  DrawingModes.cpp
//  Crystal Eye X
//
//  Created by Blair Neal on 4/30/12.
//  Copyright (c) 2012 FakeLove. All rights reserved.
//

#include <iostream>
#include "DrawingModes.h"
#include "testApp.h"

CrystalDraw::CrystalDraw() {
}

//--------------------------------------------------------------
void CrystalDraw::draw(int FXslot){
    CrystalDraw();
    



//__--_--------__----_-_-----LINES
switch (FXslot) {
    case 0: //Lines
        for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
            
            ofBeginShape();
            for( int k=0; k<contourFinder.blobs[i].nPts; k+=3){
                mapPt.x=ofMap(contourFinder.blobs[i].pts[k].x,0,camWidth,0,ofGetWidth());
                mapPt.y=ofMap(contourFinder.blobs[i].pts[k].y,0,camHeight,0,ofGetHeight());
                ofVertex( mapPt.x, mapPt.y );
                mapCent.x = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
                mapCent.y = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,ofGetHeight());
                
                //ofSetColor(255, 255, 255);
                //ofVertex(mapPt.x, mapPt.y);
                //ofLine(mapPt.x,mapPt.y,mapCent.x,mapCent.y);
                //ofSetColor(color);
                
                ofFloatColor colorBig;
                colorBig.set(colorHold.getColor(contourFinder.blobs[i].pts[k].x, contourFinder.blobs[i].pts[k].y));
                ofSetColor(colorBig);
                
                // ofCircle(mapPt.x, mapPt.y, ofRandom(5,18));
                ofFill();
                //ofCircle(mapPt.x, mapPt.y, ofRandom(ofMap(threshCounter, 255, 0, 2, 10)));
                ofNoFill();
                
                //ofCircle(mapCent.x, mapCent.y, 10);
                //ofSetColor(255, 255, 255,200);
            }
            ofEndShape(); 
        }
        
        break;
        
        //__--_--------__----_-_-----CRYSTAL EYE
    case 1: //Crystal Eye
        
        for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
            ofFill();
            //ofBeginShape();
            for( int j=0; j<contourFinder.blobs[i].nPts; j=j+ofMap(.1, 0, 1, 1, 20) ) {
                mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                
                nextMappedPt.x = ofMap(contourFinder.blobs[i].pts[ofClamp(j+10,0,contourFinder.blobs[i].nPts-10) ].x,0,camWidth,0,ofGetWidth());
                nextMappedPt.y = ofMap(contourFinder.blobs[i].pts[ofClamp(j+10,0,contourFinder.blobs[i].nPts-10) ].y,0,camHeight,0,ofGetHeight());
                
                mapCent.x = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
                mapCent.y = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,ofGetHeight());
                
                glBegin(GL_TRIANGLE_FAN);
                ofFloatColor modColor;
                modColor.set(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y));
                glColor4f(modColor.r,modColor.g,modColor.b,1.0);
                glVertex3f(mapPt.x,mapPt.y, 0);
                modColor=colorHold.getColor(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y);
                glColor4f(modColor.r,modColor.g,modColor.b,1.0);
                glVertex3f(mapCent.x,mapCent.y, 0);
                modColor=colorHold.getColor(contourFinder.blobs[i].pts[ofClamp(j+ofMap(.1, 0, 1, 1, 90),0,contourFinder.blobs[i].nPts-ofMap(.1, 0, 1, 1, 20)) ].x, contourFinder.blobs[i].pts[ofClamp(j+ofMap(.1, 0, 1, 1, 20),0,contourFinder.blobs[i].nPts-ofMap(.1, 0, 1, 1, 90)) ].y);
                glColor4f(modColor.r,modColor.g,modColor.b,1.0);
                glVertex3f(nextMappedPt.x, nextMappedPt.y,0 );
                glEnd();
            }            
            //ofSetColor(255,255,255);
        }
        break;
    case 2:
        if(mysterySwitch){
            MysteryConnect = ofMap(mystery, 0.0, 1.0, 5, 60);
            MysteryConnect2 = ofMap(mystery2, 0.0, 1.0, 10, 30);
            int scale = MysteryConnect;
            int stepSize = MysteryConnect2;
            for( int i = 0; i < (int)contourFinder.blobs.size(); i++ ) {
                ofMesh mesh;
                for( int j = 0; j < contourFinder.blobs[i].nPts - stepSize; j += stepSize ) {
                    a = contourFinder.blobs[i].pts[j];
                    b = contourFinder.blobs[i].pts[j + stepSize];
                    mappedA.x = ofMap(a.x, 0, camWidth, 0, ofGetWidth());
                    mappedA.y = ofMap(a.y, 0, camHeight, 0, ofGetHeight());
                    mappedB.x = ofMap(b.x, 0, camWidth, 0, ofGetWidth());
                    mappedB.y = ofMap(b.y, 0, camHeight, 0, ofGetHeight());
                    tangent = mappedB - mappedA;
                    normal = tangent.getRotated(90);
                    normal.normalize();
                    ofVec2f corner = (mappedA + mappedB) / 2 + normal * scale;
                    // FOR STRAIGHT SECTIONS
                    mesh.addVertex(mappedA);
                    mesh.addVertex(mappedB);
                    mesh.addVertex(corner);
                    color.set(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y));
                }
                ofSetColor(color);
                mesh.draw();	
            }
        }
        
        if(!mysterySwitch){
            MysteryConnect = ofMap(mystery, 0.0, 1.0, 5, 60);
            MysteryConnect2 = ofMap(mystery2, 0.0, 1.0, 12, 30);
            
            int scale = MysteryConnect;
            int stepSize = MysteryConnect2;
            for( int i = 0; i < (int)contourFinder.blobs.size(); i++ ) {
                ofBeginShape();
                for( int j = 0; j < contourFinder.blobs[i].nPts - stepSize; j += stepSize ) {
                    a = contourFinder.blobs[i].pts[j];
                    b = contourFinder.blobs[i].pts[j + stepSize];
                    mappedA.x = ofMap(a.x, 0,camWidth, 0, ofGetWidth());
                    mappedA.y = ofMap(a.y, 0, camHeight, 0, ofGetHeight());
                    mappedB.x = ofMap(b.x, 0, camWidth, 0, ofGetWidth());
                    mappedB.y = ofMap(b.y, 0, camHeight, 0, ofGetHeight());
                    tangent = mappedB - mappedA;
                    normal = tangent.getRotated(90);
                    normal.normalize();
                    float noisy = ofMap(ofNoise(j,j,ofGetElapsedTimef()/2.0),0,1,0,scale);
                    ofVec2f corner = (mappedA + mappedB) / 2 + normal * noisy;
                    ofCurveVertex(mappedA.x, mappedA.y);
                    ofCurveVertex(corner.x, corner.y);
                    ofCurveVertex(mappedB.x, mappedB.y);
                    color.set(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y));
                }
                ofSetColor(color);
                ofEndShape();
            }
            break;
            
        }
}