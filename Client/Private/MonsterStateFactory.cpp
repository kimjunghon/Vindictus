#include "ClientPch.h"
#include "MonsterStateFactory.h"

IMPLEMENT_SINGLETON(CMonsterStateFactory)

CMonsterStateFactory::CMonsterStateFactory()
{
}

void CMonsterStateFactory::Free()
{
	__super::Free();
}
