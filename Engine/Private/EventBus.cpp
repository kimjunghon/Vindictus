#include "EnginePch.h"
#include "EventBus.h"

CEventBus::CEventBus()
{
}

HRESULT CEventBus::Initialize()
{
    m_Subscribers = new SUBSCRIBERS[ENUM_CLASS(EVENT_TYPE::END)];

    if (nullptr == m_Subscribers)
        return E_FAIL;

    return S_OK;
}

void CEventBus::Clear()
{
    for (auto& Pair : m_Subscribers[ENUM_CLASS(EVENT_TYPE::NONSTATIC)])
        Pair.second.clear();

    m_Subscribers[ENUM_CLASS(EVENT_TYPE::NONSTATIC)].clear();
}

CEventBus* CEventBus::Create()
{
    CEventBus* pInstance = new CEventBus();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CEventBus"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEventBus::Free()
{
    __super::Free();

    for (_uint i = 0; i < ENUM_CLASS(EVENT_TYPE::END); i++)
    {
        for (auto& Pair : m_Subscribers[i])
        {
            Pair.second.clear();
        }

        m_Subscribers[i].clear();
    }

    Safe_Delete_Array(m_Subscribers);
}
