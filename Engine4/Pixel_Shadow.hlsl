Texture2D shaderTexture : register(t0); // �������� �������� �������
Texture2D depthMapTexture : register(t1); // ����� �����
Texture2D projectiveTexture : register(t2); // ������������ ��������
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
    float2 shadowTexCoord; // ���������� ��� ����� �����
    float2 projectTexCoord; // ���������� ��� ������������ ��������
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float4 projectiveColor;

    // ������������� ��������� ���� ��� ambient
    color = ambientColor;

    // ��������� ���������� ��������
    if (input.lightViewPosition.w > 0.0f)
    {
        // ���������� ��� ����� �����
        shadowTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
        shadowTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f; // ������� ��� �������� Y

        // ���������� ��� ������������ ��������
        projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
        projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f; // ������� ��� �������� Y
    }
    else
    {
        shadowTexCoord.x = 0.0f;
        shadowTexCoord.y = 0.0f;
        projectTexCoord.x = 0.0f;
        projectTexCoord.y = 0.0f;
    }

    // ���������, ��������� �� ������� � �������� ����� �����
    if ((saturate(shadowTexCoord.x) == shadowTexCoord.x) && (saturate(shadowTexCoord.y) == shadowTexCoord.y))
    {
        float texelSize = 1.0f / 4096.0f;
        float shadow = 0.0f;

        // �������� ��������� PCF ��� ��������
        depthValue = depthMapTexture.Sample(SampleTypeClamp, shadowTexCoord).r;
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w - bias;
        shadow = (lightDepthValue < depthValue) ? 1.0f : 0.0f;

        // ���������, ��������� �� ������� � �������� ������������ ��������
        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
        {
            // ��������� ������������� �����
            lightIntensity = saturate(dot(input.normal, input.lightPos));
            if (lightIntensity > 0.0f)
            {
                // ���������� ������������ ��������
                projectiveColor = projectiveTexture.Sample(SampleTypeClamp, projectTexCoord);

                // ��������� ��������� ���� � ������ ������������ �������� � �������������
                color += (diffuseColor * projectiveColor * lightIntensity * shadow);
                color = saturate(color);
            }
        }
        else
        {
            // ���� ��� ��������, �� � ���� �����, ��������� ������ ���������
            lightIntensity = saturate(dot(input.normal, input.lightPos));
            if (lightIntensity > 0.0f)
            {
                color += (diffuseColor * lightIntensity * shadow);
                color = saturate(color);
            }
        }
    }
    else
    {
        // ���� ��� ����� �����, ��������� ������ ���������
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            color += (diffuseColor * lightIntensity);
            color = saturate(color);
        }
    }

    // ���������� �������� ������� � ����������� � ������ �����
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
    color = color * textureColor;

    return color;
}