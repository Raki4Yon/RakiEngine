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

	RVector3 eye(0.f, 100.f, -100.f);
	RVector3 target(0.f, 0.f, 0.f);
	RVector3 up(0.f, 1.f, 0.f);
	NY_Camera::Get()->SetViewStatusEyeTargetUp(eye, target, up);

	UINT plane_tex = TexManager::LoadTexture("Resources/white8x8.png");

	std::shared_ptr<Model3D> plane;
	plane = std::make_shared<Model3D>();
	plane->CreatePlaneModelXZ(100, 100, 1, 1, plane_tex, nullptr);

	std::unique_ptr<Object3d> plane_object = std::make_unique<Object3d>();
	plane_object->InitObject3D(raki->GetDevice());
	plane_object->SetLoadedModelData(plane);

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

	Raki_DX12B::Get()->Finalize();
	
	raki->Finalize();
	rakiWinApp->DeleteGameWindow();

	delete raki;
	raki = nullptr;
	delete rakiWinApp;
	rakiWinApp = nullptr;

	return 0;
}