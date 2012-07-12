#ifndef DEFINITIONS_FX
#define DEFINITIONS_FX

struct UncoloredVS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	//float4 diffuse : DIFFUSE;
	//float4 spec    : SPECULAR;
};

struct TexturedVS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
};


struct TexturedVS_OUT
{
	float4 posH    : SV_POSITION;
  float3 posW    : POSITION;
  //float3 posL    : POSITION_LOCAL;
  float3 normalW : NORMAL;
	float2 texC    : TEXCOORD;
};

struct CubesVS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
  float3 offset  : offset;  
  float4 diffuseColor  : diffuse;
	float4 specularColor : specular;
};

struct StandardVS_OUT
{
	float4 posH    : SV_POSITION;
  float3 posW    : POSITION;
  float3 posL    : POSITION_LOCAL;
  float3 normalW : NORMAL;
  float4 diffuse : DIFFUSE;
  float4 spec    : SPECULAR;
};

#endif