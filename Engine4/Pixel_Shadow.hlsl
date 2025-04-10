Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float bias;
    float3 padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

float4 ShadowPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;

    // ������������� ��������� ���� ��� ambient
    color = ambientColor;

    // ��������� ������������ ���������� ��� ����� �����
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

    // ���������, ��������� �� ������� � �������� ����� �����
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // ������ ������� ��� ����� ����� (������������ 2048x2048)
        float texelSize = 1.0f / 2048.0f;
        float shadow = 0.0f;

        // PCF 3x3: ��������� 9 ������� ������ ������� �����
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                // ���������� ������� � ��������� ���������
                depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(x, y) * texelSize).r;

                // ��������� ������� ����� � ������ bias
                lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w - bias;

                // ��������� 1.0, ���� ������� �������, 0.0 � ���� � ����
                shadow += (lightDepthValue < depthValue) ? 1.0f : 0.0f;
            }
        }

        // ��������� ��������� (9 �������)
        shadow /= 9.0f;

        // ���� ������� ������� ���� �� ��������, ��������� ������������� �����
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            // ��������� ��������� ���� � ������ �������������
            color += (diffuseColor * lightIntensity * shadow); // �������� �� shadow ��� ���������
            color = saturate(color);
        }
    }
    else
    {
        // ���� ��� ����� �����, ������� ������� ��������� ����������
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            color += (diffuseColor * lightIntensity);
            color = saturate(color);
        }
    }

    // ���������� �������� � ����������� � ������ �����
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
    color = color * textureColor;

    return color;
}