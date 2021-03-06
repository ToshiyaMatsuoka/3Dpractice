#include "TrialLib.h"
#include "SoundsManager.h"
#pragma comment (lib,"DirectX_LIB.lib")
#pragma comment (lib,"SoundLIB.lib")

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
enum MESH {
	CHIPS,
	CAN,
	BOTTLE,
	MESHMAX
};
struct THING
{
	LPD3DXMESH pMesh;
	D3DMATERIAL9* pMeshMaterials;
	LPDIRECT3DTEXTURE9* pMeshTextures;
	DWORD dwNumMaterials;
	D3DXVECTOR3 vecPosition;
	float fPosX = 0, fPosY = 0, fPosZ = 0,fHeading = 0, fPitch = 0 ;

	THING()
	{
		ZeroMemory(this, sizeof(THING));
	}
};

extern LPDIRECT3D9 g_pDirect3D;
extern LPDIRECT3DDEVICE9 g_pD3Device;
//LPD3DXMESH pMesh = NULL;
//D3DMATERIAL9* pMeshMaterials = NULL;
//LPDIRECT3DTEXTURE9* pMeshTextures = NULL;
//DWORD dwNumMaterials = 0;
THING Thing[MESHMAX];

float fCameraX = 0, fCameraY = 1.0f, fCameraZ = -3.0f,
fCameraHeading = 0, fCameraPitch = 0, fPosX = 0, fPosY = 0, fPosZ = 0,
LightX = 1, LightY = 1, LightZ = 1;

bool SoundSuccess;
SoundLib::SoundsManager soundsManager;

unsigned int GameRoop(void);
void Render();
void Control(THING* pThing);
HRESULT InitThing(THING *pThing, LPCSTR szXFileName, D3DXVECTOR3* pvecPosition);
void RenderThing(THING* pThing);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstance, LPSTR szStr, INT iCmdShow)
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
		g_pDirect3D->GetAdapterDisplayMode(
			D3DADAPTER_DEFAULT,
			&g_D3DdisplayMode);

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
		g_pTexture["test"] = NULL;
		//「テクスチャオブジェクト」の作成
		if (FAILED(D3DXCreateTextureFromFileEx(g_pD3Device, "Sprite.bmp", 100, 100, 0, 0, D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT,
			0xff000000, NULL, NULL, &g_pTexture["test"])))
		{
			MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
			g_pTexture.erase("test");
			return E_FAIL;
		}
		g_pTexture.erase("test");


		InitThing(&Thing[0], "Chips.x", &D3DXVECTOR3(0, -1, 0));
		InitThing(&Thing[1], "Can.x", &D3DXVECTOR3(0, 0, 0));
		InitThing(&Thing[2], "Bottle.x", &D3DXVECTOR3(1, 0, 1));

		g_pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		g_pD3Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  //SRCの設定
		g_pD3Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pD3Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pD3Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);



		g_pD3Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		// Zバッファー処理を有効にする
		g_pD3Device->SetRenderState(D3DRS_ZENABLE, true);
		// ライトを有効にする
		g_pD3Device->SetRenderState(D3DRS_LIGHTING, true);
		// アンビエントライト（環境光）を設定する
		g_pD3Device->SetRenderState(D3DRS_AMBIENT, 0x00000000);
		// スペキュラ（鏡面反射）を有効にする
		g_pD3Device->SetRenderState(D3DRS_SPECULARENABLE, true);

		SetUpFont(50, 100, "Test", "メイリオ");
		ReadInTexture("BlackSarena.png", "TEST");

		soundsManager.AddFile("216.ブラックサレナ�V（機動戦艦ナデシコ The prince of darkness）.mp3", "BGM");
		soundsManager.AddFile("nc84131.wav", "SE");
		//soundsManager.Start("SE", false);

		FlameRoop(GameRoop);


	
}


unsigned int GameRoop(void) {
	Control(Thing);
	Render();

	CheckKeyState(DIK_ESCAPE);
	if (KeyState[DIK_ESCAPE] == KeyRelease)
	{
		return WM_QUIT;
		for (int i = 0; i<MESHMAX; i++)
		{
			SAFE_RELEASE(Thing[i].pMesh);
		}

	}
	return WM_NULL;


}

//Xファイルから読み込んだメッシュをレンダリングする関数
void Render() {
	g_pD3Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	g_pD3Device->BeginScene();

	for (int i = 0; i < 4; i++)
	{
		RenderThing(&Thing[i]);
	}
	//頂点に入れるデータを設定
	g_pD3Device->SetFVF(D3DFVF_CUSTOMVERTEX);

	CUSTOMVERTEX test[4];
	CENTRAL_STATE testCentral = { 100,100,100,100, };

	CreateSquareVertex(test, testCentral);
	RECT hoge = { 0,0,1000,100 };
	WriteWord("TEST", hoge, "Test");
	SetUpTexture(test, "TEST");
	//EasyCreateSquareVertex(-0.5, -0.5, 0.5, 0.5, "TEST");

	EndSetTexture();

}

