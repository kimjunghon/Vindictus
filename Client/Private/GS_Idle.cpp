#include "ClientPch.h"
#include "GS_Idle.h"
#include "Monster.h"

CGS_Idle::CGS_Idle()
{
}

HRESULT CGS_Idle::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);

	return S_OK;
}

void CGS_Idle::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(IDLE_FLAG::THREAT));
}

void CGS_Idle::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Idle::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);
}

CGS_Idle* CGS_Idle::Create()
{
	CGS_Idle* pInstance = new CGS_Idle();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Idle"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Idle::Free()
{
	__super::Free();
}

namespace {
	struct GS_IdleRegister {
		GS_IdleRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::IDLE), []()->CMonsterState* { return CGS_Idle::Create(); });
		};

	};
	static GS_IdleRegister    Register;
}