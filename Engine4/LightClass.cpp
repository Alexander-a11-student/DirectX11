#include "lightclass.h"

LightClass::LightClass()
{
    m_ambientColor = XMFLOAT4(1.2f, 1.2f, 1.2f, 1.2f);
    m_diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
    m_specularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_specularPower = 1.0f;
    m_position = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    m_attenuation = XMFLOAT3(1.0f, 0.1f, 0.01f); // Значения по умолчанию
    m_range = 100.0f; // Дальность по умолчанию
}

LightClass::LightClass(const LightClass& other)
{
    m_ambientColor = other.m_ambientColor;
    m_diffuseColor = other.m_diffuseColor;
    m_direction = other.m_direction;
    m_specularColor = other.m_specularColor;
    m_specularPower = other.m_specularPower;
    m_position = other.m_position;
    m_attenuation = other.m_attenuation;
    m_range = other.m_range;
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
    m_ambientColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
    m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetDirection(float x, float y, float z)
{
    m_direction = XMFLOAT3(x, y, z);
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
    m_specularColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetSpecularPower(float power)
{
    m_specularPower = power;
}

void LightClass::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT4(x, y, z, 1.0f);
}

void LightClass::SetAttenuation(float constant, float linear, float quadratic)
{
    m_attenuation = XMFLOAT3(constant, linear, quadratic);
}

void LightClass::SetRange(float range)
{
    m_range = range;
}

XMFLOAT4 LightClass::GetAmbientColor()
{
    return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
    return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
    return m_direction;
}

XMFLOAT4 LightClass::GetSpecularColor()
{
    return m_specularColor;
}

float LightClass::GetSpecularPower()
{
    return m_specularPower;
}

XMFLOAT4 LightClass::GetPosition()
{
    return m_position;
}

XMFLOAT3 LightClass::GetAttenuation()
{
    return m_attenuation;
}

float LightClass::GetRange()
{
    return m_range;
}