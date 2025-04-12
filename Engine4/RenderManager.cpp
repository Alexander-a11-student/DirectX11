////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "renderManager.h"

RenderManager::RenderManager()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Light = 0;
	m_DepthShader = 0;
	m_ShadowShader = 0;
	m_RenderTexture = 0;
	m_projectiveTexture = 0; // Инициализируем указатель
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
		Barrel[i]->SetPosition(x, -1.5f, z);
		Barrel[i]->SetSize(1.5f);
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

	// Инициализация пола
	Floor = new ModelClass();
	result = Floor->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenameFloor, modelFilenameFloor);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}
	Floor->SetPosition(0.0f, -1.5f, 0.0f); // Пол ниже планеты
	Floor->SetSize(20.0f); // Большая плоскость, чтобы тени падали

	// Создаем и инициализируем объект освещения. 
	m_Light = new LightClass;
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetPosition(-8.5f, 5.0f, -10.0f);
	m_Light->SetLookAt(0.0f, -80.0f, 0.0f);
	m_Light->GenerateViewMatrix();
	m_Light->GenerateOrthoMatrix(60.0f, 60.0f, 0.1f, 50.0f);




	// Создаем и инициализируем объект рендеринга текстуры. 
	m_RenderTexture = new RenderTextureClass;

	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, 1);
	if (!result)
	{
		MessageBox(hwnd, L"Не удалось инициализировать объект рендеринга текстуры.", L"Ошибка", MB_OK);
		return false;
	}

	// Создаем и инициализируем объект шейдера глубины. 
	m_DepthShader = new DepthShaderClass;

	result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Не удалось инициализировать объект шейдера глубины.", L"Ошибка", MB_OK);
		return false;
	}

	m_shadowMapBias = 0.005f;


	m_ShadowShader = new ShadowShaderClass;
	result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}


	// Загрузка проекционной текстуры
	char projectiveTextureFilename[128];
	strcpy_s(projectiveTextureFilename, "../Engine4/Border2.tga");
	m_projectiveTexture = new TextureClass;
	result = m_projectiveTexture->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), projectiveTextureFilename);
	if (!result)
	{
		MessageBox(hwnd, L"Could not load projective texture.", L"Error", MB_OK);
		return false;
	}


	//Старое
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


bool RenderManager::RenderSceneToTexture()
{
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;

	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	// Рендеринг планеты
	XMFLOAT3 planetPos = Planet->GetPosition();
	XMMATRIX translationMatrix = XMMatrixTranslation(planetPos.x, planetPos.y, planetPos.z);
	XMMATRIX rotationMatrix = Planet->GetRotationMatrix();
	if (XMMatrixIsIdentity(rotationMatrix)) {
		rotationMatrix = XMMatrixIdentity();
	}
	XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

	Planet->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), Planet->GetIndexCount(), modelMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) return false;

	// Рендеринг бочек
	for (int i = 0; i < 10; i++) {
		if (Barrel[i]) {
			XMFLOAT3 barrelPos = Barrel[i]->GetPosition();
			translationMatrix = XMMatrixTranslation(barrelPos.x, barrelPos.y, barrelPos.z);
			rotationMatrix = Barrel[i]->GetRotationMatrix();
			if (XMMatrixIsIdentity(rotationMatrix)) {
				rotationMatrix = XMMatrixIdentity();
			}
			modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

			Barrel[i]->Render(m_Direct3D->GetDeviceContext());
			result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), Barrel[i]->GetIndexCount(), modelMatrix, lightViewMatrix, lightProjectionMatrix);
			if (!result) return false;
		}
	}

	// Рендеринг пола
	XMFLOAT3 floorPos = Floor->GetPosition();
	translationMatrix = XMMatrixTranslation(floorPos.x, floorPos.y, floorPos.z);
	rotationMatrix = XMMatrixIdentity(); // Пол обычно не вращается
	modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

	Floor->Render(m_Direct3D->GetDeviceContext());
	result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), Floor->GetIndexCount(), modelMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) return false;

	m_Direct3D->SetBackBufferRenderTarget();
	m_Direct3D->ResetViewport();

	return true;
}

bool RenderManager::Render(HWND hwnd)
{
	timeGame += 0.001f;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix;
	bool result;

	// Начать сцену, очистив буфер белым цветом
	m_Direct3D->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	// Рендер камеры и обновление позиции
	m_Camera->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	UpdateCameraPosition();

	// Рендеринг пола
	if (Floor) {
		XMFLOAT3 floorPos = Floor->GetPosition();
		XMMATRIX translationMatrix = XMMatrixTranslation(floorPos.x, floorPos.y, floorPos.z);
		XMMATRIX rotationMatrix = XMMatrixIdentity();
		XMMATRIX modelMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);

		Floor->Render(m_Direct3D->GetDeviceContext());
		result = m_ShadowShader->Render(
			m_Direct3D->GetDeviceContext(),
			Floor->GetIndexCount(),
			modelMatrix,
			viewMatrix,
			projectionMatrix,
			lightViewMatrix,
			lightProjectionMatrix,
			Floor->GetTexture(),
			m_RenderTexture->GetShaderResourceView(),
			m_projectiveTexture->GetTexture(), // Используем TextureClass
			m_Light->GetAmbientColor(),
			m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(),
			m_shadowMapBias
		);
		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
	}

	// Рендеринг планеты
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
		result = m_ShadowShader->Render(
			m_Direct3D->GetDeviceContext(),
			Planet->GetIndexCount(),
			modelMatrix,
			viewMatrix,
			projectionMatrix,
			lightViewMatrix,
			lightProjectionMatrix,
			Planet->GetTexture(),
			m_RenderTexture->GetShaderResourceView(),
			m_projectiveTexture->GetTexture(), // Используем TextureClass
			m_Light->GetAmbientColor(),
			m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(),
			m_shadowMapBias
			);

		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
	}

	// Рендеринг бочек
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

			result = m_ShadowShader->Render(
				m_Direct3D->GetDeviceContext(),
				Barrel[i]->GetIndexCount(),
				modelMatrix,
				viewMatrix,
				projectionMatrix,
				lightViewMatrix,
				lightProjectionMatrix,
				Barrel[i]->GetTexture(),
				m_RenderTexture->GetShaderResourceView(),
				m_projectiveTexture->GetTexture(), // Используем TextureClass
				m_Light->GetAmbientColor(),
				m_Light->GetDiffuseColor(),
				m_Camera->GetPosition(),
				m_shadowMapBias
			);

			if (!result) return false;
			worldMatrix = XMMatrixIdentity();
		}
	}

	// Завершение сцены
	m_Direct3D->EndScene();

	return true;
}

bool RenderManager::Update(HWND hwnd)
{
	static float lightPositionX = -5.0f;

	// Устанавливаем обновленное положение источника света и генерируем его новую матрицу вида. 
	m_Light->SetPosition(-8.5f, 5.0f, -10.0f);
	m_Light->GenerateViewMatrix();


	bool result;

	//Рендеринг в текстуру
	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}
	
	//Рендеринг 3d сцены
	result = Render(hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void RenderManager::Shutdown()
{
	if (m_projectiveTexture)
	{
		m_projectiveTexture->Shutdown();
		delete m_projectiveTexture;
		m_projectiveTexture = 0;
	}
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}
	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	if (Floor)
	{
		Floor->Shutdown();
		delete Floor;
		Floor = 0;
	}
	if (Planet)
	{
		delete Planet;
	}

	for (int i = 0; i < 10; i++)
	{
		if (Barrel[i])
			delete Barrel[i];

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

}


