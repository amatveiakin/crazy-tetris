//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dapp/d3dUtil.h"
#include "DirectXConstants.h"

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


#endif // BOX_H
