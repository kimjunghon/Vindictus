#pragma once

//////////////STATE//////////////
namespace Client
{
	namespace Player
	{
		enum class STATE_FLAG {
			IDLE = 1 << 0,
			MOVE = 1 << 1,
			ATTACK = 1 << 2,
			SMASH = 1 << 3,
			GUARD = 1 << 4,
			HEAVYSTAND = 1 << 5,
			ROLL = 1 << 6,
			HIT = 1 << 7,
			DEAD = 1 << 31
		};

		//////////////IDLE//////////////
		enum class IDLE_FLAG {
			DEFAULT = 1 << 8,
			REST = 1 << 9
		};

		//////////////MOVE//////////////
		enum class MOVE_FLAG {
			DEFAULT = 1 << 8,
			SPRINT = 1 << 9,
			STOP = 1 << 10,
			SPRINT_STOP = 1 << 11
		};
		//////////////ATTACK//////////////
		enum class ATTACK_FLAG {
			COMBO1 = 1 << 8,
			COMBO2 = 1 << 9,
			COMBO3 = 1 << 10,
			COMBO4 = 1 << 11
		};
		//////////////SMASH//////////////
		enum class SMASH_FLAG {
			SMASH0 = 1 << 8,
			SMASH0_CHARGE = 1 << 9,
			SMASH0_CHARGE_END = 1 << 10,
			SMASH1 = 1 << 11,
			SMASH2_0 = 1 << 12,
			SMASH2_1 = 1 << 13,
			SMASH2_2 = 1 << 14,
			SMASH3_0 = 1 << 15,
			SMASH3_1 = 1 << 16,
			SMASH3_2 = 1 << 17,
			SMASH4 = 1 << 18
		};

		//////////////GUARD//////////////
		enum class GUARD_FLAG {
			GUARD_BEGIN = 1 << 8,
			GUARD_DURING = 1 << 9,
			GUARD_END = 1 << 10,
			GUARD_HIT = 1 << 11,
			GUARD_BREAK = 1 << 12,
			GUARD_ATTACK = 1 << 13,
			GUARD_COUNTER = 1 << 14,
			GUARD_WALK = 1 << 15

		};

		//////////////HEAVYSTAND//////////////
		enum class HEAVYSTAND_FLAG {
			HEAVYSTAND_BEGIN = 1 << 8,
			HEAVYSTAND_DURING = 1 << 9,
			HEAVYSTAND_END = 1 << 10,
			HEAVYSTAND_HIT = 1 << 11,
			HEAVYSTAND_BREAK = 1 << 12
		};

		//////////////ROLL//////////////
		enum class ROLL_FLAG {
			ROLL_BEGIN = 1 << 8,
			ROLL_DURING = 1 << 9,
			ROLL_END = 1 << 10,
			ROLL_RUN = 1 << 11
		};

		//////////////HIT//////////////
		enum class HIT_FLAG {
			HIT_FRONT = 1 << 8,
			HIT_BACK = 1 << 9,
			HIT_RIGHT = 1 << 10,
			HIT_LEFT = 1 << 11,
			HIT_STRONG = 1 << 12,
			HIT_DOWN = 1 << 13
		};
	}

	namespace Vampire
	{
		enum class STATE_FLAG {
			SPAWN = 1 << 0,
			IDLE = 1 << 1,
			MOVE = 1 << 2,
			ATTACK = 1 << 3,
			HIT = 1 << 4,
			DEAD = 1 << 31
		};

		enum class MOVE_FLAG {
			WALK = 1 << 8,
			FRONT = 1 << 9,
			BACK = 1 << 10,
			LEFT = 1 << 11,
			RIGHT = 1 << 12,
		};

		enum class HIT_FLAG
		{
			FRONT = 1<<8,
			BACK  = 1 << 9,
			LEFT  = 1 << 10,
			RIGHT = 1 << 11,
		};

		enum class ATTACK_FLAG
		{
			MELEE = 1 << 8,
			RANGE = 1 << 9
		};
	}

	namespace Queen
	{
		enum class STATE_FLAG {
			CUTSEAN = 1 << 0,
			IDLE = 1 << 1,
			MOVE = 1 << 2,
			ATTACK = 1 << 3,
			BURROW = 1 << 4,
			HIT = 1 << 5,
			DEAD = 1 << 31
		};

		enum class IDLE_FLAG {
			THREAT = 1 << 8,
			LOW_HEALTH = 1 << 9
		};

		enum class MOVE_FLAG {
			RUN = 1 << 8,
			TURN_LEFT = 1 << 9,
			TURN_RIGHT = 1 << 10
		};

		enum class ATTACK_FLAG {
			DOUBLE = 1 << 8,
			SWOOP = 1 << 9,
			POSION = 1 << 10,
			LEFTHAND = 1 << 11,
			RIGHTHAND = 1 << 12,
			MELLE = 1 << 13,
			TAIL = 1 << 14,
			TURN_LEFT = 1 << 15,
			TURN_RIGHT = 1 << 16
		};

		enum class BURROW_FLAG {
			BEGIN = 1 << 8,
			STAY = 1 << 9,
			MOVE = 1 << 10,
			ATTACK1 = 1 << 11,
			ATTACK2 = 1 << 12,
			END = 1 << 13
		};

		enum class HIT_FLAG {
			DAMAGE = 1 << 8,
			DOWN_BEGIN = 1 << 9,
			DOWN_DURING = 1 << 10,
			DOWN_END = 1 << 11
		};
	}
}