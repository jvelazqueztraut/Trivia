#include "ofMain.h"
#include "testApp.h"

int main(){
#ifdef _DEBUG
	ofSetupOpenGL(1024, 768, OF_WINDOW);
#else
	ofSetupOpenGL(1024, 768, OF_FULLSCREEN);
#endif
	// this replaces ofRunApp() and kicks off the running of the app
	// with the app utils
	ofRunAppWithAppUtils(new testApp());
}

#ifdef TARGET_ANDROID
#include <jni.h>

//========================================================================
extern "C"{
	void Java_cc_openframeworks_OFAndroid_init( JNIEnv*  env, jobject  thiz ){
		main();
	}
}
#endif