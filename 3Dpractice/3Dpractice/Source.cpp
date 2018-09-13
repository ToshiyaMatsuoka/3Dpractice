#include "TrialLib.h"
#include "TrialEnums.h"
#include "SoundsManager.h"
#pragma comment (lib,"DirectX_LIB.lib")
#pragma comment (lib,"SoundLIB.lib")

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

extern LPDIRECT3D9 g_pDirect3D;
extern LPDIRECT3DDEVICE9 g_pD3Device;
LPD3DXMESH pMesh = NULL;
D3DMATERIAL9* pMeshMaterials = NULL;
LPDIRECT3DTEXTURE9* pMeshTextures = NULL;
DWORD dwNumMaterials = 0;
float fCameraX = 0, fCameraY = 1.0f, fCameraZ = -3.0f,
fCameraHeading = 0, fCameraPitch = 0, fPosX = 0, fPosY = 0, fPosZ = 0;

bool SoundSuccess;
SoundLib::SoundsManager soundsManager;

unsigned int GameRoop(void);
void Render();
void Control();

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstance, LPSTR szStr, INT iCmdShow)
{
	HWND hWnd = NULL;

	InitWindowEx("3DTEST", &hWnd, 640, 540, hInst, hInstance, "Sprite.bmp");
	SoundSuccess = soundsManager.Initialize();

	
		//// 「Direct3D」オブジェクトの作成
		//if (NULL == (g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)))
		//{
		//	MessageBox(0, "Direct3Dの作成に失敗しました", "", MB_OK);
		//	return E_FAIL;
		//}
		// 「DIRECT3Dデバイス」オブジェクトの作成
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferCount = 1;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.Windowed = TRUE;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		if (FAILED(g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &g_pD3Device)))
		{
			MessageBox(0, "HALモードでDIRECT3Dデバイスを作成できません\nREFモードで再試行します", NULL, MB_OK);
			if (FAILED(g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
				D3DCREATE_MIXED_VERTEXPROCESSING,
				&d3dpp, &g_pD3Device)))
			{
				MessageBox(0, "DIRECT3Dデバイスの作成に失敗しました", NULL, MB_OK);
				return E_FAIL;
			}
		}

		// Xファイルからメッシュをロードする	
		LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

		if (FAILED(D3DXLoadMeshFromX("Chips.x", D3DXMESH_SYSTEMMEM,
			g_pD3Device, NULL, &pD3DXMtrlBuffer, NULL,
			&dwNumMaterials, &pMesh)))
		{
			MessageBox(NULL, "Xファイルの読み込みに失敗しました", NULL, MB_OK);
			return E_FAIL;
		}
		D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
		pMeshMaterials = new D3DMATERIAL9[dwNumMaterials];
		pMeshTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

		for (DWORD i = 0; i < dwNumMaterials; i++)
		{
			pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
			pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;
			pMeshTextures[i] = NULL;
			if (d3dxMaterials[i].pTextureFilename != NULL &&
				lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
			{
				if (FAILED(D3DXCreateTextureFromFile(g_pD3Device,
					d3dxMaterials[i].pTextureFilename,
					&pMeshTextures[i])))
				{
					MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
				}
			}
		}
		pD3DXMtrlBuffer->Release();
		// Zバッファー処理を有効にする
		g_pD3Device->SetRenderState(D3DRS_ZENABLE, TRUE);
		// ライトを有効にする
		g_pD3Device->SetRenderState(D3DRS_LIGHTING, TRUE);
		// アンビエントライト（環境光）を設定する
		g_pD3Device->SetRenderState(D3DRS_AMBIENT, 0x00111111);
		// スペキュラ（鏡面反射）を有効にする
		g_pD3Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

		soundsManager.AddFile("216.ブラックサレナⅢ（機動戦艦ナデシコ The prince of darkness）.mp3", "BGM");
		soundsManager.Start("BGM", false);
		soundsManager.AddFile("nc84131.wav", "SE");

		FlameRoop(GameRoop);


	
}


