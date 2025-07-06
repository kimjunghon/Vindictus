#include "EventBus.h"

CEventBus::CEventBus()
{
}

HRESULT CEventBus::Initialize(_uint iNumLevels)
{
    m_Subscribers = new SUBSCRIBERS[iNumLevels];

    if (nullptr == m_Subscribers)
        return E_FAIL;

    return S_OK;
}

void CEventBus::Clear(_uint iClearLevel)
{
    for (auto& Pair : m_Subscribers[iClearLevel])
        Pair.second.clear();

    m_Subscribers[iClearLevel].clear();
}

CEventBus* CEventBus::Create(_uint iNumLevels)
{
    CEventBus* pInstance = new CEventBus();
    if (FAILED(pInstance->Initialize(iNumLevels)))
    {
        MSG_BOX(TEXT("Failed Created : CEventBus"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEventBus::Free()
{
    __super::Free();

    for (_uint i = 0; i < m_iNumLevels; i++)
    {
        for (auto& Pair : m_Subscribers[i])
        {
            Pair.second.clear();
        }

        m_Subscribers[i].clear();
    }

    Safe_Delete_Array(m_Subscribers);
}
