//=======================================================================================
// Box.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Box.h"
#include "Vertex.h"
#include <math.h>

float sgn(float t)
{
  if (t < -MATH_EPS) return -1.0f;
  if (t >  MATH_EPS) return  1.0f;
  return 0.0f;
}



Box::Box()
: nVertices(0), nFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
}
 
Box::~Box()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}


// Procedure generation of smoothed cube

void Box::init(ID3D10Device* device, float scale, float smoothnessRadius, int angleSteps)
{
	
  
  int i, j;
  float alpha, beta;
  float x, y, z;

  md3dDevice = device;
 
	nVertices = 8 * angleSteps * angleSteps;
  nFaces    = (2 * angleSteps - 1) * (8 * angleSteps + 4) ; 

  UncoloredVertex* vertices = new UncoloredVertex[nVertices];

  for (i = 0; i < 2 * angleSteps; ++i)
    for (j = 0; j < 4 * angleSteps; ++j)
    {
      alpha = ((i - angleSteps + 0.5) * (PI / 2)) / angleSteps;
      beta  = ((j + 0.5) * (PI / 2)) / angleSteps;

      x = cosf(alpha) * cosf(beta);
      z = cosf(alpha) * sinf(beta);
      y = sinf(alpha);

      vertices[i * 4 * angleSteps + j].pos.x = scale * (smoothnessRadius * x + (1 - smoothnessRadius) * sgn(x));
      vertices[i * 4 * angleSteps + j].pos.y = scale * (smoothnessRadius * y + (1 - smoothnessRadius) * sgn(y));
      vertices[i * 4 * angleSteps + j].pos.z = scale * (smoothnessRadius * z + (1 - smoothnessRadius) * sgn(z));

      vertices[i * 4 * angleSteps + j].normal.x = x;
      vertices[i * 4 * angleSteps + j].normal.y = y;
      vertices[i * 4 * angleSteps + j].normal.z = z;
    }
      
  D3D10_BUFFER_DESC vbd;
  vbd.Usage = D3D10_USAGE_IMMUTABLE;
  vbd.ByteWidth = sizeof(UncoloredVertex) * nVertices;
  vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
  vbd.CPUAccessFlags = 0;
  vbd.MiscFlags = 0;
  D3D10_SUBRESOURCE_DATA vinitData;
  vinitData.pSysMem = vertices;
  HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
  delete[] vertices;

  // Create the index buffer

	DWORD* indices = new DWORD[3 * nFaces];
	
  for (i = 0; i < 2 * angleSteps - 1; ++i)
    for (j = 0; j < 4 * angleSteps; ++j)
    {
      indices[6 * (i * 4 * angleSteps + j) + 0] = i * 4 * angleSteps + j;
      indices[6 * (i * 4 * angleSteps + j) + 1] = (i + 1) * 4 * angleSteps + j;
      indices[6 * (i * 4 * angleSteps + j) + 2] = (i + 1) * 4 * angleSteps + ((j + 1) % (4 * angleSteps));

      indices[6 * (i * 4 * angleSteps + j) + 3] = i * 4 * angleSteps + j;
      indices[6 * (i * 4 * angleSteps + j) + 4] = (i + 1) * 4 * angleSteps + ((j + 1) % (4 * angleSteps));
      indices[6 * (i * 4 * angleSteps + j) + 5] = i * 4 * angleSteps + ((j + 1) % (4 * angleSteps));

      int a = j + 1;
    }
    
  for (i = 0; i < 4 * angleSteps - 2; ++i) 
  {  
    indices[(2 * angleSteps - 1) * 24 * angleSteps + 3 * i + 0] = 0;
    indices[(2 * angleSteps - 1) * 24 * angleSteps + 3 * i + 1] = i + 1;
    indices[(2 * angleSteps - 1) * 24 * angleSteps + 3 * i + 2] = i + 2;
  }
   
  for (i = 0; i < 4 * angleSteps - 2; ++i) 
  {  
    indices[nFaces * 3 - 3 * i - 1] = nVertices - 1;
    indices[nFaces * 3 - 3 * i - 3] = nVertices - 2 - i;
    indices[nFaces * 3 - 3 * i - 2] = nVertices - 3 - i;
  }

  D3D10_BUFFER_DESC ibd;
  ibd.Usage = D3D10_USAGE_IMMUTABLE;
  ibd.ByteWidth = sizeof(DWORD) * nFaces * 3;
  ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
  ibd.CPUAccessFlags = 0;
  ibd.MiscFlags = 0;
  D3D10_SUBRESOURCE_DATA iinitData;
  iinitData.pSysMem = indices;
  HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));


  //delete[] indices;
}

void Box::draw(int nInstances)
{
  md3dDevice->DrawIndexedInstanced(3 * nFaces, nInstances, 0, 0, 0);
}
void  Box::setVB_AndIB_AsCurrent(ID3D10Device* device, ID3D10Buffer* cubeInstancesBuffer)
{
  UINT stride[2] = 
  {
    sizeof(UncoloredVertex), 
    sizeof(CubeInstance)
  };

  UINT offset[2] = {0, 0};

	ID3D10Buffer* curVB[2];
  curVB[0] = mVB;
  curVB[1] = cubeInstancesBuffer;
  
  device->IASetVertexBuffers(0, 2, curVB, stride, offset);

  device->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
}