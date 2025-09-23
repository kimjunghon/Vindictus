#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider_Container;
NS_END

NS_BEGIN(Client)

class CTriggerBox final : public CGameObject
{
public:
	typedef struct tagTriggerDesc : public GAMEOBJECT_DESC {
		function<void()> Callback;
	}TRIGGER_DESC;

private:
	CTriggerBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTriggerBox(const CTriggerBox& Prototype);
	virtual ~CTriggerBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider_Container*	m_pCollider_Container = { nullptr };
	function<void()>		m_Callback;

private:
	HRESULT Ready_Component();

public:
	static CTriggerBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END