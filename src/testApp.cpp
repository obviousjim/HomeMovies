#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofBackground(0);
    
    srand(ofGetSeconds());
    editMode = false;
    movieA = NULL;
    movieB = NULL;

    ofxXmlSettings moviesxml;
    if(moviesxml.loadFile("movies.xml")){
        string sourcePath = moviesxml.getValue("sourcePath", "");
        
        moviesxml.pushTag("movies");
        int numMovies = moviesxml.getNumTags("movie");
        for(int i = 0; i < numMovies; i++){
            HomeMovie m;
            m.path = moviesxml.getValue("movie","", i);
            m.cuts = new ofxTimeline();
            m.cuts->setup();
            m.cuts->setName("timeline_" + m.path);
            m.cuts->setSpacebarTogglePlay(false);
            m.player = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
            
            ofQTKitPlayer* p =   new ofQTKitPlayer();
            p->setSynchronousSeeking(false);
            p->loadMovie(sourcePath+"/"+m.path, OF_QTKIT_DECODE_TEXTURE_ONLY);
            p->play();
            p->setSpeed(0.0);
            
            m.player = ofPtr<ofVideoPlayer>( new ofVideoPlayer() );
            m.player->setPlayer(ofPtr<ofBaseVideoPlayer>( p ));
            
            ofxTLVideoTrack* t = m.cuts->addVideoTrack("video");
            t->setPlayer( m.player );

//            m.player = t;
            //m.player = m.cuts->addVideoTrack(sourcePath+"/"+m.path)->getPlayer();
            m.player->setVolume(0);
            m.cuts->setDurationInSeconds(m.player->getDuration());
            m.cuts->addSwitches("cuts");
//            ofAddListener(m.cuts->events().bangFired, this, &testApp::bangTriggered);
            recents.push_back(m.path);
            movies.push_back(m);
        }
    }
    else {
        ofLogError("Movies XML file failed to load");
    }
    
    ofToggleFullscreen();
    selectMovies();
}

//--------------------------------------------------------------
void testApp::update(){
    if(!editMode){
        selectMovies();
        movieA->cuts->setPercentComplete(movieA->player->getPosition());
        movieB->cuts->setPercentComplete(movieB->player->getPosition());
    }
}
/*
//--------------------------------------------------------------
void testApp::bangTriggered(ofxTLBangEventArgs& event){
    cout << "   bang fired on timeline " << event.sender->getName() << " at time " << event.sender->getCurrentTime() << endl;
    for(int i = 0; i < movies.size(); i++){
        if(movieA->cuts == event.sender){
            movieA->player->stop();
            cout << "replacing movie " << movieA->path << endl;
            movieA = replaceMovie(movieA, movieB);

        }
        if(movieB->cuts == event.sender){
            movieB->player->stop();
            cout << "replacing movie " << movieB->path << endl;
            movieB = replaceMovie(movieB, movieA);
        }
    }
}
*/

//--------------------------------------------------------------
void testApp::draw(){
    ofRectangle screenA = ofRectangle(0,0,ofGetWidth()/2, ofGetHeight());
    ofRectangle screenB = ofRectangle(ofGetWidth()/2,0,ofGetWidth()/2, ofGetHeight());
    if(editMode){
        if(movieA != NULL){
            ofRectangle movieARect = ofRectangle(0,0, movieA->player->getWidth(),movieA->player->getHeight());
            movieARect.scaleTo(screenA, OF_ASPECT_RATIO_KEEP);
            movieA->player->draw(movieARect);
            movieA->cuts->setOffset(movieARect.getBottomLeft());
            movieA->cuts->draw();
        }
    }
    else{
        if(movieA != NULL){
            ofRectangle movieARect = ofRectangle(0,0, movieA->player->getWidth(),movieA->player->getHeight());
            movieARect.scaleTo(screenA, OF_ASPECT_RATIO_KEEP);
            movieA->player->draw(movieARect);
        }
        if(movieB != NULL){
            ofRectangle movieBRect = ofRectangle(0,0, movieA->player->getWidth(),movieA->player->getHeight());
            movieBRect.scaleTo(screenB, OF_ASPECT_RATIO_KEEP);
            movieB->player->draw(movieBRect);
        }
    }
}


//--------------------------------------------------------------
void testApp::selectMovies(){
    movieA = replaceMovie(movieA, movieB);
    movieB = replaceMovie(movieB, movieA);
}

