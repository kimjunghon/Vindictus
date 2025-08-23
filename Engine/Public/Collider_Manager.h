#pragma once
#include "Base.h"
#include "Collider.h"

NS_BEGIN(Engine)

class CGameObject;

class CCollider_Manager final : public CBase
{
private:
	typedef unordered_map<pair<_uint, _uint>, COLLIDER_TYPE, PairHash<_uint>, PairEqual<_uint>>				COLLIDER_CHANNEL;
	typedef unordered_map<CGameObject*, CCollider*>															BOUNDING_COLLDER;
	typedef unordered_map<CGameObject*, vector<CCollider*>>													ACTION_COLLDER;
	typedef unordered_set<pair<CGameObject*, CGameObject*>, PairHash<CGameObject*>, PairEqual<CGameObject*>>CHECK_COLLIDER;
private:
	CCollider_Manager();
	virtual ~CCollider_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update();

	HRESULT				Add_Channel(_uint iSrcChannel, _uint iDstChannel, COLLIDER_TYPE eType);
	HRESULT				Change_Type_ToChannel(_uint iSrcChannel, _uint iDstChannel, COLLIDER_TYPE eChangeType);
	HRESULT				Add_BoundingCollider(CGameObject* pOwner, CCollider* pBounding_Collider);
	HRESULT				Add_ActionCollider(CGameObject* pOwner, CCollider* pAction_Collider);

private:
	COLLIDER_CHANNEL		m_Channels;
	BOUNDING_COLLDER		m_Boundings;
	ACTION_COLLDER			m_Actions;
	CHECK_COLLIDER			m_CheckCollisions;
	
private:
	void				Clear_Collider();
	COLLIDER_TYPE		Check_Type(_uint iSrcChannel, _uint iDstChannel);
	_bool				Intersect_Bounding(CCollider* pSrcCollider ,CCollider* pDstCollider);
	void				Intersect_Actions(const vector<CCollider*>& SrcColliders, const vector<CCollider*>& DstColliders);
	void				Check_Collision(CCollider* pSrcCollider, CCollider* pDstCollider);
	const CCollider*	Find_BoundingCollider(CGameObject* pOwner);
	_bool				Has_ActionCollider(CGameObject* pOwner);

public:
	static CCollider_Manager*	Create();
	virtual void				Free() override;
};

NS_END