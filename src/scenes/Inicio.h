#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"


class Inicio : public ofxScene {
public:
    // set the scene name through the base class initializer
    Inicio(ofxSceneManager& sm, int& g) : sceneManager(sm), game(g), ofxScene(INICIO_SCENE_NAME, false) {

        icon.loadImage("icono.png");
        icon.setAnchorPercent(0.5,0.5);
        icon.position.setDuration(2.5);
        icon.position.setCurve(TANH);
        
        title.loadImage("00_Inicio/titulo.png");
        title.setAnchorPercent(0.5,0.5);
        title.position.setDuration(2.);
        title.position.setCurve(TANH);
        
        membrete.loadImage("membretre.png");
        membrete.setAnchorPercent(0.5,1.0);
        membrete.position.setDuration(1.5);
        membrete.position.setCurve(TANH);
        
        memotest.loadImage("00_Inicio/memotest.png");
        memotest.setAnchorPercent(0.5,0.5);
        
        trivia.loadImage("00_Inicio/trivia.png");
        trivia.setAnchorPercent(0.5,0.5);
        
        sound.loadSound("Sounds/00_0.2-Transicion0a1.wav");
        soundMemotest.loadSound("Sounds/01_1.2-Mujer.wav");
        soundTrivia.loadSound("Sounds/01_1.1-Hombre.wav");
        
        
        icon.setSize(ofGetHeight()/APP_HEIGHT);
        title.setSize(ofGetHeight()/APP_HEIGHT);
        membrete.setSize(ofGetHeight()/APP_HEIGHT);
    }
    
    // scene setup
    void setup() {
        title.setPosition(ofGetWidth()*0.65,-ofGetHeight()*0.6);
        
        if(game<0){
            icon.setPosition(ofGetWidth()*0.2,-ofGetHeight()*0.6);
            membrete.setPosition(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight());
        }
        else{
            icon.setPosition(ofGetWidth()*0.2,ofGetHeight()*0.4);
            membrete.setPosition(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()*0.2);
        }
        
        trivia.setPosition(ofGetWidth()*0.65,ofGetHeight()*0.45);
        trivia.setSize(0.);
        
        memotest.setPosition(ofGetWidth()*0.65,ofGetHeight()*0.45+trivia.getHeight()+20);
        memotest.setSize(0.);

        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            icon.position.animateTo(ofPoint(ofGetWidth()*0.2,ofGetHeight()*0.4));
            title.position.animateTo(ofPoint(ofGetWidth()*0.65,ofGetHeight()*0.25));
            membrete.position.animateTo(ofPoint(ofGetWidth()*0.5,ofGetHeight()));
            
            trivia.size.animateToAfterDelay(ofGetHeight()/APP_HEIGHT,1.);
            memotest.size.animateToAfterDelay(ofGetHeight()/APP_HEIGHT,1.5);
            
            ofLogNotice(INICIO_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!trivia.isOrWillBeAnimating() || !memotest.isOrWillBeAnimating()) {
            finishedEntering();
            ofLogNotice(INICIO_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;
        title.update(dt);
        icon.update(dt);
        membrete.update(dt);
        
        memotest.update(dt);
        trivia.update(dt);
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            icon.position.animateTo(ofPoint(ofGetWidth()*0.2,-ofGetHeight()*0.6));
            title.position.animateTo(ofPoint(ofGetWidth()*0.65,-ofGetHeight()*0.6));
            
            if(game==GAME_TRIVIA)
                membrete.position.animateToAfterDelay(ofPoint(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()*0.2),1.);
            else
                membrete.position.animateToAfterDelay(ofPoint(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()),1.);
            
            sound.play();
            ofLogNotice(INICIO_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(!membrete.isOrWillBeAnimating()) {
            finishedExiting();
            ofLogNotice(INICIO_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {        
        title.draw();
        icon.draw();
        membrete.draw();
        
        memotest.draw();
        trivia.draw();
    }
    
    // cleanup
    void exit() {
        ofLogNotice(INICIO_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        
        if(trivia.inside(ofPoint(x,y))){
            game=GAME_TRIVIA;
            soundTrivia.play();
            trivia.size.animateToAfterDelay(0.,1.);
            memotest.size.animateToAfterDelay(0.,0.);
            sceneManager.gotoScene(PREGUNTA_SCENE_NAME);
        }
        else if(memotest.inside(ofPoint(x,y))){
            game=GAME_MEMOTEST;
            soundMemotest.play();
            memotest.size.animateToAfterDelay(0.,1.);
            trivia.size.animateToAfterDelay(0.,0.);
            sceneManager.gotoScene(MEMOTEST_SCENE_NAME);
        }
    }
    
    ofxAnimatableObject<ofImage> title,icon,membrete;
    ofxAnimatableObject<ofImage> memotest,trivia;
    ofSoundPlayer sound,soundMemotest,soundTrivia;
    float time;
    int& game;
    ofxSceneManager& sceneManager;
};
