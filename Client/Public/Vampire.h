#pragma once
#include "Monster.h"

using namespace Vampire;

NS_BEGIN(Client)

class CVampire abstract : public CMonster
{
protected:
	enum VAMPIRE_ATTACK { ATTACK_NORMAL, END};

protected:
	CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire(const CVampire& Prototype);
	virtual ~CVampire() = default;

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
	virtual BT_STATE		IsSpawn() override;
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;

protected:
	HRESULT Ready_AI();

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END