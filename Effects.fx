#include "Lighthelper.fx"
#include "DirectXConstants.h"
#include "Definitions.fx"
#include "Samplers.fx"
#include "States.fx"
#include "Shadows.fx"





TexturedVS_OUT vsTextured(TexturedVS_IN vIn)
{
    TexturedVS_OUT vOut;

    // Transform to world space space.
    vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld);
    vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);

    // Transform to homogeneous clip space.
    vOut.posH = mul(float4(vOut.posW, 1.0f), gVP);

    vOut.texC = vIn.texC;
    vOut.projTexC = mul(float4(vOut.posW, 1.0f), gLightWVP);
    return vOut;
}

float4 psTextured(TexturedVS_OUT pIn) : SV_Target
{
    //Interpolating normal can make it not be of unit length so normalize it.
    float4 diffuse = gDiffuseMap.Sample(gAnisotropicSamWrap, pIn.texC);
    float4 specular = 0.2 * gDiffuseMap.Sample(gAnisotropicSamWrap, pIn.texC);
    //During linear interpolation length of normal changed
    pIn.normalW = normalize(pIn.normalW);
    //move to psTransparent?
    if (diffuse.a < 1.0f - MATH_EPS) //if makes faster (?)
    {
        //rendering transparent or sliced object, so normal may have wrong direction
        if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW = -pIn.normalW;
    }

    SurfaceInfo v = { pIn.posW, pIn.normalW, diffuse, specular, pIn.projTexC };
    float3 litCol = float3(0., 0., 0.);

        for (int i = 0; i < MAX_LIGHTS; ++i)
            litCol += litColor(v, gLight[i], gEyePosW);

    return float4(litCol, diffuse.a);

}


StandardVS_OUT vsCubes(CubesVS_IN vIn)
{
    StandardVS_OUT vOut;

    vOut.posL = vIn.posL;
    // Translate to world space space.
    vOut.posW = mul(float4(vIn.posL, 1.0), vIn.gWorld);
    //Make wave effect
    vOut.posW.x += sin(TWO_PI * gWaveProgress) * sin(4 * vOut.posW.y)  * 0.2 * cos(abs(vOut.posW.x / 0.7)); //if makes slower (?)
    //vOut.posW.x += sin(gTime +  3 * vOut.posW.y) / 4;
    vOut.posW = mul(float4(vOut.posW, 0.0f), gGlobalRotation);
    //vOut.normalW = mul(float4(vIn.normalL, 0.0f), vIn.gWorld);
    vOut.normalW = mul(float4(vIn.normalL, 0.0f), gGlobalRotation);
    // Transform to homogeneous clip space.
    vOut.posH = mul(float4(vOut.posW, 1.0f), gVP);
    // Output vertex attributes for interpolation across triangle.
    vOut.diffuse = vIn.diffuseColor;
    vOut.spec = vIn.specularColor;
    vOut.projTexC = mul(float4(vOut.posW, 1.0f), gLightWVP);
    vOut.texC = float3(vIn.posL.x * CUBE_SCALE_INVERTED / 2 + 0.5, -vIn.posL.y * CUBE_SCALE_INVERTED / 2 + 0.5, vIn.texIndex);
    return vOut;
}


float4 psCubes(StandardVS_OUT pIn) : SV_Target
{
    //Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);
    //move to psTransparent?
    if (pIn.diffuse.a < 1.0f - MATH_EPS) //if makes faster (?)
    {
        //rendering transparent or sliced object, so normal may have wrong direction
        if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW = -pIn.normalW;
    }
    //float3 texC = float3(pIn.posL.x * CUBE_SCALE_INVERTED / 2 + 0.5, -pIn.posL.y * CUBE_SCALE_INVERTED / 2 + 0.5, pIn.texIndex);
    float4 diffuseTex = gTexBonuses.Sample(gAnisotropicSamBorder, pIn.texC);
        SurfaceInfo v = { pIn.posW, pIn.normalW,
        (1 - diffuseTex.a) * pIn.diffuse + diffuseTex.a * diffuseTex,
        (1 - diffuseTex.a) * pIn.spec + diffuseTex.a * (diffuseTex + float4(1.0, 1.0, 1.0, 1.0)) / 2,
        pIn.projTexC };

    float3 litCol = float3(0., 0., 0.);

        for (int i = 0; i < MAX_LIGHTS; ++i)
            litCol += litColor(v, gLight[i], gEyePosW);

    return float4(litCol, pIn.diffuse.a);
}


