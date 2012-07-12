#include <cstdlib>
#include "d3dApp/d3dApp.h"
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
const float FIELD_INDENT_LEFT = 0.0f;
const float FIELD_INDENT_RIGHT = 0.0f;
const float FIELD_INDENT_BOTTOM = 0.0f;
const float FIELD_INDENT_TOP = 0.0f;
const float HUD_HEIGHT = 0.0f;
const float VP_WORLD_WIDTH  = FIELD_INDENT_LEFT + MAX_WORLD_FIELD_WIDTH + FIELD_INDENT_RIGHT;
const float VP_WORLD_HEIGHT = HUD_HEIGHT + FIELD_INDENT_BOTTOM + MAX_WORLD_FIELD_HEIGHT + FIELD_INDENT_TOP;

const float ANGLE_FOV_Y = 0.2f * PI; //DON'T CHANGE!!!

float EYE_TO_FIELD = 0.5f * VP_WORLD_HEIGHT / tan(ANGLE_FOV_Y / 2.0f);

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
	void drawPlayer(Player* player, float currTime);
  
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
  Wall mWall;
  Glass mGlass;
  Game mGame;
  CubeInstance* cubeInstances;

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
	
  //Shader variables
	ID3D10EffectMatrixVariable* fxVPVar;
	ID3D10EffectMatrixVariable* fxWorldVar;
	ID3D10EffectMatrixVariable* fxGlobalRotationVar;
  ID3D10EffectMatrixVariable* fxLightWVPVar;
  
  ID3D10EffectScalarVariable* fxTimeVar;
	ID3D10EffectScalarVariable* fxLightTypeVar;
  ID3D10EffectScalarVariable* fxWaveProgressVar;
  ID3D10EffectScalarVariable* fxSemicubesProgressVar;
  ID3D10EffectScalarVariable* fxOpacityVar;
  ID3D10EffectScalarVariable* fxCUBE_SCALE;
  ID3D10EffectScalarVariable* fxCUBE_SCALE_INVERTED;

  ID3D10EffectVariable*       fxLightVar;
	ID3D10EffectVariable*       fxEyePosVar;
  //ID3D10EffectVariable*       fxGlobalOffsetVar;
  ID3D10EffectVariable*       fxColorDiffuseVar;
  ID3D10EffectVariable*       fxColorSpecularVar;
  ID3D10EffectVariable*       fxClippingPlaneVar;

  ID3D10EffectShaderResourceVariable* fxDiffuseMapVar;
  ID3D10EffectShaderResourceVariable* fxColorFilterVar;


	//Rasterizer state - clean up
  ID3D10RasterizerState* rsSolidCullFront;
  ID3D10RasterizerState* rsSolidCullBack;
  ID3D10RasterizerState* rsSolidCullNone;
  ID3D10RasterizerState* rsForSemicubes;

 
  //Textures and texture views
  ID3D10ShaderResourceView* texBackWallRV;
  ID3D10ShaderResourceView* texSearchLightColorFilterRV;
  
  ID3D10Buffer* cubeInstancesBuffer;

  ID3D10BlendState* transparentBS;

  D3D10_VIEWPORT vpPlayers[MAX_PLAYERS];
  D3D10_VIEWPORT vpScreen;

  
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
   
  const float CUBE_SCALE;

  const float vpAspect;

};

CrazyTetrisApp::CrazyTetrisApp(HINSTANCE hInstance)
: D3DApp(hInstance), FX(0), techCubes(0), techSemicubes(0),
  fxVPVar(0), currTime(0.f), mPhi(PI / 2.f), mTheta(0.f), 
  CUBE_SCALE(min (MAX_WORLD_FIELD_WIDTH / (float) FIELD_WIDTH, MAX_WORLD_FIELD_HEIGHT / (float) FIELD_HEIGHT)),
  vpAspect(VP_WORLD_WIDTH / VP_WORLD_HEIGHT)
  
{
  D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
  D3DXMatrixIdentity(&mVP); 
  D3DXMatrixIdentity(&mGlobalRotation); 
  srand(28702);
  
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
  ReleaseCOM(uncoloredVertexLayout);

  ReleaseCOM(texBackWallRV);
  ReleaseCOM(texSearchLightColorFilterRV);
}

