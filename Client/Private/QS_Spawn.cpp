#include "ClientPch.h"
#include "QS_Spawn.h"
#include "Queen.h"

CQS_Spawn::CQS_Spawn()
{
}

HRESULT CQS_Spawn::Initialize()
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::CUTSEAN);

	return S_OK;
}

void CQS_Spawn::Enter(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::CUTSEAN);
}

void CQS_Spawn::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CQS_Spawn::Exit(CMonster* pMonster)
{
	m_iStateFlag = ENUM_CLASS(STATE_FLAG::CUTSEAN);
	
	CQueen* pQueen = static_cast<CQueen*>(pMonster);
	pQueen->End_CutScene();
}

CQS_Spawn* CQS_Spawn::Create()
{
	CQS_Spawn* pInstance = new CQS_Spawn();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CQS_Spawn"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQS_Spawn::Free()
{
	__super::Free();
}

namespace {
	struct QS_SpawnRegister {
		QS_SpawnRegister() {
			CMonsterStateFactory::GetInstance()->Register(ENUM_CLASS(MONSTER_STATE_TYPE::QUEEN), ENUM_CLASS(QUEEN_STATE::SPAWN), []()->CMonsterState* { return CQS_Spawn::Create(); });
		};

	};
	static QS_SpawnRegister  Register;
}