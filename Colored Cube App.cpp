//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================

#include "d3dApp.h"
#include "Box.h"
#include "Light.h"
#include "VisualEffects.h"

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
	void buildLights();
	void buildRasterizerStates();
  void buildBlendingStates();
 
private:
	
	Box mBox;

	ID3D10InputLayout* mVertexLayout;
	
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	
	ID3D10EffectScalarVariable* mfxTimeVar;
	ID3D10EffectScalarVariable* mfxLightTypeVar;

	ID3D10EffectVariable*       mfxLightVar;
	ID3D10EffectVariable*       mfxEyePosVar;
  ID3D10EffectVariable*       mfxClippingPlaneVar;

  ID3D10EffectVariable*       mfxColorDiffuse;
  ID3D10EffectVariable*       mfxColorSpecular;

	ID3D10RasterizerState* mWireframe;
	ID3D10RasterizerState* mSolidCullFront;
  ID3D10RasterizerState* mSolidCullBack;
	
  ID3D10BlendState* mTransparentBS;

	D3DXMATRIX mWorld;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	D3DXMATRIX mVP;
  D3DXMATRIX mCurrentRotation;

	int mLightType;
	Light mLights[3];
	
	D3DXVECTOR3 mEyePosW;
  D3DXVECTOR4 mClippingPlane;
	
	float mTime;


  bool wireframe;

	float mTheta;
	float mPhi;

  float ambientWhiteness; // in [0, 1]
  float opacity;

  static const float AMBIENT_WHITENESS_STEP;
  float ambientAngle;
  static const float AMBIENT_ANGLE_COEFF;
  static const float MAX_AMBIENT_ANGLE;
  static const float MIN_AMBIENT_ANGLE;

  D3DXCOLOR specularColor;
  
  
};

const float ColoredCubeApp::AMBIENT_WHITENESS_STEP = 1.0f;
const float ColoredCubeApp::AMBIENT_ANGLE_COEFF = 10.0f;
const float ColoredCubeApp::MIN_AMBIENT_ANGLE = 1.0f;
const float ColoredCubeApp::MAX_AMBIENT_ANGLE = 1024.0f;


ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI/2.0), mTime(0), wireframe(false),
  ambientAngle(4), ambientWhiteness(0.),
  specularColor(D3DXCOLOR(0.5f, 0.5f, 0.5f, 128.0f)), opacity(0.5f), mClippingPlane(D3DXVECTOR4(-1.0f, 1.0f, -1.0f, 1.0f))
{

  D3DXMatrixIdentity(&mWorld);
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP);
	D3DXMatrixIdentity(&mVP); 
  D3DXMatrixIdentity(&mCurrentRotation); 

}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	
	mBox.init(md3dDevice, 0.3f, 10);

	buildFX();
	buildVertexLayouts();
	buildLights();
	buildRasterizerStates();
  buildBlendingStates();
}


