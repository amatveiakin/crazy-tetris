#include "DirectXConstants.h"

struct Light
{
	float4 pos;
	float4 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;
  int    lightType;  //0 - off, 1 - parallel, 2 - point, 3 - spot
  float  brightness;
};

struct SurfaceInfo
{
	float3 pos;
  float3 normal;
  float4 diffuse;
  float4 spec;
};

float DiscreetSpecFactor(float specFactor)
{
  
  if (specFactor >= 0.0f & specFactor < 0.1f) return 0.0f;
  if (specFactor >= 0.1f & specFactor < 0.8f) return 0.5f;  
  if (specFactor >= 0.8f & specFactor < 1.0f) return 0.8f;   
  return specFactor;
}

float DiscreetDiffuseFactor(float diffuseFactor)
{
  
  if (diffuseFactor < 0.0f)            return 0.1f;
  if (diffuseFactor >= 0.0f & diffuseFactor < 0.5f) return 0.3f;  
  if (diffuseFactor >= 0.5f & diffuseFactor < 1.0f) return 0.66f;   
  return diffuseFactor;
}


float3 ParallelLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
 
	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -normalize(L.dir);
	
	// Add the ambient term.
	litColor += v.diffuse * L.brightness * L.ambient ;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.pos);
		float3 R         = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);
					
		//diffuseFactor = DiscreetDiffuseFactor(diffuseFactor);
		//specFactor = DiscreetSpecFactor(specFactor); 
		
		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.brightness * L.diffuse;
		litColor += specFactor * v.spec * L.brightness * L.spec;
	}
	
	return litColor;
}

float3 PointLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = float3(0.0f, 0.0f, 0.0f);
	
	// The vector from the surface to the light.
	float3 lightVec = L.pos - v.pos;
		
	// The distance from surface to light.
	float d = length(lightVec);
	
	if( d > L.range ) return float3(0.0f, 0.0f, 0.0f);
		
	// Normalize the light vector.
	lightVec /= d; 
	
	// Add the ambient light term.
	litColor += v.diffuse * L.brightness * L.ambient;	
	
	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.
	
	float diffuseFactor = dot(lightVec, v.normal);
	[branch]
	if( diffuseFactor > 0.0f )
	{
		float specPower  = max(v.spec.a, 1.0f);
		float3 toEye     = normalize(eyePos - v.pos);
		float3 R         = reflect(-lightVec, v.normal);
		float specFactor = pow(max(dot(R, toEye), 0.0f), specPower);

		//diffuseFactor = DiscreetDiffuseFactor(diffuseFactor);
		//specFactor = DiscreetSpecFactor(specFactor); 
	
		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.brightness * L.diffuse;
		litColor += specFactor * v.spec * L.brightness * L.spec;
	}
	
	// attenuate
	return litColor / dot(L.att, float3(1.0f, d, d*d));
}

float3 Spotlight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = PointLight(v, L, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(L.pos - v.pos);
	
	float s = pow(max(dot(-lightVec, normalize(L.dir)), 0.0f), L.spotPower);
	
	// Scale color by spotlight factor.
	return litColor*s;
}

float3 SearchLight(SurfaceInfo v, Light L, float3 eyePos)
{
	float3 litColor = PointLight(v, L, eyePos);
	
	// The vector from the surface to the light.
	float3 lightVec = normalize(L.pos - v.pos);
	
	float angle = acos(dot(-lightVec, normalize(L.dir)));
  float s;
  if (angle < searchAlpha)
  {
    s = 1;
  } 
  else if (angle > searchBeta)
  {
    s = 0;
  } else
    s = cos (PI / 2 * (angle - searchAlpha)/(searchBeta-searchAlpha));

  return litColor*s;
}

float3 litColor(SurfaceInfo v, Light L, float3 eyePos)
{
  if (L.brightness > 0)
  {
    if (L.lightType == 1) // Parallel
    {
      return ParallelLight(v, L, eyePos);
    }
    else if (L.lightType == 2) // Point
    {
      return PointLight(v, L, eyePos);
    }
    else if (L.lightType == 3)// Spot
    {
      return Spotlight(v, L, eyePos);
    }  if (L.lightType == 4)// Searchlight
    {
      return 
        SearchLight(v, L, eyePos);
    }  
  }
  return float3(0.0f, 0.0f, 0.0f);
 }  
 