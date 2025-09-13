#include "ClientPch.h"
#include "Projectile.h"

CProjectile::CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext}
	, m_pPoolInstance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPoolInstance);
}

CProjectile::CProjectile(const CProjectile& Prototype)
	: CGameObject { Prototype }
	, m_pPoolInstance { Prototype.m_pPoolInstance}
{
	Safe_AddRef(m_pPoolInstance);
}

HRESULT CProjectile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	m_fSpeed = pDesc->fSpeedPerSec;

	return S_OK;
}

void CProjectile::Priority_Update(_float fTimeDelta)
{
}

void CProjectile::Update(_float fTimeDelta)
{
}

void CProjectile::Late_Update(_float fTimeDelta)
{
}

HRESULT CProjectile::Render()
{
	return S_OK;
}

void CProjectile::ReturnToPool()
{
}

void CProjectile::Ready_Bezier(_uint iNumPoints, _vector* pPointArray)
{
	m_Points.clear();
	m_LUT.clear();

	m_Points.resize(iNumPoints);

	for (_uint i = 0; i < iNumPoints; i++)
		m_Points[i] = (pPointArray[i]);

	Ready_LUT();
}

_vector CProjectile::Compute_CurvePosition(_float fRatio)
{
	_uint iNumPoints = static_cast<_uint>(m_Points.size());

	_uint iMaxPoints = iNumPoints - 1;
	_float fU = 1.f - fRatio;

	_vector vPosition = {};

	for (_uint i = 0; i < iNumPoints; i++)
	{
		_uint iBinomial = Combination(iMaxPoints, i);
		_float fPowU = static_cast<_float>(pow(fU, iMaxPoints - i));
		_float fPowRatio = static_cast<_float>(pow(fRatio, i));

		_float fScale = iBinomial * fPowU * fPowRatio;

		vPosition = XMVectorAdd(vPosition, XMVectorScale(m_Points[i], fScale));
	}

	return vPosition;
}

_uint CProjectile::Combination(_uint iN, _uint iR)
{
	if (iN == iR || iR == 0)
		return 1;

	return Combination(iN - 1, iR - 1) + Combination(iN - 1, iR);
}

void CProjectile::Ready_LUT()
{
	_uint iNumDatas = 512;

	m_LUT.reserve(iNumDatas);

	_float fRatio = {};
	_float fLength = {};
	_vector vPrevPos = m_Points.front();

	LUT_DATA Data = {};
	Data.fRatio = 0.f;
	Data.fLength = 0.f;
	Data.vPosition = m_Points.front();

	m_LUT.push_back(Data);

	for(_uint i = 1; i<iNumDatas; i++)
	{
		fRatio = static_cast<_float>(i) / static_cast<_float>(iNumDatas);

		_vector vPosition = Compute_CurvePosition(fRatio);

		fLength += XMVectorGetX(XMVector3Length(XMVectorSubtract(vPosition, vPrevPos)));

		Data.fRatio = fRatio;
		Data.fLength = fLength;
		Data.vPosition = vPosition;

		m_LUT.push_back(Data);

		if (i == iNumDatas - 1)
			m_vLastDir = XMVector3Normalize(XMVectorSubtract(vPosition, vPrevPos));

		vPrevPos = vPosition;
	}

	m_fTotalDistance = fLength;
}

_vector CProjectile::Compute_CurvePosition_LUT(_float fDistance)
{
	_uint iLow = 0;
	_uint iHigh = static_cast<_uint>(m_LUT.size()) - 1;

	
	while (iLow + 1 < iHigh)
	{
		_uint iMid = (iLow + iHigh) / 2;

		if (fDistance == m_LUT[iMid].fLength)
			return m_LUT[iMid].vPosition;

		if (fDistance > m_LUT[iMid].fLength)
			iLow = iMid;
		else
			iHigh = iMid;
	}
	if (iLow >= 1)
		int a = 10;

	_float fLerpRatio = (fDistance - m_LUT[iLow].fLength) / (m_LUT[iHigh].fLength - m_LUT[iLow].fLength);
	
	_vector vPosition = XMVectorLerp(m_LUT[iHigh].vPosition, m_LUT[iLow].vPosition, fLerpRatio);

	return vPosition;
}

void CProjectile::Move(_float fTimeDelta)
{
	if (m_IsCurveFinished)
		Move_Directional(fTimeDelta);
	else
		Move_Curve(fTimeDelta);
}

void CProjectile::Move_Directional(_float fTimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	vPosition = vPosition + (m_vLastDir * (m_fSpeed * fTimeDelta));

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

void CProjectile::Move_Curve(_float fTimeDelta)
{
	m_fCurrentDistance += m_fSpeed * fTimeDelta;

	_vector vPosition = XMVectorZero();

	if (m_fCurrentDistance >= m_fTotalDistance)
	{
		_float fOverDistance = m_fCurrentDistance - m_fTotalDistance;
		
		m_fCurrentDistance = m_fTotalDistance;
		m_IsCurveFinished = true;

		vPosition = XMVectorAdd(Compute_CurvePosition_LUT(m_fCurrentDistance), (m_vLastDir * fOverDistance));
	}
	else
		vPosition = Compute_CurvePosition_LUT(m_fCurrentDistance);


	m_vPrevPos = m_pTransformCom->Get_State(STATE::POSITION);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pPoolInstance);
	Safe_Release(m_pColliderCom);
}
