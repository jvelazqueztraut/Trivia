#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"
#include "ofxAnimatableFloat.h"

#define PREGUNTA_MAX_QUESTIONS 8

#define PREGUNTA_GIZMOS_X 1780
#define PREGUNTA_GIZMOS_Y 240
#define PREGUNTA_GIZMOS_WIDTH 90
#define PREGUNTA_GIZMOS_HEIGHT 620

#define PREGUNTA_TIMER_TIME 15
#define PREGUNTA_TIMER_NUMBER 15
#define PREGUNTA_TIMER_MARGIN 4
#define PREGUNTA_TIMER_THICK (PREGUNTA_GIZMOS_HEIGHT-PREGUNTA_TIMER_NUMBER*PREGUNTA_TIMER_MARGIN)/PREGUNTA_TIMER_NUMBER

#define PREGUNTA_SCORE_NUMBER PREGUNTA_MAX_QUESTIONS
#define PREGUNTA_SCORE_MARGIN 9
#define PREGUNTA_SCORE_THICK (PREGUNTA_GIZMOS_HEIGHT-PREGUNTA_SCORE_NUMBER*PREGUNTA_SCORE_MARGIN)/PREGUNTA_SCORE_NUMBER

#define PREGUNTA_CLOCK_X 210
#define PREGUNTA_CLOCK_Y 600

#define TIMER_BEGIN_R 0
#define TIMER_BEGIN_G 180
#define TIMER_BEGIN_B 165
#define TIMER_END_R 175
#define TIMER_END_G 190
#define TIMER_END_B 200

#define ANSWERS 3

typedef struct{
    string path;
    bool taken;
}QuestionDir;

class Pregunta : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Pregunta(ofxSceneManager& sm, int& p, float& t) : sceneManager(sm), puntaje(p), tiempo(t), ofxScene(PREGUNTA_SCENE_NAME, false) {
        question.load("04_Pregunta/PREGUNTAS/pregunta01/question.png");
        question.setAnchorPercent(0.,0.);
        question.setPosition(500,300);
        
        correct.load("04_Pregunta/PREGUNTAS/pregunta01/correct.png");
        correct.setAnchorPercent(0.,0.5);
        
        false0.load("04_Pregunta/PREGUNTAS/pregunta01/false0.png");
        false0.setAnchorPercent(0.,0.5);

        false1.load("04_Pregunta/PREGUNTAS/pregunta01/false1.png");
        false1.setAnchorPercent(0.,0.5);
        
        preguntaBack.load("04_Pregunta/preguntaBack.png");
        preguntaBack.setAnchorPercent(0.31,0.19);
        preguntaBack.setPosition(498,300);
        
        respuestaCorrect.load("04_Pregunta/respuestaCorrect.png");
        respuestaCorrect.setAnchorPercent(0.06,0.45);
        respuestaCorrect.color.setRepeatType(LOOP_BACK_AND_FORTH_ONCE);
        respuestaCorrect.color.setCurve(EASE_OUT);
        
        respuestaBack.load("04_Pregunta/respuestaBack.png");
        respuestaBack.setAnchorPercent(0.06,0.45);
        for(int i=0;i<ANSWERS;i++){
            option[i].setPtr(&respuestaBack);
            option[i].setAnchorPercent(0.06,0.45);
            option[i].setPosition(450,620+100*i);
            answer[i]=&correct;
        }
        
        tick.load("04_Pregunta/tick.png");
        tick.setAnchorPercent(0.5,0.5);
        cross.load("04_Pregunta/cross.png");
        cross.setAnchorPercent(0.5,0.5);

        timer.setCurve(LINEAR);
        
        gizmosFont.load("DIN-Medium.ttf",46);
        
        soundRespuesta.load("Sounds/04_4.2-SeleccionRespuesta.wav");
        soundClock.load("Sounds/04_4.3-ContadorJuego.wav");
        soundTimer.load("Sounds/04_4.4-ContadorPregunta.wav");
        
