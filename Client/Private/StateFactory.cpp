#include "ClientPch.h"
#include "StateFactory.h"

IMPLEMENT_SINGLETON(CStateFactory)

CStateFactory::CStateFactory()
{
}

void CStateFactory::Free()
{
	__super::Free();
}
