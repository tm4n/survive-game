#ifndef __NPC_H__
#define __NPC_H__

#include "actor.h"
#include "backends/b_npcs.h"

#define NPC_ORDER_JUMP (1<<0)

class npc : public actor
{
    public:
        uint npc_type;
		int target;

		float attack_count;
		bool attack_done;
		int npc_orders;

        npc(level*, uint npc_type, vec *, vec *);
		npc(level*, uint actor_id, uint npc_type, vec *, vec *, float health, int target);
        virtual ~npc();

        virtual void movement(double time_delta);

		// inline getter functions
		float get_move_speed() {return (b_npcs::instance()->at(npc_type)->move_speed);}
		int get_attack_type() {return (b_npcs::instance()->at(npc_type)->attack_type);}
		float get_attack_speed() {return (b_npcs::instance()->at(npc_type)->attack_speed);}
		float get_attack_range() {return (b_npcs::instance()->at(npc_type)->attack_range);}
		float get_damage() {return (b_npcs::instance()->at(npc_type)->damage);}
		int get_bounty() {return (b_npcs::instance()->at(npc_type)->bounty);}
		int get_ai_type() {return (b_npcs::instance()->at(npc_type)->ai_type);}
		float get_ai_max_z_diff() {return (b_npcs::instance()->at(npc_type)->ai_max_z_diff);}
		float get_anim_speed() {return (b_npcs::instance()->at(npc_type)->anim_speed);}
    protected:
    private:
};

#endif // __NPC_H__
