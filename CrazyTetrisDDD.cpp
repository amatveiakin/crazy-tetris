#include <cstdlib>
#include <fstream>
#include "d3dApp/d3dApp.h"
#include "d3dApp/DrawableTex2D.h"
#include "Primitives.h"
#include "Light.h"
#include "Vertex.h"
#include "VisualEffects.h"
#include "Engine.h"
#include "Declarations.h"
#include "DirectXConstants.h"

//Move to Hud.h?

const float MAX_WORLD_FIELD_WIDTH  = 2.0f;
const float MAX_WORLD_FIELD_HEIGHT = 4.0f;
const float CUBE_SCALE = min (MAX_WORLD_FIELD_WIDTH / (float) FIELD_WIDTH, MAX_WORLD_FIELD_HEIGHT / (float) FIELD_HEIGHT);
const float FIELD_INDENT_LEFT = 0.0f;
const float FIELD_INDENT_RIGHT = 0.00f;
const float FIELD_INDENT_BOTTOM = 2 * CUBE_SCALE;
const float FIELD_INDENT_TOP = 4 * CUBE_SCALE;
const float HUD_HEIGHT = 2 * CUBE_SCALE;
const float VP_WORLD_WIDTH  = FIELD_INDENT_LEFT + MAX_WORLD_FIELD_WIDTH + FIELD_INDENT_RIGHT;
const float VP_WORLD_HEIGHT = HUD_HEIGHT + FIELD_INDENT_BOTTOM + MAX_WORLD_FIELD_HEIGHT + FIELD_INDENT_TOP;
const float ANGLE_FOV_Y = 0.25f * PI; //Now you CAN change it!
const float SEARCHLIGHT_RADIUS = 5 * CUBE_SCALE;
const float DIST_TO_SEARCHLIGHT = SEARCHLIGHT_RADIUS / tan(searchBeta);
const float EYE_TO_FIELD = 0.5f * VP_WORLD_HEIGHT / tan(ANGLE_FOV_Y / 2.0f);

const int   LYING_BLOCKS_INSTANCES_OFFSET      = 0;
const int   DISAPPEARING_LINE_INSTANCES_OFFSET = LYING_BLOCKS_INSTANCES_OFFSET + MAX_BLOCKS;
const int   FALLING_PIECE_INSTANCES_OFFSET     = DISAPPEARING_LINE_INSTANCES_OFFSET + MAX_BLOCKS_IN_PIECE * FIELD_WIDTH;
const int   HINT_INSTANCES_OFFSET              = FALLING_PIECE_INSTANCES_OFFSET + MAX_BLOCKS_IN_PIECE;
const int   MAX_CUBE_INSTANCES                 = HINT_INSTANCES_OFFSET + MAX_BLOCKS_IN_PIECE;

//переделать в класс, добавить шлюх и блекджек
struct PlayerScreen
{
  D3D10_VIEWPORT viewport;
  RECT rectName;
  RECT rectVictim;
};

bool keyPressed(KeyboardKey key)
{
  return (GetAsyncKeyState(key) & 0x8000) != 0;
}

class CrazyTetrisApp : public D3DApp
{
public:
  CrazyTetrisApp(HINSTANCE hInstance);
  ~CrazyTetrisApp();

  void initApp();
  void onResize();
  void updateScene(float dt);
  void drawScene();
  float fieldToWorldX(float fieldX);
  float fieldToWorldY(float fieldY);

private:
  void drawPlayer(Player* player, PlayerScreen screen);
  void drawPlayerToShadowMap(Player* player);
  void drawLyingBlocks(Player* player, bool colored);
  void drawFallingPiece(Player* player, bool colored);
  void drawHint(Player* player);
  void drawDisappearingLines(Player* player, bool colored);
  void drawWall(Player* player, bool colored);

  void loadPlayerData(Player* player);
  void loadCubeInstances(Player* player);
  void installLights(Player* player, bool isLanternOn);

  void buildFX();
  void buildVertexLayouts();
  void buildBuffers();
  void buildLights();
  void buildRasterizerStates();
  void buildBlendingStates();
  void buildTextures();
  void buildViewports();
  void setConstantBuffers();

private:

  Box mBox;
  TexturedQuad mWall;
  TexturedQuad mAim;
  Glass mGlass;
  Game mGame;
  CubeInstance* cubeInstances;
  DrawableTex2D mShadowMap;

  //
  ID3DX10Font* interfaceFont;
  //Vertex layouts
  ID3D10InputLayout* cubesVertexLayout;
  ID3D10InputLayout* texturedVertexLayout;
  ID3D10InputLayout* uncoloredVertexLayout;


  //Effects and techniques
  ID3D10Effect* FX;
  ID3D10EffectTechnique* techCubes;
  ID3D10EffectTechnique* techSemicubes;
  ID3D10EffectTechnique* techTextured;
  ID3D10EffectTechnique* techDisappearingLine;
  ID3D10EffectTechnique* techGlass;
  ID3D10EffectTechnique* techFallingPiece;

  ID3D10EffectTechnique* techSMCubes;
  ID3D10EffectTechnique* techSMSemicubes;
  ID3D10EffectTechnique* techSMTextured;
  ID3D10EffectTechnique* techSMDisappearingLine;
  ID3D10EffectTechnique* techSMFallingPiece;

  //Shader variables
  ID3D10EffectMatrixVariable* fxVPVar;
  ID3D10EffectMatrixVariable* fxWorldVar;
  ID3D10EffectMatrixVariable* fxGlobalRotationVar;
  ID3D10EffectMatrixVariable* fxLightWVPVar;

  ID3D10EffectScalarVariable* fxTimeVar;
  ID3D10EffectScalarVariable* fxLightTypeVar;
  ID3D10EffectScalarVariable* fxWaveProgressVar;
  ID3D10EffectScalarVariable* fxSemicubesProgressVar;
  ID3D10EffectScalarVariable* fxImplodeProgressVar;
  ID3D10EffectScalarVariable* fxOpacityVar;
  ID3D10EffectScalarVariable* fxColoredVar;
  ID3D10EffectScalarVariable* fxEdgeOpacityVar;
  ID3D10EffectScalarVariable* fxFaceOpacityVar;
  ID3D10EffectScalarVariable* fxCUBE_SCALE;
  ID3D10EffectScalarVariable* fxCUBE_SCALE_INVERTED;

