#include "EnginePch.h"
#include "BehaviorTree.h"
#include "BT_Node.h"

CBehaviorTree::CBehaviorTree()
{
}

HRESULT CBehaviorTree::Initialize()
{
	return S_OK;
}

void CBehaviorTree::Update()
{
	if (nullptr == m_pRootNode)
		return;

	m_pRootNode->Evaluate();
}

void CBehaviorTree::Free()
{
	__super::Free();

	Safe_Release(m_pRootNode);
}
