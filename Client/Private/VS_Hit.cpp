#include "ClientPch.h"
#include "VS_Hit.h"
#include "Monster.h"

CVS_Hit::CVS_Hit()
{
}

HRESULT CVS_Hit::Initialize()
{
	m_fCheckDegree = 45.f;

	return S_OK;
}

void CVS_Hit::Enter(CMonster* pMonster)
{
	_uint iDir = ENUM_CLASS(pMonster->Compute_TargetDir(m_fCheckDegree));

	_uint iHitFlag = ENUM_CLASS(HIT_FLAG::FRONT) << iDir;

	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT) | iHitFlag;
}

void CVS_Hit::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CVS_Hit::Exit(CMonster* pMonster)
{
}

CVS_Hit* CVS_Hit::Create()
{
	CVS_Hit* pInstance = new CVS_Hit();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CVS_Hit"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVS_Hit::Free()
{
	__super::Free();
}

namespace {
	struct VS_HitRegister {
		VS_HitRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::VAMPIRE), ENUM_CLASS(VAMPIRE_STATE::HIT), []()->CMonsterState* { return CVS_Hit::Create(); });
		};

	};
	static VS_HitRegister  Register;
}