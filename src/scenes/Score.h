#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"

#define SCORE_X (ofGetWidth()*0.65)
#define SCORE_Y (ofGetHeight()*0.5)

#define SCORE_BACK_MARGIN (60*ofGetHeight()/APP_HEIGHT)
#define SCORE_BACK_RADIUS (10*ofGetHeight()/APP_HEIGHT)

class Score : public ofxScene {
    
public:
    // set the scene name through the base class initializer
    Score(ofxSceneManager& sm, int& g, int& p, float& t) : sceneManager(sm), game(g), puntaje(p), tiempo(t), ofxScene(SCORE_SCENE_NAME, false) {
        icon.loadImage("icono.png");
        icon.setAnchorPercent(0.5,0.5);
        icon.position.setDuration(2.5);
        icon.position.setCurve(TANH);
        
        title.loadImage("05_Score/titulo.png");
        title.setAnchorPercent(0.5,0.5);
        title.position.setDuration(2.);
        title.position.setCurve(TANH);
        
        membrete.loadImage("membretre.png");
        membrete.setAnchorPercent(0.5,1.0);
        membrete.position.setDuration(1.5);
        membrete.position.setCurve(TANH);
        
        score.loadFont("font.ttf",72*ofGetHeight()/APP_HEIGHT);
        score.setText("");
        score.setAnchorPercent(0.5,0.5);
        score.setPosition(SCORE_X,SCORE_Y);

        scoreSmall.loadFont("font.ttf",32*ofGetHeight()/APP_HEIGHT);
        scoreSmall.setText("");
        scoreSmall.setAnchorPercent(0.5,0.5);
        scoreSmall.setPosition(SCORE_X+5,SCORE_Y-ofGetHeight()*0.07);
        
        next.loadImage("05_Score/next.png");
        next.setAnchorPercent(0.5,0.5);
        next.setPosition(SCORE_X,SCORE_Y+ofGetHeight()*0.125);
        
        soundInicio.loadSound("Sounds/05_5.1-BotonInicio.wav");
        soundPositions.loadSound("Sounds/05_5.2-ContadorPosiciones.wav");
        soundYourPosition.loadSound("Sounds/05_5.3-PosicionFinal.wav");
        soundEnd.loadSound("Sounds/05_5.4-FinDelJuego.wav");
        
        icon.setSize(ofGetHeight()/APP_HEIGHT);
        title.setSize(ofGetHeight()/APP_HEIGHT);
        membrete.setSize(ofGetHeight()/APP_HEIGHT);
        next.setSize(ofGetHeight()/APP_HEIGHT);

    }
    
    // scene setup
    void setup() {
        title.setPosition(ofGetWidth()*0.65,-ofGetHeight()*0.6);
        
        icon.setPosition(ofGetWidth()*0.2,-ofGetHeight()*0.6);
        if(game==GAME_TRIVIA)
            membrete.setPosition(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()*0.2);
        else
            membrete.setPosition(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight());
        
        if(game==GAME_TRIVIA){
            score.setText(ofToString(puntaje));
            scoreSmall.setText("Puntaje");
        }
        else{
            string timeString = ofToString((int)tiempo/60)+":"+ofToString(((int)tiempo)%60,2,'0');
            score.setText(timeString);
            scoreSmall.setText("Tiempo");
        }
        score.setColor(ofColor(255,0));
        scoreSmall.setColor(ofColor(255,0));

        next.setColor(ofColor(255,0));
        
        timeout=SCENE_TIMEOUT;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            title.position.animateTo(ofPoint(SCORE_X,ofGetHeight()*0.25));
            
            icon.position.animateTo(ofPoint(ofGetWidth()*0.2,ofGetHeight()*0.4));
            membrete.position.animateTo(ofPoint(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()*0.2));

            score.color.animateToAfterDelay(ofColor(255,255),0.5);
            scoreSmall.color.animateToAfterDelay(ofColor(255,255),0.5);
            next.color.animateToAfterDelay(ofColor(255,255),1.0);
            
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
        icon.update(dt);
        title.update(dt);
        membrete.update(dt);
        next.update(dt);
        score.update(dt);
        scoreSmall.update(dt);
        
        if(!isEntering() && !isExiting()){
            timeout-=dt;
            if(timeout<=0.0f)
                sceneManager.gotoScene(INICIO_SCENE_NAME);
        }
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            title.position.animateTo(ofPoint(ofGetWidth()*0.65,-ofGetHeight()*0.6));

            score.color.animateTo(ofColor(255,0));
            scoreSmall.color.animateTo(ofColor(255,0));
            next.color.animateTo(ofColor(255,0));

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
        icon.draw();
        membrete.draw();
        title.draw();
        next.draw();

        drawBack(ofRectangle(scoreSmall.position.getCurrentPosition()-ofPoint(SCORE_BACK_MARGIN*2,SCORE_BACK_MARGIN),score.position.getCurrentPosition()+ofPoint(SCORE_BACK_MARGIN*2,SCORE_BACK_MARGIN)),SCORE_BACK_RADIUS);
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
        if(next.inside(ofPoint(x,y))){
            sceneManager.gotoScene(INICIO_SCENE_NAME);
        }
    }
    
    ofxAnimatableObject<ofImage> title,icon,membrete;
    ofxAnimatableObject<ofTrueTypeFont> score,scoreSmall;
    ofxAnimatableObject<ofImage> next;
    
    ofSoundPlayer soundInicio,soundPositions,soundYourPosition,soundEnd;
    float time,timeout;
    
    int& puntaje;
    float& tiempo;
    int& game;
    ofxSceneManager& sceneManager;
    
    void drawBack(ofRectangle rect, float r, float w=5.0f){
        ofPushStyle();
        ofSetCircleResolution(100);
        ofFill();
        ofSetColor(204,204,102,ofMap(score.color.getCurrentColor().a,0,255,0,200));
        ofRectRounded(rect,r);
        ofNoFill();
        ofSetColor(0,102,51,ofMap(score.color.getCurrentColor().a,0,255,0,150));
        ofSetLineWidth(w);
        ofRectRounded(rect.x-w/2,rect.y-w/2,rect.width+w,rect.height+w,r);
        ofPopStyle();
    }
};
