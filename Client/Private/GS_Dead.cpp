#include "ClientPch.h"
#include "GS_Dead.h"
#include "Monster.h"

CGS_Dead::CGS_Dead()
{
}

HRESULT CGS_Dead::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD);

	return S_OK;
}

void CGS_Dead::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD);
}

void CGS_Dead::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->AnimIsFinished())
		pMonster->Dead();
}

void CGS_Dead::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::DEAD);
}

CGS_Dead* CGS_Dead::Create()
{
	CGS_Dead* pInstance = new CGS_Dead();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Dead"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Dead::Free()
{
	__super::Free();
}

namespace {
	struct GS_DeadRegister {
		GS_DeadRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::DEAD), []()->CMonsterState* { return CGS_Dead::Create(); });
		};

	};
	static GS_DeadRegister  Register;
}