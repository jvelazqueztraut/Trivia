#pragma once

#include "ofxAnimatableObject.h"

class Card {
public:
    Card(){
        showing=false;
    }
    
    void setup(unsigned int n, string frontFile, string backFile){
        number = n;
        front.loadImage(frontFile);
        back.loadImage(backFile);
        
        reset();
    }
    
    void updateChange(float dt){
        back.update(dt);
        front.update(dt);
    }
    
    void draw(int x, int y){
        back.draw();
        front.draw();
    }
    
    void show(float delay=0.0f){
        front.color.setCurve(SQUARE);
        front.color.animateToAfterDelay(ofColor(255,255),delay);
        front.rotation.animateToAfterDelay(ofVec3f(0,360,0),delay);
        back.color.setCurve(SQUARE);
        back.color.animateToAfterDelay(ofColor(255,0),delay);
        back.setRotation(ofVec3f(0,0,0));
        back.rotation.animateToAfterDelay(ofVec3f(0,180,0),delay);
        showing=true;
    }
    
    void hide(float delay=0.0f){
        front.color.setCurve(SQUARE);
        front.color.animateToAfterDelay(ofColor(255,0),delay);
        front.setRotation(ofVec3f(0,0,0));
        front.rotation.animateToAfterDelay(ofVec3f(0,180,0),delay);
        back.color.setCurve(SQUARE);
        back.color.animateToAfterDelay(ofColor(255,255),delay);
        back.rotation.animateToAfterDelay(ofVec3f(0,360,0),delay);
        showing=false;
    }
    
    void lock(float delay=0.0f){
        front.color.setCurve(EASE_IN);
        front.color.animateToAfterDelay(ofColor(225,(showing?255:0)),delay);
        back.color.setCurve(EASE_IN);
        back.color.animateToAfterDelay(ofColor(225,(showing?0:255)),delay);
    }
    
    void reset(){
        front.setColor(ofColor(255,0));
        front.setRotation(ofVec3f(0,180,0));
        back.setColor(ofColor(255,255));
        back.setRotation(ofVec3f(0,0,0));
        showing=false;
    }
    
    float getWidth(){
        return front.getWidth();
    }
    float getHeight(){
        return front.getHeight();
    }
    
    void setAnchorPercent(float xPct, float yPct){
        front.setAnchorPercent(xPct,yPct);
        back.setAnchorPercent(xPct,yPct);
    }
    
    bool isShowing(){
        return showing;
    }
    
    bool isChanging(){
        return (front.isOrWillBeAnimating() || back.isOrWillBeAnimating());
    }
    
    ofxAnimatableObject<ofImage> front,back;
    bool showing;
    unsigned int number;
};
