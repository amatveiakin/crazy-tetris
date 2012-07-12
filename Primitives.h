#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "d3dapp/d3dutil.h"

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale, float smoothnessRadius, int angleSteps);
	void setVB_AndIB_AsCurrent(ID3D10Device* device,  ID3D10Buffer* cubeInstancesBuffer);
  void draw(int nInstances);


private:
	DWORD nVertices;
	DWORD nFaces;

	ID3D10Device* md3dDevice;
  ID3D10Buffer* mVB;
  ID3D10Buffer* mIB;


};

class Wall
{
public:

	Wall();
	~Wall();
  void init(ID3D10Device* device, float worldWidth, float worldHeight, float texWidth, float texHeight);
  void draw();


private:
	DWORD nVertices;
	DWORD nFaces;

	ID3D10Device* md3dDevice;
  ID3D10Buffer* mVB;

};

#endif