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



	//��� 3 lab
	void GenerateSphereModel(int slices, int stacks, float radius, const char* filename);
	void SetRotationMatrix(const XMMATRIX& rotationMatrix);
	XMMATRIX GetRotationMatrix() const;
	
	//��������
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


	XMMATRIX modelMatrix;

	float orbitRadius; // ������ ������ ������ ��������
	float orbitSpeed; // �������� �������� ������ ��������
	float selfRotationSpeed; // �������� ������������ ��������	
	int parentIndex; // ������ ������������� ������� (-1 ��� ����������� ������)
	std::vector<int> satellites; // ������� ���������


	//������ ��� �������� ������ ��������
	void SetOrbitParameters(float radius, float speed);
	void SetSelfRotationSpeed(float speed);
	void SetParent(int parent);
	void AddSatellite(int satellite) { satellites.push_back(satellite); }

	float GetOrbitRadius();
	float GetOrbitSpeed();
	float GetParent();
	float GetSelfRotationSpeed();



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
	bool m_attached = false;         // ����: ����������� �� ������� � �����
	XMFLOAT3 m_offset = { 0.0f, 0.0f, 0.0f }; // �������� �� �����
	//Lab3
	XMMATRIX m_RotationMatrix; // ������� ��������

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	TextureClass* m_Texture;

	//��������
	XMFLOAT3 m_position;
	XMFLOAT3 m_size;
};



#endif