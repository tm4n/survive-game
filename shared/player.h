#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "actor.h"

class player : public actor
{
    public:

        uint class_id;

        char name[32];

        // character attributes

        uint strength;
        uint agility;
        uint intelligence;

        uint points_remain;

        uint xp;
        uint gold;

		ushort equip[PRJ_INV_EQUIPMENT];

		questlog *quests;
		skill_cont *skills;

		double respawn_timer;




		virtual float calc_max_damage();
        virtual float calc_min_damage();

        virtual float calc_defence();
        virtual float calc_att_range();
        virtual float calc_att_speed();
        virtual float calc_att_cooldown();
        virtual float calc_move_speed();

        virtual float calc_max_health();
        virtual float calc_max_mana();

        virtual uint calc_next_xp();

        void recalc_values();

		void skill_cooldown_frame(double time_delta);


        player(level *, vec *pos, vec *pan,
               uint class_id,
               const char *name,
               uint level,
               uint strength,
               uint agility,
               uint intelligence,
               uint points_remain,
               uint xp,
               uint gold,
               double respawn_timer);
		player(level *,
			   uint actor_id,
               uint class_id,
               const char *name);
        virtual ~player();
    protected:
    private:
};

#endif // __PLAYER_H__
