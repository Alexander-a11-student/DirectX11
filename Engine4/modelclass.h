////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <fstream> 
#include <cmath>
#include <vector>
#include "textureclass.h"
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;
using namespace DirectX;


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	struct ModelType
	{
		float x, y, z;
		float u, v;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilenam, char* modelFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();



	//Для 3 lab
	void GenerateSphereModel(int slices, int stacks, float radius, const char* filename);
	void SetRotationMatrix(const XMMATRIX& rotationMatrix);
	XMMATRIX GetRotationMatrix() const;
	
	//Коллизия
	bool CheckCollision(ModelClass* other);
	void SetPosition(float x, float y, float z) { m_position = XMFLOAT3(x, y, z); }
	void SetSize(float width, float height, float depth) { m_size = XMFLOAT3(width, height, depth); }
	XMFLOAT3 GetPosition() { return m_position; }
	XMFLOAT3 GetSize() { return m_size; }

	bool IsAttached() const { return m_attached; }
	void SetAttached(bool attached) { m_attached = attached; }
	void SetOffset(const XMFLOAT3& offset) { m_offset = offset; }
	XMFLOAT3 GetOffset() const { return m_offset; }

	void SetAttachmentBaseAngle(float angle) { m_AttachmentBaseAngle = angle; }
	float GetAttachmentBaseAngle() const { return m_AttachmentBaseAngle; }


	void SetSize(float radius);
	bool CheckCollisionSphere(ModelClass* other);

	XMFLOAT3 m_localOffset; // Локальное смещение относительно планеты
	XMMATRIX m_initialInverseRotation; 
	XMMATRIX m_initialRotation; // Начальная матрица вращения бочки в момент прикрепления

	void SetLocalOffset(XMFLOAT3 offset) { m_localOffset = offset; }
	XMFLOAT3 GetLocalOffset() { return m_localOffset; }
	void SetInitialInverseRotation(XMMATRIX matrix) { m_initialInverseRotation = matrix; }
	XMMATRIX GetInitialInverseRotation() { return m_initialInverseRotation; }
	void SetInitialRotation(XMMATRIX matrix) { m_initialRotation = matrix; }
	XMMATRIX GetInitialRotation() { return m_initialRotation; }


	XMMATRIX modelMatrix;

	void SetSphericalCoords(float theta, float phi) { m_theta = theta; m_phi = phi; }
	float GetTheta() const { return m_theta; }
	float GetPhi() const { return m_phi; }


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();

	float m_AttachmentBaseAngle = 0.0f;



private:
	bool m_attached = false;         // Флаг: прикреплена ли планета к бочке
	XMFLOAT3 m_offset = { 0.0f, 0.0f, 0.0f }; // Смещение от бочки
	//Lab3
	XMMATRIX m_RotationMatrix; // Матрица вращения

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	TextureClass* m_Texture;

	//Коллизия
	XMFLOAT3 m_position;
	XMFLOAT3 m_size;

	// ... существующие поля ...
	float m_theta = 0.0f; // Угол theta (азимут, от 0 до 2π)
	float m_phi = 0.0f;   // Угол phi (наклон, от 0 до π)
};



#endif