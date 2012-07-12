#ifndef SHADOWS_FX
#define SHADOWS_FX

#include "Definitions.fx"  


ShadowVS_OUT vsSMTextured(TexturedVS_IN vIn)
{
	ShadowVS_OUT vOut;
	
	// Transform to world space space.  
  vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld);
  // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vOut.posW, 1.0f), gLightWVP);
  vOut.posL = vIn.posL;
  vOut.texC = float3(vIn.texC, 0.0f);
  
  
  return vOut;
}


ShadowVS_OUT vsSMCubes(CubesVS_IN vIn)
{
	ShadowVS_OUT vOut;

  vOut.posL = vIn.posL;
	// Translate to world space space.
	vOut.posW    = mul(float4(vIn.posL, 1.0), vIn.gWorld);
  //Make wave effect
  vOut.posW.x += sin(TWO_PI * gWaveProgress) * sin(4 * vOut.posW.y)  * 0.2 * cos(abs(vOut.posW.x / 0.7)); //if makes slower (?)
  //vOut.posW.x += sin(gTime +  3 * vOut.posW.y) / 4;
  vOut.posW    = mul(float4(vOut.posW, 0.0f), gGlobalRotation);
  // Transform to homogeneous clip space.
	vOut.posH = mul(float4(vOut.posW, 1.0f), gLightWVP);
  // Output vertex attributes for interpolation across triangle.
  vOut.texC     = float3(vIn.posL.x * CUBE_SCALE_INVERTED / 2 + 0.5, -vIn.posL.y * CUBE_SCALE_INVERTED / 2 + 0.5, vIn.texIndex);	
	return vOut;
}

 
void psSMTextured(ShadowVS_OUT pIn)
{
  float4 diffuse  =       gDiffuseMap.Sample(gAnisotropicSamWrap, pIn.texC.xy);
  clip(diffuse.a - 0.15f);
}


void psSMCubes(ShadowVS_OUT pIn)
{}

void psSMSemicubes(ShadowVS_OUT pIn)
{
  float t = lerp(1.84,  0.3, sqrt(gSemicubesProgress)); //some magic constants
  clip(+ pow(abs(CUBE_SCALE_INVERTED * pIn.posL.x - t), 4)
       + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.y - t), 4) 
       + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.z - t), 4) - 3.3);
}

void psSMDisappearingLine(ShadowVS_OUT pIn)
{
  clip(dot(float4(pIn.posL, 1.0f), gClippingPlane));
}

void psSMFallingPiece(ShadowVS_OUT pIn)
{
  float3 distToGrid = ((pIn.posL * CUBE_SCALE_INVERTED + 11 - 0.1) % 2.0f - 1.8); //another VERY MAGIC constants
  float opacity = 1.0;
  if (dot(float4(pIn.posW, 1.0f), gClippingPlane) < MATH_EPS)
    if ((distToGrid.x > 0) || (distToGrid.y > 0) || (distToGrid.z > 0)) opacity = gOpacity;
  clip(opacity - 0.15f);
}

technique10 techSMCubes
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, vsSMCubes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psSMCubes() ) );
    }
}

technique10 techSMFallingPiece
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, vsSMCubes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psSMFallingPiece() ) );
    }
}

technique10 techSMDisappearingLine
{
  pass P0
  {
    SetRasterizerState(rsSemicubes);
    SetVertexShader( CompileShader( vs_4_0, vsSMCubes() ) );
    SetGeometryShader( NULL );
    SetPixelShader( CompileShader( ps_4_0, psSMDisappearingLine() ) );
  }
}

technique10 techSMSemicubes
{
    pass P0
    {
        SetRasterizerState(rsSemicubes);
        SetVertexShader( CompileShader( vs_4_0, vsSMCubes() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psSMSemicubes() ) );
    }
}

technique10 techSMTextured
{
    pass P0
    {
        
        SetVertexShader( CompileShader( vs_4_0, vsSMTextured() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, psSMTextured() ) );
    }
}



#endif