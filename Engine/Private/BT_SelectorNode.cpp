#include "EnginePch.h"
#include "BT_SelectorNode.h"

CBT_SelectorNode::CBT_SelectorNode()
{
}

BT_STATE CBT_SelectorNode::Evaluate()
{
	for (auto pNode : m_Children)
	{
		switch (pNode->Evaluate())
		{
		case BT_STATE::SUCCESS:
			return BT_STATE::SUCCESS;
		case BT_STATE::RUN:
			return BT_STATE::RUN;
		}
	}

	return BT_STATE::FAILED;
}

HRESULT CBT_SelectorNode::Add_Child(CBT_Node* pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_Children.push_back(pNode);

	return S_OK;
}

CBT_SelectorNode* CBT_SelectorNode::Create()
{
	return new CBT_SelectorNode();
}

void CBT_SelectorNode::Free()
{
	__super::Free();

	for (auto& pNode : m_Children)
		Safe_Release(pNode);

	m_Children.clear();
}
