#include "EnginePch.h"
#include "BT_SequenceNode.h"

CBT_SequenceNode::CBT_SequenceNode()
{
}

BT_STATE CBT_SequenceNode::Evaluate()
{
	if (m_Children.size() <= 0)
		return BT_STATE::FAILED;

	for (auto pNode : m_Children)
	{
		switch (pNode->Evaluate())
		{
		case BT_STATE::RUN:
			return BT_STATE::RUN;

		case BT_STATE::FAILED:
			return BT_STATE::FAILED;
		
		case BT_STATE::SUCCESS:
			continue;
		}
	}

	return BT_STATE::SUCCESS;
}

HRESULT CBT_SequenceNode::Add_Child(CBT_Node* pNode)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_Children.push_back(pNode);

	return S_OK;
}


CBT_SequenceNode* CBT_SequenceNode::Create()
{
    return new CBT_SequenceNode();
}

void CBT_SequenceNode::Free()
{
	__super::Free();

	for (auto& pNode : m_Children)
		Safe_Release(pNode);

	m_Children.clear();
}
