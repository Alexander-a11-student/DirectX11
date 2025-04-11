#define NUM_LIGHTS 4

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

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 lightDir[NUM_LIGHTS] : TEXCOORD1;
    float3 lightPos[NUM_LIGHTS] : TEXCOORD5;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float lightIntensity[NUM_LIGHTS];
    float4 colorArray[NUM_LIGHTS];
    float4 colorSum;
    float distance;
    float atten;
    int i;

    textureColor = shaderTexture.Sample(SampleType, input.tex);

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        lightIntensity[i] = saturate(dot(input.normal, input.lightPos[i]));

        distance = length(input.lightDir[i]);

        if (distance > attenuation[i].w)
        {
            atten = 0.0f;
        }
        else
        {
            atten = 1.0f / (attenuation[i].x + attenuation[i].y * distance + attenuation[i].z * distance * distance);
        }

        colorArray[i] = diffuseColor[i] * lightIntensity[i] * (atten);
    }

    colorSum = float4(0.0f, 0.0f, 0.0f, 1.0f);

    for (i = 0; i < NUM_LIGHTS; i++)
    {
        colorSum += colorArray[i];
    }

    float4 color = saturate(colorSum + 0.05) * textureColor;

    return color;
}