  ID3D10EffectVariable*       fxLightVar;
  ID3D10EffectVariable*       fxEyePosVar;
  ID3D10EffectVariable*       fxColorDiffuseVar;
  ID3D10EffectVariable*       fxColorSpecularVar;
  ID3D10EffectVariable*       fxClippingPlaneVar;

  ID3D10EffectShaderResourceVariable* fxDiffuseMapVar;
  ID3D10EffectShaderResourceVariable* fxColorFilterVar;
  ID3D10EffectShaderResourceVariable* fxTexBonusesVar;
  ID3D10EffectShaderResourceVariable* fxShadowMapVar;

  //Rasterizer state - clean up
  ID3D10RasterizerState* rsSolidCullFront;
  ID3D10RasterizerState* rsSolidCullBack;
  ID3D10RasterizerState* rsSolidCullNone;
  ID3D10RasterizerState* rsForSemicubes;


  //Textures and texture views
  vector<ID3D10ShaderResourceView*> texBackWallRV;

  ID3D10ShaderResourceView* texAimRV;
  ID3D10ShaderResourceView* texSearchLightColorFilterRV;
  ID3D10ShaderResourceView* texBonusesRV;
  ID3D10ShaderResourceView* texBonusesArrayRV;

  ID3D10Buffer* cubeInstancesBuffer;

  ID3D10BlendState* transparentBS;

  D3D10_VIEWPORT vpScreen;
  D3D10_VIEWPORT vpShadow;
  PlayerScreen scrPlayers[MAX_PLAYERS];

  D3DXMATRIX mWorld;
  D3DXMATRIX mView;
  D3DXMATRIX mProj;
  D3DXMATRIX mLightWVP;
  D3DXMATRIX mVP;
  D3DXMATRIX mGlobalRotation;

  Light Lights[MAX_LIGHTS];

  D3DXVECTOR3 eyePosW;


  float currTime;
  float mPhi, mTheta;
  //float DistToLight;

  //const float CUBE_SCALE;

  const float vpAspect;

};

CrazyTetrisApp::CrazyTetrisApp(HINSTANCE hInstance)
: D3DApp(hInstance), FX(0), techCubes(0), techSemicubes(0),
  fxVPVar(0), currTime(0.f), mPhi(PI / 2.f), mTheta(0.f),

  vpAspect(VP_WORLD_WIDTH / VP_WORLD_HEIGHT)

{
  D3DXMatrixIdentity(&mWorld);
  D3DXMatrixIdentity(&mView);
  D3DXMatrixIdentity(&mProj);
  D3DXMatrixIdentity(&mVP);
  D3DXMatrixIdentity(&mGlobalRotation);
  srand(32);

}

CrazyTetrisApp::~CrazyTetrisApp()
{
  if( md3dDevice )
    md3dDevice->ClearState();

  ReleaseCOM(FX);

  ReleaseCOM(rsSolidCullFront);
  ReleaseCOM(rsSolidCullBack);
  ReleaseCOM(rsSolidCullNone);
  ReleaseCOM(rsForSemicubes);
  ReleaseCOM(transparentBS);
  ReleaseCOM(cubeInstancesBuffer);

  ReleaseCOM(cubesVertexLayout);
  ReleaseCOM(texturedVertexLayout);
  //ReleaseCOM(uncoloredVertexLayout);

  for (size_t i = 0; i < texBackWallRV.size(); ++i) ReleaseCOM(texBackWallRV[i]);
  //ReleaseCOM(texSearchLightColorFilterRV);
}

void CrazyTetrisApp::initApp()
{
  srand(GetTickCount());

  mGame.init();
  mGame.newMatch();


  D3DApp::initApp();

  mBox.init(md3dDevice, CUBE_SCALE / 2.0f + MATH_EPS, 0.5f, 5);
  mWall.init(md3dDevice,
             CUBE_SCALE * FIELD_WIDTH  * (1.0f + CUBE_SCALE * (FIELD_HEIGHT / 2.0f + 0.5f) / EYE_TO_FIELD),
             CUBE_SCALE * (FIELD_HEIGHT) * (1.0f + CUBE_SCALE * (FIELD_HEIGHT / 2.0f + 0.5f) / EYE_TO_FIELD),
             1.0f,  1.0f);
  mAim.init(md3dDevice, CUBE_SCALE, CUBE_SCALE, 1.0f, 1.0f);
  mGlass.init(md3dDevice, CUBE_SCALE * FIELD_WIDTH, CUBE_SCALE * FIELD_HEIGHT, CUBE_SCALE);
  mShadowMap.init(md3dDevice, (int) SMAP_SIZE, (int) SMAP_SIZE, false, DXGI_FORMAT_R32G32B32A32_FLOAT); //change to false!

  buildFX();
  buildTextures();
  setConstantBuffers();

  buildVertexLayouts();
  buildLights();
  buildRasterizerStates();
  buildBlendingStates();
  buildBuffers();

 // for test purposes

  mGame.newRound(0);
  buildViewports();
  mGame.saveSettings();

}


