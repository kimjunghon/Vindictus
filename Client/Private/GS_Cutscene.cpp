#include "ClientPch.h"
#include "GS_Cutscene.h"
#include "Glasgavelen.h"

CGS_Cutscene::CGS_Cutscene()
{
}

HRESULT CGS_Cutscene::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	return S_OK;
}

void CGS_Cutscene::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(SPAWN_FLAG::START));
}

void CGS_Cutscene::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CGS_Cutscene::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::SPAWN);

	CGlasgavelen* pGlasgavelen = static_cast<CGlasgavelen*>(pMonster);
	pGlasgavelen->End_Cutscene();
}

CGS_Cutscene* CGS_Cutscene::Create()
{
	CGS_Cutscene* pInstance = new CGS_Cutscene();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_Cutscene"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_Cutscene::Free()
{
	__super::Free();
}

namespace {
	struct GS_CutsceneRegister {
		GS_CutsceneRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::CUTSCENE), []()->CMonsterState* { return CGS_Cutscene::Create(); });
		};

	};
	static GS_CutsceneRegister  Register;
}