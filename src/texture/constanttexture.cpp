/*
   FileName:      constanttexture.cpp

   Created Time:  2011-08-04 12:44:43

   Auther:        Cao Jiayin

   Email:         soraytrace@hotmail.com

   Location:      China, Shanghai

   Description:   SORT is short for Simple Open-source Ray Tracing. Anyone could checkout the source code from
                'sourceforge', https://soraytrace.svn.sourceforge.net/svnroot/soraytrace. And anyone is free to
                modify or publish the source code. It's cross platform. You could compile the source code in 
                linux and windows , g++ or visual studio 2008 is required.
*/

// include the header file
#include "constanttexture.h"

IMPLEMENT_CREATOR( ConstantTexture );

// default constructor
ConstantTexture::ConstantTexture()
{
	_init();
}

// constructor from three float
ConstantTexture::ConstantTexture( float r , float g , float b ):
	m_Color( r , g , b )
{
	_init();
}

// destructor
ConstantTexture::~ConstantTexture()
{
}

// overwrite init method
void ConstantTexture::_init()
{
	// by default , the width and height if not zero
	// because making width and height none zero costs nothing
	// while makeing them zero forbids showing the texture
	m_iTexWidth = 16;
	m_iTexHeight = 16;

	_registerAllProperty();
}

// register properties
void ConstantTexture::_registerAllProperty()
{
	_registerProperty( "color" , new ColorProperty( this ) );
}