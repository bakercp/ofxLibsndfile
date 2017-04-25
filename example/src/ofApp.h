#pragma once


#include "ofMain.h"
#include "ofxLibsndfile.h"


class ofApp: public ofBaseApp
{
public:
    void setup() override;
    void update() override;
    void draw() override;

    void readFile();
    void writeFile();

    ofSoundPlayer player;

};
