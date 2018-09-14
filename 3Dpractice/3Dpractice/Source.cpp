#include "TrialLib.h"
#include "TrialEnums.h"
#include "SoundsManager.h"
#pragma comment (lib,"DirectX_LIB.lib")
#pragma comment (lib,"SoundLIB.lib")

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

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
THING Thing[4];

float fCameraX = 0, fCameraY = 1.0f, fCameraZ = -3.0f,
fCameraHeading = 0, fCameraPitch = 0, fPosX = 0, fPosY = 0, fPosZ = 0;

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

	
		//// �uDirect3D�v�I�u�W�F�N�g�̍쐬
		//if (NULL == (g_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)))
		//{
		//	MessageBox(0, "Direct3D�̍쐬�Ɏ��s���܂���", "", MB_OK);
		//	return E_FAIL;
		//}
		// �uDIRECT3D�f�o�C�X�v�I�u�W�F�N�g�̍쐬
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
			MessageBox(0, "HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\nREF���[�h�ōĎ��s���܂�", NULL, MB_OK);
			if (FAILED(g_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
				D3DCREATE_MIXED_VERTEXPROCESSING,
				&d3dpp, &g_pD3Device)))
			{
				MessageBox(0, "DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���", NULL, MB_OK);
				return E_FAIL;
			}
		}

		//// X�t�@�C�����烁�b�V�������[�h����	
		//LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

		//if (FAILED(D3DXLoadMeshFromX("Chips.x", D3DXMESH_SYSTEMMEM,
		//	g_pD3Device, NULL, &pD3DXMtrlBuffer, NULL,
		//	&dwNumMaterials, &pMesh)))
		//{
		//	MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
		//	return E_FAIL;
		//}
		//D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
		//pMeshMaterials = new D3DMATERIAL9[dwNumMaterials];
		//pMeshTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

		//for (DWORD i = 0; i < dwNumMaterials; i++)
		//{
		//	pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		//	pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;
		//	pMeshTextures[i] = NULL;
		//	if (d3dxMaterials[i].pTextureFilename != NULL &&
		//		lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		//	{
		//		if (FAILED(D3DXCreateTextureFromFile(g_pD3Device,
		//			d3dxMaterials[i].pTextureFilename,
		//			&pMeshTextures[i])))
		//		{
		//			MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
		//		}
		//	}
		//}
		//pD3DXMtrlBuffer->Release();
		InitThing(&Thing[0], "Chips.x", &D3DXVECTOR3(0, -1, 0));
		InitThing(&Thing[1], "Can.x", &D3DXVECTOR3(0, 0, 0));
		InitThing(&Thing[2], "Bottle.x", &D3DXVECTOR3(1, 0, 1));

		// Z�o�b�t�@�[������L���ɂ���
		g_pD3Device->SetRenderState(D3DRS_ZENABLE, TRUE);
		// ���C�g��L���ɂ���
		g_pD3Device->SetRenderState(D3DRS_LIGHTING, TRUE);
		// �A���r�G���g���C�g�i�����j��ݒ肷��
		g_pD3Device->SetRenderState(D3DRS_AMBIENT, 0x00111111);
		// �X�y�L�����i���ʔ��ˁj��L���ɂ���
		g_pD3Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

		soundsManager.AddFile("216.�u���b�N�T���i�V�i�@����̓i�f�V�R The prince of darkness�j.mp3", "BGM");
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
		for (int i = 0; i<4; i++)
		{
			SAFE_RELEASE(Thing[i].pMesh);
		}

	}
	return WM_NULL;


}

