#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"

#define MEMOTEST_CARDS 2
#define MEMOTEST_CARDS_TOTAL (MEMOTEST_CARDS*2)

#define MEMOTEST_ROWS 4
#define MEMOTEST_COLUMNS 5
#define MEMOTEST_CARDS_X (ofGetWidth()*0.15)
#define MEMOTEST_CARDS_Y (ofGetHeight()*0.15)

#define MEMOTEST_GIZMOS_X 1780
#define MEMOTEST_GIZMOS_Y 240
#define MEMOTEST_GIZMOS_WIDTH 90
#define MEMOTEST_GIZMOS_HEIGHT 620

#define MEMOTEST_TIMER_TIME 15
#define MEMOTEST_TIMER_NUMBER 15
#define MEMOTEST_TIMER_MARGIN 4
#define MEMOTEST_TIMER_THICK (MEMOTEST_GIZMOS_HEIGHT-MEMOTEST_TIMER_NUMBER*MEMOTEST_TIMER_MARGIN)/MEMOTEST_TIMER_NUMBER

#define MEMOTEST_SCORE_NUMBER MEMOTEST_CARDS
#define MEMOTEST_SCORE_MARGIN 9
#define MEMOTEST_SCORE_THICK (MEMOTEST_GIZMOS_HEIGHT-MEMOTEST_SCORE_NUMBER*MEMOTEST_SCORE_MARGIN)/MEMOTEST_SCORE_NUMBER

#define MEMOTEST_CLOCK_X 210
#define MEMOTEST_CLOCK_Y 600

#define TIMER_BEGIN_R 0
#define TIMER_BEGIN_G 180
#define TIMER_BEGIN_B 165
#define TIMER_END_R 175
#define TIMER_END_G 190
#define TIMER_END_B 200

class Memotest : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Memotest(ofxSceneManager& sm, int& p, float& t) : sceneManager(sm), puntaje(p), tiempo(t), ofxScene(MEMOTEST_SCENE_NAME, false) {
        
        for(int i=0;i<MEMOTEST_CARDS;i++){
            ofxAnimatableObject< Card > newCard0;
            newCard0.setup(i,"04_Memotest/CARDS/"+ofToString(i)+".png","04_Memotest/CARDS/back.png");
            
            ofxAnimatableObject< Card > newCard1;
            newCard1.setup(i,"04_Memotest/CARDS/"+ofToString(i)+".png","04_Memotest/CARDS/back.png");
            
            cards.push_back(newCard0);
            cards.push_back(newCard1);
        }
        
        tick.load("04_Memotest/tick.png");
        tick.setAnchorPercent(0.5,0.5);

        cross.load("04_Memotest/cross.png");
        cross.setAnchorPercent(0.5,0.5);

        timer.setCurve(LINEAR);
        
        gizmosFont.load("DIN-Medium.ttf",46);
        
        soundRespuesta.load("Sounds/04_4.2-SeleccionRespuesta.wav");
        soundClock.load("Sounds/04_4.3-ContadorJuego.wav");
        soundTimer.load("Sounds/04_4.4-ContadorPregunta.wav");
        
        soundCorrect.load("Sounds/04_4.6-RespuestaCorrecta.wav");
        soundFalse.load("Sounds/04_4.7-RespuestaIncorrecta.wav");
        soundEnd.load("Sounds/04_4.9-FinalizacionTrivia.wav");
        
