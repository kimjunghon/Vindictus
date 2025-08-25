#include "ClientPch.h"
#include "VS_DeadStrong.h"
#include "Monster.h"

CVS_DeadStrong::CVS_DeadStrong()
{
}

_bool CVS_DeadStrong::CanStateChange(CMonster* pMonster)
{
	return false;
}

HRESULT CVS_DeadStrong::Initialize()
{
    return S_OK;
}

void CVS_DeadStrong::Enter(CMonster* pMonster)
{
    m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD) | ENUM_CLASS(DEAD_FLAG::STRONG);

    pMonster->LookAtTarget();
}

void CVS_DeadStrong::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->AnimIsFinished())
		pMonster->Dead();
}

void CVS_DeadStrong::Exit(CMonster* pMonster)
{
}

CVS_DeadStrong* CVS_DeadStrong::Create()
{
	CVS_DeadStrong* pInstance = new CVS_DeadStrong();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_DeadStrong"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_DeadStrong::Free()
{
	__super::Free();
}


namespace {
	struct VS_DeadStrongRegister {
		VS_DeadStrongRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::DEAD_STRONG), []()->CMonsterState* { return CVS_DeadStrong::Create(); });
		};

	};
	static VS_DeadStrongRegister  Register;
}