void CrazyTetrisApp::updateScene(float dt)
{
  D3DApp::updateScene(dt);

  currTime = mTimer.getGameTime();
  mGame.onTimer(currTime);



  // Update angles based on input to orbit camera around box.
  if ((GetAsyncKeyState('A') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000)) mTheta -= 2.0f*dt;
  if ((GetAsyncKeyState('D') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000))  mTheta += 2.0f*dt;
  if ((GetAsyncKeyState('W') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000))  mPhi -= 2.0f*dt;
  if ((GetAsyncKeyState('S') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000))  mPhi += 2.0f*dt;
  //if ((GetAsyncKeyState('Q') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000))  DistToLight -= 2.0f*dt;
  //if ((GetAsyncKeyState('E') & 0x8000)  && (GetAsyncKeyState(VK_LSHIFT) & 0x8000))  DistToLight += 2.0f*dt;

  // Restrict the angle mPhi.
  if( mPhi < 0.1f )  mPhi = 0.1f;
  if( mPhi > PI-0.1f)  mPhi = PI-0.1f;

  if(GetAsyncKeyState('R') & 0x8000)
  {
    mPhi = PI / 2;
    mTheta = 0;
  }

  if (GetAsyncKeyState(VK_F1) & 0x8000) mGame.players[2].visualEffects.fieldCleaning.enable(3.0f);
  //if(GetAsyncKeyState(VK_F2) & 0x8000)  mGame.players[2].visualEffects.fieldCleaning.disable();

  if(GetAsyncKeyState(VK_F3) & 0x8000)  mGame.players[2].visualEffects.rotatingField.enable(10.0f);
  if(GetAsyncKeyState(VK_F4) & 0x8000)  mGame.players[2].visualEffects.rotatingField.disable();

  if(GetAsyncKeyState(VK_F5) & 0x8000)  mGame.players[2].visualEffects.wave.enable(2.0f);
  if(GetAsyncKeyState(VK_F6) & 0x8000)  mGame.players[2].visualEffects.wave.disable();

  if(GetAsyncKeyState(VK_F7) & 0x8000)  mGame.players[2].visualEffects.flippedScreen.enable(1.f);
  if(GetAsyncKeyState(VK_F8) & 0x8000)  mGame.players[2].visualEffects.flippedScreen.disable();

  if(GetAsyncKeyState('1') & 0x8000)   mGame.players[1].visualEffects.lantern.enable(2.0f);
  if(GetAsyncKeyState('2')& 0x8000)     mGame.players[1].visualEffects.lantern.disable();

  if(GetAsyncKeyState('3') & 0x8000)   mGame.players[2].visualEffects.lantern.enable(2.0f);
  if(GetAsyncKeyState('4')& 0x8000)     mGame.players[2].visualEffects.lantern.disable();


  // Convert Spherical to Cartesian coordinates: mPhi measured from +y
  // and mTheta measured counterclockwise from -z.
  eyePosW.x =  (EYE_TO_FIELD + CUBE_SCALE / 2)*sinf(mPhi)*sinf(mTheta) + (FIELD_INDENT_RIGHT - FIELD_INDENT_LEFT) / 2.f;
  eyePosW.y =  (EYE_TO_FIELD + CUBE_SCALE / 2)*cosf(mPhi);
  eyePosW.z = -(EYE_TO_FIELD + CUBE_SCALE / 2)*sinf(mPhi)*cosf(mTheta) + (FIELD_INDENT_TOP - (HUD_HEIGHT + FIELD_INDENT_BOTTOM)) / 2.f;


  D3DXVECTOR3 target((FIELD_INDENT_RIGHT - FIELD_INDENT_LEFT) / 2.f, 0.f,(FIELD_INDENT_TOP - (HUD_HEIGHT + FIELD_INDENT_BOTTOM)) / 2.f);
  D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
  D3DXMatrixLookAtLH(&mView, &eyePosW, &target, &up);

}

void CrazyTetrisApp::drawScene()
{
  D3DApp::drawScene(); //Cleaning screen
  //Getting current time once

  currTime = mTimer.getGameTime();

  for (size_t i = 0; i < mGame.participants.size(); ++i)
    if (mGame.participants[i]->active)
      drawPlayer(mGame.participants[i], scrPlayers[i]);

  md3dDevice->RSSetViewports(1, &vpScreen);
  md3dDevice->RSSetState(0);
  // We specify DT_NOCLIP, so we do not care about width/height of the rect.
  RECT R = {5, 5, 0, 0};
  mFont->DrawText(0, mFrameStats.c_str() , -1, &R, DT_NOCLIP, WHITE);
  //mFrameStats.c_str()

  mSwapChain->Present(0, 0);
}

void CrazyTetrisApp::drawPlayer(Player* player, PlayerScreen screen)
{


  D3DXVECTOR4 clippingPlane;

  loadPlayerData(player);
  loadCubeInstances(player);
  installLights(player, true);

  md3dDevice->OMSetDepthStencilState(0, 0);
  float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
  //Reseting states
  md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
  //md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  //md3dDevice->RSSetState(mSolidCullBack);

  drawPlayerToShadowMap(player);

  mVP = mView * mProj;
  fxVPVar->SetMatrix((float*)&mVP);

  fxShadowMapVar->SetResource(mShadowMap.depthMap());
  //fxDiffuseMapVar->SetResource(mShadowMap.depthMap());
  md3dDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
  md3dDevice->RSSetViewports(1, &screen.viewport);

  //рисуем непрозрачные объекты
  md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
  //рисуем поле
  installLights(player, true);
  //Рисуем стенку
  drawWall(player, true);
  //рисуем всякие разные кубики
  md3dDevice->IASetInputLayout(cubesVertexLayout);
  mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
  //рисуем (полу)кубики, которые уже упали
  drawLyingBlocks(player, true);
  //рисуем убираемые линии
  drawDisappearingLines(player, true);
  //mGlass.draw();
  //упорядочиваем по удаленности и рисуем прозрачные объекты
  md3dDevice->OMSetBlendState(transparentBS, blendFactors, 0xffffffff);
  //рисуем падающую фигуру
  drawFallingPiece(player, true);
  //рисуем интерфейс
  installLights(player, false);
  //bla-bla
  drawHint(player);
  interfaceFont->DrawText(0, player->account()->name.c_str() , -1, &screen.rectName, DT_NOCLIP | DT_RIGHT, WHITE);
  interfaceFont->DrawText(0, player->victimName().c_str() , -1, &screen.rectVictim, DT_NOCLIP, RED);
  D3DXMATRIX temp;
  fxWorldVar->SetMatrix((float*) D3DXMatrixTranslation(&temp, CUBE_SCALE / 2, (- VP_WORLD_HEIGHT + HUD_HEIGHT + CUBE_SCALE) / 2 , - CUBE_SCALE / 2));
  fxDiffuseMapVar->SetResource(texAimRV);
  md3dDevice->IASetInputLayout(texturedVertexLayout);
  techTextured->GetPassByIndex( 0 )->Apply(0);
  mAim.draw();
  fxShadowMapVar->SetResource(0);
  techTextured->GetPassByIndex( 0 )->Apply(0); //to actually refresh gShadowMap
}

