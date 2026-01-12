#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <array>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "RTex.h"

//ライブラリコンパイル
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

struct SpotLightData
{
	DirectX::XMFLOAT3 light_pos = { 0,10,0 };
	float pad_1;
	DirectX::XMFLOAT3 light_rgb = { 10.0f,10.0f,10.0f };
	float light_power = 0.0f;
	DirectX::XMFLOAT3 spDirection = { 0,-1,0 }; // 射出方向 
	float spAngle = 30.0f; // 射出角度
};

struct cbuffer_b0
{
	DirectX::XMFLOAT3 eyePos;	//カメラ座標
	float pad;
};

struct cbuff_bool
{
	bool myBool;
	bool padding[3];
};

struct cbuffer_b1
{
	DirectionalLight::ConstBuffData lightData[4];
};

struct cbuffer_b2
{
	PointLightData lightData[256];
};

struct cbuffer_b3
{
	SpotLightData lightData[256];
};

class DiferredRenderingMgr
{
private:
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	//コンストラクタ、デストラクタ
	DiferredRenderingMgr(){};
	~DiferredRenderingMgr(){};

	//初期化
	void Init(ID3D12Device* dev, ID3D12GraphicsCommandList* cmd);

	//描画
	void Rendering(RTex* gBuffer, RTex* shadowMap);

	//デバッグ描画
	void ShowImGui();

	PointLightData* GetPointLight(int index);

private:
	struct VertexPosUv {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
	};

	void ShaderCompile();

	void CreateGraphicsPipeline();

	void UpdateConstBuff();

	//デバイス、コマンドリストのポインタ
	ID3D12Device				*m_dev;
	ID3D12GraphicsCommandList	*m_cmd;
	//グラフィックスパイプライン、ルートシグネチャ
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	//シェーダーオブジェクト
	ComPtr<ID3D10Blob>			m_litVS;
	ComPtr<ID3D10Blob>			m_litPS;
	//ペラポリゴンリソース
	ComPtr<ID3D12Resource>		m_vertBuff;		//頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW	m_vbview;		//VBV
	//定数バッファ（カメラ視点座標）
	ComPtr<ID3D12Resource>		m_constBuffEyePos;
	ComPtr<ID3D12Resource>		m_constBuffDirLight;
	ComPtr<ID3D12Resource>		m_constBuffPointLight;
	ComPtr<ID3D12Resource>		m_constBuffSpotLight;

	//ライト配列
	std::array<DirectionalLight, 4> directionalLights;

	//ポイントライト
	std::array<PointLightData, 256> pointlight;

	//スポットライト
	std::array<SpotLightData, 256> spotlight;

};

