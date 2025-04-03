////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_model = 0;
	m_Texture = 0;
	m_RotationMatrix = XMMatrixIdentity();
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{

}


void ModelClass::GenerateSphereModel(int slices, int stacks, float radius, const char* filename)
{
	std::vector<ModelType> verts;
	std::vector<unsigned int> indices;
	verts.reserve((slices + 1) * (stacks + 1));
	indices.reserve(slices * stacks * 6);

	// Генерация вершин: φ идёт от 0 (низ) до π (верх),
	//                   θ идёт от 0 до 2π
	for (int i = 0; i <= stacks; i++)
	{
		float phi = static_cast<float>(XM_PI) * i / stacks; // [0..π]	
		// y = -cos(phi), x,z = sin(phi)*cos/sin(theta)
		for (int j = 0; j <= slices; j++)
		{
			float theta = 2.0f * static_cast<float>(XM_PI) * j / slices;

			ModelType vtx;
			vtx.x = radius * sinf(phi) * cosf(theta);
			vtx.y = -radius * cosf(phi); // "нижняя" ориентация
			vtx.z = radius * sinf(phi) * sinf(theta);

			// Допустим, сместим u так, чтобы было примерно [1.0..2.0]
			float uRange = 2.0f;  // Ширина диапазона
			vtx.u = 1.0f + ((float)j / slices) * uRange;

			// v от 1.0 (низ) до 0.0 (верх)
			
			//vtx.v = 1.0f - ((float)i / stacks);
			vtx.v = (float)i / stacks;

			verts.push_back(vtx);
		}
	}

	// Генерация индексов по треугольникам
	for (int i = 0; i < stacks; i++)
	{
		for (int j = 0; j < slices; j++)
		{
			int first = i * (slices + 1) + j;
			int second = (i + 1) * (slices + 1) + j;

			// Первый треугольник
			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			// Второй треугольник
			indices.push_back(first + 1);
			indices.push_back(second);
			indices.push_back(second + 1);
		}
	}

	// Выводим данные (по треугольникам)
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cerr << "Cannot open " << filename << std::endl;
		return;
	}

	// Печатаем заголовок
	fout << "Vertex Count: " << indices.size() << "\n\nData:\n";
	// Устанавливаем фиксированный формат с 6 знаками после запятой
	fout << std::fixed << std::setprecision(6);

	// Каждые три индекса — один треугольник
	for (size_t idx = 0; idx < indices.size(); idx += 3)
	{
		for (int k = 0; k < 3; k++)
		{
			const ModelType& v = verts[indices[idx + k]];
			fout << v.x << " " << v.y << " " << v.z << " "
				<< v.u << " " << v.v << "\n";
		}
		fout << "\n"; // Пустая строка после каждого треугольника
	}
	fout.close();
}




bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename, char* modelFilename)
{
	bool result;

	//GenerateSphereModel(7, 7, 0.18f, "sphere.txt");
	//GenerateSphereModel(20, 20, 0.25f, "sphere2.txt");
	//GenerateSphereModel(20, 20, 2.0f, "sphere3.txt");

	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	// Загрузка данных модели. 
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void ModelClass::SetRotationMatrix(const XMMATRIX& rotationMatrix) {
	m_RotationMatrix = rotationMatrix;
}

XMMATRIX ModelClass::GetRotationMatrix() const {
	return m_RotationMatrix;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;



	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}


	// Загрузить массив вершин и массив индексов с данными. 
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].u, m_model[i].v);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Открыть файл модели. 
	fin.open(filename);

	// Если файл открыть не удалось, то выйти. 
	if (fin.fail())
	{
		return false;
	}

	// Считать до значения количества вершин. 
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Считать количество вершин. 
	fin >> m_vertexCount;

	// Установить количество индексов, равное количеству вершин. 
	m_indexCount = m_vertexCount;

	// Создать модель, используя считанное количество вершин. 
	m_model = new ModelType[m_vertexCount];

	// Считать до начала данных. 
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Считываем данные вершин. 
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].u >> m_model[i].v;
	}

	// Закрываем файл модели. 
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}




void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create and initialize the texture object.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}


bool ModelClass::CheckCollision(ModelClass* other) {
	XMFLOAT3 otherPos = other->GetPosition();
	XMFLOAT3 otherSize = other->GetSize();

	// Добавляем смещение по оси X для корректировки
	float collisionOffsetX = -0.5f; // подберите значение по необходимости

	return (abs((m_position.x + collisionOffsetX) - otherPos.x) < (m_size.x / 2 + otherSize.x / 2)) &&
		(abs(m_position.y - otherPos.y) < (m_size.y / 2 + otherSize.y / 2)) &&
		(abs(m_position.z - otherPos.z) < (m_size.z / 2 + otherSize.z / 2));
}




void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the чvertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ModelClass::SetOrbitParameters(float radius, float speed) {
	orbitRadius = radius;
	orbitSpeed = speed;
}

void ModelClass::SetSelfRotationSpeed(float speed) {
	selfRotationSpeed = speed;
}

void ModelClass::SetParent(int parent) {
	parentIndex = parent;
}


float ModelClass::GetOrbitSpeed()  { return orbitSpeed; }
float ModelClass::GetParent()  { return parentIndex; }
float ModelClass::GetSelfRotationSpeed()  { return selfRotationSpeed; }
float ModelClass::GetOrbitRadius() { return orbitRadius; }