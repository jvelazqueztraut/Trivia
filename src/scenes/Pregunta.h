#pragma once

#include <ofxAppUtils.h>
#include "scenes.h"
#include "ofxAnimatableObject.h"
#include "ofxAnimatableFloat.h"

#define PREGUNTA_MAX_QUESTIONS 8

#define PREGUNTA_OPTIONS_X (ofGetWidth()*0.075)
#define PREGUNTA_OPTIONS_Y (ofGetHeight()*0.48)
#define PREGUNTA_OPTIONS_WIDTH (ofGetWidth()*0.44)
#define PREGUNTA_OPTIONS_MARGIN (120*ofGetHeight()/APP_HEIGHT)
#define PREGUNTA_QUESTION_HEIGHT (ofGetHeight()*0.2)

#define PREGUNTA_SCORE_X (ofGetWidth()*0.75)
#define PREGUNTA_SCORE_Y (ofGetHeight()*0.15)
#define PREGUNTA_SCORE_MARGIN (50*ofGetHeight()/APP_HEIGHT)

#define PREGUNTA_NEXT_X (ofGetWidth()*0.75)
#define PREGUNTA_NEXT_Y (PREGUNTA_SCORE_Y+ofGetHeight()*0.22)

#define PREGUNTA_BACK_MARGIN (15*ofGetHeight()/APP_HEIGHT)
#define PREGUNTA_BACK_RADIUS (10*ofGetHeight()/APP_HEIGHT)

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
        frontground.loadImage("04_Pregunta/frontground.png");
        frontground.setAnchorPercent(0.98,0.5);
        frontground.setPosition(ofPoint(ofGetWidth(),ofGetHeight()*0.5));
        
        membrete.loadImage("membretre.png");
        membrete.setAnchorPercent(0.5,1.0);
        membrete.position.setDuration(1.5);
        membrete.position.setCurve(TANH);
        
        while(ofDirectory::doesDirectoryExist("04_Pregunta/questions/question"+ofToString(questionsTaken.size()+1,2,'0'))){
            QuestionDir q;
            q.path="04_Pregunta/questions/question"+ofToString(questionsTaken.size()+1,2,'0');
            q.taken=false;
            questionsTaken.push_back(q);
        }
        
        question.loadImage("04_Pregunta/questions/question01/p_question.png");
        question.setAnchorPercent(0.5,0.5);
        question.setPosition(PREGUNTA_OPTIONS_X+PREGUNTA_OPTIONS_WIDTH/2,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-0.5*PREGUNTA_QUESTION_HEIGHT);
        
        answer.loadImage("04_Pregunta/questions/question01/p_answer.png");
        answer.setAnchorPercent(0.5,0.5);
        answer.setPosition(PREGUNTA_NEXT_X,PREGUNTA_NEXT_Y+ofGetHeight()*0.2);
        
        option[CORRECT].loadImage("04_Pregunta/questions/question01/p_correct.png");
        option[FALSE_0].loadImage("04_Pregunta/questions/question01/p_false0.png");
        option[FALSE_1].loadImage("04_Pregunta/questions/question01/p_false1.png");
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].setAnchorPercent(0.,0.5);
            option[i].setPosition(PREGUNTA_OPTIONS_X,PREGUNTA_OPTIONS_Y+i*PREGUNTA_OPTIONS_MARGIN);
        }
        
        for(int i=0;i<PREGUNTA_OPTIONS;i++)
            order[i]=i;
        
        optionCorrect.loadImage("04_Pregunta/optionCorrect.png");
        optionCorrect.setAnchorPercent(0.033,0.55);
        optionIncorrect.loadImage("04_Pregunta/optionIncorrect.png");
        optionIncorrect.setAnchorPercent(0.033,0.55);
        
        correct.loadImage("04_Pregunta/correct.png");
        correct.setAnchorPercent(0.,0.5);
        correct.setPosition(PREGUNTA_OPTIONS_X+PREGUNTA_OPTIONS_WIDTH,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-0.5*PREGUNTA_QUESTION_HEIGHT);

        incorrect.loadImage("04_Pregunta/incorrect.png");
        incorrect.setAnchorPercent(0.,0.5);
        incorrect.setPosition(PREGUNTA_OPTIONS_X+PREGUNTA_OPTIONS_WIDTH,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-0.5*PREGUNTA_QUESTION_HEIGHT);
        
        next.loadImage("04_Pregunta/next.png");
        next.setAnchorPercent(0.5,0.5);
        next.setPosition(PREGUNTA_NEXT_X,PREGUNTA_NEXT_Y);

        tick.loadImage("04_Pregunta/tick.png");
        tick.setAnchorPercent(0.5,0.5);
        cross.loadImage("04_Pregunta/cross.png");
        cross.setAnchorPercent(0.5,0.5);
        
        gizmoFontBig.loadFont("font.ttf",72*ofGetHeight()/APP_HEIGHT);
        gizmoFontMiddle.loadFont("font.ttf",40*ofGetHeight()/APP_HEIGHT);
        gizmoFontSmall.loadFont("font.ttf",32*ofGetHeight()/APP_HEIGHT);
        
        soundRespuesta.loadSound("Sounds/04_4.2-SeleccionRespuesta.wav");
        soundClock.loadSound("Sounds/04_4.3-ContadorJuego.wav");
        
        soundCorrect.loadSound("Sounds/04_4.6-RespuestaCorrecta.wav");
        soundFalse.loadSound("Sounds/04_4.7-RespuestaIncorrecta.wav");
        soundEnd.loadSound("Sounds/04_4.9-FinalizacionTrivia.wav");
        
        frontground.setSize(ofGetHeight()/APP_HEIGHT);
        membrete.setSize(ofGetHeight()/APP_HEIGHT);
        question.setSize(ofGetHeight()/APP_HEIGHT);
        for(int i=0;i<PREGUNTA_OPTIONS;i++)
            option[i].setSize(ofGetHeight()/APP_HEIGHT);
        answer.setSize(ofGetHeight()/APP_HEIGHT);
        optionCorrect.setSize(ofGetHeight()/APP_HEIGHT);
        optionIncorrect.setSize(ofGetHeight()/APP_HEIGHT);
        correct.setSize(ofGetHeight()/APP_HEIGHT);
        incorrect.setSize(ofGetHeight()/APP_HEIGHT);
        next.setSize(ofGetHeight()/APP_HEIGHT);

    }
    
    // scene setup
    void setup() {        
        frontground.setColor(ofColor(255,0));
        membrete.setPosition(ofGetWidth()*0.5,ofGetHeight()+membrete.getHeight()*0.2);
        
        for(int i=0; i<questionsTaken.size(); i++){
            questionsTaken[i].taken=false;
        }
        questionsCorrect=0;
        questionsFalse=0;
        questions=0;
        loadNewQuestion();
        
        showingAnswer=false;
        changingQuestion=false;
        
        next.setColor(ofColor(255,0));
        
        tiempo=0;
        puntaje=0;
        
        timeout=SCENE_TIMEOUT;
        time=ofGetElapsedTimef();
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            frontground.color.animateTo(ofColor(255,255));
            next.color.animateToAfterDelay(ofColor(255,255),2.5);
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update enter";
        }
        
        update();
		
        // call finishedEntering() to indicate scne is done entering
        if(!question.isOrWillBeAnimating()) {
            finishedEntering();
            soundClock.play();
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update enter done";
        }
    }
    
    // normal update
    void update() {
        float t = ofGetElapsedTimef();
        float dt = t - time;
        time = t;

        frontground.update(dt);
        membrete.update(dt);
        next.update(dt);
        
        if(!isEntering() && !isExiting())
            tiempo+=dt;
        
        
        question.update(dt);
        optionCorrect.update(dt);
        optionIncorrect.update(dt);
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].update(dt);
        }
        answer.update(dt);
        correct.update(dt);
        incorrect.update(dt);

        if(showingAnswer && !answer.isOrWillBeAnimating()){
            nextQuestion(5.0);
            showingAnswer=false;
        }
        if(changingQuestion && !question.isOrWillBeAnimating()){
            if(questions>=PREGUNTA_MAX_QUESTIONS){
                nextQuestion(0.);
                soundEnd.play();
                sceneManager.gotoScene(SCORE_SCENE_NAME);
            }
            else{
                loadNewQuestion();
            }
            changingQuestion=false;
        }
        
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
            frontground.color.animateTo(ofColor(255,0));
            next.color.animateTo(ofColor(255,0));
            
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit";
        }
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(!frontground.isOrWillBeAnimating()) {
            finishedExiting();
            ofLogNotice(PREGUNTA_SCENE_NAME) << "update exit done";
        }
    }
    
    // draw
    void draw() {        
        ofSetColor(255,frontground.color.getCurrentColor().a*0.75);
        ofRectangle(0,0,ofGetWidth(),ofGetHeight());
        
        membrete.draw();
        frontground.draw();
        //drawBack(ofRectangle(PREGUNTA_OPTIONS_X-PREGUNTA_BACK_MARGIN,PREGUNTA_OPTIONS_Y-PREGUNTA_OPTIONS_MARGIN-PREGUNTA_QUESTION_HEIGHT-PREGUNTA_BACK_MARGIN,PREGUNTA_OPTIONS_WIDTH+PREGUNTA_BACK_MARGIN,PREGUNTA_QUESTION_HEIGHT+PREGUNTA_OPTIONS_MARGIN*(PREGUNTA_OPTIONS+1)+PREGUNTA_BACK_MARGIN),PREGUNTA_BACK_RADIUS);
        question.draw();
        optionIncorrect.draw();
        optionCorrect.draw();
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            option[i].draw();
        }
        answer.draw();
        
        //correct.draw();
        //incorrect.draw();
        
        next.draw();
        
        gizmoFontMiddle.drawString("Pregunta "+ofToString(questions)+" de "+ ofToString(PREGUNTA_MAX_QUESTIONS),ofGetWidth()*0.025,ofGetHeight()*0.075);

        
        drawBack(ofRectangle(PREGUNTA_SCORE_X-2*PREGUNTA_SCORE_MARGIN-PREGUNTA_BACK_MARGIN,PREGUNTA_SCORE_Y-0.5*PREGUNTA_SCORE_MARGIN-PREGUNTA_BACK_MARGIN,4*PREGUNTA_SCORE_MARGIN+2*PREGUNTA_BACK_MARGIN,4*PREGUNTA_SCORE_MARGIN+2*PREGUNTA_BACK_MARGIN),PREGUNTA_BACK_RADIUS);
        ofSetColor(frontground.color.getCurrentColor());
        
        string puntajeString = "Puntaje";
        gizmoFontSmall.drawString(puntajeString,PREGUNTA_SCORE_X-gizmoFontSmall.stringWidth(puntajeString)/2-2,PREGUNTA_SCORE_Y);
        gizmoFontBig.drawString(ofToString(puntaje),PREGUNTA_SCORE_X-gizmoFontBig.stringWidth(ofToString(puntaje))/2-2,PREGUNTA_SCORE_Y+PREGUNTA_SCORE_MARGIN+gizmoFontBig.stringHeight(ofToString(puntaje))/3);
        tick.draw(PREGUNTA_SCORE_X-PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+2*PREGUNTA_SCORE_MARGIN,tick.getWidth()*ofGetHeight()/APP_HEIGHT,tick.getHeight()*ofGetHeight()/APP_HEIGHT);
        cross.draw(PREGUNTA_SCORE_X+PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+2*PREGUNTA_SCORE_MARGIN,cross.getWidth()*ofGetHeight()/APP_HEIGHT,cross.getHeight()*ofGetHeight()/APP_HEIGHT);
        ofSetColor(51,102,51,frontground.color.getCurrentColor().a);
        gizmoFontMiddle.drawString(ofToString(questionsCorrect),PREGUNTA_SCORE_X-gizmoFontMiddle.stringWidth(ofToString(questionsCorrect))/2-2-PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+3.25*PREGUNTA_SCORE_MARGIN);
        ofSetColor(153,51,51,frontground.color.getCurrentColor().a);
        gizmoFontMiddle.drawString(ofToString(questionsFalse),PREGUNTA_SCORE_X-gizmoFontMiddle.stringWidth(ofToString(questionsFalse))/2-2+PREGUNTA_SCORE_MARGIN,PREGUNTA_SCORE_Y+3.25*PREGUNTA_SCORE_MARGIN);
                
    }
    
    // cleanup
    void exit() {
        ofLogNotice(PREGUNTA_SCENE_NAME) << "exit";
    }
    
    void mousePressed(int x, int y, int button){
        if(isEntering() || isExiting())
            return;
        if(!showingAnswer && !changingQuestion){
            for(int i=0;i<PREGUNTA_OPTIONS;i++){
                if(option[i].inside(ofPoint(x,y))){
                    soundRespuesta.play();
                    if(i==CORRECT){
                        questionsCorrect++;
                        puntaje+=10;
                        correct.color.animateTo(ofColor(255,255));
                        soundCorrect.play();
                    }
                    else{
                        questionsFalse++;
                        incorrect.color.animateTo(ofColor(255,255));
                        optionIncorrect.setPosition(option[i].position.getCurrentPosition());
                        optionIncorrect.color.animateTo(ofColor(255,255));
                        soundFalse.play();
                    }
                    showAnswer();
                    return;
                }
            }
        }
        if(next.inside(ofPoint(x,y))){
            if(changingQuestion){
                if(questions>=PREGUNTA_MAX_QUESTIONS){
                    soundEnd.play();
                    sceneManager.gotoScene(SCORE_SCENE_NAME);
                }
                else{
                    loadNewQuestion();
                }
                changingQuestion=false;
            }
            else{
                if(showingAnswer){
                    nextQuestion();
                    showingAnswer=false;
                }else{
                    questionsFalse++;
                    incorrect.color.animateTo(ofColor(255,255));
                    soundFalse.play();
                    showAnswer();
                }
            }
        }
    }
    
    void showAnswer(){
        optionCorrect.setPosition(option[CORRECT].position.getCurrentPosition());
        optionCorrect.color.animateTo(ofColor(255,255));
        
        answer.color.animateTo(ofColor(255,255));
        
        showingAnswer=true;
    }
    
    void nextQuestion(float delay=2.0f){
        for(int i=0;i<PREGUNTA_OPTIONS;i++){
            if(i==CORRECT)
                option[i].color.animateToAfterDelay(ofColor(255,0),delay+0.5);
            else
                option[i].color.animateToAfterDelay(ofColor(255,0),delay);
        }
        optionIncorrect.color.animateToAfterDelay(ofColor(255,0),delay);
        optionCorrect.color.animateToAfterDelay(ofColor(255,0),delay+0.5);
        correct.color.animateToAfterDelay(ofColor(255,0),delay);
        incorrect.color.animateToAfterDelay(ofColor(255,0),delay);
        answer.color.animateToAfterDelay(ofColor(255,0),delay+0.5);
        question.color.animateToAfterDelay(ofColor(255,0),delay+0.5);
        
        changingQuestion=true;
    }
    
    void loadNewQuestion(){
        int q=0;
        do{
            q=ofRandom(questionsTaken.size());
        }while(questionsTaken[q].taken);
        questionsTaken[q].taken=true;
        questions++;

        question.loadImage(questionsTaken[q].path+"/p_question.png");
        question.setAnchorPercent(0.5,0.5);
        question.setColor(ofColor(255,0));
        question.color.animateToAfterDelay(ofColor(255,255),0.5);
        
        answer.loadImage(questionsTaken[q].path+"/p_answer.png");
        answer.setAnchorPercent(0.5,0.5);
        answer.setColor(ofColor(255,0));
        
        option[CORRECT].loadImage(questionsTaken[q].path+"/p_correct.png");
        option[FALSE_0].loadImage(questionsTaken[q].path+"/p_false0.png");
        option[FALSE_1].loadImage(questionsTaken[q].path+"/p_false1.png");
        
        optionCorrect.setColor(ofColor(255,0));
        optionIncorrect.setColor(ofColor(255,0));
        correct.setColor(ofColor(255,0));
        incorrect.setColor(ofColor(255,0));
        
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
            option[order[i]].color.animateToAfterDelay(ofColor(255,255),0.5+0.5*i);
        }
    }
    
    ofxAnimatableObject<ofImage> question;
    ofxAnimatableObject<ofImage> option[PREGUNTA_OPTIONS];
    ofxAnimatableObject<ofImage> answer;
    ofxAnimatableObject<ofImage> optionCorrect,optionIncorrect;
    ofxAnimatableObject<ofImage> correct,incorrect;
    ofxAnimatableObject<ofImage> next;
    bool showingAnswer,changingQuestion;
    
    unsigned int order[PREGUNTA_OPTIONS];
    unsigned int questions,questionsCorrect,questionsFalse;
    vector<QuestionDir> questionsTaken;
    
    ofSoundPlayer soundRespuesta,soundClock;
    ofSoundPlayer soundCorrect,soundFalse,soundEnd;

    ofImage tick,cross;
    ofTrueTypeFont gizmoFontBig,gizmoFontMiddle,gizmoFontSmall;

    int& puntaje;
    float& tiempo;
    float time,timeout;
    
    ofxAnimatableObject<ofImage> frontground,membrete;
    ofxSceneManager& sceneManager;
    
    void drawBack(ofRectangle rect, float r, float w=5.0f){
        ofPushStyle();
        ofSetCircleResolution(100);
        ofFill();
        ofSetColor(150,200,65,ofMap(frontground.color.getCurrentColor().a,0,255,0,200));
        ofRectRounded(rect,r);
        ofNoFill();
        ofSetColor(0,102,51,ofMap(frontground.color.getCurrentColor().a,0,255,0,150));
        ofSetLineWidth(w);
        ofRectRounded(rect.x-w/2,rect.y-w/2,rect.width+w,rect.height+w,r);
        ofPopStyle();
    }
};