HomeMovie* testApp::replaceMovie(HomeMovie* currentMovie, HomeMovie* otherMovie){
    
    if(currentMovie == NULL ||
       currentMovie->player->getSpeed() < 1.0 ||
       !currentMovie->cuts->isSwitchOn("cuts"))
    {
        if(currentMovie != NULL){
            cout << ofGetElapsedTimef()
                << " replacing movie " << currentMovie->path
                << " at time " << currentMovie->player->getPosition()
                << " is playing? " << currentMovie->player->isPlaying()
                << " is on? " << currentMovie->cuts->isSwitchOn("cuts")
                << " speed is " << currentMovie->player->getSpeed() << endl;
        }

        bool movieIsValid = false;        
        while(!movieIsValid){
            movieIndex = (movieIndex + 1 ) % movies.size();
            HomeMovie* attempt = &movies[movieIndex];
            cout << "   attempting movie " << attempt->path << " at index " << movieIndex << endl;
            //TODO: custom pairings
            //TODO: custom probabilities
            if( (otherMovie == NULL || attempt->path != otherMovie->path)){
                
                if(attempt->path == "love_is_all_1280x720.mov" && ofRandomuf() < .6){
                    cout << "       'love is all' failed " << endl;
                    continue;
                }
                
                if(currentMovie!=NULL)currentMovie->player->setSpeed(0.0);
                currentMovie = attempt;
                
                ofxTLBangs* cuts = (ofxTLBangs*)(currentMovie->cuts->getTrack("cuts"));
                vector<ofxTLKeyframe*>& keyframes = cuts->getKeyframes();
                int numKeys = keyframes.size();
                cout << "       num keys! " << numKeys << endl;
                float position = 0;
                if(numKeys != 0){
                    bool validKey = false;
                    int numTries = 0;
                    int keyIndex = int(ofRandom(numKeys));
                    ofxTLKeyframe* randomKey = keyframes[keyIndex];
                    cout << "       key index " << keyIndex << endl;
                    position = (randomKey->time / 1000.0) / currentMovie->player->getDuration() + .01;
                }
                currentMovie->cuts->setPercentComplete(position);
                currentMovie->player->setPosition(position);
                currentMovie->player->setSpeed(1.0);
                
                cout << "   selected " << currentMovie->path << " at time " << position << endl;
                cout << endl;
//                recents.push_front(currentMovie->path);
                movieIsValid = true;
                return currentMovie;
            }
        }
        
        if(!movieIsValid){
            cout << "Couldn't find valid movie!!" << endl;
        }
    }
    
    return currentMovie;
}

//--------------------------------------------------------------
int testApp::recentIndex(string path){
    
    if(recents.size() > movies.size()){
        recents.pop_back();
    }
    
    int i = 0;
    for (list<string>::iterator it=recents.begin(); it!=recents.end(); ++it){
        if(*it == path){
            cout << "movie " << path << " played " << i <<  " times ago"<< endl;
            return i;
        }
        i++;
    }
    
    return INT_MAX;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
    
    if(key == 'E'){
        editMode = !editMode;
        if(editMode){
            if(movieA != NULL) movieA->player->stop();
            if(movieB != NULL) movieB->player->stop();
            movieA = &movies[0];
            currentEditMovieIndex = 0;
        }
        else{
            if(movieA != NULL) movieA->player->play();
            if(movieB != NULL) movieB->player->play();
        }
    }
    
    if(editMode){
        if(key == OF_KEY_LEFT){
            if(movieA != NULL) movieA->cuts->hide();
            currentEditMovieIndex = (currentEditMovieIndex - 1);
            if(currentEditMovieIndex < 0){
                currentEditMovieIndex = movies.size()-1;
            }
            movieA = &movies[currentEditMovieIndex];
            movieA->cuts->show();
            
        }
        else if(key == OF_KEY_RIGHT){
            if(movieA != NULL)movieA->cuts->hide();
            currentEditMovieIndex = (currentEditMovieIndex + 1) % movies.size();
            movieA = &movies[currentEditMovieIndex];
            movieA->cuts->show();
        }
        else if(key == ' '){
            if(movieA != NULL){
                if(!movieA->player->isPlaying()){
                    movieA->player->play();
                }
                else{
                    movieA->player->stop();
                }
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}