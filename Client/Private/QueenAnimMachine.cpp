#include "ClientPch.h"
#include "QueenAnimMachine.h"

CQueenAnimMachine::CQueenAnimMachine()
{
}

void CQueenAnimMachine::Initialize()
{
	__super::Initialize();

	Initialize_CUTSEAN();
	Initialize_IDLE();
	Initialize_MOVE();
	Initialize_ATTACK();
	Initialize_BURROW();
	Initialize_HIT();
	Initialize_Dead();
}

void CQueenAnimMachine::Initialize_CUTSEAN()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::CUTSEAN);

	m_AnimDatas[iFlag] = { "Cut_Sean", false, _float2(1.f, 1.f), 2.f };
}

void CQueenAnimMachine::Initialize_IDLE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::IDLE);

	m_AnimDatas[iFlag] = {"Idle", true, _float2(0.f, 1.f), 2.f};

	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::THREAT)] =	 { "Threat", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::LOW_HEALTH)] = { "Low_Health", false, _float2(1.f, 1.f), 2.f };
}

void CQueenAnimMachine::Initialize_MOVE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::RUN)] = {"Run", true, _float2(0.f, 1.f), 2.f};
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_LEFT)] = { "Turn_Left", true, _float2(0.9f, 1.f), 6.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_RIGHT)] = { "Turn_Right", true, _float2(0.9f, 1.f), 6.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_LEFT_FAST)] = { "Turn_Left_Fast", false, _float2(1.f, 1.f), 4.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_RIGHT_FAST)] = { "Turn_Right_Fast", false, _float2(1.f, 1.f), 4.f };
}

void CQueenAnimMachine::Initialize_ATTACK()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::MELLE)] = { "Attack_Close", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::LEFTLEG)] = { "Attack_LeftHand", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::RIGHTLEG)] = { "Attack_RightHand", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)] = { "Attack_Double", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::SWOOP)] = { "Attack_Swoop", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::TAIL)] = { "Attack_Tail", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::TURN_LEFT)] = { "Attack_TurnLeft", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::TURN_RIGHT)] = { "Attack_TurnRight", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::JUMP)] = { "Attack_Jump", false, _float2(1.f, 1.f), 3.f };

//	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::POSION)] = { "Attack_Poison", false, _float2(1.f, 1.f), 2.f };

}

void CQueenAnimMachine::Initialize_BURROW()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::BURROW);

	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::BEGIN)] = { "Burrow_Begin", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::STAY)] = { "Burrow_Stay", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::MOVE)] = { "Burrow_Move", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::ATTACK1)] = { "Burrow_Attack01", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::ATTACK2)] = { "Burrow_Attack02", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(BURROW_FLAG::END)] = { "Burrow_End", false, _float2(1.f, 1.f), 2.f };
}

void CQueenAnimMachine::Initialize_HIT()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::FRONT)] = { "Damage_Front", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::BACK)] = { "Damage_Back", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::LEFT)] = { "Damage_Left", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::RIGHT)] = { "Damage_Right", false, _float2(1.f, 1.f), 2.f };

	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_BEGIN)] = { "Down_Begin", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_DURING)] = { "Down_During", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_END)] = { "Down_End", false, _float2(1.f, 1.f), 2.f };
}

void CQueenAnimMachine::Initialize_Dead()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::DEAD);

	m_AnimDatas[iFlag] = { "Dying", false, _float2(1.f, 1.f), 2.4f };
}

CQueenAnimMachine* CQueenAnimMachine::Create()
{
	CQueenAnimMachine* pInstance = new CQueenAnimMachine();
	pInstance->Initialize();

	return pInstance;
}

void CQueenAnimMachine::Free()
{
	__super::Free();
}
