#include "GS_Attack.h"

CGS_Attack::CGS_Attack()
{
}

HRESULT CGS_Attack::Initialize()
{
    return E_NOTIMPL;
}

void CGS_Attack::Enter(CMonster* pMonster)
{
}

void CGS_Attack::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Attack::Exit(CMonster* pMonster)
{
}

CGS_Attack* CGS_Attack::Create()
{
	CGS_Attack* pInstance = new CGS_Attack();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Attack"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Attack::Free()
{
	__super::Free();
}

namespace {
	struct GS_AttackRegister {
		GS_AttackRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::ATTACK), []()->CMonsterState* { return CGS_Attack::Create(); });
		};

	};
	static GS_AttackRegister  Register;
}