void CrazyTetrisApp::drawPlayerToShadowMap(Player* player)
{
  D3DXVECTOR4 clippingPlane;

  mShadowMap.begin();

  md3dDevice->RSSetViewports(1, &vpShadow);
  md3dDevice->RSSetState(0);
  fxVPVar->SetMatrix((float*)&mLightWVP);
  //fxColoredVar->SetInt(0);

  float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

  //md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
  //drawWall(player, false);
  md3dDevice->IASetInputLayout(cubesVertexLayout);
  mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
  drawLyingBlocks(player, false);
  drawDisappearingLines(player, false);

  //md3dDevice->OMSetBlendState(transparentBS, blendFactors, 0xffffffff);
  drawFallingPiece(player, false);
  mShadowMap.end();
}
void CrazyTetrisApp::buildFX()
{
  DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
  shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

  ID3D10Blob* compilationErrors = 0;
  HRESULT hr = 0;
  hr = D3DX10CreateEffectFromFile(L"effects.fxo", 0, 0,
    "fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &FX, &compilationErrors, 0);

  if (hr == D3D10_ERROR_FILE_NOT_FOUND)
    hr = D3DX10CreateEffectFromFile(L"effects/effects.fx", 0, 0,
      "fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &FX, &compilationErrors, 0);

  if(FAILED(hr))
  {
    if( compilationErrors )
    {
      MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
      ReleaseCOM(compilationErrors);
    }
    DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
  }

  //Techniques
  techCubes             = FX->GetTechniqueByName("techCubes");
  techSemicubes         = FX->GetTechniqueByName("techSemicubes");
  techTextured          = FX->GetTechniqueByName("techTextured");
  techDisappearingLine  = FX->GetTechniqueByName("techDisappearingLine");
  techGlass             = FX->GetTechniqueByName("techGlass");
  techFallingPiece      = FX->GetTechniqueByName("techFallingPiece");

  techSMCubes             = FX->GetTechniqueByName("techSMCubes");
  techSMSemicubes         = FX->GetTechniqueByName("techSMSemicubes");
  techSMTextured          = FX->GetTechniqueByName("techSMTextured");
  techSMDisappearingLine  = FX->GetTechniqueByName("techSMDisappearingLine");
  techSMFallingPiece      = FX->GetTechniqueByName("techSMFallingPiece");

  //Constants
  fxCUBE_SCALE = FX->GetVariableByName("CUBE_SCALE")->AsScalar();
  fxCUBE_SCALE_INVERTED = FX->GetVariableByName("CUBE_SCALE_INVERTED")->AsScalar();

  //Matrix variables
  fxWorldVar = FX->GetVariableByName("gWorld")->AsMatrix();
  fxVPVar = FX->GetVariableByName("gVP")->AsMatrix();
  fxGlobalRotationVar = FX->GetVariableByName("gGlobalRotation")->AsMatrix();
  fxLightWVPVar = FX->GetVariableByName("gLightWVP")->AsMatrix();

  //Lights and camera position
  fxLightVar = FX->GetVariableByName("gLight");
  fxEyePosVar = FX->GetVariableByName("gEyePosW");
  //fxGlobalOffsetVar = FX->GetVariableByName("globalOffset");

  //Colors
  fxColorDiffuseVar  = FX->GetVariableByName("gColorDiffuse");
  fxColorSpecularVar = FX->GetVariableByName("gColorSpecular");
  fxOpacityVar =       FX->GetVariableByName("gOpacity")->AsScalar();
  fxEdgeOpacityVar =   FX->GetVariableByName("gEdgeOpacity")->AsScalar();
  fxFaceOpacityVar =   FX->GetVariableByName("gFaceOpacity")->AsScalar();
  fxColoredVar =       FX->GetVariableByName("gColored")->AsScalar();

  //Current time
  fxTimeVar = FX->GetVariableByName("gTime")->AsScalar();

  //Textures
  fxDiffuseMapVar  = FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
  fxColorFilterVar = FX->GetVariableByName("gColorFilter")->AsShaderResource();
  fxTexBonusesVar  = FX->GetVariableByName("gTexBonuses")->AsShaderResource();
  fxShadowMapVar   = FX->GetVariableByName("gShadowMap")->AsShaderResource();

  //Visual effect's progress
  fxWaveProgressVar      = FX->GetVariableByName("gWaveProgress")->AsScalar();
  fxSemicubesProgressVar = FX->GetVariableByName("gSemicubesProgress")->AsScalar();
  fxImplodeProgressVar   = FX->GetVariableByName("gImplodeProgress")->AsScalar();

  fxClippingPlaneVar     = FX->GetVariableByName("gClippingPlane");
}

void CrazyTetrisApp::setConstantBuffers()
{
  fxCUBE_SCALE->SetFloat(CUBE_SCALE);
  fxCUBE_SCALE_INVERTED->SetFloat(2.0f / CUBE_SCALE);
  //fxColorFilterVar->SetResource(texSearchLightColorFilterRV);
  //fxShadowMapVar->SetResource(texSearchLightColorFilterRV);
  fxTexBonusesVar->SetResource(texBonusesRV);

  D3DXCOLOR diffuse  = 5.f * BLUE;
  D3DXCOLOR specular = 5.f * (BLUE * 0.5f + WHITE * 0.5f);

  fxColorDiffuseVar->SetRawValue(&diffuse, 0, sizeof(D3DXCOLOR));
  fxColorDiffuseVar->SetRawValue(&specular, 0, sizeof(D3DXCOLOR));
}

void CrazyTetrisApp::buildVertexLayouts()
{
  int numElements;
  D3D10_PASS_DESC passDesc;

  // Create the vertex input layout for cubes (instanced!)
  D3D10_INPUT_ELEMENT_DESC cubesVertexDesc[] =
  {
    {"POSITION_LOCAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D10_INPUT_PER_VERTEX_DATA,   0},
    {"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA,   0},

    {"diffuse",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"specular",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 2 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",    1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 3 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",    2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",    3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 5 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"TEXTURE_INDEX",0, DXGI_FORMAT_R32_SINT           , 1, 6 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1}

  };
  numElements = sizeof(cubesVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
  techCubes->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(cubesVertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &cubesVertexLayout));

  D3D10_INPUT_ELEMENT_DESC texturedVertexDesc[] =
  {
    {"POSITION_LOCAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
  };
  numElements = sizeof(texturedVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
  techTextured->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(texturedVertexDesc, numElements, passDesc.pIAInputSignature,
  passDesc.IAInputSignatureSize, &texturedVertexLayout));

  /*D3D10_INPUT_ELEMENT_DESC uncoloredVertexDesc[] =
  {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
  };
  numElements = sizeof(uncoloredVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
  techGlass->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(uncoloredVertexDesc, numElements, passDesc.pIAInputSignature,
  passDesc.IAInputSignatureSize, &uncoloredVertexLayout));*/

}
void CrazyTetrisApp::buildRasterizerStates()
{
  D3D10_RASTERIZER_DESC rsDesc;

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
  rsDesc.FillMode = D3D10_FILL_SOLID;
  rsDesc.CullMode = D3D10_CULL_FRONT;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &rsSolidCullFront));

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
  rsDesc.FillMode = D3D10_FILL_SOLID;
  rsDesc.CullMode = D3D10_CULL_BACK;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &rsSolidCullBack));

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
  rsDesc.FillMode = D3D10_FILL_SOLID;
  rsDesc.CullMode = D3D10_CULL_NONE;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &rsSolidCullNone));

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
  rsDesc.FillMode = D3D10_FILL_SOLID;
  rsDesc.CullMode = D3D10_CULL_NONE;
  rsDesc.MultisampleEnable = true;  //иначе будут артефакты
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &rsForSemicubes));

}

