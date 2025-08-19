#pragma once
#include "Monster.h"

using namespace Queen;

NS_BEGIN(Client)

class CQueen final : public CMonster
{
private:
	enum ATTACK { DOUBLE, SWOOP, POISON, LEFTHAND, RIGHTHAND, END };

private:
	CQueen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CQueen(const CQueen& Prototype);
	virtual ~CQueen() = default;

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
	
	BT_STATE				CanBurrow();
	BT_STATE				Burrow();

	BT_STATE				CanNearAttack();
	BT_STATE				NearAttack();

	BT_STATE				IsLook();
	BT_STATE				LookForPlayer();


	BT_STATE				Idle();
	
private:
	_bool					m_IsLookForPlayer = {};
	_bool					m_IsBurrow = {};
	_uint					m_iBurrowActionCount = {};


	_vector					m_fCurrentRotation = {};
	_float					m_fNearAttackTime = {};
	_float					m_fNearAttackCoolTime = {};
private:
	HRESULT Ready_PawnObjects();
	HRESULT Ready_AI();
	void	Compute_AnimPosition();

	void	Check_Near(_float fTimeDelta);
	

public:
	static CQueen*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END