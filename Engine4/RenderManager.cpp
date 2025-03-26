////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "renderManager.h"

RenderManager::RenderManager()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;

}


RenderManager::RenderManager(const RenderManager& other)
{

}


RenderManager::~RenderManager()
{

}

bool RenderManager::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	srand(time(nullptr));

	bool result;

	m_Direct3D = new MyDirectX;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, false, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 0.0f, 0.0f);
	m_Camera->SetRotation(-70.0f, -70.0f, 0.0f);

	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");

	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	for (int i = 0; i < 1000; i++)
	{
		Planet[i] = new ModelClass();
		result = Planet[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet, modelFilenamePlanet);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize a moving model object.", L"Error", MB_OK);
			return false;
		}

		float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * XM_2PI;
		float distance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 50.0f + 10.0f; ;
		float x = distance * cos(angle);
		float z = distance * sin(angle);
		Planet[i]->SetPosition(x, 0.0f, z);
	}

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}

//void RenderManager::ChangeLab() {
//	NumberLab = !NumberLab;
//}




//Перемещение
//lab3 
void RenderManager::MoveForward()
{
	if (isCameraFixed) return;
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rotation = m_Camera->GetRotation();

	XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	forward = XMVector3TransformCoord(forward, rotationMatrix);

	position.x += XMVectorGetX(forward) * speed;
	position.z += XMVectorGetZ(forward) * speed;

	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveBackward()
{
	if (isCameraFixed) return;
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rotation = m_Camera->GetRotation();

	XMVECTOR backward = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	backward = XMVector3TransformCoord(backward, rotationMatrix);

	position.x += XMVectorGetX(backward) * speed;
	position.z += XMVectorGetZ(backward) * speed;

	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveLeft()
{
	if (isCameraFixed) return;
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rotation = m_Camera->GetRotation();

	XMVECTOR left = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	left = XMVector3TransformCoord(left, rotationMatrix);

	position.x += XMVectorGetX(left) * speed;
	position.z += XMVectorGetZ(left) * speed;

	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveRight()
{
	if (isCameraFixed) return;
	float speed = 0.05f;
	XMFLOAT3 position = m_Camera->GetPosition();
	XMFLOAT3 rotation = m_Camera->GetRotation();

	XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(rotation.y));
	right = XMVector3TransformCoord(right, rotationMatrix);

	position.x += XMVectorGetX(right) * speed;
	position.z += XMVectorGetZ(right) * speed;

	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveUp()
{
	if (isCameraFixed) return;
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	position.y += speed;
	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveDown()
{
	if (isCameraFixed) return;
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	position.y -= speed;
	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::TurnLeft()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 2.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();
	rotation.y -= rotationSpeed;
	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}

void RenderManager::TurnRight()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 2.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();
	rotation.y += rotationSpeed;
	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}


void RenderManager::RotateCameraUp()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 2.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.x -= rotationSpeed;  // Вращаем камеру вверх

	// Ограничиваем наклон
	if (rotation.x < -89.0f) rotation.x = -89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}

void RenderManager::RotateCameraDown()
{
	//if (isCameraFixed) return; 
	float rotationSpeed = 2.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.x += rotationSpeed;  // Вращаем камеру вниз

	// Ограничиваем наклон
	if (rotation.x > 89.0f) rotation.x = 89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}


void RenderManager::UpdateMouseMovement(int deltaX, int deltaY)
{
	//if (isCameraFixed) return;

	float sensitivity = 0.1f; // Настроим чувствительность

	float yaw = deltaX * sensitivity;
	float pitch = deltaY * sensitivity; // Инвертируем движение вверх-вниз

	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.y += yaw;   // Вращение влево-вправо
	rotation.x += pitch; // Вращение вверх-вниз (без инверсии)

	// Ограничим наклон головы, чтобы камера не переворачивалась
	if (rotation.x > 89.0f) rotation.x = 89.0f;
	if (rotation.x < -89.0f) rotation.x = -89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}

//void RenderManager::UpPlatformLeft() {
//
//	XMFLOAT3 localPosition = m_ModelsPlatformsPong[1]->GetPosition();
//
//	float x = localPosition.x;
//	float y;
//	if (localPosition.y < 6) {
//		y = localPosition.y + 0.4f;
//	}
//	else y = localPosition.y;
//	float z = localPosition.z;
//	m_ModelsPlatformsPong[1]->SetPosition(x, y, z);
//}
//
//void RenderManager::DownPlatformLeft() {
//	XMFLOAT3 localPosition = m_ModelsPlatformsPong[1]->GetPosition();
//
//	float x = localPosition.x;
//	float y;
//	if (localPosition.y > -6) {
//		y = localPosition.y - 0.4f;
//	} else y = localPosition.y;
//	float z = localPosition.z;
//
//	m_ModelsPlatformsPong[1]->SetPosition(x, y, z);
//}
//
//void RenderManager::UpPlatformRight() {
//	XMFLOAT3 localPosition = m_ModelsPlatformsPong[0]->GetPosition();
//
//	float x = localPosition.x;
//	float y;
//	if (localPosition.y < 6) {
//		y = localPosition.y + 0.4f;
//	}
//	else y = localPosition.y;
//	float z = localPosition.z;
//
//	m_ModelsPlatformsPong[0]->SetPosition(x, y, z);
//}
//
//void RenderManager::DownPlatformRight() {
//	XMFLOAT3 localPosition = m_ModelsPlatformsPong[0]->GetPosition();
//	
//	float x = localPosition.x;
//	float y;
//	if (localPosition.y > -6) {
//		y = localPosition.y - 0.4f;
//	}
//	else y = localPosition.y;
//	float z = localPosition.z;
//	m_ModelsPlatformsPong[0]->SetPosition(x, y, z);
//}
//
//void RenderManager::ResizePlatform(ModelClass* model, int countScale) {
//
//	XMFLOAT3 SizePlatform = model->GetSize();
//	float scaleFactorY = 0.2f;
//	float scaleFactorXZ = 0.1f;
//
//	if (countScale < 18) {
//		SizePlatform.x = SizePlatform.x - countScale / 120.0f;
//		SizePlatform.y = SizePlatform.y - countScale / 22.0f;
//		SizePlatform.z = SizePlatform.z - countScale / 120.0f;
//		model->SetSize(SizePlatform.x, SizePlatform.y, SizePlatform.z);
//	}
//	else {
//		model->SetSize(0.2f, 0.4f, 0.2f);
//	}
//}





void RenderManager::UpdatePlanets()
{
	for (int i = 0; i < 1000; i++)
	{
		if (Planet[i])
		{
			float angle = timeGame * (0.5f + static_cast<float>(i) / 10.0f);
			XMFLOAT3 position = Planet[i]->GetPosition();
			float distance = sqrt(position.x * position.x + position.z * position.z);
			float x = distance * cos(angle);
			float z = distance * sin(angle);
			Planet[i]->SetPosition(x, 0.0f, z);

			float selfAngle = timeGame * 100.0f;
			XMMATRIX rotationMatrix = XMMatrixRotationY(selfAngle);
			Planet[i]->SetRotationMatrix(rotationMatrix);
		}
	}
}

void RenderManager::FixCameraToPlanet(int planetIndex)
{
	if (planetIndex >= 0 && planetIndex < 3) {
		isCameraFixed = true;
		fixedPlanetIndex = planetIndex;
		m_Camera->SetLookAt(-90, -30, 0);
	}
}

void RenderManager::ReleaseCamera()
{
	isCameraFixed = false;
	fixedPlanetIndex = -1;
	m_Camera->SetLookAt(0.0f, 0.0f, 1.0f);
}

void RenderManager::UpdateCamera()
{
	if (isCameraFixed && fixedPlanetIndex >= 0) {
		XMFLOAT3 planetPos = Planet[fixedPlanetIndex]->GetPosition();

		// Параметры орбиты камеры
		float cameraOrbitRadius = 3.0f; // Расстояние камеры от планеты
		float cameraOrbitSpeed = 0.2f;  // Скорость вращения камеры вокруг планеты

		if (fixedPlanetIndex == 2) {
			cameraOrbitRadius = 6.0f;
		}

		// Вычисление угла движения камеры
		float cameraAngle = timeGame * cameraOrbitSpeed;

		// Новая позиция камеры по орбите
		float camX = planetPos.x + cameraOrbitRadius * cos(cameraAngle);
		float camZ = planetPos.z + cameraOrbitRadius * sin(cameraAngle);
		float camY = planetPos.y + 1.0f; // Немного выше планеты для лучшего обзора

		// Установка позиции камеры
		m_Camera->SetPosition(camX, camY, camZ);

		//Поворачиваем камеру в сторону планеты
	}
}






bool RenderManager::Render()
{


	timeGame += 0.0001f;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 0.0f);

	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	UpdatePlanets();
	//UpdateCamera();

	for (int i = 0; i < 1000; i++)
	{
		if (Planet[i])
		{
			XMFLOAT3 localPosition = Planet[i]->GetPosition();
			float x = localPosition.x;
			float y = localPosition.y;
			float z = localPosition.z;

			XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);
			XMMATRIX rotationMatrix = Planet[i]->GetRotationMatrix();
			XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

			Planet[i]->Render(m_Direct3D->GetDeviceContext());
			result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Planet[i]->GetIndexCount(), modelMatrix, viewMatrix, projectionMatrix, Planet[i]->GetTexture());
			if (!result) return false;
			worldMatrix = XMMatrixIdentity();
		}
	}




	//Конец СЦЕНЫ
	m_Direct3D->EndScene();

	return true;
}

bool RenderManager::Update()
{
	bool result;

	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

void RenderManager::Shutdown()
{
	for (int i = 0; i < 1000; i++)
	{
		if (Planet[i])
		{
			Planet[i]->Shutdown();
			delete Planet[i];
			Planet[i] = nullptr;
		}
	}


	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


