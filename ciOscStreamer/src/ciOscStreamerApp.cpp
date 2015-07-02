//
//  ciOscStreamerApp.cpp
//  ciOscStreamer
//
//  Created by Zhao He on 7/2/15.
//  Copyright (c) 2015 Zhao He. All rights reserved.
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/System.h"
#include "cinder/params/Params.h"
#include "OscSender.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciOscStreamerApp : public App
{
private:
    int                     mData;
    int                     mOscPort;
    float                   mFps;
    string                  mOscDestIp;
    params::InterfaceGlRef  mParams;
    std::thread             mThread;
    CameraPersp             mCam;
    CameraUi                mCamUi;
    osc::Sender             mOscSender;
    void                    runThread();
public:
    void setup()                       override;
    void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void update()                      override;
    void draw()                        override;
    void cleanup()                     override;
};

void ciOscStreamerApp::runThread()
{
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(1));
        mData++;
        osc::Message message;
        message.setAddress( "/osc/data" );
        message.addStringArg( to_string(mData) );
        message.addStringArg( "\n" );
        mOscSender.sendMessage(message);
    }
}

void ciOscStreamerApp::setup()
{
    setWindowSize( 800, 600 );
    
    mCam.lookAt( vec3(0, 0, 10), vec3(0, 0, 0), vec3(0, 1, 0) );
    mCamUi = CameraUi( &mCam );
    
    mData      = 0;
    mOscPort   = 3000;
    mOscDestIp = System::getIpAddress();
    mOscSender.setup(mOscDestIp, mOscPort);
    
    mParams = params::InterfaceGl::create( getWindow(), "Parameters", ivec2(200, 100), ColorA(0, 0, 0, 0.5f) );
    mParams->setOptions( "", "refresh = 0.01" );
    mParams->setOptions( "", "valueswidth = 100" );
    mParams->addParam( "FPS", &mFps, "readonly = true" );
    mParams->addParam( "Data", &mData, "readonly = true" );
    mParams->addParam( "OSC Dest", &mOscDestIp, "readonly = true" );
    mParams->addParam( "OSC Port", &mOscPort, "readonly = true" );
    
    mThread = std::thread(&ciOscStreamerApp::runThread, this);
}

void ciOscStreamerApp::mouseDown( MouseEvent event )
{
    mCamUi.mouseDown( event.getPos() );
}

void ciOscStreamerApp::mouseDrag( MouseEvent event )
{
    mCamUi.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void ciOscStreamerApp::update()
{
    mFps = getAverageFps();
}

void ciOscStreamerApp::draw()
{
    gl::clear();
    gl::viewport( 0, 0, getWindowWidth(), getWindowHeight() );
    
    mParams->draw();
    
    gl::setMatrices( mCam );
    gl::color( Color (0, 1, 0) );
    gl::drawCoordinateFrame();
}

void ciOscStreamerApp::cleanup()
{
    mThread.join();
}

CINDER_APP( ciOscStreamerApp, RendererGl )
