/*
 * filename :	imagetexture.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_IMAGETEXTURE
#define	SORT_IMAGETEXTURE

// include the header file
#include "texture.h"
#include "utility/referencecount.h"
#include "managers/texmanager.h"

///////////////////////////////////////////////////////////////
// definition of image texture
class ImageTexture : public Texture 
{
// public method
public:
	// default constructor
	ImageTexture() { _init(); }
	// destructor
	~ImageTexture() { Release(); }

	// load image from file
	// para 'str'  : the name of the image file to be loaded
	// result      : whether loading is successful
	bool LoadImageFromFile( const std::string& str );

	// get the texture value
	// para 'x' :	x coordinate , if out of range , use filter
	// para 'y' :	y coordinate , if out of range , use filter
	// result	:	spectrum value in the position
	virtual Spectrum GetColor( int x , int y ) const;

	// it means nothing to set the size of the image file
	// set the size of the texture
	virtual void	SetSize( unsigned w , unsigned h ){}

	// release the texture memory
	virtual void Release();

	// create texture
	Texture* CreateInstance() { return new ImageTexture(); }

	// whether the image is valid
	bool IsValid() { return (bool)m_pMemory; }
// private field
private:
	// array saving the color of image
	Reference<ImgMemory>	m_pMemory;

// private method
	// initialize default data
	void	_init();

	// set texture manager as a friend
	friend class TexManager;

	// register properties
	void _registerAllProperty();

// property handler
	class FileNameProperty : public PropertyHandler<Texture>
	{
	public:
		// constructor
		FileNameProperty(Texture* tex):PropertyHandler(tex){}

		// set value
		void SetValue( const string& value )
		{
			ImageTexture* ct = dynamic_cast<ImageTexture*>( m_target );
			
			// load image file
			ct->LoadImageFromFile( value );
		}
	};
};

#endif
