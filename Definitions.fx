#ifndef DEFINITIONS_FX
#define DEFINITIONS_FX

struct UncoloredVS_IN
{
  float3 posL    : POSITION_LOCAL;
  float3 normalL : NORMAL;
  //float4 diffuse : DIFFUSE;
  //float4 spec    : SPECULAR;
};

struct TexturedVS_IN
{
  float3 posL    : POSITION_LOCAL;
  float3 normalL : NORMAL;
  float2 texC    : TEXCOORD0;
};


struct TexturedVS_OUT
{
  float4 posH    : SV_POSITION;
  float3 posW    : POSITION_WORLD;
  //float3 posL    : POSITION_LOCAL;
  float3 normalW : NORMAL;
  float2 texC     : TEXCOORD0;
  float4 projTexC : TEXCOORD1;

};

struct CubesVS_IN
{
  float3 posL      : POSITION_LOCAL;
  float3 normalL   : NORMAL;
  float4 diffuseColor  : diffuse;
  float4 specularColor : specular;
  row_major float4x4 gWorld  : Transform;
  int texIndex     :  TEXTURE_INDEX;
};

struct StandardVS_OUT
{
  float4 posH     : SV_POSITION;
  float3 posW     : POSITION_WORLD;
  float3 posL     : POSITION_LOCAL;
  float3 normalW  : NORMAL;
  float4 diffuse  : DIFFUSE;
  float4 spec     : SPECULAR;
  //int texIndex    : TEXTURE_INDEX;
  float3 texC     : TEXCOORD0;
  float4 projTexC : TEXCOORD1;
};

cbuffer cbRare
{
  float CUBE_SCALE;
  float CUBE_SCALE_INVERTED;
  //float3 globalOffset;
}

cbuffer cbPerFrame
{
  Light    gLight[MAX_LIGHTS];
  float3   gEyePosW;
  float    gTime;
  float    gWaveProgress;
  float    gSemicubesProgress;
  float4x4 gGlobalRotation;
  float4x4 gVP;
  float4x4 gLightWVP;
  int      gColored;
};

cbuffer cbPerObject
{
  float4x4 gWorld;
  float4   gClippingPlane;
  float4   gColorDiffuse;
  float4   gColorSpecular;
  float    gOpacity;
  float    gEdgeOpacity;
  float    gFaceOpacity;
};

struct ShadowVS_OUT
{
  float4 posH     : SV_POSITION;
  float3 posW     : POSITION;
  float3 posL     : POSITION_LOCAL;
  float3 texC     : TEXCOORD0;
};


Texture2D      gDiffuseMap;
Texture2DArray gTexBonuses;

#endif