void CrazyTetrisApp::buildLights()
{
  int i;
  for (i = 0; i < 4; i++)
  {
    Lights[i].ambient   = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
    Lights[i].diffuse   = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
    Lights[i].specular  = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
  }

  //Setting parallel light
  Lights[0].dir       = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 0.0f);
  Lights[0].lightType = 1;
  Lights[0].brightness = 1.0f;
  //Lights[0].active = 0;

  //Spots for lantern effect
  Lights[1].dir = D3DXVECTOR4(0.0f, 0.0f,  1.0f, 0.0f);
  Lights[1].pos = D3DXVECTOR4(0.0f, 0.0f, -1.0f, 1.0f);
  Lights[1].att.x = 1;
  Lights[1].spotPow = 128.f;
  Lights[1].range = 100;
  Lights[1].lightType = 4;
  Lights[1].brightness = 0.0f;

  Lights[2].dir = D3DXVECTOR4(0.0f, 0.0f, -1.0f, 0.0f);
  Lights[2].pos = D3DXVECTOR4(0.0f, 0.0f,  1.0f, 1.0f);
  Lights[2].att.x = 1;
  Lights[2].spotPow = 128.f;
  Lights[2].range = 100;
  Lights[2].lightType = 4;
  Lights[2].brightness = 0.0f;

  Lights[3].lightType = 2;
  Lights[3].range = 5 * CUBE_SCALE;
  Lights[3].att.x = 1;
  Lights[3].brightness = 0;



}

void CrazyTetrisApp::buildBlendingStates()
{
  D3D10_BLEND_DESC blendDesc = {0};
  blendDesc.AlphaToCoverageEnable = false;
  blendDesc.BlendEnable[0] = true;
  blendDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
  blendDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
  blendDesc.BlendOp = D3D10_BLEND_OP_ADD;
  blendDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
  blendDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
  blendDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
  blendDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

  HR(md3dDevice->CreateBlendState(&blendDesc, &transparentBS));
}


void CrazyTetrisApp::onResize()
{
  D3DApp::onResize();

  buildViewports();

  D3DXMatrixPerspectiveFovLH(&mProj, ANGLE_FOV_Y, vpAspect, 1.0f, 100.0f); //changedepth
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
           PSTR cmdLine, int showCmd)
{
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


  CrazyTetrisApp theApp(hInstance);

  theApp.initApp();

  return theApp.run();
}


void CrazyTetrisApp::buildBuffers()
{
  D3D10_BUFFER_DESC bufferDesc =
  {
    MAX_CUBE_INSTANCES * sizeof(CubeInstance),
    D3D10_USAGE_DYNAMIC,
    D3D10_BIND_VERTEX_BUFFER,
    D3D10_CPU_ACCESS_WRITE,
    0
  };

  HR( md3dDevice->CreateBuffer(&bufferDesc, NULL, &cubeInstancesBuffer) )

}

