#include "testApp.h"

GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0};

GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0};

//--------------------------------------------------------------
void testApp::setup(){
    
    ofBackground(0, 0, 0);
    
    //ofSetVerticalSync(true);
	//ofEnableAlphaBlending();
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    blorp.loadImage("GFX/blorp3.png");
    //Video
    video.loadMovie("/Users/fakelove/Desktop/CAPETOWNBEACH.mov");
    video.play();
    video.setPaused(true);
    video.firstFrame();
    /*
    glShadeModel (GL_SMOOTH);

    //initialize lighting
    glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
    */
    
    
    camWidth = 1920;
    camHeight =1080;
    //vidGrabber.setVerbose(true);
    //vidGrabber.initGrabber(camWidth,camHeight);
    
    colorImg.allocate(camWidth,camHeight);
	grayImg.allocate(camWidth,camHeight);
    
    //FBO Stuff
    ofFbo::Settings settings;    
    settings.width = 2048;    
    settings.height = 2048;    
    settings.internalformat = GL_RGBA;    
    settings.numSamples = 0;    
    settings.useDepth = false;    
    settings.useStencil = false; 
    
    fboNew.allocate(settings);
    
    fboNew.begin();  
    ofClear(0, 0, 0, 0);  
    fboNew.end();
    threshCounter=0;
    threshold = 120;
    exposure = 255;
    snapCounter = 0;
    snapSpecial=false;
    transStart = false;
    transFinished = false;
    transCount = 0;
    
    FXcounter = 4;
    mysterySwitch=true;
    mystery = .05;
    mystery2=.05;
    
    gui.setup("Panel");
    gui.add(FXType.setup("FXType", 0, 0,5));
    gui.add(mystery.setup("Mystery", 0.2, 0.0, 1.0));
    gui.add(mystery2.setup("Mystery2", 0.2, 0.0, 1.0));
    gui.add(showVid.setup("Show Video", false));
    gui.add(mysterySwitch.setup("Mystery Switch", true));
    gui.add(Blobsize.setup("Blobsize", 3,1,40));
    gui.add(threshSlide.setup("Threshold", 120, 0, 255));
    gui.add(vidPos.setup("FrameNum", 0, 0, video.getTotalNumFrames()));
    gui.add(saveToggle.setup("RENDER", false));
    gui.add(transToggle.setup("Transition", false));
    gui.add(transLength.setup("Trans. Length", 5,0,50));
    gui.add(transStep.setup("Trans. Step", 0,0,20));
    //gui.add(transStart.setup("Transition", false));
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    //If the transition hasnt finished, then
    if(!transFinished){
        transStart = transToggle;
    }
    
    //If the transition hasn't finished..or if the flag is still false, then snapspecial equals the gui state
    if(!transFinished){
        snapSpecial=saveToggle;
    }
    
    //Don't let the slider affect positioning
    if(!snapSpecial && !transStart){
        threshold = threshSlide;
        video.setFrame(vidPos);
    }
    
    ofSetColor(255);
    bool bNewFrame = false;
    
    video.idleMovie();
    //vidGrabber.grabFrame();
    //bNewFrame = vidGrabber.isFrameNew();
    
    //if (bNewFrame){
        /* if (snapSpecial && threshCounter < exposure+1) {
             threshCounter++;
         }*/
        colorImg.setFromPixels(video.getPixels(), camWidth,camHeight);
        grayImg=colorImg;
        
        grayImg.threshold(threshold);
        contourFinder.findContours(grayImg, 10, (camWidth*camHeight)/Blobsize, 15, true);
        colorHold.setFromPixels(video.getPixels(), camWidth, camHeight, OF_IMAGE_COLOR);
   // }

}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetRectMode(OF_RECTMODE_CORNER);
     //colorImg.draw(0,0,ofGetWidth(),0.75*ofGetWidth());
     //video.draw(0, 0, ofGetWidth(), .75*ofGetWidth());
    
    if(snapSpecial || transStart){
        fboNew.begin();
    }
    
    if(((threshold==0 || threshold==1) && (snapSpecial||transStart)) || showVid){
        video.draw(0, 0, ofGetWidth(), .5625*ofGetWidth());
    }
   
    CrystalDraw(FXType);
    
    if (transStart) {
        
        fboNew.end();
        
        ofSetColor(255, 255, 255);
        
        fboNew.draw(0, 0); 
        
        threshold = ofMap(threshCounter, 0, exposure, 0, 255);
        
        if (threshCounter==transCount) {
            cout << "Save trans: " << snapCounter <<endl;
            fullCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            char fileName[255];
            sprintf(fileName, "images/images_%05i.tga", snapCounter);
            fullCapture.saveImage(fileName);
            //sprintf(snapString, "saved %s", fileName);
            snapCounter++; //New Frame
            video.setFrame(vidPos+snapCounter); //Start from chosen frame
            //snapSpecial = false;
            
            //Reset the FBO
            fboNew.begin();  
            ofClear(0, 0, 0, 0);  
            fboNew.end();
            threshold = 0;
            threshCounter=0;
            
            transCount=transCount+transLength; //Increase the count for the next pass
            //Stop cycle and move to regular rendering
        }
        
        threshCounter++;
        
        if (threshCounter>253) {
            transStart = false;
            snapSpecial=true;
            threshCounter=0;
            transFinished = true;
            snapCounter = video.getCurrentFrame();
        }
    }

    //Store the drawins in an fbo
    
    if (snapSpecial 
        && (threshCounter < exposure+1) 
        && (snapCounter < video.getTotalNumFrames()) 
        && !transStart) {
        
        fboNew.end(); 
        
        ofSetColor(255, 255, 255);
       
        fboNew.draw(0, 0); 
        threshold = ofMap(threshCounter, 0, exposure, 0, 255);
        //Currently this is not actually getting every thresh level because its happening twice per frame
        if (threshCounter==exposure) {
            fullCapture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            char fileName[255];
            sprintf(fileName, "images/images_%05i.tga", snapCounter);
            fullCapture.saveImage(fileName);
            //sprintf(snapString, "saved %s", fileName);
            snapCounter++; //New Frame
            video.setFrame(vidPos+snapCounter); //Start from chosen frame
            //snapSpecial = false;
            fboNew.begin();  
            ofClear(0, 0, 0, 0);  
            fboNew.end();
            threshold = 120;
            threshCounter = 0;
        }
        threshCounter++;
       // cout<<threshCounter<<endl;
        
    }
    else {
        //threshCounter = 0;
    }
    
    
    
    ofSetColor(255,255,255);
    if(snapSpecial){
        ofSetColor(0,50,255,50+(50*sin(ofGetElapsedTimef())));
        ofFill();
        ofRect(26, 430, 150, 30);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Rendering Started!", 30,450);
    }
    
    if(transStart){
        ofSetColor(255,0,255,50+(50*sin(ofGetElapsedTimef())));
        ofFill();
        ofRect(26, 430, 180, 30);
        ofSetColor(255,255,255);
        ofDrawBitmapString("Transition Rendering!", 30,450);
    }
    
    ofSetColor(255,255,255);
    ofDrawBitmapString("Length of Transition: " +ofToString(255/(transLength+.0001)), 30,480);
    ofDrawBitmapString("Trans toggle " +ofToString(transStart), 300,480);
    ofDrawBitmapString("Snap Toggle " +ofToString(snapSpecial), 600,480);
    ofDrawBitmapString("Frame Num: " +ofToString(video.getCurrentFrame()), 30,500);
    ofDrawBitmapString("Total Frame Num: " +ofToString(video.getTotalNumFrames()), 30,530);
    ofDrawBitmapString("Threshold: " +ofToString(threshold), 30,560);
    //ofDrawBitmapString("Current Drawing Mode: " +ofToString(FXcounter), 30,150);
    ofSetColor(255, 0, 0);
    ofFill();
    ofRect(30, 600, ofMap(video.getCurrentFrame(), 0, video.getTotalNumFrames(), 0, 100),50);
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofRect(30, 600,100, 50);

    gui.draw();
    
    if(snapCounter>2652 ||video.getCurrentFrame() >2651){
        ofExit(); //quit when done processing
    }
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key){
		case 'p':
			threshold +=3;
			if (threshold > 255) threshold = 255;
			break;
		case 'o':
			threshold -=3;
			if (threshold < 0) threshold = 0;
			break;
        case OF_KEY_RETURN:
            ofToggleFullscreen();
            break;
        case 's':
            snapSpecial=true;
            break;
        case '0':
            FXcounter++;
            FXcounter=FXcounter%6;
            cout<<FXcounter<<endl;
            break;

	}
    keyPress=key;
}

