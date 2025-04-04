﻿////////////////////////////////////////////////////////////////////////////////
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
	m_Camera->SetPosition(0.0f, 10.0f, -10.0f); // Устанавливаем камеру над бочкой
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f); // Наклоняем камеру вниз

	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");
		
	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	//Бочка

	char modelFilenameBarrel[128];
	strcpy_s(modelFilenameBarrel, "../Engine4/Barrel.txt");

	char textureFilenameBarrel[128];
	strcpy_s(textureFilenameBarrel, "../Engine4/MyBarrel.tga");


	for (int i = 0; i < 10; i++)
	{
		Barrel[i] = new ModelClass();
		result = Barrel[i]->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenameBarrel, modelFilenameBarrel);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize a moving model object.", L"Error", MB_OK);
			return false;
		}

		// Случайное распределение объектов Planet[i] по карте
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 20.0f;
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 20.0f;
		Barrel[i]->SetPosition(x, -0.2f, z);
		Barrel[i]->SetSize(0.9f);
	}


	Planet = new ModelClass();
	result = Planet->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet, modelFilenamePlanet);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the barrel model object.", L"Error", MB_OK);
		return false;
	}


	Planet->SetPosition(0.0f, 0.0f, 0.0f);
	Planet->SetSize(2.5f);

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}

//Повороты сферы
void RenderManager::MoveBarrelForward() 
{
	planetRotationAngle += 0.1f;

	// Устанавливаем флаг, что бочка движется вперед
	isMovingForward = true;

	// Перемещение бочки вперед
	XMFLOAT3 position = Planet->GetPosition();
	position.z += 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// Фиксированное вращение вокруг оси X (например, 5 градусов за движение)
	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f); // Фиксированный угол (5 градусов)

	// Получаем текущую матрицу вращения
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}

	// Добавляем фиксированное вращение вокруг оси X (вперед)
	XMMATRIX additionalRotation = XMMatrixRotationX(rotationAngleIncrement);
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);

	// Сохраняем новую матрицу вращения
	Planet->SetRotationMatrix(newRotation);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x, position.y + 10.0f, position.z - 10.0f);
}
void RenderManager::MoveBarrelBackward()
{
	planetRotationAngle += -0.1f;
	// Устанавливаем флаг, что бочка движется назад
	isMovingBackward = true;

	// Перемещение бочки назад
	XMFLOAT3 position = Planet->GetPosition();
	position.z -= 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// Фиксированное вращение вокруг оси X (например, 5 градусов за движение)
	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f); // Фиксированный угол (5 градусов)

	// Получаем текущую матрицу вращения
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}

	// Добавляем фиксированное вращение вокруг оси X (назад)
	XMMATRIX additionalRotation = XMMatrixRotationX(-rotationAngleIncrement); // Обратное вращение
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);

	// Сохраняем новую матрицу вращения
	Planet->SetRotationMatrix(newRotation);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x, position.y + 10.0f, position.z - 10.0f);
}
void RenderManager::MoveBarrelLeft()
{
	isMovingLeft = true;

	// Перемещение бочки влево
	XMFLOAT3 position = Planet->GetPosition();
	position.x -= 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// Фиксированное вращение вокруг оси Z (влево)
	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f); // Фиксированный угол (5 градусов)

	// Получаем текущую матрицу вращения
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}

	// Убираем резкий поворот на 90 градусов вокруг оси Y и добавляем фиксированное вращение вокруг оси Z
	XMMATRIX additionalRotation = XMMatrixRotationZ(rotationAngleIncrement);
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);

	// Сохраняем новую матрицу вращения
	Planet->SetRotationMatrix(newRotation);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x, position.y + 10.0f, position.z - 10.0f);
}
void RenderManager::MoveBarrelRight()
{
	isMovingRight = true;

	// Перемещение бочки вправо
	XMFLOAT3 position = Planet->GetPosition();
	position.x += 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// Фиксированное вращение вокруг оси Z (вправо)
	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f); // Фиксированный угол (5 градусов)

	// Получаем текущую матрицу вращения
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}

	// Убираем резкий поворот на 90 градусов вокруг оси Y и добавляем фиксированное вращение вокруг оси Z
	XMMATRIX additionalRotation = XMMatrixRotationZ(-rotationAngleIncrement);
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);

	// Сохраняем новую матрицу вращения
	Planet->SetRotationMatrix(newRotation);

	// Обновление позиции камеры
	m_Camera->SetPosition(position.x, position.y + 10.0f, position.z - 10.0f);
}


//Повороты камеры
void RenderManager::TurnLeft()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 5.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();
	rotation.y -= rotationSpeed;
	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}
void RenderManager::TurnRight()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 5.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();
	rotation.y += rotationSpeed;
	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}
void RenderManager::RotateCameraUp()
{
	//if (isCameraFixed) return;
	float rotationSpeed = 5.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.x -= rotationSpeed;  // Вращаем камеру вверх

	// Ограничиваем наклон
	if (rotation.x < -89.0f) rotation.x = -89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}
