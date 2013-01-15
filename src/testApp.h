#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"

typedef struct {
    string path;
    ofxTimeline* cuts;
    ofPtr<ofVideoPlayer> player;
} HomeMovie;

class testApp : public ofBaseApp{
  public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    HomeMovie* replaceMovie(HomeMovie* currentMovie, HomeMovie* otherMovie);
    
    vector<HomeMovie> movies;

    void bangTriggered(ofxTLBangEventArgs& event);
    
    HomeMovie* movieA;
    HomeMovie* movieB;
    int movieIndex;
    list<string> recents;

    bool editMode;
    int currentEditMovieIndex;
    int recentIndex(string path);
    void selectMovies();
};
