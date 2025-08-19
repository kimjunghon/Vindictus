#pragma once
#include "Component.h"

#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"

NS_BEGIN(Engine)

class CBounding;
class CGameObject;

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct tagColliderDesc
	{
		COLLIDER_OWNER				eOwner;
		COLLIDER_CHANNEL			eChannel;
		CBounding::BOUNDING_DESC*	BoundingDesc;
	}COLLIDER_DESC;

	typedef struct tagBlockCollisionData
	{
		_float fDistance = 0.f;
		_float3 vNormal = _float3(0.f, 0.f, 0.f);
	}BLOCK_COLLISION_DATA;

	typedef struct tagAttackCollisionData
	{
		_bool			IsDown = false;
		_float			fDamage = 0.f;
		_vector			vAttackPosition = XMVectorZero();
	}ATTACK_COLLISON_DATA;

	typedef struct tagCollisionData
	{
		CCollider*				pCollider;
		BLOCK_COLLISION_DATA	BlockData = {};
		_bool					IsAttack = false;
		ATTACK_COLLISON_DATA	AttackData = {};
	}COLLISION_DATA;

private:
	typedef function<void(const COLLISION_DATA&)> Collision_CallBack;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	COLLIDER_OWNER					Get_ColliderOwner() { return m_eOwner; }
	COLLIDER_CHANNEL				Get_ColliderChannel() { return m_eChannel; }
	void							SetCollisionCallBack(Collision_CallBack CallBack) { m_CallBack = CallBack; }
	void							SetAttackData(const ATTACK_COLLISON_DATA& AttackData) { m_AttackData = AttackData; }
	const ATTACK_COLLISON_DATA&		GetAttackData() { return m_AttackData; }
	void							SetEnable(_bool IsEnable) { m_IsEnable = IsEnable; }
	_bool							IsEnable() { return m_IsEnable; }
public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	void			Update(_fmatrix WorldMatrix);
	void			OnCollision(const COLLISION_DATA& Data);

public:
	_bool			Intersect(CCollider* pOtherCollider, _float* pDistance = nullptr, _float3* pNormal = nullptr);
#ifdef _DEBUG
	HRESULT			Render();
#endif

private:
	_bool					m_IsEnable = { false };
	COLLIDER				m_eType = { COLLIDER::END };
	COLLIDER_OWNER			m_eOwner = { COLLIDER_OWNER::END };
	COLLIDER_CHANNEL		m_eChannel = { COLLIDER_CHANNEL::END };
	CBounding*				m_pBounding = { nullptr };
	ATTACK_COLLISON_DATA	m_AttackData = {};
	Collision_CallBack		m_CallBack = {};

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
	_bool									m_IsColl = { false };
#endif




public:
	static CCollider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END