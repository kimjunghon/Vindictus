#include "EnginePch.h"
#include "DynamicAABBTree.h"
#include "GameObject.h"

CDynamicAABBTree::CDynamicAABBTree()
{
}

HRESULT CDynamicAABBTree::Check_Collision_AABBTree(const BoundingBox& ObjectBoundingBox, const CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	DYNAMICAABB_NODE* pNode = m_pRoot;

	if (nullptr == pNode)
		return S_OK;

	CheckCollisionNode(pNode, ObjectBoundingBox, pGameObject);

	return S_OK;
}

void CDynamicAABBTree::CheckCollisionNode(DYNAMICAABB_NODE* pNode, const BoundingBox& ObjectBoundingBox, const CGameObject* pGameObject)
{
	if (false == IsIntersects(pNode->BoundingBox, ObjectBoundingBox))
		return;

	if (pNode->IsLeaf())
	{
		//		CollisionCheck(pGameObject);
	}
	else
	{
		if (IsIntersects(pNode->pLeftChild->BoundingBox, ObjectBoundingBox))
			CheckCollisionNode(pNode->pLeftChild, ObjectBoundingBox, pGameObject);

		if (IsIntersects(pNode->pRightChild->BoundingBox, ObjectBoundingBox))
			CheckCollisionNode(pNode->pRightChild, ObjectBoundingBox, pGameObject);
	}
}


HRESULT CDynamicAABBTree::Add_Node(CGameObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr == m_pRoot)
	{
		m_pRoot = CreateNode(ObjectBoundingBox, pGameObject);

		if (nullptr == m_pRoot)
			return E_FAIL;

		return S_OK;
	}

	DYNAMICAABB_NODE* pNode = m_pRoot;
	DYNAMICAABB_NODE* pOldNode = pNode;

	while (false == pNode->IsLeaf())
	{
		pOldNode = pNode;
		pNode = LowCostChild(pNode, ObjectBoundingBox);
	}

	BoundingBox MergeBoundingBox = MergeBoundingBoxes(pNode->BoundingBox, ObjectBoundingBox);
	
	DYNAMICAABB_NODE* pParentNode = CreateNode(MergeBoundingBox);

	if (nullptr == pParentNode)
		return E_FAIL;

	DYNAMICAABB_NODE* pNewChildNode = CreateNode(ObjectBoundingBox, pGameObject);
	if (nullptr == pNewChildNode)
		return E_FAIL;

	Add_NodeMap(pGameObject, pNewChildNode);										// 변동 체크용 노드에 추가
	
	IsLeftChild(pNode->BoundingBox, ObjectBoundingBox) ? pParentNode->pLeftChild = pNode, pParentNode->pRightChild = pNewChildNode :
															pParentNode->pLeftChild = pNewChildNode, pParentNode->pRightChild = pNode;
	pNode->pParent = pParentNode;
	pNewChildNode->pParent = pParentNode;


	if (pNode == m_pRoot)
		m_pRoot = pParentNode;
	else
	{
		pOldNode->pLeftChild == pNode ? pOldNode->pLeftChild = pParentNode : pOldNode->pRightChild = pParentNode;

		pParentNode->pParent = pOldNode;
	}

	RebalancingBoundingBoxes(pParentNode->pParent);

	UpdateNodeHeight(m_pRoot);
	_int NodeBalance = CheckNodeBalance(m_pRoot);

	if (abs(NodeBalance) == 2)
	{
		DYNAMICAABB_NODE* pChangeNode = nullptr; // 현재 높이가 가장 낮은 노드

		if (NodeBalance == 2) // 왼쪽 불균형
		{
		
		}
		else if (NodeBalance == -2) // 오른쪽 불균형
		{
		
		}

		if(FAILED(Rebalancing(pParentNode, pChangeNode)))
			return E_FAIL;
	}

	return S_OK;
}

CDynamicAABBTree::DYNAMICAABB_NODE* CDynamicAABBTree::CreateNode(const BoundingBox& ObjectBoundingBox, CGameObject* pGameObject)
{
	DYNAMICAABB_NODE* pNode = new DYNAMICAABB_NODE;
	pNode->BoundingBox = ObjectBoundingBox;
	pNode->pGameObject = pGameObject;
	pNode->iHeight = 0;

	if(nullptr != pNode->pGameObject)
		Safe_AddRef(pNode->pGameObject);

	return pNode;
}


