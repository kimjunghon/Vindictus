#include "ClientPch.h"
#include "Vampire_AnimMachine.h"

CVampire_AnimMachine::CVampire_AnimMachine()
{
}

void CVampire_AnimMachine::Initialize()
{
	__super::Initialize();

	Initialize_SPAWN();
	Initialize_IDLE();
	Initialize_MOVE();
	Initialize_ATTACK();
	Initialize_HIT();
	Initialize_Dead();
}

void CVampire_AnimMachine::Initialize_SPAWN()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	m_AnimDatas[iFlag] = { "Spawn", false, _float2(1.f, 1.f), 2.f };
}

void CVampire_AnimMachine::Initialize_IDLE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::IDLE);

	m_AnimDatas[iFlag] = { "Idle", true, _float2(0.f, 1.f), 2.f };
}

void CVampire_AnimMachine::Initialize_MOVE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::WALK)] = { "Walk_Front", true, _float2(0.f, 1.f), 2.f};
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::FRONT)] =  { "Run_Front", true, _float2(0.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::RIGHT)] =  { "Run_Right", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::LEFT)] =   { "Run_Left", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::BACK)] =   { "Run_Back", false, _float2(1.f, 1.f), 2.f };
}

void CVampire_AnimMachine::Initialize_ATTACK()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AnimDatas[iFlag] = { "Attack", false, _float2(1.f, 1.f), 2.f };

	// Elder Àü¿ë
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::MELEE)] = { "Attack_Close", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::RANGE)] = { "Attack_FireBall", false, _float2(1.f, 1.f), 2.f };
}

void CVampire_AnimMachine::Initialize_HIT()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::FRONT)] = { "Damage_Front", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::BACK)] =  { "Damage_Back", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::LEFT)] =  { "Damage_Left", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::RIGHT)] = { "Damage_Right", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::STRONG_BEGIN)] = { "Damage_Strong_Begin", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::STRONG_DURING)] = { "Damage_Strong_During", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::STRONG_END)] = { "Damage_Strong_End", false, _float2(1.f, 1.f), 3.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HIT_FLAG::STRONG_UP)] = { "Damage_Strong_Up", false, _float2(1.f, 1.f), 2.f };
}

void CVampire_AnimMachine::Initialize_Dead()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::DEAD);

	m_AnimDatas[iFlag | ENUM_CLASS(DEAD_FLAG::DEFAULT)] = { "Dead", false, _float2(1.f, 1.f), 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(DEAD_FLAG::STRONG)] = { "Dead_Strong", false, _float2(1.f, 1.f), 2.f };
}

CVampire_AnimMachine* CVampire_AnimMachine::Create()
{
	CVampire_AnimMachine* pInstance = new CVampire_AnimMachine();
	pInstance->Initialize();

	return pInstance;
}

void CVampire_AnimMachine::Free()
{
	__super::Free();

}
