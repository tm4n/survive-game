#ifndef __NPC_AI_H__
#define __NPC_AI_H__

#include "npc_sv.h"

#define TICK_FIND_TARGET_LIMIT 4

#define NPC_AI_STATE_IDLE 1
#define NPC_AI_STATE_STRIDING 2    // running around, never getting too far from startpoint
#define NPC_AI_STATE_PURSUEING 3  // following an enemy
#define NPC_AI_STATE_FLEEING 4
#define NPC_AI_STATE_ATTACKING 5

#define NPC_AI_STATE_DEAD 10
//... more to come

// Basic AI type, the functions are to be redefined if someone wants a different AI
class npc_sv;

class npc_ai
{
    public:
        npc_sv *me;
        uint ai_state;
        uint ai_target;        // current AI target, depends on AI state

        static float enemy_detection_range;
        static float cancel_pursue_range;
        static float stride_probability;
        static float stride_max_range;

        npc_ai(npc_sv *me);
        virtual ~npc_ai();

        virtual void frame(double delta_time);
        virtual void set_ai_state(uint new_state);
        virtual long find_best_victim();
        virtual uint find_stride_target();
        virtual bool go_to_tile(uint ttile);
        virtual bool go_to_actor(uint enemy_tile);
        virtual bool decide_cancel_enemy();
        //virtual bool decide_casting(); // TODO: cast when usefull
        //virtual bool decide_fleeing(); // TODO: flee if afraid

        virtual bool is_enemy(actor *a);

    protected:
        bool damage_done;
        double timer_finder;
        double timer_respawn;

    private:
};

#endif // __NPC_AI_H__
