#include "ClientPch.h"
#include "PlayerAnimMachine.h"
#include "Model.h"

CPlayerAnimMachine::CPlayerAnimMachine()
{
}

void CPlayerAnimMachine::Initialize()
{
	Initialize_IDLE();
	Initialize_MOVE();
	Initialize_ATTACK();
	Initialize_SMASH();
	Initialize_GUARD();
	Initialize_HEAVYSTAND();
	Initialize_ROLL();
	Initialize_HIT();
}

void CPlayerAnimMachine::Initialize_IDLE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::IDLE);
	
	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::DEFAULT)] =	{ "Battle_Idle", true, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(IDLE_FLAG::REST)] =		{ "Battle_Rest", true, _float2{0.f, 1.f}, 2.f };
}

void CPlayerAnimMachine::Initialize_MOVE()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::MOVE);

	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::DEFAULT)] =	{ "Battle_Run", true, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::SPRINT)] =	{ "Battle_Sprint", true, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(MOVE_FLAG::STOP)] =		{ "Battle_RunStop", false, _float2{0.6f, 1.f}, 2.f };
}

void CPlayerAnimMachine::Initialize_ATTACK()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ATTACK);

	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO1)] = { "Attack_01", false, _float2{0.4f, 1.f} , 3.f};
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO2)] = { "Attack_02", false, _float2{0.4f, 1.f} , 3.f};
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO3)] = { "Attack_03", false, _float2{0.4f, 1.f} , 3.f};
	m_AnimDatas[iFlag | ENUM_CLASS(ATTACK_FLAG::COMBO4)] = { "Attack_04", false, _float2{0.4f, 1.f} , 3.f};
}

void CPlayerAnimMachine::Initialize_SMASH()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::SMASH);

	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH0)]	=			{ "Smash_00", false, _float2{0.6f, 1.f} , 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE)] =		{ "Smash_00_Charge", false, _float2{0.f, 1.f} , 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH0_CHARGE_END)] =	{ "Smash_00_ChargeEnd", false, _float2{0.8f, 1.f} , 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH1)] =			{ "Smash_01", false, _float2{0.4f, 1.f} , 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH2_0)] =			{ "Smash_02_00", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH2_1)] =			{ "Smash_02_01", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH2_2)] =			{ "Smash_02_02", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH3_0)] =			{ "Smash_03_00", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH3_1)] =			{ "Smash_03_01", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH3_2)] =			{ "Smash_03_02", false, _float2{0.4f, 1.f}  , 2.f};
	m_AnimDatas[iFlag| ENUM_CLASS(SMASH_FLAG::SMASH4)] =			{ "Smash_04", false, _float2{0.4f, 1.f} , 2.f };
}

void CPlayerAnimMachine::Initialize_GUARD()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::GUARD);

	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_BEGIN)] =		{ "Guard_Begin", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_DURING)] =		{ "Gurad_During", true, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_END)] =		{ "Guard_End", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_HIT)] =		{ "Guard_Hit", false, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_BREAK)] =		{ "Guard_Break", false, _float2{0.8f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_ATTACK)] =		{ "Guard_Attack", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_COUNTER)] =	{ "Guard_Counter", false, _float2{0.4f, 1.f} , 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(GUARD_FLAG::GUARD_WALK)] =		{ "Guard_Walk", true, _float2{0.f, 1.f}, 2.f };
}

void CPlayerAnimMachine::Initialize_HEAVYSTAND()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HEAVYSTAND);

	m_AnimDatas[iFlag | ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_BEGIN)] =	{ "HeavyStand_Begin", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_DURING)] =	{ "HeavyStand_During", true, _float2{0.f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_END)] =		{ "HeavyStand_End", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_HIT)] =		{ "HeavyStand_Hit", false, _float2{0.4f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(HEAVYSTAND_FLAG::HEAVYSTAND_BREAK)] =	{ "HeavyStand_Break", false, _float2{0.4f, 1.f} , 2.f };
}

void CPlayerAnimMachine::Initialize_ROLL()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::ROLL);

	m_AnimDatas[iFlag | ENUM_CLASS(ROLL_FLAG::ROLL_BEGIN)] =	{ "Roll_Begin", false, _float2{0.8f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ROLL_FLAG::ROLL_DURING)] =	{ "Roll_During", false, _float2{0.8f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ROLL_FLAG::ROLL_END)] =		{ "Roll_End", false, _float2{0.7f, 1.f}, 2.f };
	m_AnimDatas[iFlag | ENUM_CLASS(ROLL_FLAG::ROLL_RUN)] =		{ "Roll_Run", false, _float2{0.7f, 1.f}, 2.f };
}

void CPlayerAnimMachine::Initialize_HIT()
{
	_uint iFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_FRONT)] =	{ "Hit_Front", false, _float2{1.f, 1.f}, 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_BACK)] =	{ "Hit_Back", false, _float2{1.f, 1.f}, 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_RIGHT)] =	{ "Hit_Right", false, _float2{1.f, 1.f}, 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_LEFT)] =	{ "Hit_Left", false, _float2{1.f, 1.f}, 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_STRONG)] =	{ "Hit_Strong", false, _float2{1.f, 1.f}, 2.f };
	m_AnimDatas[iFlag| ENUM_CLASS(HIT_FLAG::HIT_DOWN)] =	{ "Hit_Down", false, _float2{1.f, 1.f}, 2.f };
}

HRESULT CPlayerAnimMachine::Set_Animation(CModel* pModelCom, _uint iState)
{
	if (nullptr == pModelCom)
		return E_FAIL;

	if (false == pModelCom->CanChangeAnimation())
		return S_OK;

	return 	pModelCom->Set_Animation(m_AnimDatas[iState]);
}

CPlayerAnimMachine* CPlayerAnimMachine::Create()
{
	CPlayerAnimMachine* pInstance = new CPlayerAnimMachine();
	pInstance->Initialize();

	return pInstance;
}

void CPlayerAnimMachine::Free()
{
	__super::Free();

}
