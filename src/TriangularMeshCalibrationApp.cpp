//
//  TriangularMeshCalibrationApp.cpp
//  TriangularMeshCalibrationApp
//
//  Created by Nien Lam on 4/13/11.
//  Copyright 2011 Chimera Island. All rights reserved.
//
//
//  Formulas
//
//  a*srcPtA.x + b*srcPtB.x + c*srcPtC.x = X
//  a*srcPtA.y + b*srcPtB.y + c*srcPtC.y = Y 
//  a + b + c         = 1.0
// ----------------------------------------
//
//  Solving for 3 equations.
//
//  c = (Y-srcPtA.y)/(srcPtB.y-srcPtA.y) - (X-srcPtA.x)/(srcPtB.x-srcPtA.x) / 
//      (srcPtA.x-srcPtC.x)/(srcPtB.x-srcPtA.x) - (srcPtA.y-srcPtC.y)/(srcPtB.y-srcPtA.y)
//
//  b = (X - srcPtA.x + c*(srcPtA.x-srcPtC.x)) / 
//		(srcPtB.x-srcPtA.x)
//
//  a = 1.0 - b - c
// ----------------------------------------
//
//  Calibrated Point
//
//  Xp = a*srcPtA.x + b*srcPtB.x + c*srcPtC.x 
//  Yp = a*srcPtA.y + b*srcPtB.y + c*srcPtC.y
// ----------------------------------------
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Struct to hold points of trinagle.
typedef struct 
{
	Vec2f ptA;
	Vec2f ptB;
	Vec2f ptC;
} Triangle;


class TriangularMeshCalibrationApp : public AppBasic 
{
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void mouseMove( MouseEvent event );	
	void update();
	void draw();
	void prepareSettings( Settings *settings );

  private:
	Triangle srcTriangle, dstTriangle;
	void drawTriangle( Triangle const &triangle );

	Vec2f srcCursor, calCursor;
	bool  drawCalibratedPoint;
	bool  getCalibratedPt( Triangle const &srcTriangle, Triangle const &dstTriangle, 
						   Vec2f const &srcPt, Vec2f &calPt );
};

void TriangularMeshCalibrationApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 800, 600 );
	settings->setFrameRate( 60.0f );
}

void TriangularMeshCalibrationApp::setup()
{
	srcTriangle.ptA = Vec2f( 100, 100 );
	srcTriangle.ptB = Vec2f( 200, 200 );
	srcTriangle.ptC = Vec2f( 50,  200 );

	dstTriangle.ptA = Vec2f( 300, 300 );
	dstTriangle.ptB = Vec2f( 400, 400 );
	dstTriangle.ptC = Vec2f( 250, 400 );
}

void TriangularMeshCalibrationApp::mouseDown( MouseEvent event )
{

}

void TriangularMeshCalibrationApp::mouseMove( MouseEvent event )
{
	srcCursor = event.getPos();
}

void TriangularMeshCalibrationApp::drawTriangle( Triangle const &triangle )
{
	gl::drawLine( triangle.ptA, triangle.ptB );
	gl::drawLine( triangle.ptB, triangle.ptC );
	gl::drawLine( triangle.ptC, triangle.ptA );
}

// Get caibrated point (calPt) with source point from source triangle and distorted triangle.
// Returns true if point is inside distorted triangle, false otherwise.
bool TriangularMeshCalibrationApp::getCalibratedPt( Triangle const &srcTriangle, Triangle const &dstTriangle, 
													Vec2f const &srcPt, Vec2f &calPt )
{
	// Coefficients.
	float a, b, c;
	
	// Some temp variables for math.
	Vec2f srcPtA = srcTriangle.ptA;
	Vec2f srcPtB = srcTriangle.ptB;
	Vec2f srcPtC = srcTriangle.ptC;
	
	// Solve for 3 equation. 
	c = ( (srcPt.y - srcPtA.y) / (srcPtB.y - srcPtA.y) - (srcPt.x - srcPtA.x) / (srcPtB.x - srcPtA.x) ) / 
		( (srcPtA.x - srcPtC.x) / (srcPtB.x - srcPtA.x) - (srcPtA.y - srcPtC.y) / (srcPtB.y - srcPtA.y) );
	
	b = ( srcPt.x - srcPtA.x + c * (srcPtA.x - srcPtC.x) ) / 
	    ( srcPtB.x - srcPtA.x );
	
	a = 1.0 - b - c;
	
	// Get calibrated point.
	calPt.x = a * dstTriangle.ptA.x + b * dstTriangle.ptB.x + c * dstTriangle.ptC.x;
	calPt.y = a * dstTriangle.ptA.y + b * dstTriangle.ptB.y + c * dstTriangle.ptC.y; 
	
	// Check if calibrated point is inside of triangle.
	return ( ! (a < 0 || b < 0 || c < 0) ); 
}

void TriangularMeshCalibrationApp::update()
{
	// Get calibrated point.
	drawCalibratedPoint = getCalibratedPt( srcTriangle, dstTriangle, srcCursor, calCursor );
}

void TriangularMeshCalibrationApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	
	glLineWidth( 2.0f );
	
	// Draw source triangle.
	gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	drawTriangle( srcTriangle );

	// Draw distorted triangle.
	gl::color( Color( 0.0f, 1.0f, 0.0f ) );
	drawTriangle( dstTriangle );

	// Draw source point.
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );
	gl::drawSolidCircle( srcCursor, 4.0f );
	
	// Draw caibrated point.
	if ( drawCalibratedPoint )
		gl::drawSolidCircle( calCursor, 4.0f );
}


CINDER_APP_BASIC( TriangularMeshCalibrationApp, RendererGl )