void CrazyTetrisApp::initApp()
{
  mGame.init();
  mGame.newMatch();
  mGame.newRound(0);
  mGame.saveSettings();

  D3DApp::initApp();
	
  mBox.init(md3dDevice, CUBE_SCALE / 2.0f, 0.5f, 5);
  mWall.init(md3dDevice, 
             CUBE_SCALE * FIELD_WIDTH  * (1.0f + CUBE_SCALE * (FIELD_HEIGHT / 2.0f + 0.5f) / EYE_TO_FIELD), 
             CUBE_SCALE * (FIELD_HEIGHT) * (1.0f + CUBE_SCALE * (FIELD_HEIGHT / 2.0f + 0.5f) / EYE_TO_FIELD), 
             1.0f,  1.0f);
  mGlass.init(md3dDevice, CUBE_SCALE * FIELD_WIDTH, CUBE_SCALE * FIELD_HEIGHT, CUBE_SCALE);
  
  buildFX();
	buildTextures();
  setConstantBuffers();

  buildVertexLayouts();
	buildLights();
	buildRasterizerStates();
  buildBlendingStates();
  buildBuffers();

 // for test purposes


}


void CrazyTetrisApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

  currTime = mTimer.getGameTime();
  mGame.onTimer(currTime);
  

  
	// Update angles based on input to orbit camera around box.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt; 

	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;
	
  if(GetAsyncKeyState('R') & 0x8000)
  {
    mPhi = PI / 2;
    mTheta = 0;
  }
  
  if(GetAsyncKeyState(VK_F1) & 0x8000)	mGame.players[2].visualEffects.flippedScreen.enable(1.0f);
  if(GetAsyncKeyState(VK_F2) & 0x8000)	mGame.players[2].visualEffects.flippedScreen.disable();

  if(GetAsyncKeyState(VK_F3) & 0x8000)	mGame.players[2].visualEffects.rotatingField.enable(10.0f);
  if(GetAsyncKeyState(VK_F4) & 0x8000)	mGame.players[2].visualEffects.rotatingField.disable();
	
  if(GetAsyncKeyState(VK_F5) & 0x8000)	mGame.players[2].visualEffects.wave.enable(2.0f);
  if(GetAsyncKeyState(VK_F6) & 0x8000)	mGame.players[2].visualEffects.wave.disable();

  if(GetAsyncKeyState(VK_F7) & 0x8000)	mGame.players[2].visualEffects.semicubes.enable(1.f);
  if(GetAsyncKeyState(VK_F8) & 0x8000)	mGame.players[2].visualEffects.semicubes.disable();

  if(GetAsyncKeyState('1') & 0x8000)	 mGame.players[2].visualEffects.lantern.enable(2.0f);
  if(GetAsyncKeyState('2')& 0x8000)	   mGame.players[2].visualEffects.lantern.disable();


  // Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	eyePosW.x =  (EYE_TO_FIELD + CUBE_SCALE / 2)*sinf(mPhi)*sinf(mTheta) - (FIELD_INDENT_LEFT - FIELD_INDENT_RIGHT);
	eyePosW.z = -(EYE_TO_FIELD + CUBE_SCALE / 2)*sinf(mPhi)*cosf(mTheta) - (HUD_HEIGHT / 2.0f + FIELD_INDENT_BOTTOM - FIELD_INDENT_TOP);
	eyePosW.y =  (EYE_TO_FIELD + CUBE_SCALE / 2)*cosf(mPhi);
  
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &eyePosW, &target, &up);

}

void CrazyTetrisApp::drawScene()
{
  D3DApp::drawScene(); //Cleaning screen
  //Getting current time once
  currTime = mTimer.getGameTime();
  
  //Set time for this frame
	fxTimeVar->SetFloat(currTime);

  for (size_t i = 0; i < mGame.activePlayers.size(); ++i)
  {
    md3dDevice->RSSetViewports(1, &vpPlayers[i]);
    drawPlayer(mGame.activePlayers[i], currTime);
  }

  md3dDevice->RSSetViewports(1, &vpScreen);
	md3dDevice->RSSetState(0);
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, WHITE);

	mSwapChain->Present(0, 0);
}

