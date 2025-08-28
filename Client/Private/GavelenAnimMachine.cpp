#include "ClientPch.h"
#include "GavelenAnimMachine.h"

CGavelenAnimMachine::CGavelenAnimMachine()
{
}

void CGavelenAnimMachine::Initialize()
{
	Initialize_SPAWN();
	Initialize_IDLE();
	Initialize_MOVE();
	Initialize_ATTACK();
	Initialize_RAGE();
	Initialize_GRAP();
	Initialize_HANG();
	Initialize_HIT();
	Initialize_Dead();
}

void CGavelenAnimMachine::Initialize_SPAWN()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	m_AnimDatas[iFlag | ENUM_CLASS(SPAWN_FLAG::STAY)] =		{ "CutSean_Idle", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(SPAWN_FLAG::START)] =	{ "CutSean", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_IDLE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::IDLE);

	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::IDLE)] =			{ "Idle", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::THREAT)] =		{ "Threat", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::LOW_HEALTH)] =	{ "Low_Health", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_MOVE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::RUN)] =			{ "Run", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN)] =		{ "Turn", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_LEFT)] =		{ "Turn_Left", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::TURN_RIGHT)] =	{ "Turn_Right", false, _float2(1.f, 1.f), 3.f };
}

void CGavelenAnimMachine::Initialize_ATTACK()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::DESEND)] =	{ "Desend", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::BLAZE)] =	{ "Blaze", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::DOUBLE)] =	{ "Attack_Double", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::LEFT)] =	{ "Attack_Left", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::RIGHT)] =	{ "Attack_Right", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_RAGE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::RAGE);

	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_BEGIN)] =	{ "Rage_Desend_Begin", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::ATTACK)] =		{ "Rage_Attack", false, _float2(1.f, 1.f), 2.f};
	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::DOUBLE)] =		{ "Attack_Double", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::BLAZE)] =			{ "Blaze", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::LEFT)] =			{ "Attack_Left", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::RIGHT)] =			{ "Attack_Right", false, _float2(1.f, 1.f), 3.f };

	m_AnimDatas[iFlag | ENUM_CLASS(RAGE_FLAG::DESEND_END)] =	{ "Rage_Desend_End", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_GRAP()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::GRAP);

	m_AnimDatas[iFlag | ENUM_CLASS(GRAP_FLAG::TRY)] =		{ "Grappling_Try", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GRAP_FLAG::SUCCESS)] =	{ "Grappling_Success", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GRAP_FLAG::FAIL)] =		{ "Grappling_Fail", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_HANG()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HANG);
	
	m_AnimDatas[iFlag | ENUM_CLASS(HANG_FLAG::BEGIN)] =		{ "Hang_Begin", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HANG_FLAG::DURING)] =	{ "Hang_During", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HANG_FLAG::END)] =		{ "Hang_End", false, _float2(1.f, 1.f), 2.f };

}

void CGavelenAnimMachine::Initialize_HIT()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::FRONT)] =			{ "Damage_Front", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::BACK)] =			{ "Damage_Back", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::RIGHT)] =			{ "Damage_Right", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::LEFT)] =			{ "Damage_Left", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_BEGIN)] =		{ "Down_Begin", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_DURING)] =	{ "Down_During", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::DOWN_END)] =		{ "Down_End", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::WINGBREAK)] =		{ "WingBreak", false, _float2(1.f, 1.f), 2.f };
}

void CGavelenAnimMachine::Initialize_Dead()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::DEAD);

	m_AnimDatas[iFlag] = { "Dying", false, _float2(1.f, 1.f), 2.f };

}

CGavelenAnimMachine* CGavelenAnimMachine::Create()
{
	CGavelenAnimMachine* pInstance = new CGavelenAnimMachine();
	pInstance->Initialize();

	return pInstance;
}

void CGavelenAnimMachine::Free()
{
	__super::Free();

}