/*
 * filename :	enum.h
 *
 * programmer :	Cao Jiayin
 */

#ifndef	SORT_ENUM
#define	SORT_ENUM

// texture output type
enum TEX_TYPE
{
	TT_BMP = 0,
	TT_WINDOW ,
	TT_END ,
};

enum TEXCOORDFILTER
{
	TCF_WARP = 0 ,
	TCF_CLAMP ,
	TCF_MIRROR
};

#endif