        soundCorrect.load("Sounds/04_4.6-RespuestaCorrecta.wav");
        soundFalse.load("Sounds/04_4.7-RespuestaIncorrecta.wav");
        soundEnd.load("Sounds/04_4.9-FinalizacionTrivia.wav");
    }
    
    // scene setup
    void setup() {        
        preguntaBack.setColor(ofColor(0,0));
        question.setColor(255,0);
        
        for(int i=0;i<ANSWERS;i++){
            option[i].setColor(ofColor(255,0));
        }
        respuestaCorrect.setColor(ofColor(255,0));

        changingQuestion=false;
        timer.reset(0.);
        questions=0;
        questionsTaken.clear();
        while(ofDirectory::doesDirectoryExist("04_Pregunta/PREGUNTAS/pregunta"+ofToString(questionsTaken.size()+1,2,'0'))){
            QuestionDir q;
            q.path="04_Pregunta/PREGUNTAS/pregunta"+ofToString(questionsTaken.size()+1,2,'0');
            q.taken=false;
            questionsTaken.push_back(q);
        }
        
        for(int i=0;i<PREGUNTA_MAX_QUESTIONS;i++)
            success[i]=false;
        
        tiempo=0;
        puntaje=0;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {            
            preguntaBack.color.animateTo(ofColor(255,255));
            
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
        
        if(!isEntering() && !isExiting() && timer.val()<5 && timer.val()>1 && timer.getTargetValue()==0 && !soundTimer.isPlaying())
            soundTimer.play();
        
        if(!isEntering() && !isExiting())
            tiempo+=dt;
        
        preguntaBack.update(dt);
        respuestaCorrect.update(dt);
        
        question.update(dt);
        for(int i=0;i<ANSWERS;i++){
            option[i].update(dt);
        }
        
        if(changingQuestion && !timer.isOrWillBeAnimating() && !question.isOrWillBeAnimating()){
            if(questions>=PREGUNTA_MAX_QUESTIONS){
                soundEnd.play();
                sceneManager.gotoScene(SCORE_SCENE_NAME);
            }
            else{
                loadNewQuestion();
                answer[0]=&correct;
                answer[1]=&false0;
                answer[2]=&false1;
                //FisherÐYates shuffle to randomize appearance
                for(int i=0;i<ANSWERS;i++){
                    int j = ofRandom(ANSWERS);
                    ofImage* tmp = answer[i];
                    answer[i]=answer[j];
                    answer[j]=tmp;
                }
                question.color.animateTo(ofColor(255,255));
                for(int i=0;i<ANSWERS;i++){
                    option[i].color.animateToAfterDelay(ofColor(255,255),0.5*i);
                }
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
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(true) {
            finishedExiting();
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {        
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
        preguntaBack.draw();
        ofPopStyle();
        question.draw();
        
        for(int i=0;i<ANSWERS;i++){
            option[i].draw();
        }
        respuestaCorrect.draw();
        for(int i=0;i<ANSWERS;i++){
            ofPushMatrix();
            ofTranslate(option[i].position.getCurrentPosition());
            ofPushStyle();
            ofSetColor(option[i].color.getCurrentColor());
            answer[i]->draw(0,0);
            ofPopStyle();
            ofPopMatrix();
        }
        
        drawTimer(PREGUNTA_GIZMOS_X,PREGUNTA_GIZMOS_Y);
        
        drawScore(PREGUNTA_GIZMOS_X-PREGUNTA_GIZMOS_WIDTH-30,PREGUNTA_GIZMOS_Y);
        
        drawClock(PREGUNTA_CLOCK_X,PREGUNTA_CLOCK_Y,55);
        ofSetColor(255);
        gizmosFont.drawString(ofToString((int)tiempo/60)+":"+ofToString(((int)tiempo)%60,2,'0'),PREGUNTA_CLOCK_X-60,PREGUNTA_CLOCK_Y+110);
    }
    
    // cleanup
    void exit() {
        ofLogNotice(PREGUNTA_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        if(!changingQuestion){
            for(int i=0;i<ANSWERS;i++){
                if(option[i].inside(ofPoint(x,y))){
                    soundRespuesta.play();
                    if(answer[i]==&correct){
                        success[questions]=true;
                        puntaje+=10;
                        soundCorrect.play();
                    }
                    else{
                        soundFalse.play();
                    }
                    nextQuestion();
                    return;
                }
            }
        }
    }
    
    void nextQuestion(){
        for(int i=0;i<ANSWERS;i++){
            if(answer[i]==&correct){
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
        
        questions++;
        changingQuestion=true;
    }
    
    void loadNewQuestion(){
        int q=0;
        do{
            q=ofRandom(questionsTaken.size());
        }while(questionsTaken[q].taken);
        questionsTaken[q].taken=true;

        question.load(questionsTaken[q].path+"/question.png");
        question.setAnchorPercent(0.,0.);
        
        correct.load(questionsTaken[q].path+"/correct.png");
        correct.setAnchorPercent(0.,0.5);
        
        false0.load(questionsTaken[q].path+"/false0.png");
        false0.setAnchorPercent(0.,0.5);
        
        false1.load(questionsTaken[q].path+"/false1.png");
        false1.setAnchorPercent(0.,0.5);
    }
    
    ofxAnimatableObject<ofImage> question;
    ofxAnimatableObject<ofImage*> option[ANSWERS];
    ofImage* answer[ANSWERS];
    ofImage correct,false0,false1;
    ofxAnimatableObject<ofImage> respuestaCorrect,preguntaBack;
    ofImage respuestaBack;
    bool success[PREGUNTA_MAX_QUESTIONS];
    bool changingQuestion;
    vector<QuestionDir> questionsTaken;
    
    ofSoundPlayer soundRespuesta,soundClock,soundTimer;
    ofSoundPlayer soundCorrect,soundFalse,soundEnd;

    ofImage tick,cross;
    ofTrueTypeFont gizmosFont;
    ofxAnimatableFloat timer;

    int questions;

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
        //ofRect(0,0,PREGUNTA_GIZMOS_WIDTH,PREGUNTA_GIZMOS_HEIGHT);
        ofTranslate(0,PREGUNTA_GIZMOS_HEIGHT);
        ofFill();
        int timeLeft = ceil(PREGUNTA_TIMER_NUMBER * timer.val()/PREGUNTA_TIMER_TIME);
        for(int i=0;i<timeLeft;i++){
            int colorR = TIMER_BEGIN_R+i*(TIMER_END_R-TIMER_BEGIN_R)/(float)PREGUNTA_TIMER_TIME;
            int colorG = TIMER_BEGIN_G+i*(TIMER_END_G-TIMER_BEGIN_G)/(float)PREGUNTA_TIMER_TIME;
            int colorB = TIMER_BEGIN_B+i*(TIMER_END_B-TIMER_BEGIN_B)/(float)PREGUNTA_TIMER_TIME;
            ofSetColor(colorR,colorG,colorB);
            ofDrawRectangle(0,-i*(PREGUNTA_TIMER_THICK+PREGUNTA_TIMER_MARGIN),PREGUNTA_GIZMOS_WIDTH,-PREGUNTA_TIMER_THICK);
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
        ofTranslate(0,PREGUNTA_GIZMOS_HEIGHT);
        ofDrawRectangle(0,0,PREGUNTA_GIZMOS_WIDTH,-PREGUNTA_GIZMOS_HEIGHT+5);
        for(int i=0;i<PREGUNTA_MAX_QUESTIONS;i++){
            if(i==questions)
                ofSetColor(28,120,160);
            else if(success[i])
                ofSetColor(0,160,155);
            else
                ofSetColor(10,66,100);
            ofDrawRectangle(0,-i*(PREGUNTA_SCORE_THICK+PREGUNTA_SCORE_MARGIN),PREGUNTA_GIZMOS_WIDTH,-PREGUNTA_SCORE_THICK);
            if(i<questions){
                ofSetColor(255);
                if(success[i])
                    tick.draw(PREGUNTA_GIZMOS_WIDTH/2,-i*(PREGUNTA_SCORE_THICK+PREGUNTA_SCORE_MARGIN)-PREGUNTA_SCORE_THICK/2);
                else
                    cross.draw(PREGUNTA_GIZMOS_WIDTH/2,-i*(PREGUNTA_SCORE_THICK+PREGUNTA_SCORE_MARGIN)-PREGUNTA_SCORE_THICK/2);
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
