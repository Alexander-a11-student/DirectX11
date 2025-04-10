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

    // Устанавливаем начальный цвет как ambient
    color = ambientColor;

    // Вычисляем проекционные координаты для карты теней
    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

    // Проверяем, находится ли пиксель в пределах карты теней
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // Размер текселя для карты теней (предполагаем 2048x2048)
        float texelSize = 1.0f / 2048.0f;
        float shadow = 0.0f;

        // PCF 3x3: суммируем 9 сэмплов вокруг текущей точки
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                // Сэмплируем глубину с небольшим смещением
                depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(x, y) * texelSize).r;

                // Вычисляем глубину света с учётом bias
                lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w - bias;

                // Добавляем 1.0, если пиксель освещён, 0.0 — если в тени
                shadow += (lightDepthValue < depthValue) ? 1.0f : 0.0f;
            }
        }

        // Усредняем результат (9 сэмплов)
        shadow /= 9.0f;

        // Если пиксель освещён хотя бы частично, вычисляем интенсивность света
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            // Добавляем диффузный свет с учётом интенсивности
            color += (diffuseColor * lightIntensity * shadow); // Умножаем на shadow для плавности
            color = saturate(color);
        }
    }
    else
    {
        // Если вне карты теней, считаем пиксель полностью освещённым
        lightIntensity = saturate(dot(input.normal, input.lightPos));
        if (lightIntensity > 0.0f)
        {
            color += (diffuseColor * lightIntensity);
            color = saturate(color);
        }
    }

    // Сэмплируем текстуру и комбинируем с цветом света
    textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
    color = color * textureColor;

    return color;
}