void CrazyTetrisApp::buildTextures()
{
  //типа пиздатый поиск, обработчик ошибок - нахуй, зато текстуры не в массиве а в векторе - ваще заебок
  WIN32_FIND_DATA FileData;
  HANDLE hSearch;
  BOOL fFinished = FALSE;
  ID3D10ShaderResourceView* temp;
  texBackWallRV.clear();
  hSearch = FindFirstFile((TEXTURES_FOLDER + L"Wall\\*.jpg").c_str(), &FileData);
  while (!fFinished)
  {
    HR( D3DX10CreateShaderResourceViewFromFile(md3dDevice, (TEXTURES_FOLDER + L"Wall\\" +  FileData.cFileName).c_str(), 0, 0, &temp, 0) );
    texBackWallRV.push_back(temp);
    if (!FindNextFile(hSearch, &FileData)) fFinished = TRUE;
  }
  FindClose(hSearch);

  //HR( D3DX10CreateShaderResourceViewFromFile(md3dDevice, (TEXTURES_FOLDER +  L"SearchLightXuy.jpg").c_str(), 0, 0, &texSearchLightColorFilterRV, 0) );
  HR( D3DX10CreateShaderResourceViewFromFile(md3dDevice, (TEXTURES_FOLDER +  L"Aim.jpg").c_str(), 0, 0, &texAimRV, 0) );

  ID3D10Texture2D* srcTex[N_BONUSES];
  for(UINT i = 0; i < N_BONUSES; ++i)
  {
    D3DX10_IMAGE_LOAD_INFO loadInfo;
    loadInfo.Width  = D3DX10_FROM_FILE;
    loadInfo.Height = D3DX10_FROM_FILE;
    loadInfo.Depth  = D3DX10_FROM_FILE;
    loadInfo.FirstMipLevel = 0;
    loadInfo.MipLevels = D3DX10_FROM_FILE;
    loadInfo.Usage = D3D10_USAGE_STAGING;
    loadInfo.BindFlags = 0;
    loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    loadInfo.MiscFlags = 0;
    loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    loadInfo.Filter = D3DX10_FILTER_NONE;
    loadInfo.MipFilter = D3DX10_FILTER_NONE;
    loadInfo.pSrcInfo  = 0;

    HR(D3DX10CreateTextureFromFile(md3dDevice,
      (BONUS_IMAGES_FOLDER + BONUS_NAME[i] + L".png").c_str(),
         //(TEXTURES_FOLDER + L"tree0.dds").c_str(),
          &loadInfo, 0,
          (ID3D10Resource**)&srcTex[i], 0));
  }

  //
  // Create the texture array.  Each element in the texture
  // array has the same format/dimensions.
  //

  D3D10_TEXTURE2D_DESC texElementDesc;
  srcTex[0]->GetDesc(&texElementDesc);

  D3D10_TEXTURE2D_DESC texArrayDesc;
  texArrayDesc.Width              = texElementDesc.Width;
  texArrayDesc.Height             = texElementDesc.Height;
  texArrayDesc.MipLevels          = texElementDesc.MipLevels;
  texArrayDesc.ArraySize          = N_BONUSES;
  texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
  texArrayDesc.SampleDesc.Count   = 1;
  texArrayDesc.SampleDesc.Quality = 0;
  texArrayDesc.Usage              = D3D10_USAGE_DEFAULT;
  texArrayDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
  texArrayDesc.CPUAccessFlags     = 0;
  texArrayDesc.MiscFlags          = 0;

  ID3D10Texture2D* texArray = 0;
  HR(md3dDevice->CreateTexture2D(&texArrayDesc, 0, &texArray));

  //
  // Copy individual texture elements into texture array.
  //

  // for each texture element...
  for(UINT i = 0; i < N_BONUSES; ++i)
  {
      // for each mipmap level...
      for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
      {
          D3D10_MAPPED_TEXTURE2D mappedTex2D;
          srcTex[i]->Map(j, D3D10_MAP_READ, 0, &mappedTex2D);

          md3dDevice->UpdateSubresource(texArray,
          D3D10CalcSubresource(j, i, texElementDesc.MipLevels),
          0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);

          srcTex[i]->Unmap(j);
      }
  }
  //
  // Create a resource view to the texture array.
  //

  D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
  viewDesc.Format = texArrayDesc.Format;
  viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
  viewDesc.Texture2DArray.MostDetailedMip = 0;
  viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
  viewDesc.Texture2DArray.FirstArraySlice = 0;
  viewDesc.Texture2DArray.ArraySize = N_BONUSES;

  HR(md3dDevice->CreateShaderResourceView(
        texArray, &viewDesc, &texBonusesRV));

  //
  // Cleanup--we only need the resource view.
  //

  ReleaseCOM(texArray);

  for(UINT i = 0; i < 4; ++i)
      ReleaseCOM(srcTex[i]);

}

void CrazyTetrisApp::buildViewports()
{
    //Updating viewports
  int nActivePlayers = mGame.participants.size();
  float vpWidth  = (float) mClientWidth / nActivePlayers;
  float vpHeight = (float) mClientHeight;

  vpWidth  = Min(vpWidth,  vpHeight * vpAspect);
  vpHeight = Min(vpHeight, vpWidth  / vpAspect);

  float leftIndent = (mClientWidth - nActivePlayers * vpWidth) / (2.0f * nActivePlayers);
  float topIndent  = (mClientHeight - vpHeight) / 2.0f;
  for (int i = 0; i < nActivePlayers; ++i)
  {
    scrPlayers[i].viewport.TopLeftX = (int) (leftIndent + i * mClientWidth / nActivePlayers);
    scrPlayers[i].viewport.TopLeftY = (int) topIndent;
    scrPlayers[i].viewport.Width =  (int) vpWidth;
    scrPlayers[i].viewport.Height = (int) vpHeight;
    scrPlayers[i].viewport.MinDepth = 0.0f;
    scrPlayers[i].viewport.MaxDepth = 1.0f;

    scrPlayers[i].rectName.left =  scrPlayers[i].viewport.TopLeftX + vpWidth * FIELD_INDENT_LEFT / VP_WORLD_WIDTH;
    scrPlayers[i].rectName.top  =  scrPlayers[i].viewport.TopLeftY + vpHeight * (1 - HUD_HEIGHT / VP_WORLD_HEIGHT);
    scrPlayers[i].rectName.right = scrPlayers[i].viewport.TopLeftX + vpWidth * (FIELD_INDENT_LEFT + (MAX_WORLD_FIELD_WIDTH - CUBE_SCALE) / 2)/ VP_WORLD_WIDTH;

    scrPlayers[i].rectVictim.left = scrPlayers[i].viewport.TopLeftX + vpWidth * (FIELD_INDENT_LEFT + (MAX_WORLD_FIELD_WIDTH + 3 * CUBE_SCALE) / 2)/ VP_WORLD_WIDTH;
    scrPlayers[i].rectVictim.top  = scrPlayers[i].viewport.TopLeftY + vpHeight* (1 - HUD_HEIGHT / VP_WORLD_HEIGHT);
  }

  //interfaceFont->Release();
  D3DX10_FONT_DESC fontDesc;
  fontDesc.Height          = vpHeight * CUBE_SCALE / VP_WORLD_HEIGHT;
  fontDesc.Width           = 0;
  fontDesc.Weight          = 0;
  fontDesc.MipLevels       = 1;
  fontDesc.Italic          = false;
  fontDesc.CharSet         = DEFAULT_CHARSET;
  fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
  fontDesc.Quality         = DEFAULT_QUALITY;
  fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;
  wcscpy(fontDesc.FaceName, L"Arial");

  D3DX10CreateFontIndirect(md3dDevice, &fontDesc, &interfaceFont);

  vpScreen.TopLeftX = 0;
  vpScreen.TopLeftY = 0;
  vpScreen.Width = mClientWidth;
  vpScreen.Height = mClientHeight;
  vpScreen.MinDepth = 0.0f;
  vpScreen.MaxDepth = 1.0f;

  vpShadow.TopLeftX = 0;
  vpShadow.TopLeftY = 0;
  vpShadow.Width = (int) SMAP_SIZE;
  vpShadow.Height= (int) SMAP_SIZE;
  vpShadow.MinDepth = 0.0f;
  vpShadow.MaxDepth = 1.0f;
}
float CrazyTetrisApp::fieldToWorldX(float fieldX)
{
  return (fieldX - (FIELD_WIDTH - 1.0f) / 2.0f) * CUBE_SCALE;
}

float CrazyTetrisApp::fieldToWorldY(float fieldY)
{
  return (fieldY - (FIELD_HEIGHT - 1.0f) / 2.0f) * CUBE_SCALE;
}


