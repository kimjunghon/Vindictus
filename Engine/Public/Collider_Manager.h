#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;
class CCollider;

class CCollider_Manager final : public CBase
{
private:
	typedef unordered_map<CGameObject*, CCollider*>			BOUNDING_COLLDER;
	typedef unordered_map<CGameObject*, vector<CCollider*>> ACTION_COLLDER;

private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update();

	HRESULT				Add_BoundingCollider(CGameObject* pOwner, CCollider* pBounding_Collider);
	HRESULT				Add_ActionCollider(CGameObject* pOwner, CCollider* pAction_Collider);

private:
	COLLIDER_TYPE		m_eResponseTable[ENUM_CLASS(COLLIDER_CHANNEL::END)][ENUM_CLASS(COLLIDER_CHANNEL::END)] = {};
	BOUNDING_COLLDER	m_Boundings;
	ACTION_COLLDER		m_Actions;

private:
	void				Clear_Collider();
	_bool				Intersect_Bounding(CCollider* pSrcCollider ,CCollider* pDstCollider);
	void				Check_Collision(const vector<CCollider*>& SrcColliders, const vector<CCollider*>& DstColliders, CGameObject* pDstOwner);
	void				OnCollision(CCollider* pSrcCollider, CCollider* pDstCollider, CGameObject* pDstOwner);
	HRESULT				Ready_ResponseTable();
	const CCollider*	Find_BoundingCollider(CGameObject* pOwner);
	_bool				Has_ActionCollider(CGameObject* pOwner);

public:
	static CCollider_Manager*	Create();
	virtual void				Free() override;
};

NS_END