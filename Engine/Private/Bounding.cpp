#include "EnginePch.h"
#include "Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice}
	, m_pDeviceContext { pDeviceContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CBounding::Initialize()
{
	return S_OK;
}

void CBounding::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
