#pragma once

namespace Client
{
	typedef struct tagStatus {
		float fFullHealth;
		float fHealth;
		float fAttackDamage;
		float fDefense;
	}STATUS;

	typedef struct tagPlayerStatus : public STATUS {
		float  fStamina;
	}PLAYER_STATUS;

	typedef struct tagQueenStatus {
		float fStunDamage;
		float fCurrentDamage;
		float fFullLegDurabillity;
		float fLegDurabillity;
		bool  IsBrokenLeg;
	}QUEEN_STATUS;

	typedef struct tagGavelenStatus {
		float fStunDamage;
		float fCurrentDamage;
		float fRagePercent;
		float fWingBrokenPercent;
	}GAVELEN_STATUS;

	typedef struct tagSlotRenderDesc {
		XMFLOAT4X4 WorldMatrix;
		XMFLOAT4X4 ViewMatrix;
		XMFLOAT4X4 ProjMatrix;
	}SLOT_RENDER_DESC;

	typedef struct tagMonsterSpawnData {
		MONSTER_TYPE	eMonsterType;
		int				iCellIndex;
		XMFLOAT3		vPosition;
	}MONSTER_SPAWN_DATA;

	typedef struct tagAttackMap
	{
		_uint			iAttackColliderIndex;
		ATTACK_TYPE		eAttackType;
		_float			fAttackRatio;
		_float2			vAttackRange;
	}ATTACK_MAP;

	typedef struct tagAttackData
	{
		size_t		iAttackID;
		ATTACK_TYPE eAttackType;
		_float		fDamage;
		_vector		vAttackPosition;
	}ATTACK_DATA;
}