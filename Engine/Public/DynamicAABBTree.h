#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;

class CDynamicAABBTree : public CBase
{
private:
	typedef struct tagDyanmicAABBNode {
		tagDyanmicAABBNode*		pParent = { nullptr };
		tagDyanmicAABBNode*		pLeftChild = { nullptr };
		tagDyanmicAABBNode*		pRightChild = { nullptr };
		BoundingBox				BoundingBox = {};
		CGameObject*			pGameObject = { nullptr };
		_uint					iHeight = {};

		_bool				IsLeaf() { return nullptr == pLeftChild && nullptr == pRightChild; }
	}DYNAMICAABB_NODE;


private:
	CDynamicAABBTree();
	virtual ~CDynamicAABBTree() = default;

public:
	HRESULT				Check_Collision_AABBTree(const BoundingBox& ObjectBoundingBox, const CGameObject* pGameObject);

	//m_pRoot로 관리
	HRESULT				Add_Node(CGameObject* pGameObject, const BoundingBox& ObjectBoundingBox);
	DYNAMICAABB_NODE*	CreateNode(const BoundingBox& ObjectBoundingBox, CGameObject* pGameObject = nullptr);
	HRESULT				RemoveNode(CGameObject* pGameObject);
	
private:
	DYNAMICAABB_NODE*								m_pRoot = { nullptr };
	unordered_map<CGameObject*, DYNAMICAABB_NODE*>	m_Nodes;

private:
	void				CheckCollisionNode(DYNAMICAABB_NODE* pNode, const BoundingBox& ObjectBoundingBox, const CGameObject* pGameObject);

	DYNAMICAABB_NODE*	LowCostChild(DYNAMICAABB_NODE* pNode, const BoundingBox& ObjectBoundingBox);							// Left, Right 둘 중 MergeBoundingBoxes를 했을때 겉넓이가 낮은쪽 return
	_float				SurfaceArea(const BoundingBox& BoundingBox);								// BoundingBox 겉넓이 return
	BoundingBox			MergeBoundingBoxes(const BoundingBox& NodeBoundingBox, const BoundingBox& ObjectBoundingBox) const;				// m_BoundingBox와 Other을 합쳐서 return
	_bool				IsLeftChild(const BoundingBox& NodeBoundingBox, const BoundingBox& ObjectBoundingBox) { return NodeBoundingBox.Center.x < ObjectBoundingBox.Center.x; }	// Left Child에 넣을지 판단
	_bool				IsIntersects(const BoundingBox& NodeBoundingBox, const BoundingBox& OtherBoundingBox) { return NodeBoundingBox.Intersects(OtherBoundingBox); }					// BoundingBox Check
	_int				UpdateNodeHeight(DYNAMICAABB_NODE* pRoot);
	_int				CheckNodeBalance(const DYNAMICAABB_NODE* pRoot);
	HRESULT				Rebalancing(DYNAMICAABB_NODE* pNode, DYNAMICAABB_NODE* pRebalancingNode);
	HRESULT				RebalancingBoundingBoxes(DYNAMICAABB_NODE* pNode);
	//m_Nodes 관리
	DYNAMICAABB_NODE*	Find_Node(CGameObject* pGameObject);
	HRESULT				Add_NodeMap(CGameObject* pGameObject, DYNAMICAABB_NODE* pNode);
	HRESULT				Remove_NodeMap(CGameObject* pGameObject);

	// m_pRoot 관리
	HRESULT				Release_Nodes(DYNAMICAABB_NODE* pRoot);

public:
	static CDynamicAABBTree*	Create();
	virtual void				Free() override;
};

NS_END