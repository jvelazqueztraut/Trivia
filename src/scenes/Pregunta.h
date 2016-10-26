#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"
#include "ofxAnimatableFloat.h"

#define PREGUNTA_MAX_QUESTIONS 2

#define PREGUNTA_OPTIONS_X (ofGetWidth()*0.05)
#define PREGUNTA_OPTIONS_Y (ofGetHeight()*0.5)
#define PREGUNTA_OPTIONS_WIDTH (ofGetWidth()*0.55)
#define PREGUNTA_OPTIONS_MARGIN 120
#define PREGUNTA_QUESTION_HEIGHT (ofGetHeight()*0.2)

#define PREGUNTA_TIMER_TIME 15
#define PREGUNTA_TIMER_NUMBER 15
#define PREGUNTA_TIMER_MARGIN 4
#define PREGUNTA_TIMER_THICK (PREGUNTA_GIZMOS_HEIGHT-PREGUNTA_TIMER_NUMBER*PREGUNTA_TIMER_MARGIN)/PREGUNTA_TIMER_NUMBER

#define PREGUNTA_SCORE_X (ofGetWidth()*0.75)
#define PREGUNTA_SCORE_Y (ofGetHeight()*0.2)
#define PREGUNTA_SCORE_MARGIN 50

#define PREGUNTA_BACK_MARGIN 40
#define PREGUNTA_BACK_RADIUS 10

typedef enum{
    CORRECT=0,
    FALSE_0,
    FALSE_1,
    PREGUNTA_OPTIONS
}Options;

typedef struct{
    string path;
    bool taken;
}QuestionDir;

class Pregunta : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Pregunta(ofxSceneManager& sm, int& p, float& t) : sceneManager(sm), puntaje(p), tiempo(t), ofxScene(PREGUNTA_SCENE_NAME, false) {
        background.load("04_Pregunta/background.png");
        background.setAnchorPercent(0.5,0.5);
        background.setPosition(ofPoint(ofGetWidth()*0.5,ofGetHeight()*0.5));
        
        while(ofDirectory::doesDirectoryExist("04_Pregunta/questions/question"+ofToString(questionsTaken.size()+1,2,'0'))){
            QuestionDir q;
            q.path="04_Pregunta/questions/question"+ofToString(questionsTaken.size()+1,2,'0');
            q.taken=false;
            questionsTaken.push_back(q);
        }
        
        question.load("04_Pregunta/questions/question01/question.png");
        question.setAnchorPercent(0.5,0.5);
        question.setPosition(PREGUNTA_OPTIONS_X+PREGUNTA_OPTIONS_WIDTH/2,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-0.5*PREGUNTA_QUESTION_HEIGHT);
        
