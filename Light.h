//=======================================================================================
// Light.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include "d3dApp/d3dUtil.h"

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}


  D3DXVECTOR4 pos;
	D3DXVECTOR4 dir;
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	D3DXVECTOR3 att;
	float spotPow;
	float range;
  __int32 lightType; //0 - off, 1 - parallel, 2 - point, 3 - spot
 	float brightness;    
  float pad4;      // not used
  
};

#endif // LIGHT_H
