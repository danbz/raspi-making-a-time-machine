#include "ofApp.h"
#include "ofMath.h"
#include "ofxGui.h"
#include "ofxEasing.h"
#include "ofUtils.cpp"
#include "ofClock.h"
#include <ctime>

float timeNow, timeLimit;
int loopNumber, loopMax, movieDuration, currentFrame, fadeSpeed;
bool playForward, showGui, showClock;
bool xFading = false;
int xFadeProgress = 0;
namespace fs = std::filesystem;

//easing stuff
auto duration = 10.f;
float initTime = 0;
auto endTime = initTime + duration;
//bool leftToRight = false;
//float endPosition, easedFrame;
int startRange, endRange, endPosition, easedFrame, numOfFiles;

//-- clock stuff
float clockRadius;
int clockPosLeft, clockPosTop;
int clockSec, clockMin, clockHrs, clockOffset;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    ofSetVerticalSync(false);

    ofSetFrameRate(15);
    
    timeLimit = 4;// this represents the five seconds you want to set as a flag to call
    timeNow = 0;// always a good practice to define your variables in setup.
    loopNumber =0; // initialise loops to off
    loopMax=5; // max times to loop each movie - controllable via gui
    movieDuration=0;
    currentFrame=0;
    playForward = true;
    
    //build gui group
    gui.setup( "Parameters", "settings.xml" );
    gui.add( duration.setup( "duration", 8.0, 1.0, 20.0 ) );
    gui.add( loopMax.setup( "loopMax", 5, 1, 10 ) );
    gui.add( fadeSpeed.setup( "fadeSpeed", 5, 1, 10 ) );
    gui.add( videoAlpha.setup( "alpha", 255, 0, 255 ) );
    gui.add( fade.setup( "fade", false));
    showGui = true;
    showClock = true;
    playForward = true;
    
    //momentMovie.setPixelFormat(OF_PIXELS_NATIVE);

    //--- easing stuff
    endPosition = ofGetWidth() - 40;
    easedFrame =0.f;
    startRange= 0;
    endRange = 0;
    
    //ofRestoreWorkingDirectoryToDefault();
    cout << "dataPathRoot(): " <<  dataPathRoot()  << endl; // prints-> dataPathRoot(): ../../../data/
    
    //read directory for number of files
    files.allowExt("mov");
    string path = "movies"; //relative to /bin/data folder
    
    files.listDir(path); // put a video path here with several video files in a folder
    std::cout << "number of files" << files.size() << endl;
    numOfFiles = files.size() & INT_MAX;;

 	this->loadNew();
    
    // -- clock setup
    clockRadius = 60.0;
    clockPosLeft = ofGetWidth()-110;
    clockPosTop = ofGetHeight()-110;;
    clockOffset = ofRandom(720);
    clock.setup();
    
    ofHideCursor();
    
    touch.init("/dev/input/event2");	// change according to your setup (evdev)
    ofLog()<<touch.getName();
    touchNumber = touch.getAbsTrackingID();
    
}

//--------------------------------------------------------------
void ofApp::loadNew(){
    loopNumber =0; // initialise loops to zero, frames to zero and set fading and forward
    currentFrame=0;
    playForward = true;
    xFading = true; // set flag to show we should be xFading as a new clip has been loaded
    videoAlpha=0; // set alpha of video clip to 0 in preparation to xFade
    // if (momentMovie.isLoaded())momentMovie.closeMovie();

    vidImage.grabScreen(0,0, ofGetWidth(), ofGetHeight() ); // grab last frame of current video
    string newMovie = files.getPath(ofRandom(numOfFiles));
    std::cout << newMovie <<endl;
    
    momentMovie.setUseTexture(false);
    
    momentMovie.load(newMovie);
	momentMovie.setLoopState(OF_LOOP_NONE);
   // momentMovie.play();
	movieDuration = momentMovie.getTotalNumFrames();
    startRange= 0;//reset for easing function
    endRange = movieDuration;
   if (fade){
       //xFade(); // if xfade is selected in gui then perform xfade.
   }
    // randomise loops and duration of newly loaded clip
    duration = ofRandom(3,10);
    loopMax = ofRandom(3,12);
    clockOffset = ofRandom(720);
    
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
            ofDisableAlphaBlending();
        }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (loopNumber>=loopMax){
        this->loadNew();
    }
    
    if (touchNumber < touch.getAbsTrackingID()){
        touchNumber = touch.getAbsTrackingID();
        loadNew();
    }
    
    //--clock updates
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    clockSec = now->tm_sec;
    //  clockMin = now->tm_min;
    clockMin = (easedFrame + clockOffset) % 60;
    clockHrs = (easedFrame+clockOffset)/60;
    // clockHrs = now->tm_hour;
    clock.update(clockSec, clockMin, clockHrs);
    
    float thisNow = ofGetElapsedTimef();
    
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
    
    easedFrame=ofxeasing::map_clamp(thisNow, initTime, endTime, startRange, endRange, &ofxeasing::cubic::easeInOut);
   momentMovie.setFrame(easedFrame);
    //momentMovie.play();
   //momentMovie.update();
    
    //cout << easedFrame << " : " << momentMovie.getCurrentFrame() << endl;

}

//--------------------------------------------------------------
void ofApp::draw(){
    momentMovie.update();
    //if (fade) xFade(); // if fade selected in gui then call xfade function
    
    momentMovie.draw(0,0,800,480); //draw frame of movie

    if ( showGui ) gui.draw();
    
    if (showClock)clock.draw( clockRadius, clockPosLeft, clockPosTop );
    
//    stringstream statusStream;
//    ofSetColor(255,255,255);
//    
//   // int recalledFrame = momentMovie.getCurrentFrame();
//    
//    statusStream  << easedFrame << " : " << momentMovie.getCurrentFrame() << endl
//    << "X: " << touch.getCoordTouch().x << endl
//    << "Y: " << touch.getCoordTouch().y << endl
//    << "BTN: " << touch.getButton() << endl
//    << "mtSlot: " << touch.getMTSlot() + 1 << endl
//    << "absPosX: " << touch.getAbsPos().x << endl
//    << "absPosY: " << touch.getAbsPos().y << endl
//    << "AbsTrackingID: " << touch.getAbsTrackingID() << endl
//    << endl;
//    ofDrawBitmapString(statusStream.str(),20,20);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key==' ') loadNew(); // show new loop video
    if ( key == 'g' ) showGui = !showGui; // show or hide the gui
    if ( key =='c' ) showClock = !showClock; //show of hide the clock

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

//--------------------------------------------------------------
void ofApp::exit(){
    
    touch.exit();
    
}