void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);

	// Update angles based on input to orbit camera around box.
	if(GetAsyncKeyState('A') & 0x8000)	mTheta -= 2.0f*dt;
	if(GetAsyncKeyState('D') & 0x8000)	mTheta += 2.0f*dt;
	if(GetAsyncKeyState('W') & 0x8000)	mPhi -= 2.0f*dt;
	if(GetAsyncKeyState('S') & 0x8000)	mPhi += 2.0f*dt;
	
	if(GetAsyncKeyState('O') & 0x8000)  wireframe = false;
	if(GetAsyncKeyState('P') & 0x8000)  wireframe = true;

  if(GetAsyncKeyState(VK_NUMPAD7) & 0x8000)
    ambientWhiteness = min(ambientWhiteness + AMBIENT_WHITENESS_STEP * dt, 1.);
  if(GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
    ambientWhiteness = max(ambientWhiteness - AMBIENT_WHITENESS_STEP * dt, 0.);

  if(GetAsyncKeyState(VK_NUMPAD9) & 0x8000)
    opacity = min(opacity + AMBIENT_WHITENESS_STEP * dt, 1.);
  if(GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
    opacity = max(opacity - AMBIENT_WHITENESS_STEP * dt, 0.);

  if(GetAsyncKeyState(VK_UP) & 0x8000)
    mClippingPlane.w = min(mClippingPlane.w + AMBIENT_WHITENESS_STEP * 2 * dt, 3.);
  if(GetAsyncKeyState(VK_DOWN) & 0x8000)
    mClippingPlane.w = max(mClippingPlane.w - AMBIENT_WHITENESS_STEP * 2 * dt, -3.);

  if(GetAsyncKeyState(VK_NUMPAD1) & 0x8000) 
  {
    mClippingPlane.x = - mClippingPlane.x;
    //Sleep(50);
  }

  if(GetAsyncKeyState(VK_NUMPAD2) & 0x8000) 
  {
    mClippingPlane.y = - mClippingPlane.y;
    //Sleep(50);
  }

  if(GetAsyncKeyState(VK_NUMPAD3) & 0x8000) 
  {
    mClippingPlane.z = - mClippingPlane.z;
    //Sleep(50);
  }

  if(GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
    ambientAngle = min(ambientAngle * pow(AMBIENT_ANGLE_COEFF, dt), MAX_AMBIENT_ANGLE);
  if(GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
    ambientAngle = max(ambientAngle / pow(AMBIENT_ANGLE_COEFF, dt), MIN_AMBIENT_ANGLE);


	if(GetAsyncKeyState('1') & 0x8000)	mLightType = 0;
	if(GetAsyncKeyState('2') & 0x8000)	mLightType = 1;
	if(GetAsyncKeyState('3') & 0x8000)	mLightType = 2;
	
	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	mEyePosW.x =  5.0f*sinf(mPhi)*sinf(mTheta);
	mEyePosW.z = -5.0f*sinf(mPhi)*cosf(mTheta);
	mEyePosW.y =  5.0f*cosf(mPhi);

	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &mEyePosW, &target, &up);



  mTime = mTimer.getGameTime();

	mLights[1].pos.x = 3.0f*cosf( mTime);
	mLights[1].pos.z = 3.0f*sinf( mTime);
	mLights[1].pos.y = 0.0f;

  D3DXMatrixRotationY(&mCurrentRotation, mTime);
	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = mEyePosW;
	D3DXVec3Normalize(&mLights[2].dir, &(target-mEyePosW));
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
  D3DXCOLOR diffuseColors[] =
  {
    RED, RED, RED,  GREEN,
    BLUE, BLUE, RED,  GREEN,
    YELLOW, BLUE, BLUE,  GREEN,
    YELLOW,  YELLOW, YELLOW,  GREEN
  };
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (wireframe) md3dDevice->RSSetState(mWireframe); else md3dDevice->RSSetState(mSolidCullBack);
   
	// set constants
	mVP = mView*mProj;
	
	mfxTimeVar->SetFloat(mTime);
	
	mfxLightTypeVar->SetInt(mLightType);
	mfxLightVar->SetRawValue(&mLights[mLightType],0,sizeof(Light));
	mfxEyePosVar->SetRawValue(&mEyePosW,0,sizeof(D3DXVECTOR3));
  
  
  

  D3D10_TECHNIQUE_DESC techDesc;
  mTech->GetDesc( &techDesc );
  for(UINT p = 0; p < techDesc.Passes; ++p)
  {
    mfxClippingPlaneVar->SetRawValue(&D3DXVECTOR4(0,0,0,0),0,sizeof(D3DXVECTOR4));  
    md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);       
    for (int i =  0; i < 8; ++i)
      for (int j =  0; j < 8; ++j)
      {

        float offsX = i * 0.5f - 2.f;
        float offsY = j * 0.5f - 2.f;
        D3DXMATRIX temp1, temp2;
        D3DXMatrixMultiply(&temp1, &mCurrentRotation, D3DXMatrixTranslation(&temp1, offsX, offsY, 0.0f));
        D3DXMatrixScaling(&temp2, 0.1f, 0.1f, 0.1f);
        
        D3DXMatrixMultiply(&mWorld, &temp2, &temp1);
        
        mWVP = mWorld*mVP;
		    mfxWVPVar->SetMatrix((float*)&mWVP);
		    mfxWorldVar->SetMatrix((float*) &mWorld);
        
        D3DXCOLOR curSpecularColor = diffuseColors[(4 * j + i) % 16] * (1 - ambientWhiteness) +
                                     WHITE                    * ambientWhiteness;
        curSpecularColor.a = ambientAngle;
        mfxColorSpecular->SetRawValue(&curSpecularColor, 0, sizeof(D3DXCOLOR)); 
        diffuseColors[(4 * j + i) % 16].a = opacity;
        mfxColorDiffuse->SetRawValue(&diffuseColors[(4 * j + i) % 16], 0, sizeof(D3DXCOLOR));

        mTech->GetPassByIndex( p )->Apply(0);
        md3dDevice->RSSetState(mSolidCullBack);
        mBox.draw();
      }    
    
    mfxClippingPlaneVar->SetRawValue(&mClippingPlane,0,sizeof(D3DXVECTOR4));  
    md3dDevice->OMSetBlendState(mTransparentBS, blendFactors, 0xffffffff);    
    for (int i =  0; i < 8; ++i)
      for (int j =  0; j < 8; ++j)
      {

        float offsX = i * 0.5f - 2.f;
        float offsY = j * 0.5f - 2.f;
        D3DXMATRIX temp1, temp2;
        D3DXMatrixTranslation(&temp1, offsX, offsY, 0.0f);
        D3DXMatrixScaling(&temp2, 0.25f, 0.25f, 0.25f);
        D3DXMatrixMultiply(&mWorld, &temp2, &temp1);

        mWVP = mWorld*mVP;
		    mfxWVPVar->SetMatrix((float*)&mWVP);
		    mfxWorldVar->SetMatrix((float*) &mWorld);
        D3DXCOLOR curSpecularColor = diffuseColors[(4 * j + i) % 16] * (1 - ambientWhiteness) +
                                     WHITE                    * ambientWhiteness;
        curSpecularColor.a = ambientAngle;
        mfxColorSpecular->SetRawValue(&curSpecularColor, 0, sizeof(D3DXCOLOR)); 
        diffuseColors[(4 * j + i) % 16].a = opacity;
        mfxColorDiffuse->SetRawValue(&diffuseColors[(4 * j + i) % 16], 0, sizeof(D3DXCOLOR));

        mTech->GetPassByIndex( p )->Apply(0);
        md3dDevice->RSSetState(mSolidCullFront);
        mBox.draw();

        md3dDevice->RSSetState(mSolidCullBack);
        mBox.draw();
      }
      
    }
	md3dDevice->RSSetState(0);
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"effects.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");
	
	mfxWorldVar = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	
	mfxLightTypeVar = mFX->GetVariableByName("gLightType")->AsScalar();
	mfxLightVar = mFX->GetVariableByName("gLight");
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
  mfxClippingPlaneVar = mFX->GetVariableByName("gClippingPlane");

  mfxColorDiffuse = mFX->GetVariableByName("gColorDiffuse");
  mfxColorSpecular = mFX->GetVariableByName("gColorSpecular");
  
  mfxTimeVar = mFX->GetVariableByName("gTime")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0}

	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 4, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
