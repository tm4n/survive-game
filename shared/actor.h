#ifndef ACTOR_H
#define ACTOR_H

#include "defs.h"
#include "level.h"
#include "item_cont.h"
#include "bufflist.h"

#define ACTOR_TYPE_NPC 1
#define ACTOR_TYPE_PLAYER 2
#define ACTOR_TYPE_CONTAINER 3

#define ACTOR_STATE_IDLE 1
#define ACTOR_STATE_WALKING 2
#define ACTOR_STATE_ATTACKING 5
#define ACTOR_STATE_ATTACK_PAUSE 6 //needed for cooldown
#define ACTOR_STATE_CASTING 6 // TODO: casting: use timer, reset timer on damage
//#define ACTOR_STATE_INTERACTING 7 // talking to ppl, pickup up stuff.. might be needed to
// not allow too ppl access one container

#define ACTOR_STATE_DEAD 10


// interaction types, might be moves somewhere else
#define ACTOR_INTERACT_TYPE_LOOT 1


class level;

class actor
{
    public:
        uint type;   // see ACTOR_TYPE

        level *lvl;
        uint id;    //  id in level
        ushort faction;
        uint current_tile;

        vec position, angle;

        // Attributes, either read from file or dynamically created
        float max_health;
        float health;
        float mana;
        float max_mana;
        uint chlevel;
        float bounty_xp;

        float att_dmg_min;
        float att_dmg_max;
        float defence;
        float att_range;
        float att_speed;
        float att_cooldown;
        float move_speed;

        bufflist *buffl;

		item_cont *inv;

        float move_force;
        double timer_attack;
        double timer_attack_cooldown;

        uint target; // either next node to walk to or entity to attack now

        ushort state;

        actor(level *, uint type, vec *, vec *);
		actor(level *, uint actor_id, uint type);
        virtual ~actor();

        virtual float get_damage();
        virtual float calc_max_damage();
        virtual float calc_min_damage();

        virtual float calc_defence();
        virtual float calc_att_range();
        virtual float calc_att_speed();
        virtual float calc_att_cooldown();
        virtual float calc_move_speed();

        virtual float calc_max_health();
        virtual float calc_max_mana();

        virtual void change_health(float);
        virtual void change_mana(float);

        virtual bool do_damage(uint target_actor, float damage);

        void move(vec *);
        void move_rel(vec *);
        void turn(vec *);
        void turn_to(vec *, double time_delta);
		virtual void set_state(uint new_state);

        void state_manager(double time_delta);

        virtual void frame(double time_delta);

    protected:


    private:
};

#endif // ACTOR_H
