#include "ClientPch.h"
#include "MonsterState.h"
#include "Monster.h"


CMonsterState::CMonsterState()
{
}

_bool CMonsterState::CanStateChange(CMonster* pMonster)
{
    return (pMonster->AnimCanChange() || pMonster->AnimIsFinished());
}

HRESULT CMonsterState::Initialize()
{
    return S_OK;
}

void CMonsterState::Enter(CMonster* pMonster)
{
}

void CMonsterState::Update(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterState::Exit(CMonster* pMonster)
{
}

void CMonsterState::Free()
{
    __super::Free();
}
