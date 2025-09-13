#include "ClientPch.h"
#include "PuppyAnimMachine.h"

CPuppyAnimMachine::CPuppyAnimMachine()
{
}

void CPuppyAnimMachine::Initialize()
{
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::IDLE)] = {"Idle", true, _float2(0.f, 1.f), 2.f};
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::WALK)] = { "Walk", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::RUN)] = { "Run", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::SIT_DOWN)] = { "Sit_Down", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::EXCITING)] = { "Exciting", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::EATING)] = { "Eating", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[ENUM_CLASS(STATE_FLAG::DIGGING)] = { "Digging", false, _float2(1.f, 1.f), 2.f };
}


CPuppyAnimMachine* CPuppyAnimMachine::Create()
{
	CPuppyAnimMachine* pInstance = new CPuppyAnimMachine();
	pInstance->Initialize();
	return pInstance;
}

void CPuppyAnimMachine::Free()
{
	__super::Free();
}
