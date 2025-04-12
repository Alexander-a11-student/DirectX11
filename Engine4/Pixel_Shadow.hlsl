Texture2D shaderTexture : register(t0); // Основная текстура объекта
Texture2D depthMapTexture : register(t1); // Карта теней
Texture2D projectiveTexture : register(t2); // Проекционная текстура
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
    float2 shadowTexCoord; // Координаты для карты теней
    float2 projectTexCoord; // Координаты для проекционной текстуры
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
    float4 projectiveColor;

    // Устанавливаем начальный цвет как ambient
    color = ambientColor;

    // Вычисляем координаты отдельно
    if (input.lightViewPosition.w > 0.0f)
    {
        // Координаты для карты теней
        shadowTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
        shadowTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f; // Пробуем без инверсии Y

        // Координаты для проекционной текстуры
        projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
        projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f; // Пробуем без инверсии Y
    }
    else
    {
        shadowTexCoord.x = 0.0f;
        shadowTexCoord.y = 0.0f;
        projectTexCoord.x = 0.0f;
        projectTexCoord.y = 0.0f;
    }

    // Проверяем, находится ли пиксель в пределах карты теней
    if ((saturate(shadowTexCoord.x) == shadowTexCoord.x) && (saturate(shadowTexCoord.y) == shadowTexCoord.y))
    {
        float texelSize = 1.0f / 4096.0f;
        float shadow = 0.0f;

        // Временно отключаем PCF для точности
        depthValue = depthMapTexture.Sample(SampleTypeClamp, shadowTexCoord).r;
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w - bias;
        shadow = (lightDepthValue < depthValue) ? 1.0f : 0.0f;

        // Проверяем, находится ли пиксель в пределах проекционной текстуры
        if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
        {
            // Вычисляем интенсивность света
            lightIntensity = saturate(dot(input.normal, input.lightPos));
            if (lightIntensity > 0.0f)
            {
                // Сэмплируем проекционную текстуру
                projectiveColor = projectiveTexture.Sample(SampleTypeClamp, projectTexCoord);

                // Добавляем диффузный свет с учётом проекционной текстуры и интенсивности
                color += (diffuseColor * projectiveColor * lightIntensity * shadow);
                color = saturate(color);
            }
        }
        else
        {
            // Если вне текстуры, но в зоне теней, применяем только освещение
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
        // Если вне карты теней, применяем только освещение
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            color += (diffuseColor * lightIntensity);
            color = saturate(color);
        }
    }

    // Сэмплируем текстуру объекта и комбинируем с цветом света
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
    color = color * textureColor;

    return color;
}