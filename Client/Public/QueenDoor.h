#pragma once
#include "MapObject.h"

NS_BEGIN(Engine)
class CColliderContainer;
NS_END

NS_BEGIN(Client)

class CPool_Instance;

class CQueenDoor final : public CMapObject
{
private:
	CQueenDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CQueenDoor(const CQueenDoor& Prototype);
	virtual ~CQueenDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CColliderContainer* m_pColliderContainer = { nullptr };
	CPool_Instance*		m_pPoolInstance = { nullptr };

	_vector				m_vEffectPosition = {};

	_bool				m_IsOpen = {};
	_bool				m_IsDown = {};
	
	_float				m_fMinY = {};
	_float				m_fEffectDelay = {};
	_float				m_fCurrentEffectDelay = {};
	_float				m_fDownSpeed = {};

private:
	HRESULT Ready_Colliders();
	void	OpenDoor(_float fTimeDelta);
	void	Event_QueenDead(const EVENT_QUEEN_DEAD& Event);

public:
	static CQueenDoor*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END