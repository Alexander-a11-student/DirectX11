////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "renderManager.h"

RenderManager::RenderManager()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_LightShader = 0;
	m_Lights = 0;


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

	cameraDistance = 15.0f; // Начальное расстояние от планеты
	cameraYaw = XMConvertToRadians(0.0f);   // Начальный угол по горизонтали (в радианах)
	cameraPitch = XMConvertToRadians(45.0f); // Начальный угол по вертикали (в радианах)

	//Планета
	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");
		
	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	//Бочка

	char modelFilenameBarrel[128];
	strcpy_s(modelFilenameBarrel, "../Engine4/Barrel.txt");

	char textureFilenameBarrel[128];
	strcpy_s(textureFilenameBarrel, "../Engine4/MyBarrel.tga");

	//Пол
	char modelFilenameFloor[128];
	strcpy_s(modelFilenameFloor, "../Engine4/Boll.txt");

	char textureFilenameFloor[128];
	strcpy_s(textureFilenameFloor, "../Engine4/Circle2.tga");


	Floor = new ModelClass();
	result = Floor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenameFloor, modelFilenameFloor);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}

	Floor->SetPosition(0.0f, -1.0f, 0.0f); // Устанавливаем позицию пола
	Floor->SetSize(20.0f); // Устанавливаем размер пола


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


	// Create and initialize the light shader object.
	m_LightShader = new LightShaderClass;

	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the number of lights we will use.
	m_numLights = 4;
	// Create and initialize the light objects array.
	m_Lights = new LightClass[m_numLights];

	// Manually set the color and position of each light.
	m_Lights[0].SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);  // Red
	m_Lights[0].SetPosition(-3.0f, 1.0f, 3.0f);

	m_Lights[1].SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);  // Green
	m_Lights[1].SetPosition(3.0f, 1.0f, 3.0f);

	m_Lights[2].SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);  // Blue
	m_Lights[2].SetPosition(-3.0f, 1.0f, -3.0f);

	m_Lights[3].SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);  // White
	m_Lights[3].SetPosition(3.0f, 1.0f, -3.0f);



	return true;
}

//Повороты сферы
void RenderManager::MoveBarrelForward()
{
	isMovingForward = true;

	XMFLOAT3 position = Planet->GetPosition();
	XMFLOAT3 direction = GetCameraDirection();

	const float moveSpeed = 0.1f;

	position.x += direction.x * moveSpeed;
	position.z += direction.z * moveSpeed;
	Planet->SetPosition(position.x, position.y, position.z);

	// Определяем ось вращения (перпендикулярно направлению движения)
	XMFLOAT3 rotationAxis = { -direction.z, 0.0f, direction.x }; // (x, z) -> (-z, x)
	float length = sqrt(rotationAxis.x * rotationAxis.x + rotationAxis.z * rotationAxis.z);
	if (length > 0.0f)
	{
		rotationAxis.x /= length;
		rotationAxis.z /= length;
	}

	// Вращаем планету вокруг этой оси
	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f);
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMMATRIX additionalRotation = XMMatrixRotationAxis(axis, -rotationAngleIncrement); // Инвертируем угол
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);
	Planet->SetRotationMatrix(newRotation);

	UpdateCameraPosition();
}

void RenderManager::MoveBarrelBackward()
{
	isMovingBackward = true;

	XMFLOAT3 position = Planet->GetPosition();
	XMFLOAT3 direction = GetCameraDirection();

	const float moveSpeed = 0.1f;

	position.x -= direction.x * moveSpeed;
	position.z -= direction.z * moveSpeed;
	Planet->SetPosition(position.x, position.y, position.z);

	// Ось вращения та же, но вращаем в противоположную сторону
	XMFLOAT3 rotationAxis = { -direction.z, 0.0f, direction.x };
	float length = sqrt(rotationAxis.x * rotationAxis.x + rotationAxis.z * rotationAxis.z);
	if (length > 0.0f)
	{
		rotationAxis.x /= length;
		rotationAxis.z /= length;
	}

	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f);
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMMATRIX additionalRotation = XMMatrixRotationAxis(axis, rotationAngleIncrement); // Убираем инверсию
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);
	Planet->SetRotationMatrix(newRotation);

	UpdateCameraPosition();
}