void CrazyTetrisApp::drawPlayer(Player* player, float currTime)
{
  D3DXVECTOR4 clippingPlane;
  //Reseting states
  md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};

  mVP = mView * mProj;
	fxVPVar->SetMatrix((float*)&mVP);

  D3DXMATRIX temp, tempX, tempY;
  D3DXMatrixMultiply(&mGlobalRotation, 
                     D3DXMatrixRotationY(&tempY, 2 * PI * player->visualEffects.rotatingField.progress(currTime)),
                     D3DXMatrixRotationX(&tempX,     PI * player->visualEffects.flippedScreen.progress(currTime)));
  
  
  //Calculating lights
  Lights[1].pos.x = fieldToWorldX(player->visualEffects.lantern.positionX(currTime));
  Lights[1].pos.y = fieldToWorldY(player->visualEffects.lantern.positionY(currTime));
  //Lights[1].pos.x += sin(TWO_PI * player->visualEffects.wave.progress(currTime)) * sin(2 *  Lights[1].pos.y)  * 0.33;
  Lights[1].pos.z = -1.5f;
  Lights[1].dir = D3DXVECTOR4(0.2f, 0.2f,  1.0f, 0.0f);
  
  Lights[2].pos.x = fieldToWorldX(player->visualEffects.lantern.positionX(currTime));
  Lights[2].pos.y = fieldToWorldY(player->visualEffects.lantern.positionY(currTime));
  //Lights[2].pos.x += sin(TWO_PI * player->visualEffects.wave.progress(currTime)) * sin(2 *  Lights[2].pos.y)  * 0.33;
  Lights[2].pos.z =  1.5f;
  Lights[2].dir = D3DXVECTOR4(0.0f, 0.0f,  -1.0f, 0.0f);
 
  D3DXVECTOR3 target = Lights[1].pos + Lights[1].dir;
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 pos(Lights[1].pos.x, Lights[1].pos.y, Lights[1].pos.z);
  D3DXMatrixLookAtLH(&tempX, &pos, &target, &up);
	D3DXMatrixPerspectiveFovLH(&tempY, ANGLE_FOV_Y, vpAspect, 1.0f, 1000.0f);
  mLightWVP = tempX * tempY;
  fxLightWVPVar->SetMatrix((float*) mLightWVP);

  Lights[0].brightness = 1 - player->visualEffects.lantern.progress(currTime);
  Lights[1].brightness =     player->visualEffects.lantern.progress(currTime);
  Lights[2].brightness =     player->visualEffects.lantern.progress(currTime);

  D3DXVec4Transform(&Lights[1].pos, &Lights[1].pos, &mGlobalRotation);
  D3DXVec4Transform(&Lights[1].dir, &Lights[1].dir, &mGlobalRotation);
  
  D3DXVec4Transform(&Lights[2].pos, &Lights[2].pos, &mGlobalRotation);
  D3DXVec4Transform(&Lights[2].dir, &Lights[2].dir, &mGlobalRotation);

  fxLightVar->SetRawValue(&Lights, 0, MAX_LIGHTS * sizeof(Light) );
	fxEyePosVar->SetRawValue(&eyePosW,0,sizeof(D3DXVECTOR3));
   
  float semicubesProgress = player->visualEffects.semicubes.progress(currTime);

  fxWaveProgressVar->SetFloat(player->visualEffects.wave.progress(currTime));
  fxSemicubesProgressVar->SetFloat(semicubesProgress);
  
  fxGlobalRotationVar->SetMatrix((float*) &mGlobalRotation);
  fxWorldVar->SetMatrix((float*) &mWorld);


  //рисуем непрозрачные объекты  
  md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);       

  //рисуем стенку
  fxWorldVar->SetMatrix((float*) 
    D3DXMatrixTranslation(&tempX, 0.0f, 0.0f, CUBE_SCALE * FIELD_HEIGHT / 2));
  md3dDevice->IASetInputLayout(texturedVertexLayout);
  techTextured->GetPassByIndex( 0 )->Apply(0);
  mWall.draw();
  
  //рисуем (полу)кубики, которые уже упали
  md3dDevice->IASetInputLayout(cubesVertexLayout);
  
  if (semicubesProgress > MATH_EPS) techSemicubes->GetPassByIndex( 0 )->Apply(0);
                               else techCubes    ->GetPassByIndex( 0 )->Apply(0); 
  
  mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
  cubeInstancesBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void** ) &cubeInstances); 
  for (size_t i = 0; i < player->lyingBlockImages.size(); ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[i].mWorld, 
                           fieldToWorldX(player->lyingBlockImages[i].positionX(currTime)),
                           fieldToWorldY(player->lyingBlockImages[i].positionY(currTime)),
                           0);

    cubeInstances[i].diffuseColor  = player->lyingBlockImages[i].color; 
    cubeInstances[i].specularColor = player->lyingBlockImages[i].color * 0.5f + WHITE * 0.5f;
    cubeInstances[i].specularColor.a = 128.f;

  }
  cubeInstancesBuffer->Unmap();
  mBox.draw(player->lyingBlockImages.size());
  
  //бортики
  clippingPlane = D3DXVECTOR4(0.0, 0.0f, 0.0f, 0.0f);
	fxClippingPlaneVar->SetRawValue(&clippingPlane, 0, sizeof(D3DXVECTOR4));
  fxOpacityVar->SetFloat(0.0);
  mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
  cubeInstancesBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void** ) &cubeInstances); 
  for (size_t i = 0; i < FIELD_HEIGHT; ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[2 * i].mWorld, 
                           fieldToWorldX(-1),
                           fieldToWorldY(i),
                           0);

    cubeInstances[2 * i].diffuseColor  = WHITE; 
    cubeInstances[2 * i].specularColor = WHITE;
    cubeInstances[2 * i].specularColor.a = 128.f;

    D3DXMatrixTranslation(&cubeInstances[2 * i + 1].mWorld, 
                           fieldToWorldX(FIELD_WIDTH),
                           fieldToWorldY(i),
                           0);

    cubeInstances[2 * i + 1].diffuseColor  = WHITE; 
    cubeInstances[2 * i + 1].specularColor = WHITE;
    cubeInstances[2 * i + 1].specularColor.a = 128.f;

  }

    for (int i = 0; i < FIELD_WIDTH + 2; ++i) 
  {
    D3DXMatrixTranslation(&cubeInstances[2 * FIELD_HEIGHT + i].mWorld, 
                           fieldToWorldX(i - 1),
                           fieldToWorldY(-1),
                           0);

    cubeInstances[2 * FIELD_HEIGHT + i].diffuseColor  = WHITE; 
    cubeInstances[2 * FIELD_HEIGHT + i].specularColor = WHITE;
    cubeInstances[2 * FIELD_HEIGHT + i].specularColor.a = 128.f;

  }
  cubeInstancesBuffer->Unmap();

  techFallingPiece->GetPassByIndex( 0 )->Apply(0);
  mBox.draw(2 * FIELD_HEIGHT + FIELD_WIDTH + 2);
  techFallingPiece->GetPassByIndex( 1 )->Apply(0);
  mBox.draw(2 * FIELD_HEIGHT + FIELD_WIDTH + 2);


  //рисуем убираемые линии
  
  for (size_t i = 0; i < player->disappearingLines.size(); ++i)
  {

    clippingPlane = -D3DXVECTOR4(2.0f * (i % 2) - 1.0f, 1.0f, 1.0f, 1.5f * (CUBE_SCALE + MATH_EPS) * (2.f * player->disappearingLines[i].progress(currTime) - 1.f));
    fxClippingPlaneVar->SetRawValue(&clippingPlane,0,sizeof(D3DXVECTOR4));

    mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
    cubeInstancesBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void** ) &cubeInstances); 
    for (int j = 0; j < FIELD_WIDTH; ++j)
    {
      D3DXMatrixTranslation(&cubeInstances[j].mWorld, 
                             fieldToWorldX((float) j),
                             fieldToWorldY((float) player->disappearingLines[i].row),
                             0.0f);
      
      cubeInstances[j].diffuseColor  =  player->disappearingLines[i].blockColor[j]; 
      cubeInstances[j].specularColor =   WHITE;
      cubeInstances[j].specularColor.a = 32.f;
    }
    cubeInstancesBuffer->Unmap();
    md3dDevice->IASetInputLayout(cubesVertexLayout);
    techDisappearingLine->GetPassByIndex( 0 )->Apply(0);
    mBox.draw(FIELD_WIDTH);
  }

   md3dDevice->OMSetBlendState(transparentBS, blendFactors, 0xffffffff);    
  //упорядочиваем по удаленности и рисуем прозрачные объекты
  
     //рисуем падающую фигуру
  md3dDevice->OMSetBlendState(transparentBS, blendFactors, 0xffffffff);
  clippingPlane = D3DXVECTOR4(0.0, -1.0f, 0.0f, CUBE_SCALE * FIELD_HEIGHT / 2.0f);
	D3DXVec4Transform(&clippingPlane, &clippingPlane, &mGlobalRotation);
  fxClippingPlaneVar->SetRawValue(&clippingPlane, 0, sizeof(D3DXVECTOR4));
  fxOpacityVar->SetFloat(0.3);
  mBox.setVB_AndIB_AsCurrent(md3dDevice, cubeInstancesBuffer);
  cubeInstancesBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void** ) &cubeInstances); 
  for (size_t i = 0; i < player->fallingBlockImages.size(); ++i)
  {
    D3DXMatrixTranslation(&cubeInstances[i].mWorld, 
                           fieldToWorldX(player->fallingBlockImages[i].positionX(currTime)),
                           fieldToWorldY(player->fallingBlockImages[i].positionY(currTime)),
                           0);

    cubeInstances[i].diffuseColor  = player->fallingBlockImages[i].color; 
    cubeInstances[i].specularColor = player->fallingBlockImages[i].color * 0.5f + WHITE * 0.5f;
    cubeInstances[i].specularColor.a = 128.f;

  }
  cubeInstancesBuffer->Unmap();
  techFallingPiece->GetPassByIndex( 0 )->Apply(0);
  mBox.draw(player->fallingBlockImages.size());
  techFallingPiece->GetPassByIndex( 1 )->Apply(0);
  mBox.draw(player->fallingBlockImages.size());

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
	hr = D3DX10CreateEffectFromFile(L"effects.fx", 0, 0, 
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

  //Current time
  fxTimeVar = FX->GetVariableByName("gTime")->AsScalar();

  //Textures
  fxDiffuseMapVar = FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
  fxColorFilterVar = FX->GetVariableByName("gColorFilter")->AsShaderResource();

  //Visual effect's progress
  fxWaveProgressVar      = FX->GetVariableByName("gWaveProgress")->AsScalar();
  fxSemicubesProgressVar = FX->GetVariableByName("gSemicubesProgress")->AsScalar();


  fxClippingPlaneVar     = FX->GetVariableByName("gClippingPlane");
}

