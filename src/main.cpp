#include "ofMain.h"
#include "testApp.h"

int main(){
#ifdef _DEBUG
	ofSetupOpenGL(APP_WIDTH, APP_HEIGHT, OF_WINDOW);
#else
	ofSetupOpenGL(APP_WIDTH, APP_HEIGHT, OF_FULLSCREEN);
#endif
	// this replaces ofRunApp() and kicks off the running of the app
	// with the app utils
	ofRunAppWithAppUtils(new testApp());
}