void CrazyTetrisApp::loadCubeInstances(Player* player)
{

  cubeInstancesBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void** ) &cubeInstances);
  for (size_t i = 0; i < player->lyingBlockImages.size(); ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[i].mWorld,
                           fieldToWorldX(player->lyingBlockImages[i].positionX(currTime)),
                           fieldToWorldY(player->lyingBlockImages[i].positionY(currTime)),
                           0);
    float bonusProgress = 2 * player->lyingBlockImages[i].bonusImage.progress(currTime) - 1;
    float scale = abs(bonusProgress);
    D3DXMATRIX mScale;
    D3DXMatrixScaling(&mScale, scale, scale, scale);
    D3DXMatrixMultiply(&cubeInstances[i].mWorld, &mScale, &cubeInstances[i].mWorld);

    cubeInstances[LYING_BLOCKS_INSTANCES_OFFSET + i].diffuseColor  = player->lyingBlockImages[i].color;
    cubeInstances[LYING_BLOCKS_INSTANCES_OFFSET + i].specularColor = player->lyingBlockImages[i].color * 0.5f + WHITE * 0.5f;
    cubeInstances[LYING_BLOCKS_INSTANCES_OFFSET + i].specularColor.a = 128.f;
    if (bonusProgress > 0)
      cubeInstances[LYING_BLOCKS_INSTANCES_OFFSET + i].texIndex = player->lyingBlockImages[i].bonus;
    else
      cubeInstances[LYING_BLOCKS_INSTANCES_OFFSET + i].texIndex = 0;
  }

    for (size_t i = 0; i < player->disappearingLines.size(); ++i)
      for (int j = 0; j < FIELD_WIDTH; ++j)
      {
        D3DXMatrixTranslation(&cubeInstances[DISAPPEARING_LINE_INSTANCES_OFFSET + i * FIELD_WIDTH + j].mWorld,
                               fieldToWorldX((float) j),
                               fieldToWorldY((float) player->disappearingLines[i].row),
                               0.0f);

        cubeInstances[DISAPPEARING_LINE_INSTANCES_OFFSET + i * FIELD_WIDTH + j].diffuseColor  =  player->disappearingLines[i].blockColor[j];
        cubeInstances[DISAPPEARING_LINE_INSTANCES_OFFSET + i * FIELD_WIDTH + j].specularColor =   WHITE;
        cubeInstances[DISAPPEARING_LINE_INSTANCES_OFFSET + i * FIELD_WIDTH + j].specularColor.a = 32.f;
      }


  for (size_t i = 0; i < player->fallingBlockImages.size(); ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[FALLING_PIECE_INSTANCES_OFFSET + i].mWorld,
                           fieldToWorldX(player->fallingBlockImages[i].positionX(currTime)),
                           fieldToWorldY(player->fallingBlockImages[i].positionY(currTime)),
                           0);

    cubeInstances[FALLING_PIECE_INSTANCES_OFFSET + i].diffuseColor  = player->fallingBlockImages[i].color;
    cubeInstances[FALLING_PIECE_INSTANCES_OFFSET + i].specularColor = player->fallingBlockImages[i].color * 0.5f + WHITE * 0.5f;
    cubeInstances[FALLING_PIECE_INSTANCES_OFFSET + i].specularColor.a = 128.f;

  }

  for (size_t i = 0; i < player->nextPieces[0].nBlocks(); ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[HINT_INSTANCES_OFFSET + i].mWorld,
                           fieldToWorldX((float) player->nextPieces[0].absoluteCoords(i).col),
                           fieldToWorldY((float) player->nextPieces[0].absoluteCoords(i).row),
                           0);
    cubeInstances[HINT_INSTANCES_OFFSET + i].diffuseColor  = player->nextPieces[0].color();
    cubeInstances[HINT_INSTANCES_OFFSET + i].specularColor = player->nextPieces[0].color() * 0.5f + WHITE * 0.5f;
    cubeInstances[HINT_INSTANCES_OFFSET + i].specularColor.a = 128.f;
  }



  cubeInstancesBuffer->Unmap();
}

void CrazyTetrisApp::installLights(Player* player, bool isLanternOn)
{

  float x = (float) fieldToWorldX(player->visualEffects.lantern.positionX(currTime));
  float y = (float) fieldToWorldY(player->visualEffects.lantern.positionY(currTime));
  float lanternProgress = player->visualEffects.lantern.progress(currTime);
  float implodeProgress = player->visualEffects.fieldCleaning.progress(currTime);

  //Calculating lights
  if (isLanternOn)
  {
    Lights[1].pos.x = x;
    Lights[1].pos.y = y;
    //Lights[1].pos.x += sin(TWO_PI * player->visualEffects.wave.progress(currTime)) * sin(2 *  Lights[1].pos.y)  * 0.33;
    Lights[1].pos.z = -DIST_TO_SEARCHLIGHT;
    Lights[1].dir = D3DXVECTOR4(0.0f, 0.0f,  1.0f, 0.0f);

    float angle = atan2(Lights[1].pos.x, Lights[1].pos.y);
    float coeff = sqrt(2 + 1 * cos(5 * angle + 3 * implodeProgress));
    Lights[1].pos *= -sin(coeff * implodeProgress) / tan(coeff) + cos(coeff * implodeProgress);

    Lights[2].pos.x = x;
    Lights[2].pos.y = y;
    //Lights[2].pos.x += sin(TWO_PI * player->visualEffects.wave.progress(currTime)) * sin(2 *  Lights[2].pos.y)  * 0.33;
    Lights[2].pos.z =  DIST_TO_SEARCHLIGHT;
    Lights[2].dir = D3DXVECTOR4(0.0f, 0.0f,  -1.0f, 0.0f);

    Lights[2].pos *= -sin(coeff * implodeProgress) / tan(coeff) + cos(coeff * implodeProgress);

    D3DXVec4Transform(&Lights[1].pos, &Lights[1].pos, &mGlobalRotation);
    D3DXVec4Transform(&Lights[1].dir, &Lights[1].dir, &mGlobalRotation);

    D3DXVec4Transform(&Lights[2].pos, &Lights[2].pos, &mGlobalRotation);
    D3DXVec4Transform(&Lights[2].dir, &Lights[2].dir, &mGlobalRotation);

    int shadingLight;
    if (Lights[1].dir.z > 0) shadingLight = 1; else shadingLight = 2;

    D3DXVECTOR3 target = Lights[shadingLight].pos + Lights[shadingLight].dir;
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 pos(Lights[shadingLight].pos.x, Lights[shadingLight].pos.y, Lights[shadingLight].pos.z);
    D3DXMATRIX temp1, temp2;
    D3DXMatrixLookAtLH(&temp1, &pos, &target, &up);
    D3DXMatrixPerspectiveFovLH(&temp2, 2 * searchBeta, 1, 1.0f, 100.0f);
    mLightWVP = temp1 * temp2;
    fxLightWVPVar->SetMatrix((float*) mLightWVP);

    Lights[0].brightness = 1 - lanternProgress;
    Lights[1].brightness =     lanternProgress;
    Lights[2].brightness =     lanternProgress;
  }
  else
  {
    Lights[0].brightness = 1;
    Lights[1].brightness = 0;
    Lights[2].brightness = 0;
  }


  fxLightVar->SetRawValue(&Lights, 0, MAX_LIGHTS * sizeof(Light) );
  fxEyePosVar->SetRawValue(&eyePosW,0,sizeof(D3DXVECTOR3));
}