void RenderManager::MoveBarrelLeft()
{
	isMovingLeft = true;

	XMFLOAT3 position = Planet->GetPosition();
	XMFLOAT3 direction = GetCameraDirection();

	const float moveSpeed = 0.1f;

	XMFLOAT3 leftDirection = { -direction.z, 0.0f, direction.x };
	position.x += leftDirection.x * moveSpeed;
	position.z += leftDirection.z * moveSpeed;
	Planet->SetPosition(position.x, position.y, position.z);

	// Ось вращения — перпендикулярна направлению движения влево
	XMFLOAT3 rotationAxis = { -leftDirection.z, 0.0f, leftDirection.x };
	float length = sqrt(rotationAxis.x * rotationAxis.x + rotationAxis.z * rotationAxis.z);
	if (length > 0.0f)
	{
		rotationAxis.x /= length;
		rotationAxis.z /= length;
	}

	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f);
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMMATRIX additionalRotation = XMMatrixRotationAxis(axis, -rotationAngleIncrement); // Инвертируем угол
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);
	Planet->SetRotationMatrix(newRotation);

	UpdateCameraPosition();
}

void RenderManager::MoveBarrelRight()
{
	isMovingRight = true;

	XMFLOAT3 position = Planet->GetPosition();
	XMFLOAT3 direction = GetCameraDirection();

	const float moveSpeed = 0.1f;

	XMFLOAT3 rightDirection = { direction.z, 0.0f, -direction.x };
	position.x += rightDirection.x * moveSpeed;
	position.z += rightDirection.z * moveSpeed;
	Planet->SetPosition(position.x, position.y, position.z);

	// Ось вращения — перпендикулярна направлению движения вправо
	XMFLOAT3 rotationAxis = { -rightDirection.z, 0.0f, rightDirection.x };
	float length = sqrt(rotationAxis.x * rotationAxis.x + rotationAxis.z * rotationAxis.z);
	if (length > 0.0f)
	{
		rotationAxis.x /= length;
		rotationAxis.z /= length;
	}

	constexpr float rotationAngleIncrement = XMConvertToRadians(5.0f);
	XMMATRIX currentRotation = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(currentRotation))
	{
		currentRotation = XMMatrixIdentity();
	}
	XMVECTOR axis = XMLoadFloat3(&rotationAxis);
	XMMATRIX additionalRotation = XMMatrixRotationAxis(axis, -rotationAngleIncrement); // В обратную сторону
	XMMATRIX newRotation = XMMatrixMultiply(currentRotation, additionalRotation);
	Planet->SetRotationMatrix(newRotation);

	UpdateCameraPosition();
}


//Повороты камеры
void RenderManager::TurnLeft()
{
	constexpr float rotationSpeed = XMConvertToRadians(5.0f); // В радианах
	cameraYaw -= rotationSpeed;
	UpdateCameraPosition();
}

void RenderManager::TurnRight()
{
	constexpr float rotationSpeed = XMConvertToRadians(5.0f);
	cameraYaw += rotationSpeed;
	UpdateCameraPosition();
}

void RenderManager::RotateCameraUp()
{
	constexpr float rotationSpeed = XMConvertToRadians(5.0f);
	cameraPitch += rotationSpeed;
	if (cameraPitch > XMConvertToRadians(89.0f)) cameraPitch = XMConvertToRadians(89.0f); // Ограничение
	UpdateCameraPosition();
}

void RenderManager::RotateCameraDown()
{
	constexpr float rotationSpeed = XMConvertToRadians(5.0f);
	cameraPitch -= rotationSpeed;
	if (cameraPitch < XMConvertToRadians(-89.0f)) cameraPitch = XMConvertToRadians(-89.0f); // Ограничение
	UpdateCameraPosition();
}

void RenderManager::UpdateMouseMovement(int deltaX, int deltaY)
{
	float sensitivity = 0.001f; // Чувствительность в радианах на пиксель
	cameraYaw += deltaX * sensitivity; // Инвертируем для естественного управления
	cameraPitch -= deltaY * sensitivity; // Оставляем как есть

	// Ограничиваем pitch
	if (cameraPitch > XMConvertToRadians(89.0f)) cameraPitch = XMConvertToRadians(89.0f);
	if (cameraPitch < XMConvertToRadians(-89.0f)) cameraPitch = XMConvertToRadians(-89.0f);

	UpdateCameraPosition();
}