//Semicubes section





float4 psSemicubes(StandardVS_OUT pIn) : SV_Target
{
    float t = lerp(1.84, 0.3, sqrt(gSemicubesProgress)); //some magic constants
    clip(+pow(abs(CUBE_SCALE_INVERTED * pIn.posL.x - t), 4)
        + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.y - t), 4)
        + pow(abs(CUBE_SCALE_INVERTED * pIn.posL.z - t), 4) - 3.3);

    //Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);

    if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW = -pIn.normalW;

    SurfaceInfo v = { pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec, pIn.projTexC };
    float3 litCol = float3(0., 0., 0.);

        for (int i = 0; i < MAX_LIGHTS; ++i)
            litCol += litColor(v, gLight[i], gEyePosW);

    return float4(litCol, pIn.diffuse.a);
}

float4 psDisappearingLine(StandardVS_OUT pIn) : SV_Target
{
    clip(dot(float4(pIn.posL, 1.0f), gClippingPlane));

    //Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);

    if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW = -pIn.normalW;

    SurfaceInfo v = { pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec, pIn.projTexC };
    float3 litCol = float3(0., 0., 0.);

        for (int i = 0; i < MAX_LIGHTS; ++i)
            litCol += litColor(v, gLight[i], gEyePosW);

    return float4(litCol, pIn.diffuse.a);
    //return float4(1., 1., 1., 1.);
}

float4 psFallingPiece(StandardVS_OUT pIn) : SV_Target
{
    float3 distToGrid = ((pIn.posL * CUBE_SCALE_INVERTED + 11 - 0.1) % 2.0f - 1.8); //another VERY MAGIC constants
    //float3 distToGrid = ((pIn.posL * CUBE_SCALE_INVERTED +2 ) % 2.0f - 1.8);
    //clip(-min(max(max(distToGrid.x, distToGrid.y), distToGrid.z), -dot(float4(pIn.posW, 1.0f), gClippingPlane)));
    float opacity = gEdgeOpacity;
    if (dot(float4(pIn.posW, 1.0f), gClippingPlane) < MATH_EPS)
        if ((distToGrid.x > 0) || (distToGrid.y > 0) || (distToGrid.z > 0)) opacity = gFaceOpacity;
    clip(opacity - MATH_EPS);

    //Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);

    if (dot(float4(pIn.normalW, 0), float4(gEyePosW - pIn.posW, 0)) < 0)  pIn.normalW = -pIn.normalW;

    SurfaceInfo v = { pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec, pIn.projTexC };
    float3 litCol = float3(0., 0., 0.);

        for (int i = 0; i < MAX_LIGHTS; ++i)
            litCol += litColor(v, gLight[i], gEyePosW);

    return float4(litCol, opacity * pIn.diffuse.a);


    //return float4(1., 1., 1., 1.);
}

technique10 techCubes
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, vsCubes()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psCubes()));
    }
}

technique10 techFallingPiece
{
    pass P0
    {
        SetRasterizerState(rsCullFront);
        SetBlendState(bsTransparent, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, vsCubes()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psFallingPiece()));
    }
    pass P1
    {
        SetRasterizerState(rsCullBack);
        SetBlendState(bsTransparent, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, vsCubes()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psFallingPiece()));
    }
}

technique10 techDisappearingLine
{
    pass P0
    {
        SetRasterizerState(rsSemicubes);
        SetVertexShader(CompileShader(vs_4_0, vsCubes()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psDisappearingLine()));
    }
}

technique10 techSemicubes
{
    pass P0
    {
        SetRasterizerState(rsSemicubes);
        SetVertexShader(CompileShader(vs_4_0, vsCubes()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psSemicubes()));
    }
}

technique10 techTextured
{
    pass P0
    {

        SetVertexShader(CompileShader(vs_4_0, vsTextured()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, psTextured()));
    }
}
