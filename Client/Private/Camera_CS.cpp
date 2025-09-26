#include "ClientPch.h"
#include "Camera_CS.h"

CCamera_CS::CCamera_CS(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera { pDevice, pDeviceContext }
{
}

CCamera_CS::CCamera_CS(const CCamera_CS& Prototype)
	: CCamera { Prototype }
	, m_fNoiseFreqPitch{ Prototype.m_fNoiseFreqPitch }
	, m_fNoiseAmpPitch{ Prototype.m_fNoiseAmpPitch }
	, m_fNoiseFreqRoll{ Prototype.m_fNoiseFreqRoll }
	, m_fNoiseAmpRoll{ Prototype.m_fNoiseAmpRoll }
	, m_fShakeFreqPitch{ Prototype.m_fShakeFreqPitch }
	, m_fShakeAmpPitch{ Prototype.m_fShakeAmpPitch }
	, m_fShakeFreqRoll{ Prototype.m_fShakeFreqRoll }
	, m_fShakeAmpRoll{ Prototype.m_fShakeAmpRoll }
{
}

HRESULT CCamera_CS::Initialize_Prototype()
{
	m_fNoiseFreqPitch = 0.7f;
	m_fNoiseAmpPitch = XMConvertToRadians(3.f);
	m_fNoiseFreqRoll = 0.5f;
	m_fNoiseAmpRoll = XMConvertToRadians(2.f);

	m_fShakeFreqPitch = 1.2f;
	m_fShakeAmpPitch = XMConvertToRadians(5.f);
	m_fShakeFreqRoll = 1.f;
	m_fShakeAmpRoll = XMConvertToRadians(3.f);

	return S_OK;
}

HRESULT CCamera_CS::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_CS_DESC* pDesc = static_cast<CAMERA_CS_DESC*>(pArg);

	if (FAILED(Ready_Datas(pDesc->pFilePath)))
		return E_FAIL;

	return S_OK;
}

void CCamera_CS::Priority_Update(_float fTimeDelta)
{
}

void CCamera_CS::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_iCurrentIndex + 1 >= m_iNumNodes)
	{
		if (m_fCurrentTime >= m_CS_Datas[m_iCurrentIndex].fTime)
		{
			m_pGameInstance->Change_Camera(TEXT("Player_Camera"));
			return;
		}
	}
	else
	{
		if (m_fCurrentTime >= m_CS_Datas[m_iCurrentIndex + 1].fTime)
		{
			m_iCurrentIndex++;
			m_fCurrentEffectTime = 0.f;
		}
	}

	switch (m_CS_Datas[m_iCurrentIndex].eType)
	{
	case CS_TYPE::STOP:
		Update_STOP();
		break;

	case CS_TYPE::LINEAR:
		Update_LINEAR();
		break;

	case CS_TYPE::SPLINE:
		Update_SPLINE();
		break;
	}

	switch (m_CS_Datas[m_iCurrentIndex].eEffect)
	{
	case CS_EFFECT::SHAKE:
		Update_Shake(fTimeDelta);
		break;
	case CS_EFFECT::ROTATE_NOISE:
		Update_RotateNoise(fTimeDelta);
		break;
	}	
}

void CCamera_CS::Late_Update(_float fTimeDelta)
{
}

HRESULT CCamera_CS::Render()
{
	return S_OK;
}

void CCamera_CS::Reset(void* pArg)
{
	CAMERA_CS_RESET_DESC* pDesc = static_cast<CAMERA_CS_RESET_DESC*>(pArg);

	m_pOwnerWorldMatrix = pDesc->pOwnerWorldMatrix;
	m_OffsetMatrix = pDesc->OffsetMatrix;

	m_fCurrentTime = 0.f;
	m_iCurrentIndex = 0.f;
}

HRESULT CCamera_CS::Ready_Datas(const _char* pFilePath)
{
	std::ifstream File(pFilePath, ios::binary);
	if (!File)
	{
		MSG_BOX(TEXT("Failed File Open"));
		return E_FAIL;
	}

	_uint iNumData = {};

	File.read(reinterpret_cast<_char*>(&iNumData), sizeof(_uint));

	m_iNumNodes = iNumData;

	for (_uint i = 0; i < iNumData; i++)
	{
		CS_DATA CS_Data = {};
		File.read(reinterpret_cast<_char*>(&CS_Data), sizeof(CS_DATA));

		m_CS_Datas.push_back(CS_Data);
	}

	File.close();

	return S_OK;
}

void CCamera_CS::Update_STOP()
{
	_matrix OffsetMatrix = XMMatrixMultiply(XMMatrixRotationQuaternion(m_CS_Datas[m_iCurrentIndex].vRotation),
		XMMatrixTranslationFromVector(m_CS_Datas[m_iCurrentIndex].vPosition));


	_matrix CombinedMatrix = XMMatrixMultiply(XMMatrixMultiply(m_OffsetMatrix, OffsetMatrix), XMLoadFloat4x4(m_pOwnerWorldMatrix));
	//_matrix CombinedMatrix = XMMatrixMultiply(OffsetMatrix, XMLoadFloat4x4(m_pOwnerWorldMatrix));

	m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}

