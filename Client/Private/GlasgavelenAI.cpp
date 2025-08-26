#include "ClientPch.h"
#include "GlasgavelenAI.h"
#include "Glasgavelen.h"

CGlasgavelenAI::CGlasgavelenAI()
{
}

HRESULT CGlasgavelenAI::Initialize(CGlasgavelen* pControlledGlasgavelen)
{
	if (nullptr == pControlledGlasgavelen)
		return E_FAIL;

	m_pControlledGlasgavelen = pControlledGlasgavelen;

	if (FAILED(Ready_Nodes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelenAI::Ready_Nodes()
{
	CBT_SelectorNode* pSelectorNode = CBT_SelectorNode::Create();
	m_pRootNode = pSelectorNode;

		CBT_SelectorNode* pActionSelectorNode = CBT_SelectorNode::Create();
		pSelectorNode->Add_Child(pActionSelectorNode);

			pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanOtherAction(); }));
			
			CBT_SelectorNode* pAttackSelectorNode = CBT_SelectorNode::Create();

			CBT_SequenceNode* pRageSequenceNode = CBT_SequenceNode::Create();
			
			pRageSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->Is_Rage(); }));
			pRageSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanAttack(); }));
			pRageSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanAttackRange(); }));
			pRageSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->Rage_Attack(); }));

			pAttackSelectorNode->Add_Child(pRageSequenceNode);

			//Attack
			CBT_SequenceNode* pAttackSequenceNode = CBT_SequenceNode::Create();
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanAttack(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanAttackRange(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->Attack(); }));

			pAttackSelectorNode->Add_Child(pAttackSequenceNode);

			pActionSelectorNode->Add_Child(pAttackSelectorNode);

			CBT_SequenceNode* pPatrolSequenceNode = CBT_SequenceNode::Create();

			pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->CanAttackRange(); }));
			pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledGlasgavelen->Patrol(); }));
			pActionSelectorNode->Add_Child(pPatrolSequenceNode);

			CBT_SequenceNode* pChaseSequenceNode = CBT_SequenceNode::Create();
			pChaseSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->Chase(); }));

			pActionSelectorNode->Add_Child(pChaseSequenceNode);

			pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledGlasgavelen->Idle(); }));

	return S_OK;
}

CGlasgavelenAI* CGlasgavelenAI::Create(CGlasgavelen* pControlledGlasgavelen)
{
	CGlasgavelenAI* pInstance = new CGlasgavelenAI();
	pInstance->Initialize(pControlledGlasgavelen);

	return pInstance;
}

void CGlasgavelenAI::Free()
{
	__super::Free();
}
