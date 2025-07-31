#include "EnginePch.h"
#include "BT_ActionNode.h"

CBT_ActionNode::CBT_ActionNode(function<BT_STATE()> Action)
	: m_Action { Action }
{
}

BT_STATE CBT_ActionNode::Evaluate()
{
	if (nullptr == m_Action)
		return BT_STATE::FAILED;

	return m_Action();
}

CBT_ActionNode* CBT_ActionNode::Create(function<BT_STATE()> Action)
{
	return new CBT_ActionNode(Action);
}

void CBT_ActionNode::Free()
{
	__super::Free();
}
