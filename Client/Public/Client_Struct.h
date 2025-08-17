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

}