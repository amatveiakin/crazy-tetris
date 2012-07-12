//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float smoothnessRadius, int angleSteps);
	void draw();

private:
	DWORD nVertices;
	DWORD nFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

};


#endif // BOX_H
