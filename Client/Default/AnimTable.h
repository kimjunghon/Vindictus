#pragma once

//////////////STATE//////////////
namespace Client
{
	enum class STATE_FLAG {
		IDLE		= 1 << 0,
		MOVE		= 1 << 1,
		ATTACK		= 1 << 2,
		SMASH		= 1 << 3,
		GUARD		= 1 << 4,
		HEAVYSTAND	= 1 << 5,
		ROLL		= 1 << 6,
		HIT			= 1 << 7
	};


//////////////IDLE//////////////
	enum class IDLE_FLAG {
		DEFAULT		= 1 << 8,
		REST		= 1 << 9
	};

//////////////MOVE//////////////
	enum class MOVE_FLAG {
		DEFAULT			= 1 << 8,
		SPRINT			= 1 << 9,
		STOP			= 1 << 10,
		SPRINT_STOP		= 1 << 11
	};
//////////////ATTACK//////////////
	enum class ATTACK_FLAG {
		COMBO1	= 1 << 8,
		COMBO2	= 1 << 9,
		COMBO3	= 1 << 10,
		COMBO4	= 1 << 11
	};
//////////////SMASH//////////////
	enum class SMASH_FLAG {
		SMASH0				= 1 << 8,
		SMASH0_CHARGE		= 1 << 9,
		SMASH0_CHARGE_END	= 1 << 10,
		SMASH1				= 1 << 11,
		SMASH2_0			= 1 << 12,
		SMASH2_1			= 1 << 13,
		SMASH2_2			= 1 << 14,
		SMASH3_0			= 1 << 15,
		SMASH3_1			= 1 << 16,
		SMASH3_2			= 1 << 17,
		SMASH4				= 1 << 18
	};

//////////////GUARD//////////////
	enum class GUARD_FLAG {
		GUARD_BEGIN		= 1 << 8,
		GUARD_DURING	= 1 << 9,
		GUARD_END		= 1 << 10,
		GUARD_HIT		= 1 << 11,
		GUARD_BREAK		= 1 << 12,
		GUARD_ATTACK	= 1 << 13,
		GUARD_COUNTER	= 1 << 14,
		GUARD_WALK		= 1 << 15

	};

//////////////HEAVYSTAND//////////////
	enum class HEAVYSTAND_FLAG {
		HEAVYSTAND_BEGIN	= 1 << 8,
		HEAVYSTAND_DURING	= 1 << 9,
		HEAVYSTAND_END		= 1 << 10,
		HEAVYSTAND_HIT		= 1 << 11,
		HEAVYSTAND_BREAK	= 1 << 12
	};

//////////////ROLL//////////////
	enum class ROLL_FLAG {
		ROLL_BEGIN	= 1 << 8,
		ROLL_DURING	= 1 << 9,
		ROLL_END	= 1 << 10
	};

//////////////HIT//////////////
	enum class HIT_FLAG {
		HIT_FRONT	= 1 << 8,
		HIT_BACK	= 1 << 9,
		HIT_RIGHT	= 1 << 10,
		HIT_LEFT	= 1 << 11,
		HIT_STRONG	= 1 << 12,
		HIT_DOWN	= 1 << 13
	};

}