#include <iostream>

#include <Raki_DX12B.h>
#include "FPS.h"
#include "Audio.h"
#include "TexManager.h"
#include <RenderTargetManager.h>
#include <DifferrdRenderingMgr.h>
#include <GraphicManager.h>

#include "NY_Object3DMgr.h"
#include "SpriteManager.h"
#include "Raki_imguiMgr.h"
#include <NY_Camera.h>
#include "fbxModel.h"
#include "FbxLoader.h"


using namespace DirectX;
using namespace Microsoft::WRL;

//-----------RakiEngine_Alpha.ver-----------//


#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{

#pragma region GameValue

	Raki_WinAPI* rakiWinApp;
	rakiWinApp = new Raki_WinAPI;
	rakiWinApp->CreateGameWindow();

	Raki_DX12B::Create();

	Raki_DX12B* raki = new Raki_DX12B;
	Raki_DX12B::Set(raki);
	raki->Initialize(rakiWinApp, false);

	GraphicManager graphicmgr;
	graphicmgr.Init();
	
	Audio::Init();

	RVector3 eye(0.f, 200.f, -200.f);
	RVector3 target(0.f, 0.f, 0.f);
	RVector3 up(0.f, 1.f, 0.f);
	NY_Camera::Get()->SetViewStatusEyeTargetUp(eye, target, up);

	UINT whiteA_tex = TexManager::LoadTexture("Resources/white8x8.png");
	UINT plane_tex = TexManager::LoadTexture("Resources/grid8x8.png");

	std::shared_ptr<Model3D> plane;
	plane = std::make_shared<Model3D>();
	plane->CreatePlaneModelXZ(160, 160, 16, 16, plane_tex, nullptr);

	std::shared_ptr<Model3D> box;
	box = std::make_shared<Model3D>();
	box->CreateBoxModel(10, 1, 1, plane_tex);

	std::unique_ptr<Object3d> plane_object = std::make_unique<Object3d>();
	plane_object->InitObject3D(raki->GetDevice());
	plane_object->SetLoadedModelData(plane);

	std::unique_ptr<Object3d> box_object = std::make_unique<Object3d>();
	box_object->InitObject3D(raki->GetDevice());
	box_object->SetLoadedModelData(box);
	box_object->SetAffineParamRotate(RVector3(0, 45, 0));

	//std::shared_ptr<fbxModel> capo_model = std::make_shared<fbxModel>();
	//capo_model.reset(FbxLoader::GetInstance()->LoadFBXFile("Capoeira"));
	//std::shared_ptr<Object3d> cap_object = make_shared<Object3d>();
	//cap_object->InitObject3D(raki->GetDevice());

	//ポイントライトを並べる
	


#pragma endregion GameValue

	FPS::Get()->Start();

	while (true)
	{
		if (rakiWinApp->ProcessMessage()) { break; }

		Input::StartGetInputState();

		


		graphicmgr.StartDraw();
		//3D通常描画ここから
		graphicmgr.StartDeferredDraw();

		plane_object->DrawObject();
		box_object->DrawObject();


		//3D通常描画ここまで
		graphicmgr.EndDefferdDraw();

		//パーティクル3Dここから
		graphicmgr.StartParticleDraw3D();

		//スプライトここから
		graphicmgr.StartSpriteDraw();

		//描画ここまで

		graphicmgr.FinishDraw();

		FPS::Get()->run();
	}
	
	myImgui::FinalizeImGui();
	
	raki->Finalize();
	rakiWinApp->DeleteGameWindow();

	delete raki;
	raki = nullptr;
	delete rakiWinApp;
	rakiWinApp = nullptr;

	return 0;
}