HRESULT CDynamicAABBTree::RemoveNode(CGameObject* pGameObject)
{
	DYNAMICAABB_NODE* pNode = Find_Node(pGameObject);

	if (nullptr == pNode || false == pNode->IsLeaf())
		return E_FAIL;

	if (pNode == m_pRoot)
	{
		Safe_Release(m_pRoot->pGameObject);
		Safe_Delete(m_pRoot);
	}
	else
	{
		DYNAMICAABB_NODE* pGrandNode = pNode->pParent->pParent;
		DYNAMICAABB_NODE* pOtherChild = pNode->pParent->pLeftChild == pNode ? pNode->pParent->pRightChild : pNode->pParent->pLeftChild;

		if (nullptr == pOtherChild)
			return E_FAIL;

		if (pNode->pParent == m_pRoot)
		{
			m_pRoot = pOtherChild;
		}
		else
		{
			pGrandNode->pLeftChild == pNode->pParent ? pGrandNode->pLeftChild = pOtherChild : pGrandNode->pRightChild = pOtherChild;
			pOtherChild->pParent = pGrandNode;
		}

		pNode->pParent->pLeftChild = nullptr;
		pNode->pParent->pRightChild = nullptr;
		
		Safe_Release(pNode->pGameObject);
		Safe_Delete(pNode->pParent);
		Safe_Delete(pNode);
	}


	UpdateNodeHeight(m_pRoot);
	Remove_NodeMap(pGameObject);

	return S_OK;
}

CDynamicAABBTree::DYNAMICAABB_NODE* CDynamicAABBTree::Find_Node(CGameObject* pGameObject)
{
	auto Pair = m_Nodes.find(pGameObject);

	if (Pair == m_Nodes.end())
		return nullptr;

	return Pair->second;
}

HRESULT CDynamicAABBTree::Add_NodeMap(CGameObject* pGameObject, DYNAMICAABB_NODE* pNode)
{
	if (nullptr == pGameObject ||
		nullptr == pNode)
		return E_FAIL;

	Safe_AddRef(pGameObject);
	m_Nodes.emplace(pGameObject, pNode);

	return S_OK;
}

HRESULT CDynamicAABBTree::Remove_NodeMap(CGameObject* pGameObject)
{
	auto Pair = m_Nodes.find(pGameObject);

	if (Pair == m_Nodes.end())
		return E_FAIL;

	Safe_Release(const_cast<CGameObject*&>(Pair->first));
	
	m_Nodes.erase(Pair);

	return S_OK;
}

BoundingBox CDynamicAABBTree::MergeBoundingBoxes(const BoundingBox& NodeBoundingBox, const BoundingBox& ObjectBoundingBox) const
{
	BoundingBox NewBoundingBox = {};

	XMVECTOR vMergeMin = XMVectorMin(XMVectorSubtract(XMLoadFloat3(&ObjectBoundingBox.Center), XMLoadFloat3(&ObjectBoundingBox.Extents)),
		XMVectorSubtract(XMLoadFloat3(&NodeBoundingBox.Center), XMLoadFloat3(&NodeBoundingBox.Extents)));

	XMVECTOR vMergeMax = XMVectorMax(XMVectorAdd(XMLoadFloat3(&ObjectBoundingBox.Center), XMLoadFloat3(&ObjectBoundingBox.Extents)),
		XMVectorAdd(XMLoadFloat3(&NodeBoundingBox.Center), XMLoadFloat3(&NodeBoundingBox.Extents)));

	_float3 vMergeCenterPos = {};
	XMStoreFloat3(&vMergeCenterPos, XMVectorScale(XMVectorAdd(vMergeMin, vMergeMax), 0.5f));

	_float3 vMergeExtents = {};
	XMStoreFloat3(&vMergeExtents, XMVectorScale(XMVectorSubtract(vMergeMax, vMergeMin), 0.5f));

	NewBoundingBox.Center = vMergeCenterPos;
	NewBoundingBox.Extents = vMergeExtents;

	return NewBoundingBox;
}

_int CDynamicAABBTree::UpdateNodeHeight(DYNAMICAABB_NODE* pRoot)
{
	if (nullptr == pRoot)
		return 0;  

	_int iLeftHeight = 0;
	_int iRightHeight = 0;

	iLeftHeight = (pRoot && pRoot->pLeftChild) ? UpdateNodeHeight(pRoot->pLeftChild) : -1;
	iRightHeight = (pRoot && pRoot->pRightChild) ? UpdateNodeHeight(pRoot->pRightChild): -1;

	pRoot->iHeight = max(iLeftHeight, iRightHeight) + 1;

	return pRoot->iHeight;
}

