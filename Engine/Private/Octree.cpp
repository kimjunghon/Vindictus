#include "Octree.h"
#include "CollisionObject.h"

COctree::COctree()
{
}

HRESULT COctree::Initialize(const OCTREE_DESC& tDesc)
{
	if (nullptr != m_pRoot)
		return E_FAIL;

	m_pRoot = Create_Node(tDesc.vCenterPosition, tDesc.vExtents, 0);

	//if (FAILED(Ready_Octree(m_pRoot, tDesc.vCenterPosition, tDesc.vExtents, tDesc.iOctreeDepth)))
	//	return E_FAIL;

	m_iMaxOctreeDepth = tDesc.iMaxOctreeDepth;
	m_iMaxObjectCount = tDesc.iMaxObjectCount;

	return S_OK;
}

HRESULT COctree::Ready_Octree(OCTREE_NODE* pNode, _float3 vCenterPosition, _float3 vExtents, _uint iOctreeDepth)
{
	/*if (nullptr == pNode)
		return E_FAIL;

	if (iOctreeDepth <= 0)
		return S_OK;

	_float3 vOffset;
	_float3 vChildCenter;
	_float3 vChildExtents;

	XMVECTOR vTemp = XMLoadFloat3(&vExtents);
	vTemp = XMVectorScale(vTemp, 0.5f);
	XMStoreFloat3(&vChildExtents, vTemp);

	for (_uint i = 0; i < 8; i++)
	{
		vOffset.x = ((i & 1) ? vChildExtents.x : -vChildExtents.x);
		vOffset.y = ((i & 4) ? vChildExtents.y : -vChildExtents.y);
		vOffset.z = ((i & 2) ? vChildExtents.z : -vChildExtents.z);

		vChildCenter.x = vOffset.x + vCenterPosition.x;
		vChildCenter.y = vOffset.y + vCenterPosition.y;
		vChildCenter.z = vOffset.z + vCenterPosition.z;

		OCTREE_NODE* pChildNode = Create_Node(vChildCenter, vChildExtents);

		if (nullptr == pChildNode)
			return E_FAIL;

		pNode->pChilds[i] = pChildNode;
		Ready_Octree(pChildNode, vChildCenter, vChildExtents, iOctreeDepth - 1);
	}*/

	return S_OK;
}

HRESULT COctree::Devide_Node(OCTREE_NODE* pNode, _float3 vCenterPosition, _float3 vExtents)
{
	if (nullptr == pNode)
		return E_FAIL;

	_float3 vOffset;
	_float3 vChildCenter;
	_float3 vChildExtents;

	XMVECTOR vTemp = XMLoadFloat3(&vExtents);
	vTemp = XMVectorScale(vTemp, 0.5f);
	XMStoreFloat3(&vChildExtents, vTemp);

	for (_uint i = 0; i < 8; i++)
	{
		vOffset.x = ((i & 1) ? vChildExtents.x : -vChildExtents.x);		// 홀수 일때 +, 짝수일떄 -
		vOffset.y = ((i & 4) ? vChildExtents.y : -vChildExtents.y);		// 4 이상 +, 4 미만 -
		vOffset.z = ((i & 2) ? vChildExtents.z : -vChildExtents.z);		// 2,3,6,7 + , 나머지 - ( == y 값 바뀔때까지 -,-, +,+ )

		vChildCenter.x = vOffset.x + vCenterPosition.x;
		vChildCenter.y = vOffset.y + vCenterPosition.y;
		vChildCenter.z = vOffset.z + vCenterPosition.z;

		OCTREE_NODE* pChildNode = Create_Node(vChildCenter, vChildExtents, pNode->iOctreeDepth + 1);

		if (nullptr == pChildNode)
			return E_FAIL;

		pNode->pChilds[i] = pChildNode;
	}

	return S_OK;
}

HRESULT COctree::ReInsertObjects(OCTREE_NODE* pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	vector<CCollisionObject*> TempObjects;
	
	swap(pNode->Objects, TempObjects);

	pNode->Objects.clear();

	for (auto iter : TempObjects)
	{
		if(FAILED(Add_ObjectToNode(pNode, iter, iter->Get_BoundingBox())))
			return E_FAIL;

		Safe_Release(iter);
	}

	return S_OK;
}