//X�t�@�C������ǂݍ��񂾃��b�V���������_�����O����֐�
void Render() {
	////���[���h�g�����X�t�H�[���i��΍��W�ϊ��j
	//D3DXMATRIXA16 matWorld, matPosition, matRotation;
	//D3DXMatrixIdentity(&matWorld);
	////D3DXMatrixTranslation(&matPosition, fPosX, fPosY, fPosZ);
	//D3DXMatrixRotationY(&matWorld, timeGetTime() / 3000.0f);
	//D3DXMatrixRotationX(&matRotation, 0.5f);
	//D3DXMatrixMultiply(&matWorld, &matWorld, &/*matRotation*/matPosition);
	//g_pD3Device->SetTransform(D3DTS_WORLD, &matWorld);
	// �r���[�g�����X�t�H�[���i���_���W�ϊ��j
	//D3DXVECTOR3 vecEyePt(fCameraX, fCameraY, fCameraZ); //�J�����i���_�j�ʒu
	//D3DXVECTOR3 vecLookatPt(fCameraX, fCameraY - 1.0f, fCameraZ + 3.0f);//�����ʒu
	//D3DXVECTOR3 vecUpVec(0.0f, 1.0f, 0.0f);//����ʒu
	//D3DXMATRIXA16 matView, matCameraPosition, matHeading, matPitch;
	//D3DXMatrixIdentity(&matView);
	//D3DXMatrixRotationY(&matHeading, fCameraHeading);
	//D3DXMatrixRotationX(&matPitch, fCameraPitch);
	//D3DXMatrixLookAtLH(&matCameraPosition, &vecEyePt, &vecLookatPt, &vecUpVec);
	//D3DXMatrixMultiply(&matView, &matView, &matHeading);
	//D3DXMatrixMultiply(&matView, &matView, &matPitch);
	//D3DXMatrixMultiply(&matView, &matView, &matCameraPosition);
	//g_pD3Device->SetTransform(D3DTS_VIEW, &matView);
	//// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j
	//D3DXMATRIXA16 matProj;
	//D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	//g_pD3Device->SetTransform(D3DTS_PROJECTION, &matProj);
	//// ���C�g�����Ă� ���F�ŋ��ʔ��˂���ɐݒ�
	//D3DXVECTOR3 vecDirection(0, 0, 1);
	//D3DLIGHT9 light;
	//ZeroMemory(&light, sizeof(D3DLIGHT9));
	//light.Type = D3DLIGHT_DIRECTIONAL;
	//light.Diffuse.r = 1.0f;
	//light.Diffuse.g = 1.0f;
	//light.Diffuse.b = 1.0f;
	//light.Specular.r = 1.0f;
	//light.Specular.g = 1.0f;
	//light.Specular.b = 1.0f;
	//D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDirection);
	//light.Range = 200.0f;
	//g_pD3Device->SetLight(0, &light);
	//g_pD3Device->LightEnable(0, TRUE);
	// �����_�����O
	g_pD3Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	g_pD3Device->BeginScene();
	for (int i = 0; i < 4; i++)
	{
		RenderThing(&Thing[i]);
	}
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

	if (InputKEY(DIK_NUMPAD7))pThing->fPosZ -= 0.01f;
	if (InputKEY(DIK_NUMPAD9))pThing->fPosZ += 0.01f;
	if (InputKEY(DIK_NUMPAD2))pThing->fPosY -= 0.01f;
	if (InputKEY(DIK_NUMPAD8))pThing->fPosY += 0.01f;
	if (InputKEY(DIK_NUMPAD4))pThing->fPosX -= 0.01f;
	if (InputKEY(DIK_NUMPAD6))pThing->fPosX += 0.01f;
	if (InputKEY(DIK_NUMPAD1))pThing->fHeading += 0.1f;
	if (InputKEY(DIK_NUMPAD3))pThing->fPitch += 0.1f;

}


HRESULT InitThing(THING *pThing, LPCSTR szXFileName, D3DXVECTOR3* pvecPosition)
{
	// ���b�V���̏����ʒu
	memcpy(&pThing->vecPosition, pvecPosition, sizeof(D3DXVECTOR3));
	// X�t�@�C�����烁�b�V�������[�h����	
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX(szXFileName, D3DXMESH_SYSTEMMEM,
		g_pD3Device, NULL, &pD3DXMtrlBuffer, NULL,
		&pThing->dwNumMaterials, &pThing->pMesh)))
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", szXFileName, MB_OK);
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
				MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
			}
		}
	}
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

void RenderThing(THING* pThing){
	//���[���h�g�����X�t�H�[���i��΍��W�ϊ��j
	D3DXMATRIXA16 matWorld, matPosition, fHeading, fPitch;
	D3DXMatrixIdentity(&matWorld);
	for (int i = 0; i < 4; i++) {
		D3DXMatrixTranslation(&matPosition, pThing->vecPosition.x + pThing->fPosX, pThing->vecPosition.y+ pThing->fPosY,
			pThing->vecPosition.z+ pThing->fPosZ);
		D3DXMatrixRotationY(&fHeading, pThing->fHeading);
		D3DXMatrixRotationX(&fPitch, pThing->fPitch);

	}
	D3DXMatrixMultiply(&matWorld, &matWorld, &matPosition);
	D3DXMatrixMultiply(&matWorld, &matWorld, &fHeading);
	D3DXMatrixMultiply(&matWorld, &matWorld, &fPitch);

	g_pD3Device->SetTransform(D3DTS_WORLD, &matWorld);
	// �r���[�g�����X�t�H�[���i���_���W�ϊ��j
	D3DXVECTOR3 vecEyePt(fCameraX, fCameraY, fCameraZ); //�J�����i���_�j�ʒu
	D3DXVECTOR3 vecLookatPt(fCameraX, fCameraY - 1.0f, fCameraZ + 3.0f);//�����ʒu
	D3DXVECTOR3 vecUpVec(0.0f, 1.0f, 0.0f);//����ʒu
	D3DXMATRIXA16 matView, matCameraPosition, matHeading, matPitch;
	D3DXMatrixIdentity(&matView);
	D3DXMatrixRotationY(&matHeading, fCameraHeading);
	D3DXMatrixRotationX(&matPitch, fCameraPitch);
	D3DXMatrixLookAtLH(&matCameraPosition, &vecEyePt, &vecLookatPt, &vecUpVec);
	D3DXMatrixMultiply(&matView, &matView, &matHeading);
	D3DXMatrixMultiply(&matView, &matView, &matPitch);
	D3DXMatrixMultiply(&matView, &matView, &matCameraPosition);
	g_pD3Device->SetTransform(D3DTS_VIEW, &matView);
	// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	g_pD3Device->SetTransform(D3DTS_PROJECTION, &matProj);
	// ���C�g�����Ă� ���F�ŋ��ʔ��˂���ɐݒ�
	D3DXVECTOR3 vecDirection(1, 1, 1);
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
	// �����_�����O	 
	for (DWORD i = 0; i<pThing->dwNumMaterials; i++)
	{
		g_pD3Device->SetMaterial(&pThing->pMeshMaterials[i]);
		g_pD3Device->SetTexture(0, pThing->pMeshTextures[i]);
		pThing->pMesh->DrawSubset(i);
	}
}