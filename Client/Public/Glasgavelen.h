#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CGlasgavelenSword;
class CBody;

class CGlasgavelen final : public CMonster
{
private:
	CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelen(const CGlasgavelen& Prototype);
	virtual ~CGlasgavelen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) override;

public:
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;

private:
	CGlasgavelenSword* m_pSword = { nullptr };

private:
	HRESULT Ready_PawnObjects();
	HRESULT	Ready_AI();
	HRESULT	Ready_GavelenStates();

	HRESULT	Ready_Collider();
	HRESULT	Ready_Collider_Bounding();
	HRESULT	Ready_Collider_Body_Hit();
	HRESULT	Ready_Collider_Attack();
	HRESULT	Ready_AttackMapping();

public:
	static CGlasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END