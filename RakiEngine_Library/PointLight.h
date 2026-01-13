#pragma once
#include <DirectXMath.h>
#include "RVector.h"

//ì_åıåπÇÃç\ë¢ëÃ
struct PointLightData
{
	DirectX::XMFLOAT3 light_pos = { 0,1,0 };
	float pad_1;
	DirectX::XMFLOAT3 light_rgb = {0.8f,0.8f,0.2f};
	float light_power = 0.0f;
};

class PointLight
{
public:

	PointLight() = default;



private:

	PointLightData light_data;

};

