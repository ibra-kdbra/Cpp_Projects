#include "ofxAsync.h"

std::map<int, shared_ptr<ofThread> > ofxAsync::runners = {};
ofxAsync::AsyncExit ofxAsync::asyncExit = ofxAsync::AsyncExit();
int ofxAsync::thread_id_max = 0;

int ofxAsync::run(std::function<void()> func){
    int runner_id = ++thread_id_max;
    auto runner = make_shared<AsyncRunner>();
    runners[runner_id] = runner;
    runner->setup(func);
    runner->startThread();
    return runner_id;
}

int ofxAsync::run(std::function<void(ofThread*)> func){
    int runner_id = ++thread_id_max;
    auto runner = make_shared<AsyncRunnerWithArg>();
    runners[runner_id] = runner;
    runner->setup(func);
    runner->startThread();
    return runner_id;
}

void ofxAsync::update(){
    for(auto it = runners.begin(); it != runners.end(); ++it) {
        auto& key = it->first;
        auto e = runners[key];
        if(!e || !e->isThreadRunning()){
            runners.erase(key);
            return;
        }
    }
}

bool ofxAsync::exists(int thread_id){
    return runners.count(thread_id) > 0 && runners[thread_id]->isThreadRunning();
}

bool ofxAsync::isRunning(int thread_id){
    return runners.count(thread_id) > 0 && runners[thread_id]->isThreadRunning();
}

void ofxAsync::stop(int thread_id, bool wait_until_stop){
    if(runners.count(thread_id) > 0 && runners[thread_id]->isThreadRunning()){
        auto e = runners[thread_id];
        e->stopThread();
        if(wait_until_stop){
            e->waitForThread(false);
        }
    }
}

void ofxAsync::stopAll(bool wait_until_stop){
    for(auto it = runners.begin(); it != runners.end(); ++it) {
        auto& key = it->first;
        auto e = runners[key];
        if(e->isThreadRunning()){
            e->stopThread();
            
            if(wait_until_stop){
                e->waitForThread(false);
            }
        }
    }
    
    if(wait_until_stop){
        while(true){
            if( runners.size() == 0 ){
                break;
            }else{
                auto& key = runners.begin()->first;
                runners.erase(key);
            }
        }
    }
}

void ofxAsync::waitFor(int thread_id){
    if(runners.count(thread_id) > 0 && runners[thread_id]->isThreadRunning()){
        auto e = runners[thread_id];
        e->waitForThread(false);
    }
}

void ofxAsync::waitForAll(){
    for(auto it = runners.begin(); it != runners.end(); ++it) {
        auto& key = it->first;
        auto e = runners[key];
        if(e->isThreadRunning()){
            e->waitForThread(false);
        }
    }
}

boost::optional<shared_ptr<ofThread>> ofxAsync::getThread(int thread_id){
    if(runners.count(thread_id) > 0 && runners[thread_id]->isThreadRunning()){
        auto e = runners[thread_id];
        return e;
    }else{
        return boost::none;
    }
}