void CrazyTetrisApp::setConstantBuffers()
{
  fxCUBE_SCALE->SetFloat(CUBE_SCALE);
  fxCUBE_SCALE_INVERTED->SetFloat(2.0f / CUBE_SCALE);

  fxDiffuseMapVar->SetResource(texBackWallRV);
  fxColorFilterVar->SetResource(texSearchLightColorFilterRV);
  //D3DXVECTOR3 offs(FIELD_INDENT_LEFT - FIELD_INDENT_RIGHT,
  //                 HUD_HEIGHT / 2.0f + FIELD_INDENT_BOTTOM - FIELD_INDENT_TOP,
  //                 0.0f);
  //fxGlobalOffsetVar->SetRawValue(&offs, 0, sizeof(D3DXVECTOR3));

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
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D10_INPUT_PER_VERTEX_DATA,   0},
		{"NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA,   0},
    
    {"diffuse",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"specular",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},   
    {"Transform",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 2 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 3 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1},
    {"Transform",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 5 * 16, D3D10_INPUT_PER_INSTANCE_DATA, 1}
  };
  numElements = sizeof(cubesVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
	techCubes->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(cubesVertexDesc, numElements, passDesc.pIAInputSignature,
	passDesc.IAInputSignatureSize, &cubesVertexLayout));

  D3D10_INPUT_ELEMENT_DESC texturedVertexDesc[] =
  {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
  };
  numElements = sizeof(texturedVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
  techTextured->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(texturedVertexDesc, numElements, passDesc.pIAInputSignature,
	passDesc.IAInputSignatureSize, &texturedVertexLayout));

  D3D10_INPUT_ELEMENT_DESC uncoloredVertexDesc[] =
  {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
  };
  numElements = sizeof(uncoloredVertexDesc) / sizeof(D3D10_INPUT_ELEMENT_DESC);
  techGlass->GetPassByIndex(0)->GetDesc(&passDesc);
  HR(md3dDevice->CreateInputLayout(uncoloredVertexDesc, numElements, passDesc.pIAInputSignature,
	passDesc.IAInputSignatureSize, &uncoloredVertexLayout));

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
  rsDesc.MultisampleEnable = false;  //иначе будут артефакты
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &rsForSemicubes));

}