unsigned int GameRoop(void) {
	Control();
	Render();

	CheckKeyState(DIK_ESCAPE);
	if (KeyState[DIK_ESCAPE] == KeyRelease)
	{
		return WM_QUIT;
		SAFE_RELEASE(pMesh);

	}
	return WM_NULL;


}

//Xファイルから読み込んだメッシュをレンダリングする関数
void Render(){
	//ワールドトランスフォーム（絶対座標変換）
	D3DXMATRIXA16 matWorld, matPosition, matRotation;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matPosition, fPosX, fPosY, fPosZ);
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 3000.0f);
	//D3DXMatrixRotationX(&matRotation, 0.5f);
	D3DXMatrixMultiply(&matWorld, &matWorld, &/*matRotation*/matPosition);
	g_pD3Device->SetTransform(D3DTS_WORLD, &matWorld);
	// ビュートランスフォーム（視点座標変換）

	D3DXVECTOR3 vecEyePt(fCameraX, fCameraY, fCameraZ); //カメラ（視点）位置
	D3DXVECTOR3 vecLookatPt(fCameraX, fCameraY - 1.0f, fCameraZ + 3.0f);//注視位置
	D3DXVECTOR3 vecUpVec(0.0f, 1.0f, 0.0f);//上方位置
	D3DXMATRIXA16 matView, matCameraPosition, matHeading, matPitch;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixRotationY(&matHeading, fCameraHeading);
	D3DXMatrixRotationX(&matPitch, fCameraPitch);
	D3DXMatrixLookAtLH(&matCameraPosition, &vecEyePt, &vecLookatPt, &vecUpVec);
	D3DXMatrixMultiply(&matView, &matView, &matHeading);
	D3DXMatrixMultiply(&matView, &matView, &matPitch);
	D3DXMatrixMultiply(&matView, &matView, &matCameraPosition);
	g_pD3Device->SetTransform(D3DTS_VIEW, &matView);
	// プロジェクショントランスフォーム（射影変換）
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pD3Device->SetTransform(D3DTS_PROJECTION, &matProj);
	// ライトをあてる 白色で鏡面反射ありに設定
	D3DXVECTOR3 vecDirection(0, 0, 1);
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDirection);
	light.Range = 200.0f;
	g_pD3Device->SetLight(0, &light);
	g_pD3Device->LightEnable(0, TRUE);
	// レンダリング
	g_pD3Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);

	if (SUCCEEDED(g_pD3Device->BeginScene()))
	{
		for (DWORD i = 0; i<dwNumMaterials; i++)
		{
			g_pD3Device->SetMaterial(&pMeshMaterials[i]);
			g_pD3Device->SetTexture(0, pMeshTextures[i]);
			pMesh->DrawSubset(i);
		}
		g_pD3Device->EndScene();
	}
	g_pD3Device->Present(NULL, NULL, NULL, NULL);
}

void Control() {
	if(InputKEY(DIK_A))	fCameraX -= 0.1f;
	if(InputKEY(DIK_D))fCameraX += 0.1f;
	if(InputKEY(DIK_Q))fCameraY -= 0.1f;
	if(InputKEY(DIK_E))fCameraY += 0.1f;
	if(InputKEY(DIK_W))fCameraZ -= 0.1f;
	if(InputKEY(DIK_S))fCameraZ += 0.1f;
	if(InputKEY(DIK_LEFT))fCameraHeading -= 0.1f;
	if(InputKEY(DIK_RIGHT))fCameraHeading += 0.1f;
	if(InputKEY(DIK_UP))fCameraPitch -= 0.1f;
	if(InputKEY(DIK_DOWN))fCameraPitch += 0.1f;

	if (InputKEY(DIK_NUMPAD7))fPosZ -= 0.01f;
	if (InputKEY(DIK_NUMPAD9))fPosZ += 0.01f;
	if (InputKEY(DIK_NUMPAD2))fPosY -= 0.01f;
	if (InputKEY(DIK_NUMPAD8))fPosY += 0.01f;
	if (InputKEY(DIK_NUMPAD4))fPosX -= 0.01f;
	if (InputKEY(DIK_NUMPAD6))fPosX += 0.01f;







}