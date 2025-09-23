#include "ClientPch.h"
#include "Effect_Prefab.h"

CEffect_Prefab::CEffect_Prefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CEffect { pDevice, pDeviceContext }
{
}

CEffect_Prefab::CEffect_Prefab(const CEffect_Prefab& Prototype)
	: CEffect { Prototype }
	, m_Effects { Prototype.m_Effects }
	, m_iNumMaxEffect { Prototype.m_iNumMaxEffect }
{
}

HRESULT CEffect_Prefab::Initialize_Prototype(const Value& Effects)
{
	for (auto& Effect : Effects.GetArray())
	{
		m_iNumMaxEffect++;

		PREFAB_EFFECT PrefabEffect = {};

		string TempName = {};
		_tchar EffectName[MAX_PATH] = {};

		if (Effect.HasMember("Name") && Effect["Name"].IsString())
		{
			TempName = Effect["Name"].GetString();
			MultiByteToWideChar(CP_UTF8, 0, TempName.c_str(), static_cast<_int>(TempName.size()), EffectName, static_cast<_int>(TempName.size()));
			PrefabEffect.strEffectName = EffectName;
		}

		if (Effect.HasMember("SpawnTime") && Effect["SpawnTime"].IsFloat())
			PrefabEffect.fSpawnTime = Effect["SpawnTime"].GetFloat();

		_float4x4 OffsetMatrix = {};
		
		if (Effect.HasMember("OffsetMatrix") && Effect["OffsetMatrix"].IsArray())
		{
			const auto& Matrix = Effect["OffsetMatrix"].GetArray();
			for (SizeType i = 0; i < Matrix.Size(); i++)
			{
				const auto& Row = Matrix[i];

				for (SizeType j = 0; j < Row.Size(); j++)
					OffsetMatrix.m[i][j] = Row[j].GetFloat();
			}
		}

		PrefabEffect.OffsetMatrix = XMLoadFloat4x4(&OffsetMatrix);

		m_Effects.push_back(PrefabEffect);
	}

	return S_OK;
}

HRESULT CEffect_Prefab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CEffect_Prefab::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Prefab::Update(_float fTimeDelta)
{
	if (false == m_IsActive)
		return;

	m_fCurrentTime += fTimeDelta;

	while (m_Effects[m_iCurrentEffect].fSpawnTime <= m_fCurrentTime)
	{
		EFFECT_SPAWN_DESC EffectDesc = {};

		EffectDesc.SpawnWorldMatrix = XMMatrixMultiply(m_Effects[m_iCurrentEffect].OffsetMatrix, m_pTransformCom->Get_WorldMatrix());
		EffectDesc.IsEmissive = m_IsEmissive;

		if (m_Effects[m_iCurrentEffect].strEffectName == TEXT("Shiled_Particle"))
			int a = 10;
		m_pPool_Instance->Request_SpawnEffect(m_Effects[m_iCurrentEffect].strEffectName, &EffectDesc);

		m_iCurrentEffect++;

		if (m_iCurrentEffect >= m_iNumMaxEffect)
		{
			ReturnToPool();
			break;
		}
	}
}

void CEffect_Prefab::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect_Prefab::Render()
{
	return S_OK;
}

HRESULT CEffect_Prefab::Spawn(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_SPAWN_DESC* pDesc = static_cast<EFFECT_SPAWN_DESC*>(pArg);

	_matrix CurrentWorldMatrix = pDesc->SpawnWorldMatrix;

	m_IsEmissive = pDesc->IsEmissive;

	m_pTransformCom->Set_WorldMatrix(CurrentWorldMatrix);

	m_IsActive = true;

	m_iCurrentEffect = 0;
	m_fCurrentTime = 0.f;

	return S_OK;
}

CEffect_Prefab* CEffect_Prefab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Value& Effects)
{
	CEffect_Prefab* pInstance = new CEffect_Prefab(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(Effects)))
	{
		MSG_BOX(TEXT("Failed Created : CEffect_Prefab"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Prefab::Clone(void* pArg)
{
	CEffect_Prefab* pInstance = new CEffect_Prefab(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CEffect_Prefab"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Prefab::Free()
{
	__super::Free();
}