//--------------------------------------------------------------
void testApp::CrystalDraw(int FXType){
    //__--_--------__----_-_-----LINES
    switch (FXType) {
        case 0: //Lines
            ofSetColor(255, 255, 255);
            ofSetLineWidth(1);
            MysteryConnect = ofMap(mystery, 0.0, 1.0, 2, 60);
            MysteryConnect2 = ofMap(mystery2, 0.0, 1.0, 3, 15);
            ofSetCircleResolution(MysteryConnect2);
            for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
                 
                ofBeginShape();
                for( int k=0; k<contourFinder.blobs[i].nPts; k+=3){
                    mapPt.x=ofMap(contourFinder.blobs[i].pts[k].x,0,camWidth,0,ofGetWidth());
                    mapPt.y=ofMap(contourFinder.blobs[i].pts[k].y,0,camHeight,0,ofGetHeight());
                    ofVertex( mapPt.x, mapPt.y );
                    mapCent.x = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
                    mapCent.y = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,ofGetHeight());
                    /*
                    ofSetColor(255, 255, 255);
                    ofVertex(mapPt.x, mapPt.y);
                    ofLine(mapPt.x,mapPt.y,mapCent.x,mapCent.y);
                    ofSetColor(color);
                    */
                    
                    ofFloatColor colorBig;
                    colorBig.set(colorHold.getColor(contourFinder.blobs[i].pts[k].x, contourFinder.blobs[i].pts[k].y));
                    ofSetColor(colorBig);
                    ofFill();
                     ofCircle(mapPt.x, mapPt.y, ofRandom(2,MysteryConnect));
                    
                    ofCircle(mapPt.x, mapPt.y, ofRandom(ofMap(threshCounter, 255, 0, 2, 10)));
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
                for( int j=5; j<contourFinder.blobs[i].nPts-5; j=j+ofMap(mystery, 0, 1, 1, 20) ) {
                    mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                    mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                    
                    nextMappedPt.x = ofMap(contourFinder.blobs[i].pts[ofClamp(j+5,0,contourFinder.blobs[i].nPts-5) ].x,0,camWidth,0,ofGetWidth());
                    nextMappedPt.y = ofMap(contourFinder.blobs[i].pts[ofClamp(j+5,0,contourFinder.blobs[i].nPts-5) ].y,0,camHeight,0,ofGetHeight());
                    
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
                    modColor=
                    colorHold.getColor(contourFinder.blobs[i].pts[ofClamp(j+ofMap(mystery, 0, 1, 1, 20),0,contourFinder.blobs[i].nPts-ofMap(mystery , 0, 1, 1, 20)) ].x,
                        contourFinder.blobs[i].pts[ofClamp(j+ofMap(.1, 0, 1, 1, 20),0,contourFinder.blobs[i].nPts-ofMap(mystery, 0, 1, 1, 20)) ].y);
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
            }
            break;
        case 3: //
            for(int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
                //ofSetColor(color);
                ofNoFill();
                ofBeginShape();
                for( int j=0; j<contourFinder.blobs[i].nPts; j=j+ofMap(mystery, 0, 1, 1, 20) ) {
                    mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                    mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                    ofVertex(mapPt);
                    
                    //Draw connections to center - OPTIONAL
                    //ofSetLineWidth(ofMap(connectDist [i][j], 0, 300, .1, 7));
                    if(mysterySwitch){
                        mapCent.x = ofMap(contourFinder.blobs[i].centroid.x,0,camWidth,0,ofGetWidth());
                        mapCent.y = ofMap(contourFinder.blobs[i].centroid.y,0,camHeight,0,ofGetHeight());
                        int centDist;
                        centDist = ofDist(mapPt.x,mapPt.y, mapCent.x,mapCent.y);
                        ofSetLineWidth(ofMap(centDist, 0, 400, .03, 2.5));
                        color.set(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y));
                        ofSetColor(color);
                        ofLine(mapPt.x,
                               mapPt.y,
                               mapCent.x,
                               mapCent.y
                               );
                    }
                }
                
                //ExtraSketches
                if (mystery2>0) {    
                    ofSetLineWidth(ofMap(lineThick, 0.0, 1.0, 0.0, 3.0));
                    for (int k = 1; k<ofMap(mystery2, 0.0, 1.0, 1, 30); k++) {
                        //color.setHsb(i*k*7, 255, 255);
                        ofSetColor(color);
                        for( int j=0; j<contourFinder.blobs[i].nPts; j+=k*4 ) {
                            mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                            mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                            ofVertex( mapPt.x, mapPt.y );
                        }
                    }
                }
                ofSetLineWidth(ofMap(lineThick, 0.0, 1.0, 0.0, 10.0));
                ofEndShape();  
            }
            break;
        case 4: //Klimt
            for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
                for( int j=0; j<contourFinder.blobs[i].nPts; j=j+ofMap(mystery, 0, 1, 1, 80)){
                   
                    mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                    mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                    
                    ofFill();
                    int randomRectX= ofMap(threshold, 0, 255, 30, 1);//ofRandom(1,ofMap(mystery2, 0, 1, 1, 80));
                    
                    //int randomRectY= ofRandom(1,ofMap(mystery2, 0, 1, 1, 80));
                     ofSetColor(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y)) ;
                    ofRect(mapPt, randomRectX,randomRectX);
                    
                    ofNoFill();
                    ofSetLineWidth(.5);
                    ofSetColor(0, 0, 0);
                    ofRect(mapPt, randomRectX,randomRectX);
                }
            }
            break;
            
        case 5: //3d Sphere
            ofEnableAlphaBlending();
            for( int i=0; i<(int)contourFinder.blobs.size(); i++ ) {
                for( int j=0; j<contourFinder.blobs[i].nPts; j=j+ofMap(mystery, 0, 1, 1, 80)){
                    
                    mapPt.x=ofMap(contourFinder.blobs[i].pts[j].x,0,camWidth,0,ofGetWidth());
                    mapPt.y=ofMap(contourFinder.blobs[i].pts[j].y,0,camHeight,0,ofGetHeight());
                    
                    //ofFill();
                    int randomRectX= ofMap(threshold, 0, 255, 30, 1);//ofRandom(1,ofMap(mystery2, 0, 1, 1, 80));
                    
                    //int randomRectY= ofRandom(1,ofMap(mystery2, 0, 1, 1, 80));
                    ofSetColor(colorHold.getColor(contourFinder.blobs[i].pts[j].x, contourFinder.blobs[i].pts[j].y)) ;
                    //ofRect(mapPt, randomRectX,randomRectX);
                    
                    
                    blorp.draw(mapPt, randomRectX, randomRectX);
                   
                    //ofNoFill();
                    //ofSetLineWidth(.5);
                    //ofSetColor(0, 0, 0);
                    //ofRect(mapPt, randomRectX,randomRectX);
                }
            }
            ofDisableAlphaBlending();
            break;

        
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}