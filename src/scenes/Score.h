#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"


class Score : public ofxScene {
    
public:
    // set the scene name through the base class initializer
    Score(ofxSceneManager& sm, int& p, float& t) : sceneManager(sm), puntaje(p), tiempo(t), ofxScene(SCORE_SCENE_NAME, false) {
        title.load("05_Score/titulo.png");
        title.setAnchorPercent(0.5,0.5);
        title.setPosition(ofPoint(ofGetWidth()*0.5,ofGetHeight()*0.2));
        
        score.load("DIN-LightAlternate.ttf",48);
        score.setText("");
        score.setAnchorPercent(0.5,0.5);
        score.setPosition(ofPoint(210,410));
        
        scoreSmall.load("DIN-LightAlternate.ttf",16);
        scoreSmall.setText("");
        scoreSmall.setAnchorPercent(0.5,0.5);
        scoreSmall.setPosition(ofPoint(210,410));
        
        soundInicio.load("Sounds/05_5.1-BotonInicio.wav");
        soundPositions.load("Sounds/05_5.2-ContadorPosiciones.wav");
        soundYourPosition.load("Sounds/05_5.3-PosicionFinal.wav");
        soundEnd.load("Sounds/05_5.4-FinDelJuego.wav");
    }
    
    // scene setup
    void setup() {
        
        title.setColor(255,0);
        
        score.setText(ofToString(puntaje));
        score.setSize(0);
        
        scoreSmall.setText("En un tiempo de "+ofToString(tiempo,2)+" segundos");
        scoreSmall.setSize(0);

        title.load("05_Score/titulo.png");
        title.setAnchorPercent(0.5,0.5);
        
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            title.color.animateTo(ofColor(255,255));
            
            score.size.animateTo(1.0);
            scoreSmall.size.animateTo(1.0);
            
            soundInicio.play();
            ofLogNotice(SCORE_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!score.isOrWillBeAnimating()) {
            finishedEntering();
            ofLogNotice(SCORE_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;
        title.update(dt);
        score.update(dt);
        scoreSmall.update(dt);
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            title.color.animateTo(ofColor(255,0));
            score.size.animateTo(0.);
            scoreSmall.size.animateTo(0.);
            
            soundEnd.play();
            
            ofLogNotice(SCORE_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(!score.isOrWillBeAnimating()) {
            finishedExiting();
            ofLogNotice(SCORE_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {
        title.draw();
        score.draw();
        scoreSmall.draw();
    }
    
    // cleanup
    void exit() {
        ofLogNotice(SCORE_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        sceneManager.gotoScene(MEMOTEST_SCENE_NAME);
    }
    
    ofxAnimatableObject<ofImage> title;
    ofxAnimatableObject<ofTrueTypeFont> score,scoreSmall;
    
    ofSoundPlayer soundInicio,soundPositions,soundYourPosition,soundEnd;
    float time;
    
    int& puntaje;
    float& tiempo;
    ofxSceneManager& sceneManager;
};