void CrazyTetrisApp::loadPlayerData(Player* player)
{
  D3DXMATRIX temp, tempX, tempY;
  D3DXMatrixMultiply(&mGlobalRotation,
                     D3DXMatrixRotationY(&tempY, 2 * PI * player->visualEffects.rotatingField.progress(currTime)),
                     D3DXMatrixRotationX(&tempX,     PI * player->visualEffects.flippedScreen.progress(currTime)));
  fxGlobalRotationVar->SetMatrix((float*) &mGlobalRotation);

  fxSemicubesProgressVar->SetFloat(player->visualEffects.semicubes.progress(currTime));
  fxWaveProgressVar->SetFloat(player->visualEffects.wave.progress(currTime));
  fxImplodeProgressVar->SetFloat(player->visualEffects.fieldCleaning.progress(currTime));
}



void CrazyTetrisApp::drawWall(Player* player, bool colored)
{
  D3DXMATRIX temp;
  fxDiffuseMapVar->SetResource(texBackWallRV[player->backgroundSeed % texBackWallRV.size()]);
  fxWorldVar->SetMatrix((float*) D3DXMatrixTranslation(&temp, 0.0f, 0.0f, CUBE_SCALE * FIELD_HEIGHT / 2));
  md3dDevice->IASetInputLayout(texturedVertexLayout);
  if (colored)
        techTextured->GetPassByIndex( 0 )->Apply(0);
  else  techSMTextured->GetPassByIndex( 0 )->Apply(0);
  mWall.draw();
}


void CrazyTetrisApp::drawLyingBlocks(Player* player, bool colored)
{
  if (colored)
  {
    if (player->visualEffects.semicubes.progress(currTime) > MATH_EPS)
           techSemicubes->GetPassByIndex( 0 )->Apply(0);
      else techCubes    ->GetPassByIndex( 0 )->Apply(0);
  } else
  {
    if (player->visualEffects.semicubes.progress(currTime) > MATH_EPS)
           techSMSemicubes->GetPassByIndex( 0 )->Apply(0);
      else techSMCubes    ->GetPassByIndex( 0 )->Apply(0);
  }
  mBox.draw(player->lyingBlockImages.size(), LYING_BLOCKS_INSTANCES_OFFSET);
}


void CrazyTetrisApp::drawHint(Player* player)
{
  fxEdgeOpacityVar->SetFloat(player->visualEffects.hint.progress(currTime));
  fxFaceOpacityVar->SetFloat(0.3f * player->visualEffects.hintMaterialization.progress(currTime));
  techFallingPiece->GetPassByIndex( 0 )->Apply(0);
  mBox.draw(player->nextPieces[0].nBlocks(), HINT_INSTANCES_OFFSET);
  techFallingPiece->GetPassByIndex( 1 )->Apply(0);
  mBox.draw(player->nextPieces[0].nBlocks(), HINT_INSTANCES_OFFSET);


}

void CrazyTetrisApp::drawDisappearingLines(Player* player, bool colored)
{
  for (size_t i = 0; i < player->disappearingLines.size(); ++i)
  {
    D3DXVECTOR4 clippingPlane = -D3DXVECTOR4(2.0f * (i % 2) - 1.0f, 1.0f, 1.0f,
                1.5f * (CUBE_SCALE + MATH_EPS) * (2.f * player->disappearingLines[i].progress(currTime) - 1.f));
    fxClippingPlaneVar->SetRawValue(&clippingPlane,0,sizeof(D3DXVECTOR4));
    if (colored)
          techDisappearingLine->GetPassByIndex( 0 )->Apply(0);
    else  techSMDisappearingLine->GetPassByIndex( 0 )->Apply(0);

    mBox.draw(FIELD_WIDTH, DISAPPEARING_LINE_INSTANCES_OFFSET + i * FIELD_WIDTH);
  }
}

void CrazyTetrisApp::drawFallingPiece(Player* player, bool colored)
{
  fxEdgeOpacityVar->SetFloat(1.f);
  fxFaceOpacityVar->SetFloat(0.3f);
  D3DXVECTOR4 clippingPlane = D3DXVECTOR4(0.0, -1.0f, 0.0f, CUBE_SCALE * FIELD_HEIGHT / 2.0f);
  D3DXVec4Transform(&clippingPlane, &clippingPlane, &mGlobalRotation);
  fxClippingPlaneVar->SetRawValue(&clippingPlane, 0, sizeof(D3DXVECTOR4));
  fxOpacityVar->SetFloat(0.3f);
  if (colored)
  {
    techFallingPiece->GetPassByIndex( 0 )->Apply(0);
    mBox.draw(player->fallingBlockImages.size(), FALLING_PIECE_INSTANCES_OFFSET);
    techFallingPiece->GetPassByIndex( 1 )->Apply(0);
    mBox.draw(player->fallingBlockImages.size(), FALLING_PIECE_INSTANCES_OFFSET);
  } else
  {
    techSMFallingPiece->GetPassByIndex( 0 )->Apply(0);
    mBox.draw(player->fallingBlockImages.size(), FALLING_PIECE_INSTANCES_OFFSET);
  }
}