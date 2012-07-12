#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "d3dapp/d3dutil.h"
class Primitive
{
public:
  Primitive();
  ~Primitive();
protected:
	DWORD nVertices;
	DWORD nFaces;

	ID3D10Device* md3dDevice;
  ID3D10Buffer* mVB;
};

class Box : public Primitive
{

public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale, float smoothnessRadius, int angleSteps);
	void setVB_AndIB_AsCurrent(ID3D10Device* device,  ID3D10Buffer* cubeInstancesBuffer);
  void draw(int nInstances);

private:
  ID3D10Buffer* mIB;


};

class Wall : public Primitive
{
public:

	Wall();
	~Wall();
  void init(ID3D10Device* device, float worldWidth, float worldHeight, float texWidth, float texHeight);
  void draw();

};



class Glass : public Primitive
{
public:
  Glass();
  ~Glass();
  void init(ID3D10Device* device, float width, float height, float depth);
  void draw();

private:
  ID3D10Buffer* mIB;
};

#endif