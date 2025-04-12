#define NUM_LIGHTS 100

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightColorBuffer
{
    float4 diffuseColor[NUM_LIGHTS];
};

cbuffer LightAttenuationBuffer
{
    float4 attenuation[NUM_LIGHTS]; // x: constant, y: linear, z: quadratic, w: range
};

cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
    float4 colorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        // Вычисляем направление света
        float3 lightDir = lightPosition[i].xyz - input.worldPosition;
        float distance = length(lightDir);
        float3 lightPos = normalize(lightDir);

        // Вычисляем интенсивность
        float lightIntensity = saturate(dot(input.normal, lightPos));

        // Вычисляем затухание
        float atten = 0.0f;
        if (distance <= attenuation[i].w)
        {
            atten = 1.0f / (attenuation[i].x + attenuation[i].y * distance + attenuation[i].z * distance * distance);
        }

        // Добавляем вклад света
        colorSum += diffuseColor[i] * lightIntensity * atten;
    }

    float4 color = saturate(colorSum + 0.05) * textureColor;
    return color;
}