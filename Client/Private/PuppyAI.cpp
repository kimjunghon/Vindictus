#include "ClientPch.h"
#include "PuppyAI.h"
#include "Puppy.h"

CPuppyAI::CPuppyAI()
{
}

HRESULT CPuppyAI::Initialize(CPuppy* pControlledPuppy)
{
	if (nullptr == pControlledPuppy)
		return E_FAIL;

	m_pControlledPuppy = pControlledPuppy;

	if (FAILED(Ready_Nodes()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPuppyAI::Ready_Nodes()
{
    CBT_SelectorNode* pSelectorNode = CBT_SelectorNode::Create();

    m_pRootNode = pSelectorNode;

        CBT_SelectorNode* pActionSelectorNode = CBT_SelectorNode::Create();
        pSelectorNode->Add_Child(pActionSelectorNode);
                
            pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledPuppy->CanOtherAction(); }));
            pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledPuppy->IsNear(); }));
            pActionSelectorNode->Add_Child(CBT_ActionNode::Create([this]()->BT_STATE { return m_pControlledPuppy->Idle(); }));



	return S_OK;
}

CPuppyAI* CPuppyAI::Create(CPuppy* pControlledPuppy)
{
    CPuppyAI* pInstance = new CPuppyAI();
    if (FAILED(pInstance->Initialize(pControlledPuppy)))
    {
        MSG_BOX(TEXT("Failed Created : CPuppyAI"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPuppyAI::Free()
{
    __super::Free();
}
