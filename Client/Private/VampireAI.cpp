#include "ClientPch.h"
#include "VampireAI.h"
#include "Vampire.h"

CVampireAI::CVampireAI()
{
}

HRESULT CVampireAI::Initialize(CVampire* pControlledVampire)
{
    if (nullptr == pControlledVampire)
        return E_FAIL;

    m_pControlledVampire = pControlledVampire;
   
    if (FAILED(Ready_Nodes()))
        return E_FAIL;

    return S_OK;
}

HRESULT CVampireAI::Ready_Nodes()
{
    CBT_SelectorNode* pSelectorNode = CBT_SelectorNode::Create();

    m_pRootNode = pSelectorNode;

        CBT_SelectorNode* pActionSelectorNode = CBT_SelectorNode::Create();
        
        pSelectorNode->Add_Child(pActionSelectorNode);
            
            pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->IsSpawn(); }));

            pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->CanOtherAction(); }));

            CBT_SequenceNode* pAttackSequenceNode = CBT_SequenceNode::Create();

            pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->CanAttack(); }));
            pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->CanAttackRange(); }));
            pAttackSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->Attack(); }));
            
            pActionSelectorNode->Add_Child(pAttackSequenceNode);

            CBT_SequenceNode* pPatrolSequenceNode = CBT_SequenceNode::Create();

            pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->CanAttackRange(); }));
            pPatrolSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE {return m_pControlledVampire->Patrol(); }));
            pActionSelectorNode->Add_Child(pPatrolSequenceNode);

            CBT_SequenceNode* pChaseSequenceNode = CBT_SequenceNode::Create();
            pChaseSequenceNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledVampire->Chase(); }));
            
            pActionSelectorNode->Add_Child(pChaseSequenceNode);
            
            
    return S_OK;

}

CVampireAI* CVampireAI::Create(CVampire* pControlledVampire)
{
    CVampireAI* pInstance = new CVampireAI();
    if (FAILED(pInstance->Initialize(pControlledVampire)))
    {
        MSG_BOX(TEXT("Failed Created : CVampireAI"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVampireAI::Free()
{
    __super::Free();
}
