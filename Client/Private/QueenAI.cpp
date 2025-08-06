#include "ClientPch.h"
#include "QueenAI.h"
#include "Queen.h"

CQueenAI::CQueenAI()
{
}

HRESULT CQueenAI::Initialize(CQueen* pControlledQueen)
{
	if (nullptr == pControlledQueen)
		return E_FAIL;

	m_pControlledQueen = pControlledQueen;

	if (FAILED(Ready_Nodes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQueenAI::Ready_Nodes()
{
	CBT_SelectorNode* pSelectorNode = CBT_SelectorNode::Create();

	m_pRootNode = pSelectorNode;


		CBT_SelectorNode* pActionSelectorNode = CBT_SelectorNode::Create();
	
		pSelectorNode->Add_Child(pActionSelectorNode);

		pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanOtherAction(); }));
	
			//Near Attack
			CBT_SequenceNode* pNearAttackSequenceNode = CBT_SequenceNode::Create();
			pNearAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->CanNearAttack(); }));
			pNearAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->NearAttack(); }));

			pActionSelectorNode->Add_Child(pNearAttackSequenceNode);

			//Attack
			CBT_SequenceNode* pAttackSequenceNode = CBT_SequenceNode::Create();
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttack(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttackRange(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->Attack(); }));

			pActionSelectorNode->Add_Child(pAttackSequenceNode);

			CBT_SequenceNode* pPatrolSequenceNode = CBT_SequenceNode::Create();

			pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttackRange(); }));
			pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->Patrol(); }));
			pActionSelectorNode->Add_Child(pPatrolSequenceNode);

			CBT_SequenceNode* pChaseSequenceNode = CBT_SequenceNode::Create();
			pChaseSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->Chase(); }));

			pActionSelectorNode->Add_Child(pChaseSequenceNode);

			pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->Idle(); }));


	return S_OK;
}

CQueenAI* CQueenAI::Create(CQueen* pControlledQueen)
{
	CQueenAI* pInstance = new CQueenAI();
	pInstance->Initialize(pControlledQueen);

	return pInstance;
}

void CQueenAI::Free()
{
	__super::Free();
}
