#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
public:

protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	_bool			NeedCloned() { return m_bNeedCloned; }

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	_bool						m_IsCloned = { false };
	_bool						m_bNeedCloned = { true };

#ifdef _DEBUG
public:
	virtual HRESULT Render() { return S_OK; }
#endif

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void		Free() override;
};

NS_END