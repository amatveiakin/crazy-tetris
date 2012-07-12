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
	float3 posL      : POSITION;
	float3 normalL   : NORMAL;
  float4 diffuseColor  : diffuse;
	float4 specularColor : specular;
  row_major float4x4 gWorld  : Transform;
  int texIndex     :  TEXTURE_INDEX;
};

struct StandardVS_OUT
{
	float4 posH    : SV_POSITION;
  float3 posW    : POSITION;
  float3 posL    : POSITION_LOCAL;
  float3 normalW : NORMAL;
  float4 diffuse : DIFFUSE;
  float4 spec    : SPECULAR;
  int texIndex   :  TEXTURE_INDEX;
};

#endif