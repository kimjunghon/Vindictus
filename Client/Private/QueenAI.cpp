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

		BT_STATE				IsStun();
		BT_STATE				Stun();

		pSelectorNode->Add_Child(pActionSelectorNode);
				
				//Stun
				pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->IsStun(); }));

				// CanOtherAction
				pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanOtherAction(); }));

				//Burrow
				CBT_SequenceNode* pBurrowStartSequenceNode = CBT_SequenceNode::Create();

				pBurrowStartSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->CanBurrow(); }));
				pBurrowStartSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->Burrow(); }));

				pActionSelectorNode->Add_Child(pBurrowStartSequenceNode);

				CBT_SequenceNode* pBurrowActionSequenceNode = CBT_SequenceNode::Create();
				
				
				pBurrowActionSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->IsBurrow(); }));

				CBT_SelectorNode* pBurrowActionSelectionNode = CBT_SelectorNode::Create();

					pBurrowActionSelectionNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->BurrowEnd(); }));

					
					CBT_SequenceNode* pBurrowAttackSequenceNode = CBT_SequenceNode::Create();

					pBurrowAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttackRange(); }));
					pBurrowAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->BurrowAttack(); }));
					
				
				pBurrowActionSelectionNode->Add_Child(pBurrowAttackSequenceNode);
				
				pBurrowActionSelectionNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->BurrowMove(); }));

				pBurrowActionSequenceNode->Add_Child(pBurrowActionSelectionNode);
				
				pActionSelectorNode->Add_Child(pBurrowActionSequenceNode);


				// Look
				CBT_SequenceNode* pTurnSequnceNode = CBT_SequenceNode::Create();

				pTurnSequnceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->IsLook(); }));
				
				CBT_SelectorNode* pTurnSelectorNode = CBT_SelectorNode::Create();

				//Near Attack
				CBT_SequenceNode* pNearAttackSequenceNode = CBT_SequenceNode::Create();
				pNearAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->CanNearAttack(); }));
				pNearAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledQueen->NearAttack(); }));

				pTurnSelectorNode->Add_Child(pNearAttackSequenceNode);
				
				pTurnSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->Turn(); }));

				//Selector 추가
				pTurnSequnceNode->Add_Child(pTurnSelectorNode);

				pActionSelectorNode->Add_Child(pTurnSequnceNode);


			//Attack
			CBT_SequenceNode* pAttackSequenceNode = CBT_SequenceNode::Create();
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttack(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->CanAttackRange(); }));
			pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledQueen->Attack(); }));
			//Selector 추가
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