void RenderManager::RotateCameraDown()
{
	//if (isCameraFixed) return; 
	float rotationSpeed = 5.0f;
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



bool RenderManager::Render(HWND hwnd)
{
	timeGame += 0.0001f;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 0.0f);

	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	for (int i = 0; i < 10; i++) {
		if (!Barrel[i]->IsAttached() && Planet->CheckCollisionSphere(Barrel[i])) {
			Barrel[i]->SetAttached(true);

			XMFLOAT3 planetPos = Planet->GetPosition();
			XMFLOAT3 barrelPos = Barrel[i]->GetPosition();

			// Вычисляем вектор от центра планеты к бочке
			XMFLOAT3 direction = {
				barrelPos.x - planetPos.x,
				barrelPos.y - planetPos.y,
				barrelPos.z - planetPos.z
			};

			// Нормализуем вектор направления
			float length = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
			if (length > 0.0f) {
				direction.x /= length;
				direction.y /= length;
				direction.z /= length;
			}

			// Устанавливаем бочку на поверхность планеты
			float planetRadius = Planet->GetSize().x / 2.0f;
			float barrelRadius = Barrel[i]->GetSize().x / 2.0f;
			float distanceToSurface = planetRadius + barrelRadius;

			XMFLOAT3 globalOffset = {
				direction.x * distanceToSurface,
				direction.y * distanceToSurface,
				direction.z * distanceToSurface
			};

			// Получаем текущую матрицу вращения планеты
			XMMATRIX planetRotation = Planet->GetRotationMatrix();
			if (XMMatrixIsIdentity(planetRotation)) {
				planetRotation = XMMatrixIdentity();
			}

			// Вычисляем обратную матрицу вращения планеты
			XMMATRIX inversePlanetRotation = XMMatrixInverse(nullptr, planetRotation);
			Barrel[i]->SetInitialInverseRotation(inversePlanetRotation);

			// Преобразуем глобальное смещение в локальное
			XMVECTOR globalOffsetVector = XMLoadFloat3(&globalOffset);
			XMVECTOR localOffsetVector = XMVector3Transform(globalOffsetVector, inversePlanetRotation);
			XMFLOAT3 localOffset;
			XMStoreFloat3(&localOffset, localOffsetVector);
			Barrel[i]->SetLocalOffset(localOffset);

			// Сохраняем начальную матрицу вращения бочки
			XMMATRIX barrelRotation = Barrel[i]->GetRotationMatrix();
			if (XMMatrixIsIdentity(barrelRotation)) {
				barrelRotation = XMMatrixIdentity();
			}
			Barrel[i]->SetInitialRotation(barrelRotation);

			// Устанавливаем начальную позицию
			Barrel[i]->SetPosition(
				planetPos.x + globalOffset.x,
				planetPos.y + globalOffset.y,
				planetPos.z + globalOffset.z
			);

			Barrel[i]->SetOffset(globalOffset);
		}
		else if (Barrel[i]->IsAttached()) {
			XMFLOAT3 planetPos = Planet->GetPosition();
			XMFLOAT3 localOffset = Barrel[i]->GetLocalOffset();

			// Получаем текущую матрицу вращения планеты
			XMMATRIX planetRotation = Planet->GetRotationMatrix();
			if (XMMatrixIsIdentity(planetRotation)) {
				planetRotation = XMMatrixIdentity();
			}

			// Преобразуем локальное смещение в глобальное
			XMVECTOR localOffsetVector = XMLoadFloat3(&localOffset);
			XMVECTOR globalOffsetVector = XMVector3Transform(localOffsetVector, planetRotation);
			XMFLOAT3 globalOffset;
			XMStoreFloat3(&globalOffset, globalOffsetVector);

			// Обновляем позицию бочки
			Barrel[i]->SetPosition(
				planetPos.x + globalOffset.x,
				planetPos.y + globalOffset.y,
				planetPos.z + globalOffset.z
			);

			// Получаем начальную матрицу вращения бочки
			XMMATRIX initialBarrelRotation = Barrel[i]->GetInitialRotation();
			if (XMMatrixIsIdentity(initialBarrelRotation)) {
				initialBarrelRotation = XMMatrixIdentity();
			}

			// Получаем начальную обратную матрицу вращения планеты
			XMMATRIX initialInversePlanetRotation = Barrel[i]->GetInitialInverseRotation();

			// Вычисляем относительное вращение планеты с момента прикрепления
			XMMATRIX relativeRotation = XMMatrixMultiply(initialInversePlanetRotation, planetRotation);

			// Применяем относительное вращение планеты к начальной ориентации бочки
			XMMATRIX barrelRotation = XMMatrixMultiply(initialBarrelRotation, relativeRotation);

			// Устанавливаем новую матрицу вращения бочки
			Barrel[i]->SetRotationMatrix(barrelRotation);
		}

		if (Barrel[i]) {
			XMFLOAT3 localPosition = Barrel[i]->GetPosition();
			float x = localPosition.x;
			float y = localPosition.y;
			float z = localPosition.z;

			XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);
			XMMATRIX rotationMatrix = Barrel[i]->GetRotationMatrix();
			XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

			Barrel[i]->Render(m_Direct3D->GetDeviceContext());
			result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Barrel[i]->GetIndexCount(), modelMatrix, viewMatrix, projectionMatrix, Barrel[i]->GetTexture());
			if (!result) return false;
			worldMatrix = XMMatrixIdentity();
		}
	}

	if (Planet)
	{
		XMFLOAT3 localPosition = Planet->GetPosition();
		float x = localPosition.x;
		float y = localPosition.y;
		float z = localPosition.z;

		XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);
		XMMATRIX rotationMatrix = Planet->GetRotationMatrix();
		if (XMMatrixIsIdentity(rotationMatrix))
		{
			rotationMatrix = XMMatrixIdentity();
		}

		XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

		Planet->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Planet->GetIndexCount(), modelMatrix, viewMatrix, projectionMatrix, Planet->GetTexture());
		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
	}

	// Конец сцены
	m_Direct3D->EndScene();

	return true;
}

bool RenderManager::Update(HWND hwnd)
{
	bool result;

	result = Render(hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void RenderManager::Shutdown()
{
	for (int i = 0; i < 10; i++)
	{
		if (Barrel[i])
		{
			Barrel[i]->Shutdown();
			delete Barrel[i];
			Barrel[i] = nullptr;
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

	if (Planet) {
		Planet->Shutdown();
		delete Planet;
		Planet = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


