#include "ClientPch.h"
#include "GS_Turn.h"
#include "Monster.h"

CGS_Turn::CGS_Turn()
{
}

HRESULT CGS_Turn::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);

    return S_OK;
}

void CGS_Turn::Enter(CMonster* pMonster)
{
    DIR eRightDir = pMonster->Compute_TargetDir(0.f);

	ChangeActionFlag(GetTurnFlag(eRightDir));
}

void CGS_Turn::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Turn::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::MOVE);
}

_uint CGS_Turn::GetTurnFlag(DIR eRightDir)
{
	_uint iFlag = {};

	switch (eRightDir)
	{
	case DIR::RIGHT:
		iFlag = ENUM_CLASS(MOVE_FLAG::TURN_RIGHT);
		break;
	case DIR::LEFT:
		iFlag = ENUM_CLASS(MOVE_FLAG::TURN_LEFT);
		break;
	}

    return iFlag;
}

CGS_Turn* CGS_Turn::Create()
{
	CGS_Turn* pInstance = new CGS_Turn();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Spawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Turn::Free()
{
	__super::Free();
}

namespace {
	struct GS_TurnRegister {
		GS_TurnRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::TURN), []()->CMonsterState* { return CGS_Turn::Create(); });
		};

	};
	static GS_TurnRegister  Register;
}