void CCamera_CS::Update_LINEAR()
{
	if (m_iCurrentIndex + 1 >= m_iNumNodes)
	{
		Update_STOP();
		return;
	}

	_float fTime = m_fCurrentTime - m_CS_Datas[m_iCurrentIndex].fTime;
	_float fTimeDiff = m_CS_Datas[m_iCurrentIndex + 1].fTime - m_CS_Datas[m_iCurrentIndex].fTime;
	_float fRatio = fTime / fTimeDiff;

	_vector vLerpPosition = XMVectorLerp(m_CS_Datas[m_iCurrentIndex].vPosition, m_CS_Datas[m_iCurrentIndex + 1].vPosition, fRatio);
	_vector vLerpRotation = XMQuaternionSlerp(m_CS_Datas[m_iCurrentIndex].vRotation, m_CS_Datas[m_iCurrentIndex + 1].vRotation, fRatio);

	_matrix OffsetMatrix = XMMatrixMultiply(XMMatrixRotationQuaternion(vLerpRotation), XMMatrixTranslationFromVector(vLerpPosition));

	_matrix CombinedMatrix = XMMatrixMultiply(XMMatrixMultiply(m_OffsetMatrix, OffsetMatrix), XMLoadFloat4x4(m_pOwnerWorldMatrix));
//	_matrix CombinedMatrix = XMMatrixMultiply(OffsetMatrix, XMLoadFloat4x4(m_pOwnerWorldMatrix));

	m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}

void CCamera_CS::Update_SPLINE()
{
	if (m_iNumNodes <= 2)
	{
		Update_LINEAR();
		return;
	}

	if (m_iCurrentIndex + 1 >= m_iNumNodes)
	{
		Update_STOP();
		return;
	}

	_float fTime = m_fCurrentTime - m_CS_Datas[m_iCurrentIndex].fTime;
	_float fTimeDiff = m_CS_Datas[m_iCurrentIndex + 1].fTime - m_CS_Datas[m_iCurrentIndex].fTime;
	_float fRatio = fTime / fTimeDiff;

	_vector vPoints[4] = {};

	Find_Points(vPoints);

	_float fRatio2 = static_cast<_float>(pow(fRatio, 2.f));
	_float fRatio3 = static_cast<_float>(pow(fRatio, 3.f));

	_vector vSplinePosition = 0.5f * ((2.f * vPoints[1]) + (vPoints[2] - vPoints[0]) * fRatio + (vPoints[0] * 2.f - vPoints[1] * 5.f + vPoints[2] * 4.f - vPoints[3]) * fRatio2 +
		(vPoints[1] * 3.f - vPoints[0] - vPoints[2] * 3.f + vPoints[3]) * fRatio3);

	_vector vLerpRotation = XMQuaternionSlerp(m_CS_Datas[m_iCurrentIndex].vRotation, m_CS_Datas[m_iCurrentIndex + 1].vRotation, fRatio);

	_matrix OffsetMatrix = XMMatrixMultiply(XMMatrixRotationQuaternion(vLerpRotation), XMMatrixTranslationFromVector(vSplinePosition));

	_matrix CombinedMatrix = XMMatrixMultiply(XMMatrixMultiply(m_OffsetMatrix, OffsetMatrix), XMLoadFloat4x4(m_pOwnerWorldMatrix));
//	_matrix CombinedMatrix = XMMatrixMultiply(OffsetMatrix, XMLoadFloat4x4(m_pOwnerWorldMatrix));

	m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}

void CCamera_CS::Find_Points(_vector* pPoints)
{
	pPoints[1] = m_CS_Datas[m_iCurrentIndex].vPosition;
	pPoints[2] = m_CS_Datas[m_iCurrentIndex + 1].vPosition;

	if (m_iCurrentIndex == 0)
		pPoints[0] = pPoints[1];
	else
		pPoints[0] = m_CS_Datas[m_iCurrentIndex - 1].vPosition;

	if (m_iCurrentIndex + 2 >= m_iNumNodes)
		pPoints[3] = pPoints[2];
	else
		pPoints[3] = m_CS_Datas[m_iCurrentIndex + 2].vPosition;
}

void CCamera_CS::Update_Shake(_float fTimeDelta)
{
	m_fCurrentEffectTime += fTimeDelta;

	if (m_iCurrentIndex + 1 >= m_iNumNodes)
		return;

	_float fTimeDiff = m_CS_Datas[m_iCurrentIndex + 1].fTime - m_CS_Datas[m_iCurrentIndex].fTime;

	_float fTimeRatio = 1.f - (m_fCurrentEffectTime / fTimeDiff);

	_float fAmpPitch = m_pGameInstance->Rand(0.f, m_fShakeAmpPitch);
	_float fAmpRoll = m_pGameInstance->Rand(0.f, m_fShakeAmpRoll);

	_float fPitch = sin(m_fCurrentEffectTime * m_fShakeFreqPitch) * fAmpPitch * fTimeRatio;

	_float fRoll = sin(m_fCurrentEffectTime * m_fShakeFreqRoll) * fAmpRoll * fTimeRatio;

	_vector vRotate = XMQuaternionRotationRollPitchYaw(fPitch, 0.f, fRoll);

	m_pTransformCom->TurnQuaternion(vRotate);
}

void CCamera_CS::Update_RotateNoise(_float fTimeDelta)
{
	m_fCurrentEffectTime += fTimeDelta;

	_float fPitch = sin(m_fCurrentEffectTime * m_fNoiseFreqPitch) * m_fNoiseAmpPitch;

	_float fRoll = sin(m_fCurrentEffectTime * m_fNoiseFreqRoll) * m_fNoiseAmpRoll;

	_vector vRotate = XMQuaternionRotationRollPitchYaw(fPitch, 0.f, fRoll);

	m_pTransformCom->TurnQuaternion(vRotate);
}

CCamera_CS* CCamera_CS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_CS* pInstance = new CCamera_CS(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CCamera_CS"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_CS::Clone(void* pArg)
{
	CCamera_CS* pInstance = new CCamera_CS(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CCamera_CS"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_CS::Free()
{
	__super::Free();
	m_CS_Datas.clear();
}
