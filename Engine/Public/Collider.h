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

	typedef struct tagCollisionData
	{
		CGameObject*	pOwner;
		CCollider*		pCollider;
	}COLLISION_DATA;

private:
	typedef function<void(const COLLISION_DATA&)> Collision_CallBack;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	COLLIDER_OWNER		Get_ColliderOwner() { return m_eOwner; }
	COLLIDER_CHANNEL	Get_ColliderChannel() { return m_eChannel; }
	void				SetCollisionCallBack(Collision_CallBack CallBack) { m_CallBack = CallBack; }

public:
	virtual HRESULT Initialize_Prototype(COLLIDER eType);
	virtual HRESULT Initialize(void* pArg) override;
	void			Update(_fmatrix WorldMatrix);
	void			OnCollision(CGameObject* pOwner, CCollider* pCollider);

public:
	_bool			Intersect(CCollider* pOtherCollider);
#ifdef _DEBUG
	HRESULT			Render();
#endif

private:
	COLLIDER				m_eType = { COLLIDER::END };
	COLLIDER_OWNER			m_eOwner = { COLLIDER_OWNER::END };
	COLLIDER_CHANNEL		m_eChannel = { COLLIDER_CHANNEL::END };
	CBounding*				m_pBounding = { nullptr };

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