        option[CORRECT].load("04_Pregunta/questions/question01/correct.png");
        option[FALSE_0].load("04_Pregunta/questions/question01/false0.png");
        option[FALSE_1].load("04_Pregunta/questions/question01/false1.png");
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].setAnchorPercent(0.,0.5);
            option[i].setPosition(PREGUNTA_OPTIONS_X,PREGUNTA_OPTIONS_Y+i*PREGUNTA_OPTIONS_MARGIN);
        }
        
        for(int i=0;i<PREGUNTA_OPTIONS;i++)
            order[i]=i;
        
        respuestaCorrect.load("04_Pregunta/respuestaCorrect.png");
        respuestaCorrect.setAnchorPercent(0.06,0.45);
        respuestaCorrect.color.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
        respuestaCorrect.color.setCurve(EASE_OUT);
        
        tick.load("04_Pregunta/tick.png");
        tick.setAnchorPercent(0.5,0.5);
        cross.load("04_Pregunta/cross.png");
        cross.setAnchorPercent(0.5,0.5);

        timer.setCurve(LINEAR);
        
        puntajeFont.load("DIN-Medium.ttf",36);
        puntajeFontSmall.load("DIN-Medium.ttf",30);
        
        soundRespuesta.load("Sounds/04_4.2-SeleccionRespuesta.wav");
        soundClock.load("Sounds/04_4.3-ContadorJuego.wav");
        soundTimer.load("Sounds/04_4.4-ContadorPregunta.wav");
        
        soundCorrect.load("Sounds/04_4.6-RespuestaCorrecta.wav");
        soundFalse.load("Sounds/04_4.7-RespuestaIncorrecta.wav");
        soundEnd.load("Sounds/04_4.9-FinalizacionTrivia.wav");
    }
    
    // scene setup
    void setup() {        
        background.setColor(ofColor(255,0));

        question.setColor(255,0);
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].setColor(ofColor(255,0));
        }
        respuestaCorrect.setColor(ofColor(255,0));
        
        for(int i=0; i<questionsTaken.size(); i++){
            questionsTaken[i].taken=false;
        }
        questionsCorrect=0;
        questionsFalse=0;
        
        changingQuestion=false;
        timer.reset(0.);
        
        tiempo=0;
        puntaje=0;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            background.color.animateTo(ofColor(255,255));

            timer.setDuration(1.5);
            timer.animateToAfterDelay(PREGUNTA_TIMER_TIME,1.5);
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!timer.isOrWillBeAnimating()) {
            finishedEntering();
            soundClock.play();
            changingQuestion=true;
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;
        timer.update(dt);
        background.update(dt);
        if(!isEntering() && !isExiting() && timer.val()<5 && timer.val()>1 && timer.getTargetValue()==0 && !soundTimer.isPlaying())
            soundTimer.play();
        
        if(!isEntering() && !isExiting())
            tiempo+=dt;
        
        respuestaCorrect.update(dt);
        
        question.update(dt);
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].update(dt);
        }
        
        if(changingQuestion && !timer.isOrWillBeAnimating() && !question.isOrWillBeAnimating()){
            if((questionsCorrect+questionsFalse)>=PREGUNTA_MAX_QUESTIONS){
                soundEnd.play();
                sceneManager.gotoScene(SCORE_SCENE_NAME);
            }
            else{
                loadNewQuestion();
            }
            timer.setDuration(PREGUNTA_TIMER_TIME);
            timer.animateToAfterDelay(0.,1.0);
            changingQuestion=false;
        }
        else if(!isEntering() && !isExiting() && !changingQuestion && !timer.isOrWillBeAnimating()){
            nextQuestion();
        }
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            background.color.animateToAfterDelay(ofColor(255,0),1.);
            
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(!background.isOrWillBeAnimating()) {
            finishedExiting();
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {        
        background.draw();
        
        drawBack(ofRectangle(PREGUNTA_OPTIONS_X-PREGUNTA_BACK_MARGIN,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-PREGUNTA_QUESTION_HEIGHT-PREGUNTA_BACK_MARGIN,PREGUNTA_OPTIONS_WIDTH+PREGUNTA_BACK_MARGIN,PREGUNTA_QUESTION_HEIGHT+PREGUNTA_OPTIONS_MARGIN*(PREGUNTA_OPTIONS+1)+PREGUNTA_BACK_MARGIN),PREGUNTA_BACK_RADIUS);
        question.draw();
        
        respuestaCorrect.draw();
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].draw();
        }
        
        drawBack(ofRectangle(PREGUNTA_SCORE_X-2*PREGUNTA_SCORE_MARGIN-PREGUNTA_BACK_MARGIN,PREGUNTA_SCORE_Y-0.5*PREGUNTA_SCORE_MARGIN-PREGUNTA_BACK_MARGIN,4*PREGUNTA_SCORE_MARGIN+2*PREGUNTA_BACK_MARGIN,4.5*PREGUNTA_SCORE_MARGIN+PREGUNTA_BACK_MARGIN),PREGUNTA_BACK_RADIUS);
        ofSetColor(background.color.getCurrentColor());
        string puntajeString = "Puntaje";
        puntajeFontSmall.drawString(puntajeString,PREGUNTA_SCORE_X-puntajeFontSmall.stringWidth(puntajeString)/2,PREGUNTA_SCORE_Y);
        puntajeFont.drawString(ofToString(puntaje),PREGUNTA_SCORE_X-puntajeFont.stringWidth(ofToString(puntaje))/2,PREGUNTA_SCORE_Y+PREGUNTA_SCORE_MARGIN);
        tick.draw(PREGUNTA_SCORE_X-PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+2*PREGUNTA_SCORE_MARGIN);
        cross.draw(PREGUNTA_SCORE_X+PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+2*PREGUNTA_SCORE_MARGIN);
        puntajeFontSmall.drawString(ofToString(questionsCorrect),PREGUNTA_SCORE_X-puntajeFontSmall.stringWidth(ofToString(questionsCorrect))/2-PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+3.25*PREGUNTA_SCORE_MARGIN);
        puntajeFontSmall.drawString(ofToString(questionsFalse),PREGUNTA_SCORE_X-puntajeFontSmall.stringWidth(ofToString(questionsFalse))/2+PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+3.25*PREGUNTA_SCORE_MARGIN);
    }
    
    // cleanup
    void exit() {
        ofLogNotice(PREGUNTA_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        if(!changingQuestion){
            for(int i=0;i<PREGUNTA_OPTIONS;i++){
                if(option[i].inside(ofPoint(x,y))){
                    soundRespuesta.play();
                    if(i==CORRECT){
                        questionsCorrect++;
                        puntaje+=10;
                        soundCorrect.play();
                    }
                    else{
                        questionsFalse++;
                        soundFalse.play();
                    }
                    nextQuestion();
                    return;
                }
            }
        }
    }
    
    void nextQuestion(){
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            if(i==CORRECT){
                respuestaCorrect.setPosition(option[i].position.getCurrentPosition());
                option[i].color.animateToAfterDelay(ofColor(255,0),1.5);
            }
            else{
                option[i].color.animateToAfterDelay(ofColor(255,0),1.0);
            }
        }
        respuestaCorrect.color.animateTo(ofColor(255,255));
        
        question.color.animateToAfterDelay(ofColor(255,0),1.5);
        
        timer.setDuration(1.5);
        timer.pause();
        timer.animateToAfterDelay(PREGUNTA_TIMER_TIME,1.0);
        
        changingQuestion=true;
    }
    
    void loadNewQuestion(){
        int q=0;
        do{
            q=ofRandom(questionsTaken.size());
        }while(questionsTaken[q].taken);
        questionsTaken[q].taken=true;

        question.load(questionsTaken[q].path+"/question.png");
        question.setAnchorPercent(0.5,0.5);
        question.setColor(ofColor(255,0));
        question.color.animateTo(ofColor(255,255));
        
        option[CORRECT].load(questionsTaken[q].path+"/correct.png");
        option[FALSE_0].load(questionsTaken[q].path+"/false0.png");
        option[FALSE_1].load(questionsTaken[q].path+"/false1.png");
        
        for(int r = 0; r < 2; r++){
            for(int i=0;i<PREGUNTA_OPTIONS;i++){
                int j = ofRandom(PREGUNTA_OPTIONS);
                unsigned int tmp = order[i];
                order[i]=order[j];
                order[j]=tmp;
            }
        }
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].setAnchorPercent(0.,0.5);
            option[order[i]].setPosition(PREGUNTA_OPTIONS_X,PREGUNTA_OPTIONS_Y+i*PREGUNTA_OPTIONS_MARGIN);
            option[order[i]].setColor(ofColor(255,0));
            option[order[i]].color.animateToAfterDelay(ofColor(255,255),0.5*i);
        }
    }
    
    ofxAnimatableObject<ofImage> question;
    ofxAnimatableObject<ofImage> option[PREGUNTA_OPTIONS];
    unsigned int order[PREGUNTA_OPTIONS];
    ofxAnimatableObject<ofImage> respuestaCorrect;
    bool changingQuestion;
    
    unsigned int questionsCorrect,questionsFalse;
    vector<QuestionDir> questionsTaken;
    
    ofSoundPlayer soundRespuesta,soundClock,soundTimer;
    ofSoundPlayer soundCorrect,soundFalse,soundEnd;

    ofImage tick,cross;
    ofTrueTypeFont puntajeFont,puntajeFontSmall;
    ofxAnimatableFloat timer;

    int& puntaje;
    float& tiempo;
    float time;
    
    ofxAnimatableObject<ofImage> background;
    ofxSceneManager& sceneManager;
    
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