        firstCard=-1;
        secondCard=-1;
    }
    
    // scene setup
    void setup() {
        
        unsigned int order[MEMOTEST_CARDS_TOTAL];
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++)
            order[i]=i;
        for(int r = 0; r < 2; r++){
            for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
                int j = ofRandom(MEMOTEST_CARDS_TOTAL);
                unsigned int tmp = order[i];
                order[i]=order[j];
                order[j]=tmp;
            }
        }
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cards[i].reset();
            cards[i].setSize(0.3);
            cards[i].setAnchorPercent(0.5,0.5);
            cards[order[i]].setPosition(MEMOTEST_CARDS_X+(i%MEMOTEST_COLUMNS)*(ofGetWidth()*0.8/MEMOTEST_COLUMNS),MEMOTEST_CARDS_Y+int(i/MEMOTEST_COLUMNS)*(ofGetHeight()*0.8/MEMOTEST_ROWS));
        }
        
        firstCard=-1;
        secondCard=-1;
        
        timer.reset(0.);
        
        cardsFound=0;
        for(int i=0;i<MEMOTEST_CARDS;i++)
            success[i]=false;
        tiempo=0;
        puntaje=0;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {            
            
            timer.setDuration(1.5);
            timer.animateToAfterDelay(MEMOTEST_TIMER_TIME,1.5);
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!timer.isOrWillBeAnimating()) {
            finishedEntering();
            soundClock.play();
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;
        timer.update(dt);
        
        if(!isEntering() && !isExiting() && timer.val()<5 && timer.val()>1 && timer.getTargetValue()==0 && !soundTimer.isPlaying())
            soundTimer.play();
        
        if(!isEntering() && !isExiting())
            tiempo+=dt;
        
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cards[i].update(dt);
            cards[i].Card::updateChange(dt);
        }
        
        if(firstCard>=0 && secondCard>=0){
            if(!cards[firstCard].Card::isChanging() && !cards[secondCard].Card::isChanging()){
                if(cards[firstCard].Card::number == cards[secondCard].Card::number){
                    cards[firstCard].lock();
                    cards[secondCard].lock();
                    
                    success[cardsFound++]=true;
                    puntaje+=10;
                    
                    if(cardsFound>=MEMOTEST_CARDS){
                        soundEnd.play();
                        sceneManager.gotoScene(SCORE_SCENE_NAME);
                    }
                }
                else{
                    cards[firstCard].Card::hide(0.5);
                    cards[secondCard].Card::hide(0.5);
                }
                
                firstCard=-1;
                secondCard=-1;
            }
        }
        
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(true) {
            finishedExiting();
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {
        ofEnableAlphaBlending();
        
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cards[i].draw();
        }
        
        drawTimer(MEMOTEST_GIZMOS_X,MEMOTEST_GIZMOS_Y);
        
        drawScore(MEMOTEST_GIZMOS_X-MEMOTEST_GIZMOS_WIDTH-30,MEMOTEST_GIZMOS_Y);
        
        drawClock(MEMOTEST_CLOCK_X,MEMOTEST_CLOCK_Y,55);
        ofSetColor(255);
        gizmosFont.drawString(ofToString((int)tiempo/60)+":"+ofToString(((int)tiempo)%60,2,'0'),MEMOTEST_CLOCK_X-60,MEMOTEST_CLOCK_Y+110);
    }
    
    // cleanup
    void exit() {
        ofLogNotice(MEMOTEST_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        if(firstCard<0 || secondCard<0){
            for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
                if(!cards[i].Card::isShowing() && cards[i].inside(ofPoint(x,y))){
                    cards[i].Card::show();
                    if(firstCard>=0)
                        secondCard=i;
                    else
                        firstCard=i;
                    soundRespuesta.play();
                    return;
                }
            }
        }
    }

    vector< ofxAnimatableObject< Card > > cards;
    int cardsFound;
    bool success[MEMOTEST_CARDS];
    
    int firstCard,secondCard;
    
    ofSoundPlayer soundRespuesta,soundClock,soundTimer;
    ofSoundPlayer soundCorrect,soundFalse,soundEnd;

    ofImage tick,cross;
    ofTrueTypeFont gizmosFont;
    ofxAnimatableFloat timer;
    
    int& puntaje;
    float& tiempo;
    float time;
    
    ofxSceneManager& sceneManager;
    
    void drawTimer(int x, int y){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(x,y);
        ofSetColor(255);
        //gizmosFont.drawString(ofToString((int)timer.val()/60,2,'0')+":"+ofToString(((int)timer.val())%60,2,'0'),0,0);
        //ofNoFill();
        //ofRect(0,0,MEMOTEST_GIZMOS_WIDTH,MEMOTEST_GIZMOS_HEIGHT);
        ofTranslate(0,MEMOTEST_GIZMOS_HEIGHT);
        ofFill();
        int timeLeft = ceil(MEMOTEST_TIMER_NUMBER * timer.val()/MEMOTEST_TIMER_TIME);
        for(int i=0;i<timeLeft;i++){
            int colorR = TIMER_BEGIN_R+i*(TIMER_END_R-TIMER_BEGIN_R)/(float)MEMOTEST_TIMER_TIME;
            int colorG = TIMER_BEGIN_G+i*(TIMER_END_G-TIMER_BEGIN_G)/(float)MEMOTEST_TIMER_TIME;
            int colorB = TIMER_BEGIN_B+i*(TIMER_END_B-TIMER_BEGIN_B)/(float)MEMOTEST_TIMER_TIME;
            ofSetColor(colorR,colorG,colorB);
            ofDrawRectangle(0,-i*(MEMOTEST_TIMER_THICK+MEMOTEST_TIMER_MARGIN),MEMOTEST_GIZMOS_WIDTH,-MEMOTEST_TIMER_THICK);
        }
        ofPopStyle();
        ofPopMatrix();
    }
    
    void drawScore(int x, int y){
        ofPushMatrix();
        ofPushStyle();
        ofTranslate(x,y);
        ofSetColor(255);
        //gizmosFont.drawString(ofToString(score),0,0);
        ofFill();
        ofSetColor(6,52,89);
        ofTranslate(0,MEMOTEST_GIZMOS_HEIGHT);
        ofDrawRectangle(0,0,MEMOTEST_GIZMOS_WIDTH,-MEMOTEST_GIZMOS_HEIGHT+5);
        for(int i=0;i<MEMOTEST_CARDS;i++){
            if(i==cardsFound)
                ofSetColor(28,120,160);
            else if(success[i])
                ofSetColor(0,160,155);
            else
                ofSetColor(10,66,100);
            ofDrawRectangle(0,-i*(MEMOTEST_SCORE_THICK+MEMOTEST_SCORE_MARGIN),MEMOTEST_GIZMOS_WIDTH,-MEMOTEST_SCORE_THICK);
            if(i<cardsFound){
                ofSetColor(255);
                if(success[i])
                    tick.draw(MEMOTEST_GIZMOS_WIDTH/2,-i*(MEMOTEST_SCORE_THICK+MEMOTEST_SCORE_MARGIN)-MEMOTEST_SCORE_THICK/2);
                else
                    cross.draw(MEMOTEST_GIZMOS_WIDTH/2,-i*(MEMOTEST_SCORE_THICK+MEMOTEST_SCORE_MARGIN)-MEMOTEST_SCORE_THICK/2);
            }
        }
        ofPopStyle();
        ofPopMatrix();
    }
    
    void drawClock(int x, int y, int r){
        ofPushMatrix();
        ofTranslate(x,y);
        ofPushStyle();
        ofPath arc;
        arc.setCircleResolution(360);
        arc.setFillColor(ofColor(0,255,212));
        arc.arc(0, 0, r, r, -90,-89+360*tiempo/60.);
        arc.draw();
        ofSetColor(255);
        ofSetCircleResolution(360);
        ofDrawCircle(0,0,2);
        ofNoFill();
        ofSetLineWidth(2);
        ofDrawCircle(0,0,r);
        ofPopStyle();
        ofPopMatrix();
    }
};
