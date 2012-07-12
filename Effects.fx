#include "lighthelper.fx"

const float PI       = 3.14159265358979323f;
const float TWO_PI   = 2 * 3.14159265358979323f;
const float MATH_EPS = 0.001f;


cbuffer cbRare
{
  float CUBE_SCALE_INVERTED;
}

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gVP;
  
	//float4 gColorDiffuse;
	//float4 gColorSpecular; 
};

cbuffer cbPerFrame
{
	Light  gLight;
	float3 gEyePosW;
	float  gTime;
  float  gWaveProgress;
  float  gSemicubesProgress;

  float4x4 gGlobalRotation;
};


struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	//float4 diffuse : DIFFUSE;
	//float4 spec    : SPECULAR;
};

struct CubesVS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
  float3 offset  : offset;  
  float4 diffuseColor  : diffuse;
	float4 specularColor : specular;
};


struct VS_OUT
{
	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 posL    : POSITION_LOCAL;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
    float4 spec    : SPECULAR;
};

VS_OUT vsCubes(CubesVS_IN vIn)
{
	VS_OUT vOut;
	
	vOut.posL = vIn.posL;
	
	// Translate to world space space.
	vOut.posW    = vIn.posL + vIn.offset;
  
  //Make wave effect
  vOut.posW.x += sin(TWO_PI * gWaveProgress) * sin(2 * vOut.posW.y)  * 0.33; //if makes slower (?)
  //vOut.posW.x += sin(gTime +  3 * vOut.posW.y) / 4;
  
  vOut.posW    = mul(float4(vOut.posW, 0.0f), gGlobalRotation);
  vOut.normalW = mul(float4(vIn.normalL, 0.0f), gGlobalRotation);

  // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vOut.posW, 1.0f), gVP);

  // Output vertex attributes for interpolation across triangle.
	vOut.diffuse = vIn.diffuseColor;
	vOut.spec    = vIn.specularColor;
	
	return vOut;
}



VS_OUT vsStandard(VS_IN vIn)
{
	VS_OUT vOut;
	
	vOut.posL = vIn.posL;
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0f), gWorld);
  vOut.posW.x += sin(TWO_PI * gWaveProgress) * sin(2 * vOut.posW.y)  * 0.33; //if makes slower (?)
  //vOut.posW.x += sin(gTime +  3 * vOut.posW.y) / 4;
  vOut.posW    = mul(float4(vOut.posW, 0.0f), gGlobalRotation);

  vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
  vOut.normalW = mul(float4(vOut.normalW, 0.0f), gGlobalRotation);

  // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vOut.posW, 1.0f), gVP);


	// Output vertex attributes for interpolation across triangle.
	//vOut.diffuse = gColorDiffuse;
	//vOut.spec    = gColorSpecular;
	
	return vOut;
}
 
float4 psStandard(VS_OUT pIn) : SV_Target
{
  //Interpolating normal can make it not be of unit length so normalize it.
  pIn.normalW = normalize(pIn.normalW);
  
  //move to psTransparent?
  if (pIn.diffuse.a < 1.0f - MATH_EPS) //if makes faster (?)
  {    
    //rendering transparent or sliced object, so normal may have wrong direction
    if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW =  - pIn.normalW;
  }


  SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
  
  float3 litColor;
  //add lighttype!
  if (gLight.lightType == 0) // Parallel
  {
	  litColor = ParallelLight(v, gLight, gEyePosW);
  }
  else if (gLight.lightType == 1) // Point
  {
	  litColor = PointLight(v, gLight, gEyePosW);
	}
	else // Spot
	{
	  litColor = Spotlight(v, gLight, gEyePosW);
	}
	   
  return float4(litColor, pIn.diffuse.a);
}

RasterizerState rsSemicubes 
{
  FillMode = Solid; 
  CullMode = None;
  AntiAliasedLineEnable = true;
  MultisampleEnable = false;
};


float4 psSemicubes(VS_OUT pIn) : SV_Target
{
  float t = lerp(1.84,  0.3, sqrt(gSemicubesProgress)); //some magic constants
  clip(+ pow(abs(CUBE_SCALE_INVERTED * pIn.posL.x - t), 4)
       + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.y - t), 4) 
       + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.z - t), 4) - 3.3);

  //Interpolating normal can make it not be of unit length so normalize it.
  pIn.normalW = normalize(pIn.normalW);
  
  if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW =  - pIn.normalW;

  SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
  
 float3 litColor;
  //add lighttype!
  if (gLight.lightType == 0) // Parallel
  {
	  litColor = ParallelLight(v, gLight, gEyePosW);
  }
  else if (gLight.lightType == 1) // Point
  {
	  litColor = PointLight(v, gLight, gEyePosW);
	}
	else // Spot
	{
	  litColor = Spotlight(v, gLight, gEyePosW);
	}
   
  //return float4(litColor, pIn.diffuse.a);
  return float4(1., 1., 1., 1.);
}

technique10 techCubes
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, vsCubes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psStandard() ) );
    }
}

technique10 techSemicubes
{
    pass P0
    {
        SetRasterizerState(rsSemicubes);  //if uncomment this, porting to OpenGL will be easier. But it's slow.
        SetVertexShader( CompileShader( vs_4_0, vsCubes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psSemicubes() ) );
    }
}
