////////////////////////////////////////////////////////////////////////////////
// Filename: renderManager.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_


//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include "DirectX.h"
#include "cameraclass.h" 
#include "modelclass.h" 
//#include "colorshaderclass.h" 
#include "textureshaderclass.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdio>
#include <string>
#include <iomanip> 

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class RenderManager
{
public:
	RenderManager();
	RenderManager(const RenderManager&);
	~RenderManager();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Update();



	void TurnLeft();
	void TurnRight();
	void RotateCameraUp();
	void RotateCameraDown();
	void UpdateMouseMovement(int deltaX, int deltaY);

	void UpdatePlanets();
	void FixCameraToPlanet(int planetIndex);
	void ReleaseCamera();
	void UpdateCamera();

	void MoveBarrelForward();
	void MoveBarrelBackward();

	bool isCameraFixed = false;
	int fixedPlanetIndex = -1;
	XMFLOAT3 cameraOffset = XMFLOAT3(5.0f, 5.0f, 5.0f); // Смещение от планеты




	



private:
	bool Render();
	MyDirectX* m_Direct3D;
	CameraClass* m_Camera;
	TextureShaderClass* m_TextureShader;

	//Lab3
	ModelClass* Planet[100];
	float timeGame = 0.0f;

	ModelClass* Barrel;
	bool isMovingForward = false;
	bool isMovingBackward = false;
	float rotationAngle = 0.0f;

};

#endif
