#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"

#define MEMOTEST_CARDS 2

#define MEMOTEST_CARDS_TOTAL (MEMOTEST_CARDS*2)

#define MEMOTEST_ROWS 3
#define MEMOTEST_COLUMNS 6
#define MEMOTEST_CARDS_X (ofGetWidth()*0.13)
#define MEMOTEST_CARDS_Y (ofGetHeight()*0.20)
#define MEMOTEST_CARDS_WIDTH (ofGetWidth()*0.58)
#define MEMOTEST_CARDS_HEIGHT (ofGetHeight()*0.58)

#define MEMOTEST_CLOCK_X MEMOTEST_CARDS_X+MEMOTEST_CARDS_WIDTH+MEMOTEST_BACK_MARGIN*3
#define MEMOTEST_CLOCK_Y MEMOTEST_CARDS_Y-MEMOTEST_BACK_MARGIN

#define MEMOTEST_BACK_MARGIN 40
#define MEMOTEST_BACK_RADIUS 10

class Memotest : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Memotest(ofxSceneManager& sm, int& p, float& t) : sceneManager(sm), puntaje(p), tiempo(t), ofxScene(MEMOTEST_SCENE_NAME, false) {
        background.load("04_Memotest/background.png");
        background.setAnchorPercent(0.5,0.5);
        background.setPosition(ofPoint(ofGetWidth()*0.5,ofGetHeight()*0.5));

        for(int i=0;i<MEMOTEST_CARDS;i++){
            cards[i*2+0].setup(i,"04_Memotest/cards/"+ofToString(i)+".png","04_Memotest/cards/back.png");
            cards[i*2+1].setup(i,"04_Memotest/cards/"+ofToString(i)+".png","04_Memotest/cards/back.png");
        }
        
        clock.load("04_Memotest/clock.png");
        clock.setAnchorPercent(0.5,0.5);
        gizmosFont.load("font.ttf",46);
        
        soundRespuesta.load("Sounds/04_4.2-SeleccionRespuesta.wav");
        soundClock.load("Sounds/04_4.3-ContadorJuego.wav");
        
