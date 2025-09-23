#include "EnginePch.h"
#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
		if (true == pBone->Compare_Name(m_szName))
			return true;
		m_iBoneIndex++;
		return false;
		});

	m_iNumKeyFrame = max(max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumScalingKeys);

	_float3 vScale = {};
	_float4 vRotation = {};
	_float3 vPosition = {};

	for (_uint i = 0; i < m_iNumKeyFrame; i++)
	{
		KEYFRAME KeyFrame = {};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mScalingKeys[i].mTime);
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mScalingKeys[i].mTime);
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = static_cast<_float>(pAIChannel->mPositionKeys[i].mTime);
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(ifstream& File, const vector<CBone*>& Bones)
{
	size_t iNameLength = {};
	File.read(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
	File.read(m_szName, sizeof(_char) * iNameLength);

	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
		if (true == pBone->Compare_Name(m_szName))
			return true;
		m_iBoneIndex++;
		return false;
		});

	File.read(reinterpret_cast<_char*>(&m_iNumKeyFrame), sizeof(_uint));

	for (_uint i = 0; i < m_iNumKeyFrame; i++)
	{
		KEYFRAME KeyFrame = {};
		File.read(reinterpret_cast<_char*>(&KeyFrame), sizeof(KEYFRAME));
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	_vector vScale, vRotation, vPosition;

	if (fCurrentTrackPosition == 0.f)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME        LastKeyFrame = m_KeyFrames.back();

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vPosition), 1.f);
	}
	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			(*pCurrentKeyFrameIndex)++;

		_float fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation), fRatio);
		vPosition = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex].vPosition), XMLoadFloat3(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition), fRatio), 1.f);
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Update_AnimChangeTransformationMatrix(const vector<CBone*>& Bones, _float fRatio, _bool IsRootMotionRotate, _bool* pFirstCall)
{
	_vector vScale, vRotation, vPosition;

	if(*pFirstCall)
	{
		_matrix PrevTransformationMatrix = Bones[m_iBoneIndex]->Get_TransformationMatrix();

		XMMatrixDecompose(&m_vChangePrevScale, &m_vChangePrevRotation, &m_vChangePrevPosition, PrevTransformationMatrix);

		if (Bones[m_iBoneIndex]->Compare_Name("ValveBiped.Bip01") || Bones[m_iBoneIndex]->Compare_Name("root") || Bones[m_iBoneIndex]->Compare_Name("Bip01"))
		{
			m_vChangePrevPosition = XMVectorSetX(m_vChangePrevPosition, m_KeyFrames[0].vPosition.x);
			m_vChangePrevPosition = XMVectorSetY(m_vChangePrevPosition, m_KeyFrames[0].vPosition.y);
			if(IsRootMotionRotate)
				m_vChangePrevRotation = XMLoadFloat4(&m_KeyFrames[0].vRotation);
		}
	}

	vScale = XMVectorLerp(m_vChangePrevScale, XMLoadFloat3(&m_KeyFrames[0].vScale), fRatio);
	vRotation = XMQuaternionSlerp(m_vChangePrevRotation, XMLoadFloat4(&m_KeyFrames[0].vRotation), fRatio);
	vPosition = XMVectorSetW(XMVectorLerp(m_vChangePrevPosition, XMLoadFloat3(&m_KeyFrames[0].vPosition), fRatio), 1.f);

	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed Created : CChannel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Create(ifstream& File, const vector<CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();
	if (FAILED(pInstance->Initialize(File, Bones)))
	{
		MSG_BOX(TEXT("Failed Created : CChannel"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	__super::Free();
}
