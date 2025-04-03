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
	m_Camera->SetPosition(-0.5f, 3.0f, -4.0f); // ������������� ������ ��� ������
	m_Camera->SetRotation(45.0f, 0.0f, 0.0f); // ��������� ������ ����

	char modelFilenamePlanet[128];
	strcpy_s(modelFilenamePlanet, "../Engine4/sphere.txt");

	char textureFilenamePlanet[128];
	strcpy_s(textureFilenamePlanet, "../Engine4/TexturePlanet2.tga");

	//�����

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

		// ��������� ������������� �������� Planet[i] �� �����
		float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
		float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f - 50.0f;
		Barrel[i]->SetPosition(x, 0.0f, z);
		Barrel[i]->SetSize(0.1f, 0.1f, 0.1f);
	}


	Planet = new ModelClass();
	result = Planet->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), textureFilenamePlanet, modelFilenamePlanet);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the barrel model object.", L"Error", MB_OK);
		return false;
	}


	Planet->SetPosition(0.0f, 0.0f, 0.0f);
	Planet->SetSize(5.0f, 5.0f, 5.0f);

	m_TextureShader = new TextureShaderClass;
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}


	return true;
}


//����
//lab4
void RenderManager::MoveBarrelForward()
{
	// ������������� ����, ��� ����� �������� ������
	isMovingForward = true;

	// ����������� ����� ������
	XMFLOAT3 position = Planet->GetPosition();
	position.z += 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// ���������� ������� ������
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
	//m_Camera->SetLookAt(position.x, position.y, position.z);
}


void RenderManager::MoveBarrelBackward()
{
	// ������������� ����, ��� ����� �������� �����
	isMovingBackward = true;

	// ����������� ����� �����
	XMFLOAT3 position = Planet->GetPosition();
	position.z -= 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// ���������� ������� ������
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
	//m_Camera->SetLookAt(position.x, position.y, position.z);
}

void RenderManager::MoveBarrelLeft()
{
	// ������������� ����, ��� ����� �������� �����
	isMovingLeft = true;

	// ������� ����� �� 90 �������� �����
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-90.0f));
	Planet->SetRotationMatrix(rotationMatrix);

	// ����������� ����� �����
	XMFLOAT3 position = Planet->GetPosition();
	position.x -= 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// ���������� ������� ������
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
}

void RenderManager::MoveBarrelRight()
{
	// ������������� ����, ��� ����� �������� ������
	isMovingRight = true;

	// ������� ����� �� 90 �������� ������
	XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(90.0f));
	Planet->SetRotationMatrix(rotationMatrix);

	// ����������� ����� ������
	XMFLOAT3 position = Planet->GetPosition();
	position.x += 0.1f;
	Planet->SetPosition(position.x, position.y, position.z);

	// ���������� ������� ������
	m_Camera->SetPosition(position.x - 0.5f, position.y + 3.0f, position.z - 4.0f);
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

	rotation.x -= rotationSpeed;  // ������� ������ �����

	// ������������ ������
	if (rotation.x < -89.0f) rotation.x = -89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}

void RenderManager::RotateCameraDown()
{
	//if (isCameraFixed) return; 
	float rotationSpeed = 2.0f;
	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.x += rotationSpeed;  // ������� ������ ����

	// ������������ ������
	if (rotation.x > 89.0f) rotation.x = 89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}


void RenderManager::UpdateMouseMovement(int deltaX, int deltaY)
{
	//if (isCameraFixed) return;

	float sensitivity = 0.1f; // �������� ����������������

	float yaw = deltaX * sensitivity;
	float pitch = deltaY * sensitivity; // ����������� �������� �����-����

	XMFLOAT3 rotation = m_Camera->GetRotation();

	rotation.y += yaw;   // �������� �����-������
	rotation.x += pitch; // �������� �����-���� (��� ��������)

	// ��������� ������ ������, ����� ������ �� ����������������
	if (rotation.x > 89.0f) rotation.x = 89.0f;
	if (rotation.x < -89.0f) rotation.x = -89.0f;

	m_Camera->SetRotation(rotation.x, rotation.y, rotation.z);
}



