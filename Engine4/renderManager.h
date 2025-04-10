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
//#include "textureshaderclass.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstdio>
#include <string>
#include <iomanip> 

#include "LightShaderClass.h"
#include "LightClass.h"

#include "rendertextureclass.h"
#include "displayplaneclass.h"
#include "depthshaderclass.h" 
#include "shadowshaderclass.h" 


/////////////
// GLOBALS //
/////////////

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;

const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 4096;
const int SHADOWMAP_HEIGHT = 4096;


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
	bool Update(HWND hwnd);



	void TurnLeft();
	void TurnRight();
	void RotateCameraUp();
	void RotateCameraDown();
	void UpdateMouseMovement(int deltaX, int deltaY);
	void UpdateCameraPosition();
	void UpdateMouseWheel(int wheelDelta); // Новый метод для зума


	void MoveBarrelForward();
	void MoveBarrelBackward();
	void MoveBarrelLeft();
	void MoveBarrelRight();

	XMFLOAT3 GetCameraDirection(); // Новый метод для получения вектора направления
	// Другие методы...

	



private:
	bool Render(HWND hwnd);
	bool RenderSceneToTexture();


	MyDirectX* m_Direct3D;
	CameraClass* m_Camera;

	//Lab3
	ModelClass* Planet;
	ModelClass* Floor;


	float timeGame = 0.0f;

	ModelClass* Barrel[10];

	bool isMovingForward = false;
	bool isMovingBackward = false;
	bool isMovingLeft = false;
	bool isMovingRight = false;

	float rotationAngle = 0.0f;
	float currentAngle = 0.0f;

	float cameraDistance; // Расстояние от центра планеты до камеры
	float cameraYaw;      // Угол поворота вокруг оси Y (горизонталь)
	float cameraPitch;    // Угол наклона (вертикаль)

	//Lab5
	ShadowShaderClass* m_ShadowShader;
	LightClass* m_Light;

	RenderTextureClass* m_RenderTexture;
	DepthShaderClass* m_DepthShader;


	float m_shadowMapBias;

};

#endif
