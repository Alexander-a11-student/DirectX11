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


	XMMATRIX modelMatrix;

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();




private:
	//Lab3
	XMMATRIX m_RotationMatrix; // Матрица вращения

	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	TextureClass* m_Texture;

	//Коллизия
	XMFLOAT3 m_position;
	XMFLOAT3 m_size;
};

#endif