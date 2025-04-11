////////////////////////////////////////////////////////////////////////////////
// Filename: texture.vs
////////////////////////////////////////////////////////////////////////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;

    // �������������� ������� �������
    input.position.w = 1.0f;

    // �������� ���� �� �������� � ����� UV
    float4 textureColor = shaderTexture.SampleLevel(SampleType, input.tex, 0);

    // �������� ����� ���� (17, 164, 197) � ��������� [0, 1]
    float3 targetColor = float3(17.0f / 255.0f, 164.0f / 255.0f, 197.0f / 255.0f);
    
    // ����� ��� ��������� (��������� ��� ����������)
    float threshold = 0.2f;

    // ���������, ������ �� ���� ������� � �������� ������ �����
    if (all(abs(textureColor.rgb - targetColor) < threshold))
    {
        // ���� ��� ����, ������������ �������
        float3 normalizedPosition = normalize(input.position.xyz);
        float extrusionAmount = 0.3f; // ��������� ��� ��������� ������
        input.position.xyz -= normalizedPosition * extrusionAmount;
    }

    // ������������ ������ ������������� (��������� ��� ���������)
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // ��������� ���������� ����������
    output.tex = input.tex;

    return output;
}