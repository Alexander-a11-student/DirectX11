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
	m_Camera->SetPosition(-0.5f, 3.0f, -4.0f); // Устанавливаем камеру над бочкой
	m_Camera->SetRotation(45.0f, 0.0f, 0.0f); // Наклоняем камеру вниз

	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");

	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	//Бочка

	char modelFilenameBarrel[128];
	strcpy_s(modelFilenameBarrel, "../Engine4/Barrel.txt");

	char textureFilenameBarrel[128];
	strcpy_s(textureFilenameBarrel, "../Engine4/MyBarrel.tga");


	for (int i = 0; i < 100; i++)
	{
		Planet[i] = new ModelClass();
		result = Planet[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet, modelFilenamePlanet);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize a moving model object.", L"Error", MB_OK);
			return false;
		}

		// Случайное распределение объектов Planet[i] по карте
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
		Planet[i]->SetPosition(x, 0.0f, z);
	}


	Barrel = new ModelClass();
	result = Barrel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenameBarrel, modelFilenameBarrel);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the barrel model object.", L"Error", MB_OK);
		return false;
	}


	Barrel->SetPosition(0.0f, 0.0f, 0.0f);

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


//Игра
//lab4
void RenderManager::MoveBarrelForward()
{
	// Устанавливаем флаг, что бочка движется вперед
	isMovingForward = true;

	// Перемещение бочки вперед
	XMFLOAT3 position = Barrel->GetPosition();
	position.z += 0.1f;
	Barrel->SetPosition(position.x, position.y, position.z);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
	//m_Camera->SetLookAt(position.x, position.y, position.z);
}


void RenderManager::MoveBarrelBackward()
{
	// Устанавливаем флаг, что бочка движется назад
	isMovingBackward = true;

	// Перемещение бочки назад
	XMFLOAT3 position = Barrel->GetPosition();
	position.z -= 0.1f;
	Barrel->SetPosition(position.x, position.y, position.z);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
	//m_Camera->SetLookAt(position.x, position.y, position.z);
}

void RenderManager::MoveBarrelLeft()
{
	// Устанавливаем флаг, что бочка движется влево
	isMovingLeft = true;

	// Поворот бочки на 90 градусов влево
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	Barrel->SetRotationMatrix(rotationMatrix);

	// Перемещение бочки влево
	XMFLOAT3 position = Barrel->GetPosition();
	position.x -= 0.1f;
	Barrel->SetPosition(position.x, position.y, position.z);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
}

void RenderManager::MoveBarrelRight()
{
	// Устанавливаем флаг, что бочка движется вправо
	isMovingRight = true;

	// Поворот бочки на 90 градусов вправо
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	Barrel->SetRotationMatrix(rotationMatrix);

	// Перемещение бочки вправо
	XMFLOAT3 position = Barrel->GetPosition();
	position.x += 0.1f;
	Barrel->SetPosition(position.x, position.y, position.z);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
}




//Перемещение
//lab3 

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






void RenderManager::UpdatePlanets()
{
	for (int i = 0; i < 100; i++)
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


	for (int i = 0; i < 100; i++)
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

	if (Barrel)
	{
		XMFLOAT3 localPosition = Barrel->GetPosition();

		float x = localPosition.x;
		float y = localPosition.y;
		float z = localPosition.z;

		XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);

		// Поворот бочки на 90 градусов вокруг оси Z
		XMMATRIX initialRotation = XMMatrixRotationZ(XMConvertToRadians(90.0f));

		// Вращение бочки вокруг своей оси, если она движется вперед, назад, влево или вправо
		if (isMovingForward || isMovingBackward || isMovingLeft || isMovingRight)
		{
			rotationAngle += (isMovingForward || isMovingRight ? -0.1f : -0.1f);
			XMMATRIX rotationMatrix = XMMatrixRotationY(rotationAngle); // Вращение вокруг оси Z
			initialRotation = XMMatrixMultiply(rotationMatrix, initialRotation);
			isMovingForward = false; // Сбрасываем флаг
			isMovingBackward = false; // Сбрасываем флаг
			isMovingLeft = false; // Сбрасываем флаг
			isMovingRight = false; // Сбрасываем флаг
		}

		XMMATRIX modelMatrix = XMMatrixMultiply(initialRotation, translationMatrix);

		Barrel->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Barrel->GetIndexCount(), modelMatrix, viewMatrix, projectionMatrix, Barrel->GetTexture());
		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
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
	for (int i = 0; i < 100; i++)
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