void ColoredCubeApp::buildRasterizerStates()
{
	D3D10_RASTERIZER_DESC rsDesc;
	
	ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.FillMode = D3D10_FILL_WIREFRAME;
	rsDesc.CullMode = D3D10_CULL_NONE;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
	HR(md3dDevice->CreateRasterizerState(&rsDesc, &mWireframe));

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.FillMode = D3D10_FILL_SOLID;
	rsDesc.CullMode = D3D10_CULL_FRONT;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &mSolidCullFront));

  ZeroMemory(&rsDesc, sizeof(D3D10_RASTERIZER_DESC));
	rsDesc.FillMode = D3D10_FILL_SOLID;
	rsDesc.CullMode = D3D10_CULL_BACK;
  rsDesc.MultisampleEnable = true;
  rsDesc.AntialiasedLineEnable = true;
  HR(md3dDevice->CreateRasterizerState(&rsDesc, &mSolidCullBack));

}

void ColoredCubeApp::buildLights()
{

	mLightType = 0;
 
	// Parallel light.
	mLights[0].dir      = D3DXVECTOR3(0.0, -0.0f, 1.0f);
	mLights[0].ambient  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	mLights[0].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
	mLights[0].specular = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
 
	// Pointlight--position is changed every frame to animate.
	mLights[1].ambient  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	mLights[1].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[1].specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	mLights[1].att.x    = 0.0f;
	mLights[1].att.y    = 0.5f;
	mLights[1].att.z    = 0.0f;
	mLights[1].range    = 50.0f;

	// Spotlight--position and direction changed every frame to animate.
	mLights[2].ambient  = D3DXCOLOR(0.2f, 0.2f, 0.4f, 1.0f);
	mLights[2].diffuse  = D3DXCOLOR(0.4f, 0.4f, 1.0f, 1.0f);
	mLights[2].specular = D3DXCOLOR(0.4f, 0.4f, 1.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 4.0f;
	mLights[2].range    = 10000.0f;
}

void ColoredCubeApp::buildBlendingStates()
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

  HR(md3dDevice->CreateBlendState(&blendDesc, &mTransparentBS));
}


void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}
