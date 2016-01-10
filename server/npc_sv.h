#ifndef __NPC_SV_H__
#define __NPC_SV_H__

#include "npc.h"
#include "game.h"
#include "level_sv.h"

class level_sv;

class npc_sv : public npc
{
    public:

        npc_sv(level_sv *, uint, vec *, vec *, int *);
        virtual ~npc_sv();

        virtual void frame(double time_delta);
		virtual void event_callback(int event_type, actor *ac);

		virtual void callback_attack_done(int target, const vec &target_pos);

    protected:

		actor *get_nearest_target(bool get_barrier);

		float npc_target_timer;
		float send_pos_timer;
		float jump_timer;
		float death_timer;
		bool sv_attack_done;
		int old_target;
		vec last_position;
		
		int *npc_counter;

    private:
};

#endif // __NPC_SV_H__
