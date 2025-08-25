#include "ClientPch.h"
#include "VS_Idle.h"

CVS_Idle::CVS_Idle()
{
}

HRESULT CVS_Idle::Initialize()
{
	return S_OK;
}

void CVS_Idle::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::IDLE);
}

void CVS_Idle::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CVS_Idle::Exit(CMonster* pMonster)
{
}

CVS_Idle* CVS_Idle::Create()
{
	CVS_Idle* pInstance = new CVS_Idle();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Idle"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Idle::Free()
{
	__super::Free();
}

namespace {
	struct VS_IdleRegister {
		VS_IdleRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::IDLE), []()->CMonsterState* { return CVS_Idle::Create(); });
		};

	};
	static VS_IdleRegister  Register;
}