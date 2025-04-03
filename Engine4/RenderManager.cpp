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
	srand(static_cast<unsigned>(time(nullptr)));

	bool result;

	m_Direct3D = new MyDirectX;
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, false, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	m_Camera->SetPosition(0.0f, 200.0f, 0.0f);
	m_Camera->SetRotation(0.0f, -89.0f, 0.0f);

	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");

	char modelFilenamePlanet_0[128];
	strcpy_s(modelFilenamePlanet_0, "../Engine4/sphere2.txt");

	char modelFilenamePlanet_1[128];
	strcpy_s(modelFilenamePlanet_1, "../Engine4/sphere3.txt");




	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	char textureFilenamePlanet_0[128];
	strcpy_s(textureFilenamePlanet_0, "../Engine4/Circle2.tga");

	char textureFilenamePlanet_1[128];
	strcpy_s(textureFilenamePlanet_1, "../Engine4/Sun.tga");




	// Инициализация центральной звезды (индекс 0)
	Planet[0] = new ModelClass();
	result = Planet[0]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet_1, modelFilenamePlanet_1);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize central star.", L"Error", MB_OK);
		return false;
	}
	Planet[0]->SetPosition(0.0f, 0.0f, 0.0f);
	Planet[0]->SetSelfRotationSpeed(0.5f);
	Planet[0]->SetParent(-1); // Нет родителя

	// Инициализация 100 ключевых планет вокруг звезды с хаотичным распределением
	int numPlanets = 100;
	for (int i = 1; i <= numPlanets; i++) {
		Planet[i] = new ModelClass();
		result = Planet[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet, modelFilenamePlanet);
		if (!result) {
			MessageBox(hwnd, L"Could not initialize a planet.", L"Error", MB_OK);
			return false;
		}

		// Хаотичная дистанция и угол
		float angle = static_cast<float>(rand()) / RAND_MAX * XM_2PI; // Случайный угол
		float baseDistance = 50.0f + static_cast<float>(rand()) / RAND_MAX * 500.0f; // Случайная базовая дистанция
		float distanceVariation = static_cast<float>(rand()) / RAND_MAX * 10.0f; // Дополнительное случайное отклонение

		float distance = baseDistance + distanceVariation;

		Planet[i]->SetOrbitParameters(distance, 0.1f + static_cast<float>(rand()) / RAND_MAX * 0.5f); // Случайная скорость орбиты
		Planet[i]->SetSelfRotationSpeed(0.1f + static_cast<float>(rand()) / RAND_MAX * 0.4f); // Случайная скорость вращения
		Planet[i]->SetParent(0); // Родитель - центральная звезда
		Planet[0]->AddSatellite(i); // Добавляем планету как спутник звезды

		float x = distance * cos(angle);
		float z = distance * sin(angle);
		Planet[i]->SetPosition(x, 0.0f, z);
	}

	// Инициализация спутников для планет (луны) и их собственных мини-спутников
	int remainingObjects = 900; // Оставшиеся объекты после центральной звезды и планет
	for (int i = numPlanets + 1; i < 1000 && remainingObjects > 0; i++) {
		// Случайно выбираем, будет ли это луна или мини-спутник
		int parentType = rand() % 2; // 0 - планета, 1 - уже существующий спутник
		int maxParentIndex = (parentType == 0) ? numPlanets : (i - 1); // Максимальный индекс родителя

		if (maxParentIndex < 1) continue; // Если нет подходящих родителей, пропускаем

		int parentIndex = 1 + (rand() % maxParentIndex); // Случайный родитель

		Planet[i] = new ModelClass();
		result = Planet[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet_0, modelFilenamePlanet_0);
		if (!result) {
			MessageBox(hwnd, L"Could not initialize a moon or mini-moon.", L"Error", MB_OK);
			return false;
		}

		// Меньшие орбиты для спутников
		float angle = static_cast<float>(rand()) / RAND_MAX * XM_2PI;
		float baseMoonDistance = 10.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f;
		float moonDistance = baseMoonDistance * (parentType == 0 ? 1.0f : 0.5f); // Мини-спутники ближе

		Planet[i]->SetOrbitParameters(moonDistance, 0.5f + static_cast<float>(rand()) / RAND_MAX * 4.0f); // Более быстрая орбита для спутников
		Planet[i]->SetSelfRotationSpeed(0.3f + static_cast<float>(rand()) / RAND_MAX * 0.7f);
		Planet[i]->SetParent(parentIndex); // Устанавливаем родителя
		Planet[parentIndex]->AddSatellite(i); // Добавляем как спутник

		XMFLOAT3 parentPos = Planet[parentIndex]->GetPosition();
		float x = parentPos.x + moonDistance * cos(angle);
		float z = parentPos.z + moonDistance * sin(angle);
		Planet[i]->SetPosition(x, 0.0f, z);

		remainingObjects--;
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

void RenderManager::MoveForward()
{
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
	float speed = 0.2f;
	XMFLOAT3 position = m_Camera->GetPosition();
	position.y += speed;
	m_Camera->SetPosition(position.x, position.y, position.z);
}

void RenderManager::MoveDown()
{
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



void RenderManager::UpdatePlanets() {
	timeGame += 0.016f; // Пример: обновление каждые 16 мс (60 FPS)

	for (int i = 0; i < 1000; i++) {
		if (Planet[i]) {
			if (Planet[i]->GetParent() == -1) {

				// Собственное вращение (вращение вокруг своей оси)
				float selfAngle = timeGame * Planet[i]->GetSelfRotationSpeed();
				XMMATRIX rotationMatrix = XMMatrixRotationY(selfAngle);
				Planet[i]->SetRotationMatrix(rotationMatrix);
				// Центральная звезда неподвижна
				continue;
			}

			int parentIndex = Planet[i]->GetParent();
			XMFLOAT3 parentPos = Planet[parentIndex]->GetPosition();

			// Вращение вокруг родителя
			float orbitSpeed = Planet[i]->GetOrbitSpeed();
			float angle = timeGame * orbitSpeed;
			float orbitRadius = Planet[i]->GetOrbitRadius();

			float x = parentPos.x + orbitRadius * cos(angle);
			float z = parentPos.z + orbitRadius * sin(angle);

			// Обновляем позицию
			Planet[i]->SetPosition(x, 0.0f, z);

			// Собственное вращение (вращение вокруг своей оси)
			float selfAngle = timeGame * Planet[i]->GetSelfRotationSpeed();
			XMMATRIX rotationMatrix = XMMatrixRotationY(selfAngle);
			Planet[i]->SetRotationMatrix(rotationMatrix);
		}
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


