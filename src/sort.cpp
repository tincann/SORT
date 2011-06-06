/*
 * filename  :	sort.cpp
 *
 * programmer:	Cao Jiayin
 */

// include the header file
#include <iostream>
#include "geometry/vector.h"
#include "geometry/point.h"
#include "geometry/normal.h"
#include "texture/texture.h"
#include "texture/texshower/bmpsaver.h"

// use standard namespace
using namespace std;

void output( const string& str , const Vector& v )
{
	cout<<"vector "<<str<<":"<<v.x<<" "<<v.y<<" "<<v.z<<endl;
}
void output( const string& str , const Point& p )
{
	cout<<"point "<<str<<":"<<p.x<<" "<<p.y<<" "<<p.z<<endl;
}
// the enter point for the program
int main( int argc , char** argv )
{
	Texture tex;
	TexShower* t = new BmpSaver();
	tex.SetTexShower( t );
	tex.Output( "hello.bmp" );

	Texture::SetTexShower( 0 );

  	return 0;
} 
