#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CCollisionObject;

class COctree final : public CBase
{
public:
	typedef struct tagOctreeDesc {
		_float3		vCenterPosition;
		_float3		vExtents;
		_uint		iMaxOctreeDepth;
		_uint		iMaxObjectCount;
	}OCTREE_DESC;

private:
	typedef struct tagOctreeNode {
		tagOctreeNode*				pChilds[8] = { nullptr };
		BoundingBox					BoundingBox;
		vector<CCollisionObject*>	Objects;
		_bool						bCulled = { false };
		_uint						iOctreeDepth;
	}OCTREE_NODE;

private:
	COctree();
	virtual ~COctree() = default;

private:
	_bool	IsIntersect(const OCTREE_NODE* pNode, const BoundingBox& ObjectBoundingBox) {
		return pNode->BoundingBox.Intersects(ObjectBoundingBox);
	}

	_bool	IsLeafNode(OCTREE_NODE* pNode) {
		return pNode->pChilds[0] == nullptr;
	}

public:
	HRESULT Initialize(const OCTREE_DESC& tDesc);
	HRESULT Add_ObjectToTree(CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);
	HRESULT Add_ObjectToNode(OCTREE_NODE* pNode, CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);
	HRESULT	CheckCollisionOctree(const CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);

	void	Priority_Update(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);
	
private:
	OCTREE_NODE*			m_pRoot = { nullptr };
	_uint					m_iMaxOctreeDepth = {};
	_uint					m_iMaxObjectCount = {};
	

private:
	HRESULT			Ready_Octree(OCTREE_NODE* pNode, _float3 vCenterPosition, _float3 vExtents, _uint iOctreeDepth);
	HRESULT			Devide_Node(OCTREE_NODE* pNode, _float3 vCenterPosition, _float3 vExtents);
	HRESULT			ReInsertObjects(OCTREE_NODE* pNode);
	void			CheckCollisionNode(OCTREE_NODE* pNode, const CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox);

	OCTREE_NODE*	Create_Node(_float3 vCenterPosition, _float3 vExtents, _uint iOctreeDepth);

	HRESULT			Release_Nodes(OCTREE_NODE* pRoot);

public:
	static COctree* Create(const OCTREE_DESC& tDesc);
	virtual void	Free() override;
};

NS_END