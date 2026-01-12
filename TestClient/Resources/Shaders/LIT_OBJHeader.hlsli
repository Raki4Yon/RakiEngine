
//ワールド座標テクスチャ

//カメラ視点レジスタ b0
cbuffer cbuff0 : register(b0)
{
    float3 eyePos : packoffset(c0);
}

struct DirectionalLight
{
    float4 lightDir;
    uint isActive;
    uint isUseSpecular;
};

cbuffer cbuff1 : register(b1)
{
    DirectionalLight lightStatus[4];
}

struct PointLight
{
    float3 pos;
    float pad;
    float3 rgb;
    float power;
};

cbuffer cbuff_point_light : register(b2)
{
    PointLight pointlight_status[256];
}

struct SpotLight
{
    float3 pos;
    float pad;
    float3 rgb;
    float power;
    float3 spDirection; // スポットライトの射出方向 
    float spAngle; // スポットライトの射出角度
};

cbuffer cbuff_spot_light : register(b3)
{
    SpotLight spotlight_status[256];
}


struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};