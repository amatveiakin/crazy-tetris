#include "DirectXConstants.h"
#include "Samplers.fx"

Texture2D gColorFilter;
Texture2D gShadowMap;



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
  float4 projTexC;
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

float ShadowFactor(float4 projTexC)
{
    // Complete projection by doing division by w.
    projTexC.xyz /= projTexC.w;
    // Points outside the light volume are in shadow.
    if( projTexC.x < -1.0f || projTexC.x > 1.0f ||
        projTexC.y < -1.0f || projTexC.y > 1.0f ||
        projTexC.z < 0.0f )
        return 0.0f;
    // Transform from NDC space to texture space.
    projTexC.x = +0.5f*projTexC.x + 0.5f;
    projTexC.y = -0.5f*projTexC.y + 0.5f;
    // Depth in NDC space.
    float depth = projTexC.z;
    // Sample shadow map to get nearest depth to light.
    float s0 = gShadowMap.Sample(gShadowSam,
          projTexC.xy).r;
    float s1 = gShadowMap.Sample(gShadowSam,
          projTexC.xy + float2(SMAP_DX, 0)).r;
    float s2 = gShadowMap.Sample(gShadowSam,
          projTexC.xy + float2(0, SMAP_DX)).r;
    float s3 = gShadowMap.Sample(gShadowSam,
          projTexC.xy + float2(SMAP_DX, SMAP_DX)).r;
    // Is the pixel depth <= shadow map value?
    float result0 = depth <= s0 + SHADOW_EPSILON;
    float result1 = depth <= s1 + SHADOW_EPSILON;
    float result2 = depth <= s2 + SHADOW_EPSILON;
    float result3 = depth <= s3 + SHADOW_EPSILON;
    // Transform to texel space.
    float2 texelPos = SMAP_SIZE*projTexC.xy;
    // Determine the interpolation amounts.
    float2 t = frac( texelPos );
    // Interpolate results.
    return lerp(lerp(result0, result1, t.x),
                lerp(result2, result3, t.x), t.y);
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
	
  //float4 projTexC = mul(float4(v.pos, 1.0f), gLightWVP);
  //projTexC.xyz /= projTexC.w;
  // Transform from NDC space to texture space.
  //projTexC.x = +0.5f*projTexC.x + 0.5f;
  //projTexC.y = -0.5f*projTexC.y + 0.5f;
  
  float s ;//= float4(1, 1, 1, 1);
  //s = gColorFilter.Sample(gAnisotropicSamBorder, projTexC.xy); 
  s = ShadowFactor(v.projTexC);
	float angle = acos(dot(-lightVec, normalize(L.dir)));
  
  if (angle < searchAlpha)
  {
    s *= 1;
  } 
  else if (angle > searchBeta)
  {
    s *= 0;
  } else
    s *= cos (PI / 2 * (angle - searchAlpha)/(searchBeta-searchAlpha));

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
 

