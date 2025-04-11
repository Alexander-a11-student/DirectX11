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

    // Инициализируем позицию вершины
    input.position.w = 1.0f;

    // Получаем цвет из текстуры в точке UV
    float4 textureColor = shaderTexture.SampleLevel(SampleType, input.tex, 0);

    // Заданный синий цвет (17, 164, 197) в диапазоне [0, 1]
    float3 targetColor = float3(17.0f / 255.0f, 164.0f / 255.0f, 197.0f / 255.0f);
    
    // Порог для сравнения (увеличили для надежности)
    float threshold = 0.2f;

    // Проверяем, близок ли цвет вершины к целевому синему цвету
    if (all(abs(textureColor.rgb - targetColor) < threshold))
    {
        // Если это море, экструдируем вершину
        float3 normalizedPosition = normalize(input.position.xyz);
        float extrusionAmount = 0.3f; // Регулируй для заметного сдвига
        input.position.xyz -= normalizedPosition * extrusionAmount;
    }

    // Оригинальная логика трансформации (оставляем без изменений)
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Сохраняем текстурные координаты
    output.tex = input.tex;

    return output;
}