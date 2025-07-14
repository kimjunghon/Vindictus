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

CController_UI* CController_UI::Create()
{
	return new CController_UI();
}

void CController_UI::Free()
{
	__super::Free();
}
