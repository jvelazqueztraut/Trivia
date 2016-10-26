#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
#ifdef _DEBUG
    ofShowCursor();
#else
	ofHideCursor();
#endif
    ofLogWarning()<<"Resolution: "<<ofGetWidth()<<","<<ofGetHeight();
    // setup for nice jaggy-less rendering
	ofSetVerticalSync(true);
	ofBackground(153,204,51);
    
    ofSoundSetVolume(0.0);
    
    background.load("background.png");
    background.setAnchorPercent(0.5,0.5);
    
    inicio.load("logo.png");
    inicio.setAnchorPercent(0.5,0.5);
    inicio.setPosition(ofGetWidth()*0.1,ofGetHeight()*0.9);
    
    puntaje=0;
    tiempo=0.;
    
	// load scenes
    sceneManager.add(new Inicio(sceneManager));
    sceneManager.add(new Pregunta(sceneManager, puntaje, tiempo));
    sceneManager.add(new Memotest(sceneManager, puntaje, tiempo));
    sceneManager.add(new Score(sceneManager, puntaje, tiempo));
    
    //sceneManager.setup(true);	// true = setup all the scenes now (not on the fly)
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside
	
	// start with a specific scene
	// set now to true in order to ignore the scene fade and change now
	sceneManager.gotoScene(INICIO_SCENE_NAME, false);
	lastScene = sceneManager.getCurrentSceneIndex();
	
	//
	// you can also turn off the auto sceneManager update and draw calls with:
	setSceneManagerUpdate(false);
	setSceneManagerDraw(false);
	//
	// the input callbacks in your scenes will be called if they are implemented
	//
	setSceneManager(&sceneManager);

	time = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void testApp::update() {
	float t = ofGetElapsedTimef();
	float dt = t - time;
	time = t;
    
    sceneManager.update();
    inicio.update(dt);
    
    ofSoundUpdate();
}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetColor(255);
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()*0.5,ofGetHeight()*0.5);
    if((APP_WIDTH/APP_HEIGHT)>(ofGetWidth()/ofGetHeight()))
        ofScale(ofGetHeight()/APP_HEIGHT,ofGetHeight()/APP_HEIGHT);
    else
        ofScale(ofGetWidth()/APP_WIDTH,ofGetWidth()/APP_WIDTH);
    background.draw(0,0);
    ofPopMatrix();
    
    sceneManager.draw();
    inicio.draw();
    
	// drop out of the auto transform space back to OF screen space
#ifdef _DEBUG
	// draw current scene info using the ofxBitmapString stream interface
	// to ofDrawBitmapString
	ofSetColor(255);
	ofxBitmapString(12, ofGetHeight()-8)
		<< "Current Scene: " << sceneManager.getCurrentSceneIndex()
		<< " " << sceneManager.getCurrentSceneName() << endl;
#endif
}

// current scene input functions are called automatically before calling these
//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	switch (key) {
        case ' ':
            ofLogWarning()<<"Resolution: "<<ofGetWidth()<<","<<ofGetHeight();
            break;
            
		case 'd':
			bDebug = !bDebug;
			break;
			
		/*case 'a':
			setAspect(!getAspect());
			break;
			
		case 'c':
			setCentering(!getCentering());
			break;
			
		case 'm':
			setMirrorX(!getMirrorX());
			break;
			
		case 'n':
			setMirrorY(!getMirrorY());
			break;
			
		case 'q':
			setWarp(!getWarp());
			break;*/
			
		case 'f':
			ofToggleFullscreen();
			break;
	
		case OF_KEY_LEFT:
			sceneManager.prevScene();
			break;
			
		case OF_KEY_RIGHT:
			sceneManager.nextScene();
			break;
			
		case OF_KEY_DOWN:
			if(sceneManager.getCurrentScene()) { // returns NULL if no scene selected
				lastScene = sceneManager.getCurrentSceneIndex();
			}
			sceneManager.noScene();
			break;
			
		case OF_KEY_UP:
			sceneManager.gotoScene(lastScene);
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    if(inicio.inside(ofPoint(x,y))){
        sceneManager.gotoScene(INICIO_SCENE_NAME, false);
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
    ofLogWarning()<<"Resolution change: "<<ofGetWidth()<<","<<ofGetHeight();
}
