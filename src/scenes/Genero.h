#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"

#define GENERO_SEPARATION (200*ofGetWidth()/APP_WIDTH)

class Genero : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Genero(ofxSceneManager& sm) : sceneManager(sm), ofxScene(GENERO_SCENE_NAME, false) {
        title.load("01_Genero/titulo.png");
        title.setAnchorPercent(0.5,0.5);
        title.setPosition(ofPoint(ofGetWidth()*0.5,ofGetHeight()*0.2));
        
        mujer.load("01_Genero/mujer.png");
        mujer.setAnchorPercent(0.5,0.5);
        mujer.setPosition(ofPoint(ofGetWidth()*0.5+GENERO_SEPARATION,ofGetHeight()*0.5));
        
        mujerTitulo.load("01_Genero/mujerTitulo.png");
        mujerTitulo.setAnchorPercent(0.5,0.0);
        mujerTitulo.setPosition(ofPoint(ofGetWidth()*0.5+GENERO_SEPARATION,ofGetHeight()*0.5+120));
        
        hombre.load("01_Genero/hombre.png");
        hombre.setAnchorPercent(0.5,0.5);
        hombre.setPosition(ofPoint(ofGetWidth()*0.5-GENERO_SEPARATION,ofGetHeight()*0.5));
        
        hombreTitulo.load("01_Genero/hombreTitulo.png");
        hombreTitulo.setAnchorPercent(0.5,0.0);
        hombreTitulo.setPosition(ofPoint(ofGetWidth()*0.5-GENERO_SEPARATION,ofGetHeight()*0.5+120));
        
        soundMujer.load("Sounds/01_1.2-Mujer.wav");
        soundHombre.load("Sounds/01_1.1-Hombre.wav");
    }
    
    // scene setup
    void setup() {
        title.setColor(ofColor(255,0));

        mujer.setColor(ofColor(255,0));
        mujer.setSize(1.);
        hombre.setColor(ofColor(255,0));
        hombre.setSize(1.);
        
        mujerTitulo.setColor(ofColor(255,0));
        hombreTitulo.setColor(ofColor(255,0));

        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            title.color.animateTo(ofColor(255,255));
            mujer.color.animateTo(ofColor(255,255));
            hombre.color.animateTo(ofColor(255,255));
            mujerTitulo.color.animateTo(ofColor(255,255));
            hombreTitulo.color.animateTo(ofColor(255,255));
            
            ofLogNotice(GENERO_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!mujer.isOrWillBeAnimating() && !hombre.isOrWillBeAnimating()) {
            finishedEntering();
            ofLogNotice(GENERO_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;
        title.update(dt);
        mujer.update(dt);
        mujerTitulo.update(dt);
        hombre.update(dt);
        hombreTitulo.update(dt);
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {            
            
            title.color.animateTo(ofColor(255,0));
            mujerTitulo.color.animateTo(ofColor(255,0));
            hombreTitulo.color.animateTo(ofColor(255,0));
            
            ofLogNotice(GENERO_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(!title.isOrWillBeAnimating()) {
            finishedExiting();            
            ofLogNotice(GENERO_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {
        title.draw();
        ofPushStyle();
        //ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        mujer.draw();
        hombre.draw();
        ofPopStyle();
        mujerTitulo.draw();
        hombreTitulo.draw();
    }
    
    // cleanup
    void exit() {
        ofLogNotice(GENERO_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        if(mujer.inside(ofPoint(x,y))){
            soundMujer.play();
            sceneManager.gotoScene(PREGUNTA_SCENE_NAME);
        }
        else if(hombre.inside(ofPoint(x,y))){
            soundHombre.play();
            sceneManager.gotoScene(MEMOTEST_SCENE_NAME);
        }
    }
    
    ofxAnimatableObject<ofImage> title;
    ofxAnimatableObject<ofImage> mujer,mujerTitulo,hombre,hombreTitulo;
    ofSoundPlayer soundMujer,soundHombre;
    float time;
    ofxSceneManager& sceneManager;
};