        soundCorrect.load("Sounds/04_4.6-RespuestaCorrecta.wav");
        soundFalse.load("Sounds/04_4.7-RespuestaIncorrecta.wav");
        soundEnd.load("Sounds/04_4.9-FinalizacionTrivia.wav");
        
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++)
            order[i]=i;
        
        firstCard=-1;
        secondCard=-1;
    }
    
    // scene setup
    void setup() {
        
        for(int r = 0; r < 2; r++){
            for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
                int j = ofRandom(MEMOTEST_CARDS_TOTAL);
                unsigned int tmp = order[i];
                order[i]=order[j];
                order[j]=tmp;
            }
        }
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cards[i].setSize(0.);
            cards[i].reset();
            //cards[i].setSize(0.3);
            cards[i].setAnchorPercent(0.5,0.5);
            cards[order[i]].setPosition(MEMOTEST_CARDS_X+(i%MEMOTEST_COLUMNS)*(MEMOTEST_CARDS_WIDTH/MEMOTEST_COLUMNS),MEMOTEST_CARDS_Y+int(i/MEMOTEST_COLUMNS)*(MEMOTEST_CARDS_HEIGHT/MEMOTEST_ROWS));
        }
        
        firstCard=-1;
        secondCard=-1;
        
        background.setColor(ofColor(255,0));
        
        cardsFound=0;

        tiempo=0;
        puntaje=0;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {            
            background.color.animateTo(ofColor(255,255));
            
            for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
                cards[order[i]].size.animateToAfterDelay(1.,i*0.1);
            }
            
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update enter";
        }
        
        update();
        
        bool cardsAnimating=false;
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cardsAnimating|=cards[i].isOrWillBeAnimating();
        }
		
        // call finishedEntering() to indicate scne is done entering
        if(!cardsAnimating) {
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

        background.update(dt);
        
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
                    
                    cardsFound++;
                    puntaje+=10;
                    
                    if(cardsFound>=MEMOTEST_CARDS){
                        soundEnd.play();
                        sceneManager.gotoScene(SCORE_SCENE_NAME);
                    }
                    else{
                        soundCorrect.play();
                    }
                }
                else{
                    cards[firstCard].Card::hide(0.5);
                    cards[secondCard].Card::hide(0.5);
                    soundFalse.play();
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
            background.color.animateToAfterDelay(ofColor(255,0),1.);
            
            for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
                cards[order[i]].size.animateToAfterDelay(0.,0.5+i*0.05);
            }
            
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update exit";
        }
        
        update();
		
        bool cardsAnimating=false;
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cardsAnimating|=cards[i].isOrWillBeAnimating();
        }
        // call finishedExiting() to indicate scene is done exiting
        if(!cardsAnimating || !background.isOrWillBeAnimating()) {
            finishedExiting();
            ofLogNotice(MEMOTEST_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {
        background.draw();
        
        drawBack(ofRectangle(MEMOTEST_CARDS_X-MEMOTEST_BACK_MARGIN-cards[0].getWidth()*0.5,MEMOTEST_CARDS_Y-MEMOTEST_BACK_MARGIN-cards[0].getHeight()*0.5,MEMOTEST_CARDS_WIDTH+2*MEMOTEST_BACK_MARGIN,MEMOTEST_CARDS_HEIGHT+2*MEMOTEST_BACK_MARGIN),MEMOTEST_BACK_RADIUS);
        
        for(int i=0;i<MEMOTEST_CARDS_TOTAL;i++){
            cards[i].draw();
        }
        
        drawBack(ofRectangle(MEMOTEST_CLOCK_X-MEMOTEST_BACK_MARGIN-clock.getWidth()*0.5,MEMOTEST_CLOCK_Y-MEMOTEST_BACK_MARGIN-clock.getHeight()*0.5,clock.getWidth()+2*MEMOTEST_BACK_MARGIN,clock.getHeight()*1.5+2*MEMOTEST_BACK_MARGIN),MEMOTEST_BACK_RADIUS);
        ofSetColor(background.color.getCurrentColor());
        clock.draw(MEMOTEST_CLOCK_X,MEMOTEST_CLOCK_Y);
        string timeString = ofToString((int)tiempo/60)+":"+ofToString(((int)tiempo)%60,2,'0');
        gizmosFont.drawString(timeString,MEMOTEST_CLOCK_X-gizmosFont.stringWidth(timeString)/2,MEMOTEST_CLOCK_Y+clock.getHeight());
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

    ofxAnimatableObject<Card> cards[MEMOTEST_CARDS_TOTAL];
    unsigned int order[MEMOTEST_CARDS_TOTAL];
    int cardsFound;
    
    int firstCard,secondCard;
    
    ofSoundPlayer soundRespuesta,soundClock;
    ofSoundPlayer soundCorrect,soundFalse,soundEnd;

    ofImage clock;
    ofTrueTypeFont gizmosFont;
    
    int& puntaje;
    float& tiempo;
    float time;
    ofxAnimatableObject<ofImage> background;
    ofxSceneManager& sceneManager;
    
    /*void drawClock(int x, int y, int r){
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
    }*/
    
    void drawBack(ofRectangle rect, float r, float w=5.0f){
        ofPushStyle();
        ofSetCircleResolution(100);
        ofFill();
        ofSetColor(255,ofMap(background.color.getCurrentColor().a,0,255,0,50));
        ofDrawRectRounded(rect,r);
        ofNoFill();
        ofSetColor(255,ofMap(background.color.getCurrentColor().a,0,255,0,100));
        ofSetLineWidth(w);
        ofDrawRectRounded(rect.x-w/2,rect.y-w/2,rect.width+w,rect.height+w,r);
        ofPopStyle();
    }
};
