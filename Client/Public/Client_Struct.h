#pragma once

namespace Client
{
	typedef struct tagPlayerStatus {
		float fFullHealth;
		float fHealth;
		float fStamina;
		float fAttackDamage;
		float fDefense;
	}PLAYER_STATUS;

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
		_bool			IsDown;
		_float			fAttackRatio;
		_float2			vAttackRange;
	}ATTACK_MAP;
}