void RenderManager::UpdatePlanet()
{
	// ���������� �������� �������� (����, �� ������� �������� �������������� ������ ����)
	const float rotationSpeed = 0.01f; // ����� ��������� ��� ������ ��������
	rotationAngle += rotationSpeed;

	// ��� ������ �������, ������� ����������� � �����, ��������� �������
	for (int i = 0; i < 10; i++)
	{
		if (Barrel[i]->IsAttached())
		{
			// �������� ������� �����
			XMFLOAT3 barrelPos = Planet->GetPosition();

			// �������� �������� ��������, ���������� ��� ������������
			XMFLOAT3 offset = Barrel[i]->GetOffset();

			// ����������� offset � XMVECTOR
			XMVECTOR offsetVector = XMLoadFloat3(&offset);

			// ������� ������� �������� ������ ��� Y (���� ��������� ������ ��� � ��������)
			XMMATRIX rotationMatrix = XMMatrixRotationY(rotationAngle);

			// ��������� �������� � offset
			offsetVector = XMVector3Transform(offsetVector, rotationMatrix);

			// ��������� ����� ��������
			XMFLOAT3 newOffset;
			XMStoreFloat3(&newOffset, offsetVector);

			// ��������� ������� ������� ������������ ������� ������� �����
			Barrel[i]->SetPosition(
				barrelPos.x + newOffset.x,
				barrelPos.y + newOffset.y,
				barrelPos.z + newOffset.z
			);
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

	if (isMovingForward)
	{
		planetRotationAngle += 0.05f; // ��� �������� ������ ����������� ����
	}
	if (isMovingBackward)
	{
		planetRotationAngle -= 0.05f; // ��� �������� ����� ��������� ���� (�������� ��������)
	}

	for (int i = 0; i < 10; i++)
	{
		if (!Barrel[i]->IsAttached() && Planet->CheckCollision(Barrel[i]))
		{
			// ��� ������������ ��������� �������� �� �����
			XMFLOAT3 barrelPos = Planet->GetPosition();
			XMFLOAT3 planetPos = Barrel[i]->GetPosition();
			XMFLOAT3 offset = {
				planetPos.x - barrelPos.x,
				planetPos.y - barrelPos.y,
				planetPos.z - barrelPos.z
			};
			Barrel[i]->SetAttached(true);
			// ��������� ������� ���������� ���� ��� ������� ��� ���� �������
			Barrel[i]->SetAttachmentBaseAngle(planetRotationAngle);
			Barrel[i]->SetOffset(offset);
		}
		else if (Barrel[i]->IsAttached()) // ���� ������� ����������� � ��������� � ������� � ������ ��������
		{
			XMFLOAT3 barrelPos = Planet->GetPosition();
			XMFLOAT3 offset = Barrel[i]->GetOffset();

			// ��������� ������� ����� ������� ���������� ����� � ����� � ������ �������������
			float deltaAngle = planetRotationAngle - Barrel[i]->GetAttachmentBaseAngle();

			// ����������� offset � ������ �������� ������ ��� Y (������ �������� �� ������ ���)
			XMVECTOR offsetVector = XMLoadFloat3(&offset);
			XMMATRIX rotationMat = XMMatrixRotationX(deltaAngle);
			offsetVector = XMVector3Transform(offsetVector, rotationMat);
			XMFLOAT3 newOffset;
			XMStoreFloat3(&newOffset, offsetVector);

			Barrel[i]->SetPosition(
				barrelPos.x + newOffset.x,
				barrelPos.y + newOffset.y,
				barrelPos.z + newOffset.z
			);
		}

		if (Barrel[i])
		{
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

		// ������� ����� �� 90 �������� ������ ��� Z
		XMMATRIX initialRotation = XMMatrixRotationZ(XMConvertToRadians(90.0f));

		// �������� ����� ������ ����� ���, ���� ��� ��������
		if (isMovingForward || isMovingBackward || isMovingLeft || isMovingRight)
		{
			if (isMovingForward) {
				rotationAngle -= 0.1f;
			}
			if (isMovingBackward) {
				rotationAngle += 0.1f;
			}
			if (isMovingLeft) {
				rotationAngle -= 0.1f;
			}
			if (isMovingRight) {
				rotationAngle -= 0.1f;
			}
			XMMATRIX rotationMatrix = XMMatrixRotationY(rotationAngle); // ������� ������ ��� Y (��� ������������� �������� ���)
			initialRotation = XMMatrixMultiply(rotationMatrix, initialRotation);
			isMovingForward = false;
			isMovingBackward = false;
			isMovingLeft = false;
			isMovingRight = false;
		}

		XMMATRIX modelMatrix = XMMatrixMultiply(initialRotation, translationMatrix);

		Planet->Render(m_Direct3D->GetDeviceContext());
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), Planet->GetIndexCount(), modelMatrix, viewMatrix, projectionMatrix, Planet->GetTexture());
		if (!result) return false;
		worldMatrix = XMMatrixIdentity();
	}




	//����� �����
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