_int CDynamicAABBTree::CheckNodeBalance(const DYNAMICAABB_NODE* pRoot)
{
	if (nullptr == pRoot)
		return 0;

	_int iLeftHeight = 0;
	_int iRightHeight = 0;

	iLeftHeight = (pRoot && pRoot->pLeftChild) ? pRoot->pLeftChild->iHeight : 0;
	iRightHeight = (pRoot && pRoot->pRightChild) ? pRoot->pRightChild->iHeight : 0;

	return iLeftHeight - iRightHeight;
}

HRESULT CDynamicAABBTree::Rebalancing(DYNAMICAABB_NODE* pNode, DYNAMICAABB_NODE* pRebalancingNode)
{
	if (nullptr == pNode || nullptr == pRebalancingNode)
		return E_FAIL;

	DYNAMICAABB_NODE* pParentNode = pNode->pParent;
	DYNAMICAABB_NODE* pRebalancingParentNode = pRebalancingNode->pParent;

	pParentNode->pLeftChild == pNode ? pParentNode->pLeftChild = pRebalancingNode : pParentNode->pRightChild = pRebalancingNode;

	pRebalancingParentNode->pLeftChild == pRebalancingNode ? pRebalancingParentNode->pLeftChild = pNode : pRebalancingParentNode->pRightChild = pNode;

	pNode->pParent = pRebalancingParentNode;
	pRebalancingNode->pParent = pParentNode;

	RebalancingBoundingBoxes(pParentNode);
	RebalancingBoundingBoxes(pRebalancingParentNode);
	
	UpdateNodeHeight(m_pRoot);

	return S_OK;
}

HRESULT CDynamicAABBTree::RebalancingBoundingBoxes(DYNAMICAABB_NODE* pNode)
{
	pNode->BoundingBox = MergeBoundingBoxes(pNode->pLeftChild->BoundingBox, pNode->pRightChild->BoundingBox);

	if (nullptr != pNode->pParent)
	{
		RebalancingBoundingBoxes(pNode->pParent);
	}
	
	return S_OK;
}

CDynamicAABBTree::DYNAMICAABB_NODE* CDynamicAABBTree::LowCostChild(DYNAMICAABB_NODE* pNode, const BoundingBox& ObjectBoundingBox)
{
	BoundingBox LeftMergeBoundingBox = MergeBoundingBoxes(pNode->pLeftChild->BoundingBox, ObjectBoundingBox);

	BoundingBox RightMergeBoundingBox = MergeBoundingBoxes(pNode->pRightChild->BoundingBox, ObjectBoundingBox);

	return SurfaceArea(LeftMergeBoundingBox) < SurfaceArea(RightMergeBoundingBox) ? pNode->pLeftChild : pNode->pRightChild;

	return nullptr;
}

_float CDynamicAABBTree::SurfaceArea(const BoundingBox& BoundingBox)
{
	_float fX = BoundingBox.Extents.x * 2.f;
	_float fY = BoundingBox.Extents.y * 2.f;
	_float fZ = BoundingBox.Extents.z * 2.f;

	return ((fX * fY) + (fY * fZ) + (fZ * fX)) * 2.f;
}

HRESULT CDynamicAABBTree::Release_Nodes(DYNAMICAABB_NODE* pRoot)
{
	if (nullptr == pRoot)
		return S_OK;

	if (pRoot->IsLeaf())
	{
		Safe_Release(pRoot->pGameObject);
		Safe_Delete(pRoot);
	}
	else
	{
		Release_Nodes(pRoot->pLeftChild);
		Release_Nodes(pRoot->pRightChild);

		Safe_Release(pRoot->pGameObject);
		Safe_Delete(pRoot);
	}

	return S_OK;
}

CDynamicAABBTree* CDynamicAABBTree::Create()
{
	return new CDynamicAABBTree();
}

void CDynamicAABBTree::Free()
{
	__super::Free();

	Release_Nodes(m_pRoot);

	for (auto& Pair : m_Nodes)
	{
		Safe_Release(const_cast<CGameObject*&>(Pair.first));
		Safe_Delete(Pair.second);
	}

	m_Nodes.clear();
}