void CrazyTetrisApp::buildLights()
{
  int i;
  for (i = 0; i < 3; i++)
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
  Lights[1].att.y = 1;
  Lights[1].spotPow = 16.f;
  Lights[1].range = 10;
  Lights[1].lightType = 4;
  Lights[1].brightness = 0.0f;

  Lights[2].dir = D3DXVECTOR4(0.0f, 0.0f, -1.0f, 0.0f);
  Lights[2].pos = D3DXVECTOR4(0.0f, 0.0f,  1.0f, 1.0f);
  Lights[2].att.y = 1;
  Lights[2].spotPow = 16.f;
  Lights[2].range = 10;
  Lights[2].lightType = 4;
  Lights[2].brightness = 0.0f;


  
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

	//md3dDevice->RSSetViewports(1, &vpPlayers[1]);


	//float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f * PI, vpAspect, 1.0f, 1000.0f);
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
    MAX_BLOCKS * sizeof(CubeInstance),
    D3D10_USAGE_DYNAMIC,
    D3D10_BIND_VERTEX_BUFFER,
    D3D10_CPU_ACCESS_WRITE,
    0
  };

  HR( md3dDevice->CreateBuffer(&bufferDesc, NULL, &cubeInstancesBuffer) )
 
}

void CrazyTetrisApp::buildTextures()
{
  HR( D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"Resources/Textures/Wall.dds", 0, 0, &texBackWallRV, 0) );
  HR( D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"Resources/Textures/SearchLightMM.jpg", 0, 0, &texSearchLightColorFilterRV, 0) );
}

