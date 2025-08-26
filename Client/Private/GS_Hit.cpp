#include "ClientPch.h"
#include "GS_Hit.h"
#include "Monster.h"

CGS_Hit::CGS_Hit()
{
}

HRESULT CGS_Hit::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_fCheckDegree = 45.f;

	return S_OK;
}

void CGS_Hit::Enter(CMonster* pMonster)
{
	_uint iDir = ENUM_CLASS(pMonster->Compute_TargetDir(m_fCheckDegree));

	_uint iHitFlag = ENUM_CLASS(HIT_FLAG::FRONT) << iDir;

	ChangeActionFlag(iHitFlag);
}

void CGS_Hit::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Hit::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CGS_Hit* CGS_Hit::Create()
{
	CGS_Hit* pInstance = new CGS_Hit();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Hit"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Hit::Free()
{
	__super::Free();
}

namespace {
	struct GS_HitRegister {
		GS_HitRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::HIT), []()->CMonsterState* { return CGS_Hit::Create(); });
		};

	};
	static GS_HitRegister   Register;
}