void RenderManager::UpdateCameraPosition()
{
	// Получаем позицию планеты
	XMFLOAT3 planetPos = Planet->GetPosition();

	// Вычисляем позицию камеры в сферических координатах
	float x = cameraDistance * cos(cameraPitch) * sin(cameraYaw);
	float y = cameraDistance * sin(cameraPitch);
	float z = cameraDistance * cos(cameraPitch) * cos(cameraYaw);

	// Устанавливаем позицию камеры относительно планеты
	XMFLOAT3 cameraPos = { planetPos.x + x, planetPos.y + y, planetPos.z + z };
	m_Camera->SetPosition(cameraPos.x, cameraPos.y, cameraPos.z);

	// Направляем камеру на центр планеты
	XMVECTOR cameraPosition = XMLoadFloat3(&cameraPos);
	XMVECTOR planetPosition = XMLoadFloat3(&planetPos);
	XMVECTOR directionToPlanet = XMVectorSubtract(planetPosition, cameraPosition);
	directionToPlanet = XMVector3Normalize(directionToPlanet);

	// Вычисляем углы вращения камеры, чтобы она смотрела на планету
	float pitch = asinf(-XMVectorGetY(directionToPlanet));
	float yaw = atan2f(XMVectorGetX(directionToPlanet), XMVectorGetZ(directionToPlanet));

	m_Camera->SetRotation(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), 0.0f);
}

void RenderManager::UpdateMouseWheel(int wheelDelta)
{
	const float zoomSpeed = 0.5f; // Скорость зума
	cameraDistance -= wheelDelta * zoomSpeed / 120.0f; // Нормализуем (±120 — стандартное значение)

	// Ограничиваем расстояние
	if (cameraDistance < 5.0f) cameraDistance = 5.0f;  // Минимальное расстояние
	if (cameraDistance > 50.0f) cameraDistance = 50.0f; // Максимальное расстояние

	UpdateCameraPosition(); // Обновляем позицию камеры
}

XMFLOAT3 RenderManager::GetCameraDirection()
{
	// Вычисляем вектор направления, куда смотрит камера (от камеры к планете)
	float dirX = -sin(cameraYaw); // Инвертируем
	float dirZ = -cos(cameraYaw); // Инвертируем
	XMFLOAT3 direction = { dirX, 0.0f, dirZ };
	return direction;
}

bool RenderManager::Render(HWND hwnd)
{
	timeGame += 0.0001f;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	XMFLOAT4 diffuseColor[4], lightPosition[4];
	int i;

	bool result;

	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 0.0f);

	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Get the light properties.
	for (i = 0; i < m_numLights; i++)
	{
		// Create the diffuse color array from the four light colors.
		diffuseColor[i] = m_Lights[i].GetDiffuseColor();

		// Create the light position array from the four light positions.
		lightPosition[i] = m_Lights[i].GetPosition();
	}

	UpdateCameraPosition();

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

			result = m_LightShader->Render(
				m_Direct3D->GetDeviceContext(),
				Barrel[i]->GetIndexCount(),
				modelMatrix,
				viewMatrix, 
				projectionMatrix, 
				Barrel[i]->GetTexture(), 
				diffuseColor, 
				lightPosition
			);

			if (!result) return false;
			worldMatrix = XMMatrixIdentity();
		}
	}

	// Рендеринг пола
	if (Floor)
	{
		XMFLOAT3 localPosition = Floor->GetPosition();
		float x = localPosition.x;
		float y = localPosition.y;
		float z = localPosition.z;

		XMMATRIX translationMatrix = XMMatrixTranslation(x, y, z);
		XMMATRIX rotationMatrix = Floor->GetRotationMatrix();
		if (XMMatrixIsIdentity(rotationMatrix))
		{
			rotationMatrix = XMMatrixIdentity();
		}

		XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

		Floor->Render(m_Direct3D->GetDeviceContext());
		result = m_LightShader->Render(
			m_Direct3D->GetDeviceContext(),
			Floor->GetIndexCount(),
			modelMatrix,
			viewMatrix,
			projectionMatrix,
			Floor->GetTexture(),
			diffuseColor,
			lightPosition
		);

		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
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
		result = m_LightShader->Render(
			m_Direct3D->GetDeviceContext(),
			Planet->GetIndexCount(),
			modelMatrix,
			viewMatrix,
			projectionMatrix,
			Planet->GetTexture(),
			diffuseColor,
			lightPosition
		);

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
	// Освобождение ресурсов пола
	if (Floor)
	{
		Floor->Shutdown();
		delete Floor;
		Floor = nullptr;
	}

	// Release the light object.
	if (m_Lights)
	{
		delete[] m_Lights;
		m_Lights = 0;
	}

	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	for (int i = 0; i < 10; i++)
	{
		if (Barrel[i])
		{
			Barrel[i]->Shutdown();
			delete Barrel[i];
			Barrel[i] = nullptr;
		}
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
