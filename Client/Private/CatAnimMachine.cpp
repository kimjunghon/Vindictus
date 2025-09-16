#include "ClientPch.h"
#include "CatAnimMachine.h"

CCatAnimMachine::CCatAnimMachine()
{
}

void CCatAnimMachine::Initialize()
{
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::IDLE)] = { "Idle", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::GROOMING)] = { "Grooming", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::LOVELY)] = { "Lovely", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::SLEEP)] = { "Sleep", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::REAL_SLEEP)] = { "Real_Sellp", false, _float2(1.f, 1.f), 2.f };
}

CCatAnimMachine* CCatAnimMachine::Create()
{
	CCatAnimMachine* pInstance = new CCatAnimMachine();
	pInstance->Initialize();
	return pInstance;
}

void CCatAnimMachine::Free()
{
	__super::Free();
}
