#include "EnginePch.h"
#include "Pawn.h"
#include "PawnObject.h"
#include "GameInstance.h"

CPawn::CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
	
}

CPawn::CPawn(const CPawn& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPawn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPawn::Priority_Update(_float fTimeDelta)
{
}

void CPawn::Update(_float fTimeDelta)
{
}

void CPawn::Late_Update(_float fTimeDelta)
{
}

HRESULT CPawn::Render()
{
	return S_OK;
}

HRESULT CPawn::Add_PawnObject(const _wstring& strPawnObjectTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	if (nullptr != Find_PawnObject(strPawnObjectTag))
		return E_FAIL;

	CPawnObject* pPawnObject = dynamic_cast<CPawnObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pPawnObject)
		return E_FAIL;

	m_PawnObjects.emplace(strPawnObjectTag, pPawnObject);
	
	return S_OK;
}

HRESULT CPawn::Add_PawnObject(const _wstring& strPawnObjectTag, CPawnObject* pPawnObject)
{
	if (nullptr == pPawnObject)
		return E_FAIL;

	m_PawnObjects.emplace(strPawnObjectTag, pPawnObject);

	return S_OK;
}

HRESULT CPawn::Remove_PawnObject(const _wstring& strPawnObjectTag)
{
	CPawnObject* pPawnObject = Find_PawnObject(strPawnObjectTag);
	
	if (nullptr == pPawnObject)
		return E_FAIL;
	
	Safe_Release(pPawnObject);
	m_PawnObjects.erase(strPawnObjectTag);

	return S_OK;
}

CPawnObject* CPawn::Find_PawnObject(const _wstring& strPawnObjectTag)
{
	auto Pair = m_PawnObjects.find(strPawnObjectTag);
	if (Pair == m_PawnObjects.end())
		return nullptr;

	return Pair->second;
}

void CPawn::Free()
{
	__super::Free();

	for (auto& Pair : m_PawnObjects)
		Safe_Release(Pair.second);
	m_PawnObjects.clear();
}
