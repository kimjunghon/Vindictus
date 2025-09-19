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
	enum class COLLIDER_STATE { BEGIN, DURING, END, STATE_END };

	typedef struct tagColliderDesc
	{
		_uint						iOwner;
		_uint						iChannel;
		CBounding::BOUNDING_DESC*	BoundingDesc;
	}COLLIDER_DESC;

	typedef struct tagBlockCollisionData
	{
		_float fDistance = 0.f;
		_float3 vNormal = _float3(0.f, 0.f, 0.f);
		_float3 vCollisionPos = _float3(0.f, 0.f, 0.f);
	}BLOCK_COLLISION_DATA;

	typedef struct tagCollisionData
	{
		_bool					IsEnable;
		CCollider*				pCollider;
		BLOCK_COLLISION_DATA	BlockData = {};
		void*					pDesc;
	}COLLISION_DATA;

	typedef function<void(const COLLISION_DATA&)> Collision_CallBack;

private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	_uint							Get_ColliderOwner() { return m_iOwner; }
	_uint							Get_ColliderChannel() { return m_iChannel; }
	void							SetCollisionCallBack(_uint iCallBackIndex, Collision_CallBack CallBack) { m_CallBack[iCallBackIndex] = CallBack; }
	_bool							IsEnable() { return m_IsEnable; }
	void							Set_Desc(void* pDesc) { m_pDesc = pDesc; }
	void*							Get_Desc() { return m_pDesc; }

public:
	virtual HRESULT					Initialize_Prototype(COLLIDER eType);
	virtual HRESULT					Initialize(void* pArg) override;
	void							Update(_fmatrix WorldMatrix, _bool IsRotate = true);
	void							SetEnable(_bool IsEnable);

public:
	_bool			Intersect_BoundingChannel(CCollider* pOtherCollider);
	void			Check_Collision(COLLISION_DATA& Data);

#ifdef _DEBUG
	virtual HRESULT	Render() override;
#endif

private:
	_bool						m_IsEnable = { true };
	COLLIDER					m_eType = { COLLIDER::END };
	_uint						m_iOwner = {};
	_uint						m_iChannel = {  };
	void*						m_pDesc = { nullptr };
	Collision_CallBack			m_CallBack[ENUM_CLASS(COLLIDER_STATE::STATE_END)] = {};
	unordered_set<CCollider*>	m_CollisionColliders;
	CBounding*					m_pBounding = { nullptr };

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = { nullptr };
	BasicEffect*							m_pEffect = { nullptr };
	ID3D11InputLayout*						m_pInputLayout = { nullptr };
	_bool									m_IsColl = { false };
#endif

private:
	_bool			Intersect(CCollider* pOtherCollider, _float* pDistance = nullptr, _float3* pNormal = nullptr, _float3* pCollisionPos = nullptr);
	void			OnCollision(COLLIDER_STATE eState, const COLLISION_DATA& Data);
	_bool			IsColliding(CCollider* pCollider);
	void			EraseCollisionCollider(CCollider* pCollider);
public:
	static CCollider*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, COLLIDER eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END