

#ifndef VERTEX_H
#define VERTEX_H

#include "d3dapp/d3dUtil.h"
 
struct UncoloredVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct TexturedVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
  D3DXVECTOR2 texC;
};

struct CubeInstance
{
  D3DXVECTOR3 offset;
  D3DXCOLOR   diffuseColor;
  D3DXCOLOR   specularColor;
};

#endif // VERTEX_H