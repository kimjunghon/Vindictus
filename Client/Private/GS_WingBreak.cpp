#include "ClientPch.h"
#include "GS_WingBreak.h"
#include "Glasgavelen.h"

CGS_WingBreak::CGS_WingBreak()
{
}

HRESULT CGS_WingBreak::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);

	m_vChangeModelRange = _float2(210.f, 214.f);

	return S_OK;
}

void CGS_WingBreak::Enter(CMonster* pMonster)
{
	ChangeActionFlag(ENUM_CLASS(HIT_FLAG::WINGBREAK));
	static_cast<CGlasgavelen*>(pMonster)->WingBreak();
}

void CGS_WingBreak::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->IsAnimationInRangeTrackPosition(m_vChangeModelRange))
	{
		if (false == m_IsModelChange)
		{
			static_cast<CGlasgavelen*>(pMonster)->Change_BrokenModel();
			m_IsModelChange = true;
		}
	}
}
void CGS_WingBreak::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::HIT);
}

CGS_WingBreak* CGS_WingBreak::Create()
{
	CGS_WingBreak* pInstance = new CGS_WingBreak();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CGS_WingBreak"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGS_WingBreak::Free()
{
	__super::Free();
}

namespace {
	struct GS_WingBreakRegister {
		GS_WingBreakRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::GLASGAVELEN), ENUM_CLASS(GAVELEN_STATE::WINGBREAK), []()->CMonsterState* { return CGS_WingBreak::Create(); });
		};

	};
	static GS_WingBreakRegister   Register;
}