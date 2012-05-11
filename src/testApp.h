#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "DrawingModes.h"
#include "ofxGui.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void CrystalDraw(int FXslot);
    
        ofVideoGrabber      vidGrabber;
        
        ofImage colorHold;
        ofImage fullCapture;
    
        ofxCvColorImage         colorImg;
        ofxCvGrayscaleImage     grayImg;
        ofxCvGrayscaleImage		blackwhiteImage;
        ofxCvContourFinder      contourFinder;
        
        //Motion Detection
        ofxCvGrayscaleImage		cameraGrayPrevImage;
        ofxCvGrayscaleImage		cameraGrayDiffImage;
        ofxCvFloatImage			cameraDiffFloatImage;
        ofxCvFloatImage			cameraMotionFloatImage;
        ofxCvGrayscaleImage     camMotionGray;
    
        int                 FXcounter;
        int keyPress;
        
        int 				threshold;
        int                 camWidth;
        int                 camHeight;
    
        ofPoint             mapPt;
        ofPoint             nextMappedPt;
        ofPoint             mapCent;
        
        int                 ptAvg;
        int                 ptSum;
        float                 linethick;
        
        //Bool for taking a picture
        bool        snapPhoto;
    int snapCounter; //How many frames!
        //Special FBO and threshold capturer
        bool        snapSpecial;
        int         threshChange;
        int         threshCounter;
        ofFbo       fboNew;
        int         exposure; //how many steps through the threshold counter?
        int         flashCounter;
        
        int tog;
        
        //For DINO MODE
        ofImage     numbers[4];
        ofVec2f     a, b, tangent, normal, mappedA, mappedB;
        
        //For node connection mode - Currently not included
        int         connectDist[100][100];
        int         blobConnect[100];
        int         MysteryConnect;
        int         MysteryConnect2;
        int         connectCount;
    
    ofVideoPlayer video;
    
    //color Switching
    float       hCycle;
    ofColor     color;
    ofColor     ModColor;
    
    //Variables from gui
    int         blurAmt;
    float       lineThick;
    //float       mystery;
    //float       mystery2;
    bool        addBlend;
    //bool        mysterySwitch;
    //bool        BWSwitch;
    
   // bool        backSwitch;
    bool        motionSwitch;
    
    bool        shufflin; //Everyday I'm (for randomizing app variables)
    
    //GUIGUIGUIGUIGUIGUIGUIGUIGUIGUIGUIGUI
    ofxFloatSlider mystery;
	ofxFloatSlider mystery2;
    ofxIntSlider threshSlide;
	ofxIntSlider circleResolution;
    ofxIntSlider FXType;
    ofxIntSlider Blobsize;
    ofxIntSlider vidPos;
    ofxToggle showVid;
	ofxToggle mysterySwitch;
	ofxToggle BWSwitch;
    ofxToggle backSwitch;
	ofxButton ringButton;
    
	ofxPanel gui;
    
		
};
