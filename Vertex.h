

#ifndef VERTEX_H
#define VERTEX_H

#include "d3dapp/d3dUtil.h"

struct ColoredVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};
 
struct UncoloredVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct CubeInstance
{
  D3DXVECTOR3 offset;
  D3DXCOLOR   diffuseColor;
  D3DXCOLOR   specularColor;
};

#endif // VERTEX_H