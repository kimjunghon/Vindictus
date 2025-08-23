#include "ClientPch.h"
#include "Controller_UI.h"

CController_UI::CController_UI()
{
}

HRESULT CController_UI::Initialize()
{
	return S_OK;
}

void CController_UI::Update(_float fTimeDelta)
{
}

HRESULT CController_UI::UI_Input(INPUT_UI_DESC* pOut)
{
	INPUT_UI_DESC UI_Desc = {};

	if (m_pGameInstance->Get_KeyDown(DIK_ESCAPE))
		UI_Desc.bOption = true;
	if (m_pGameInstance->Get_KeyDown(DIK_LCONTROL))
		UI_Desc.bMouse = true;
	if (m_pGameInstance->Get_KeyDown(DIK_I))
		UI_Desc.bInventory = true;

	*pOut = UI_Desc;

	return S_OK;
}

CController_UI* CController_UI::Create()
{
	return new CController_UI();	
}

void CController_UI::Free()
{ 
	__super::Free();
}