void Control(THING* pThing) {
	if(InputKEY(DIK_A))fCameraX -= 0.1f;
	if(InputKEY(DIK_D))fCameraX += 0.1f;
	if(InputKEY(DIK_Q))fCameraY -= 0.1f;
	if(InputKEY(DIK_E))fCameraY += 0.1f;
	if(InputKEY(DIK_W))fCameraZ -= 0.1f;
	if(InputKEY(DIK_S))fCameraZ += 0.1f;
	if(InputKEY(DIK_LEFT))fCameraHeading -= 0.1f;
	if(InputKEY(DIK_RIGHT))fCameraHeading += 0.1f;
	if(InputKEY(DIK_UP))fCameraPitch -= 0.1f;
	if(InputKEY(DIK_DOWN))fCameraPitch += 0.1f;

	if (InputKEY(DIK_J))LightX -= 0.1f;
	if (InputKEY(DIK_L))LightX += 0.1f;
	if (InputKEY(DIK_U))LightY -= 0.1f;
	if (InputKEY(DIK_O))LightY += 0.1f;
	if (InputKEY(DIK_I))LightZ -= 0.1f;
	if (InputKEY(DIK_K))LightZ += 0.1f;
	if (InputKEY(DIK_Y));
	if (InputKEY(DIK_H));
	if (InputKEY(DIK_N));
	if (InputKEY(DIK_M));

	if (InputKEY(DIK_SUBTRACT))pThing->fPosZ -= 0.01f;
	if (InputKEY(DIK_ADD))pThing->fPosZ += 0.01f;
	if (InputKEY(DIK_NUMPAD2))pThing->fPosY -= 0.01f;
	if (InputKEY(DIK_NUMPAD8))pThing->fPosY += 0.01f;
	if (InputKEY(DIK_NUMPAD4))pThing->fPosX -= 0.01f;
	if (InputKEY(DIK_NUMPAD6))pThing->fPosX += 0.01f;
	if (InputKEY(DIK_NUMPAD1))pThing->fHeading -= 0.1f;
	if (InputKEY(DIK_NUMPAD3))pThing->fHeading += 0.1f;
	if (InputKEY(DIK_NUMPAD7))pThing->fPitch -= 0.1f;
	if (InputKEY(DIK_NUMPAD9))pThing->fPitch += 0.1f;

}


HRESULT InitThing(THING *pThing, LPCSTR szXFileName, D3DXVECTOR3* pvecPosition)
{
	// メッシュの初期位置
	memcpy(&pThing->vecPosition, pvecPosition, sizeof(D3DXVECTOR3));
	// Xファイルからメッシュをロードする	
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX(szXFileName, D3DXMESH_SYSTEMMEM,
		g_pD3Device, NULL, &pD3DXMtrlBuffer, NULL,
		&pThing->dwNumMaterials, &pThing->pMesh)))
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました", szXFileName, MB_OK);
		return E_FAIL;
	}
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	pThing->pMeshMaterials = new D3DMATERIAL9[pThing->dwNumMaterials];
	pThing->pMeshTextures = new LPDIRECT3DTEXTURE9[pThing->dwNumMaterials];

	for (DWORD i = 0; i<pThing->dwNumMaterials; i++)
	{
		pThing->pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		pThing->pMeshMaterials[i].Ambient = pThing->pMeshMaterials[i].Diffuse;
		pThing->pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(g_pD3Device,
				d3dxMaterials[i].pTextureFilename,
				&pThing->pMeshTextures[i])))
			{
				MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
			}
		}
	}
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

void RenderThing(THING* pThing){
	//ワールドトランスフォーム（絶対座標変換）
	D3DXMATRIXA16 matWorld, matPosition, fHeading, fPitch;
	D3DXMatrixIdentity(&matWorld);
	for (int i = 0; i < MESHMAX; i++) {
		D3DXMatrixTranslation(&matPosition, pThing->vecPosition.x + pThing->fPosX, pThing->vecPosition.y+ pThing->fPosY,
			pThing->vecPosition.z+ pThing->fPosZ);
		D3DXMatrixRotationY(&fHeading, pThing->fHeading);
		D3DXMatrixRotationX(&fPitch, pThing->fPitch);

	}
	D3DXMatrixMultiply(&matWorld, &matWorld, &matPosition);
	D3DXMatrixMultiply(&matWorld, &matWorld, &fHeading);
	D3DXMatrixMultiply(&matWorld, &matWorld, &fPitch);

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
	D3DXVECTOR3 vecDirection(LightX, LightY, LightZ);
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
	light.Range = 2000.0f;
	g_pD3Device->SetLight(0, &light);
	g_pD3Device->LightEnable(0, true);

	D3DXVECTOR3 vecDirection1(-LightX,- LightY, -LightZ);
	D3DLIGHT9 light1;
	ZeroMemory(&light1, sizeof(D3DLIGHT9));
	light1.Type = D3DLIGHT_DIRECTIONAL;
	light1.Diffuse.r = 1.0f;
	light1.Diffuse.g = 1.0f;
	light1.Diffuse.b = 1.0f;
	light1.Specular.r = 1.0f;
	light1.Specular.g = 1.0f;
	light1.Specular.b = 1.0f;

	D3DXVec3Normalize((D3DXVECTOR3*)&light1.Direction, &vecDirection1);
	light1.Range = 2000.0f;
	g_pD3Device->SetLight(1, &light1);
	g_pD3Device->LightEnable(1, true);

	// レンダリング	 
	for (DWORD i = 0; i<pThing->dwNumMaterials; i++)
	{
		g_pD3Device->SetMaterial(&pThing->pMeshMaterials[i]);
		g_pD3Device->SetTexture(0, pThing->pMeshTextures[i]);
		pThing->pMesh->DrawSubset(i);
	}
}