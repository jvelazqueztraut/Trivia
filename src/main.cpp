#include "ofMain.h"
#include "testApp.h"

int main(){
#ifdef FULLSCREEN
	ofSetupOpenGL(APP_WIDTH, APP_HEIGHT, OF_FULLSCREEN);
#else
	ofSetupOpenGL(1024, 768, OF_FULLSCREEN);
#endif
	// this replaces ofRunApp() and kicks off the running of the app
	// with the app utils
	ofRunAppWithAppUtils(new testApp());
}