COctree::OCTREE_NODE* COctree::Create_Node(_float3 vCenterPosition, _float3 vExtents, _uint iOctreeDepth)
{
	OCTREE_NODE* pNode = new OCTREE_NODE();
	
	pNode->BoundingBox.Center = vCenterPosition;
	pNode->BoundingBox.Extents = vExtents;
	pNode->iOctreeDepth = iOctreeDepth;

	return pNode;
}

HRESULT COctree::Add_ObjectToTree(CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	OCTREE_NODE* pNode = m_pRoot;

	if (false == IsIntersect(pNode, ObjectBoundingBox))
		return S_OK;

	Add_ObjectToNode(pNode, pGameObject, ObjectBoundingBox);

	return S_OK;
}

HRESULT COctree::Add_ObjectToNode(OCTREE_NODE* pNode, CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	if (IsLeafNode(pNode))
	{
		if (pNode->Objects.size() + 1 > m_iMaxObjectCount && pNode->iOctreeDepth < m_iMaxOctreeDepth)
		{
			if (FAILED(Devide_Node(pNode, pNode->BoundingBox.Center, pNode->BoundingBox.Extents)))
				return E_FAIL;

			ReInsertObjects(pNode);
		}
		else
		{
			pNode->Objects.push_back(pGameObject);
			Safe_AddRef(pGameObject);
		}
	}
	else
	{
		vector<_uint> IntersectIndex;

		for (_uint i = 0; i < 8; i++)
		{
			if (IsIntersect(pNode->pChilds[i], ObjectBoundingBox))
				IntersectIndex.push_back(i);
		}

		if (IntersectIndex.size() == 1)
			Add_ObjectToNode(pNode->pChilds[IntersectIndex[0]], pGameObject, ObjectBoundingBox);
		else
		{
			pNode->Objects.push_back(pGameObject);
			Safe_AddRef(pGameObject);
		}
	}

	return S_OK;
}

HRESULT COctree::CheckCollisionOctree(const CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	if (nullptr == m_pRoot ||
		false == IsIntersect(m_pRoot, ObjectBoundingBox))
		return S_OK;

	CheckCollisionNode(m_pRoot, pGameObject, ObjectBoundingBox);

	return S_OK;
}

void COctree::CheckCollisionNode(OCTREE_NODE* pNode, const CCollisionObject* pGameObject, const BoundingBox& ObjectBoundingBox)
{
	for (auto iter : pNode->Objects)
	{
		if(iter != pGameObject && iter->Get_BoundingBox().Intersects(ObjectBoundingBox))
		{
			//CheckObb(iter, pGameObject);
		}
	}

	if (false == IsLeafNode(pNode))
	{
		for (_uint i = 0; i < 8; i++)
		{
			if (IsIntersect(pNode->pChilds[i], ObjectBoundingBox))
				CheckCollisionNode(pNode->pChilds[i], pGameObject, ObjectBoundingBox);
		}
	}
}

HRESULT COctree::Release_Nodes(OCTREE_NODE* pRoot)
{
	if (nullptr == pRoot)
		return S_OK;

	if (IsLeafNode(pRoot))
	{
		for (auto& iter : pRoot->Objects)
			Safe_Release(iter);

		Safe_Delete(pRoot);
	}
	else
	{
		for (_uint i = 0; i < 8; i++)
		{
			for (auto& iter : pRoot->Objects)
				Safe_Release(iter);

			Release_Nodes(pRoot->pChilds[i]);
		}

		Safe_Delete(pRoot);
	}


	return S_OK;
}

COctree* COctree::Create(const OCTREE_DESC& tDesc)
{
	COctree* pInstance = new COctree();

	if (FAILED(pInstance->Initialize(tDesc)))
	{
		MSG_BOX(TEXT("Failed Crated : COctree"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COctree::Free()
{
	__super::Free();

	Release_Nodes(m_pRoot);

}
