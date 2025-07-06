#include "Component.h"
#include "GameInstance.h"
CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice {pDevice}
	, m_pDeviceContext {pDeviceContext}
	, m_pGameInstance { CGameInstance::GetInstance()}
	, m_IsCloned {false}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

CComponent::CComponent(const CComponent& Prototype)
	: m_pDevice{ Prototype.m_pDevice }
	, m_pDeviceContext{ Prototype.m_pDeviceContext }
	, m_pGameInstance{ Prototype.m_pGameInstance }
	, m_IsCloned {true}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
}