void CrazyTetrisApp::buildViewports()
{
    //Updating viewports
  int nActivePlayers = mGame.activePlayers.size(); //Сделать нормально
	float vpWidth  = (float) mClientWidth / nActivePlayers; 
  float vpHeight = (float) mClientHeight;

  vpWidth  = Min(vpWidth,  vpHeight * vpAspect);
  vpHeight = Min(vpHeight, vpWidth  / vpAspect);
  
  float leftIndent = (mClientWidth - nActivePlayers * vpWidth) / 2.0f;
  float topIndent  = (mClientHeight - vpHeight) / 2.0f;
  for (int i = 0; i < nActivePlayers; ++i)
  {
    vpPlayers[i].TopLeftX = (int) (leftIndent + i * vpWidth);
    vpPlayers[i].TopLeftY = (int) topIndent;
    vpPlayers[i].Width = (int) vpWidth;
    vpPlayers[i].Height = (int) vpHeight;
    vpPlayers[i].MinDepth = 0.0f;
    vpPlayers[i].MaxDepth = 1.0f;
  }

  vpScreen.TopLeftX = 0;
  vpScreen.TopLeftY = 0;
  vpScreen.Width = mClientWidth;
  vpScreen.Height = mClientHeight;
  vpScreen.MinDepth = 0.0f;
  vpScreen.MaxDepth = 1.0f;

}
float CrazyTetrisApp::fieldToWorldX(float fieldX)
{
  return (fieldX - (FIELD_WIDTH - 1.0f) / 2.0f) * CUBE_SCALE;
}

float CrazyTetrisApp::fieldToWorldY(float fieldY)
{
  return (fieldY - (FIELD_HEIGHT - 1.0f) / 2.0f) * CUBE_SCALE;
}

