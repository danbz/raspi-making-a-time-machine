#include "ofApp.h"
#include "ofMath.h"
#include "ofxGui.h"
#include "ofxEasing.h"
#include "ofUtils.cpp"

int loopNumber;
int loopMax;
int movieDuration;
int currentFrame;
bool playForward;
bool showGui;
bool xFading = false;
int xFadeProgress = 0;
 namespace fs = std::filesystem;
int fadeSpeed;


//easing stuff
auto duration = 10.f;
float initTime = 0;
auto endTime = initTime + duration;
//bool leftToRight = false;
int startRange;
int endRange;
float endPosition;
float easedFrame;
int numOfFiles;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetVerticalSync(false);

    loopNumber =0; // initialise loops to off
    loopMax=5; // max times to loop each movie - controllable via gui
    movieDuration=0;
    currentFrame=0;
    playForward = true;
    //build gui group
    gui.setup( "Parameters", "settings.xml" );
    gui.add( duration.setup( "duration", 5.0, 1.0, 10.0 ) );
    gui.add( loopMax.setup( "loopMax", 5, 1, 10 ) );
    gui.add( fadeSpeed.setup( "fadeSpeed", 5, 1, 10 ) );
    gui.add( videoAlpha.setup( "alpha", 255, 0, 255 ) );
    gui.add( fade.setup( "fade", false));
    showGui = true;

    momentMovie.setPixelFormat(OF_PIXELS_NATIVE);
    // CGDisplayHideCursor(kCGDirectMainDisplay);

    //--- easing stuff
    endPosition = ofGetWidth() - 40;
    easedFrame =0.f;
    startRange= 0;
    endRange = 0;
    
    momentMovie.setPixelFormat(OF_PIXELS_NATIVE);
    
    //ofRestoreWorkingDirectoryToDefault();
    cout << "dataPathRoot(): " <<  dataPathRoot()  << endl; // prints-> dataPathRoot(): ../../../data/
    
    //read directory for number of files
    files.allowExt("mov");
    //string path = "/Users/danbuzzo/Desktop/lapses";
    string path = "movies"; //relative to /bin/data folder
    
    files.listDir(path); // put a video path here with several video files in a folder
    std::cout << "number of files" << files.size() << endl;
    numOfFiles = files.size() & INT_MAX;;

 	this->loadNew();
}

//--------------------------------------------------------------
void ofApp::loadNew(){
    loopNumber =0; // initialise loops to zero, frames to zero and set fading and forward
    currentFrame=0;
    playForward = true;
    xFading = true; // set flag to show we should be xFading as a new clip has been loaded
    videoAlpha=0; // set alpha of video clip to 0 in preparation to xFade
    
    // std::cout << "loopDone: " << loopNumber << endl;
    // if (momentMovie.isLoaded())momentMovie.closeMovie();

    vidImage.grabScreen(0,0, ofGetWidth(), ofGetHeight() ); // grab last frame of current video
    string newMovie = files.getPath(ofRandom(numOfFiles));
    std::cout << newMovie <<endl;
    momentMovie.load(newMovie);
	momentMovie.setLoopState(OF_LOOP_NONE);
 
	movieDuration = momentMovie.getTotalNumFrames();
    startRange= 0;//reset for easing function
    endRange = movieDuration;
//    if (fade){
//        xFade(); // if xfade is selected in gui then perform xfade.
//    }
}

//--------------------------------------------------------------
void ofApp::xFade(){

        if (xFading) {
            ofEnableAlphaBlending();
            ofEnableBlendMode( OF_BLENDMODE_ALPHA );
            ofSetColor( 255, 255 ); // draw the captured last frame of the previous video
            vidImage.draw( 0, 0, ofGetWidth(), ofGetHeight() );  // draw the captured frame to screen
            ofSetColor( 255, videoAlpha );

            //--- fade progression
            if (videoAlpha<255) {
                videoAlpha = videoAlpha+fadeSpeed;
                if (videoAlpha>255){videoAlpha=255;}
                }
            else {
                videoAlpha=255;
                xFading = false; // xfading complete
            }
        }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (loopNumber>=loopMax){
        this->loadNew();
    }

    momentMovie.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
   // if (fade) xFade(); // if fade selected in gui then call xfade function

    
    float now = ofGetElapsedTimef();

    if (ofGetElapsedTimef() > endTime) {// start a loop left to right
        initTime = ofGetElapsedTimef();
        endTime = initTime + duration;
        playForward = !playForward;
        if (playForward){
            startRange= 0;
            endRange = movieDuration;
        } else {
            startRange= movieDuration;
            endRange = 0;
            loopNumber ++;
        }
    }

    easedFrame=ofxeasing::map_clamp(now, initTime, endTime, startRange, endRange, &ofxeasing::cubic::easeInOut);
    // cout << easedFrame ;
    momentMovie.setFrame(easedFrame);
    momentMovie.draw(0,0,ofGetWidth(),ofGetHeight()); //draw frame of movie

    //ofDrawBitmapString(easedFrame(),10,10);
    if ( showGui ) gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ') loadNew(); // show new loop video
    if ( key == 'g' ) showGui = !showGui; // show or hide the gui
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
