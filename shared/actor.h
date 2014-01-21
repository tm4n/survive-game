#ifndef ACTOR_H
#define ACTOR_H

#include "defs.h"
#include "level.h"

#define ACTOR_TYPE_PLAYER 1
#define ACTOR_TYPE_BOX 2
#define ACTOR_TYPE_CONTAINER 3
#define ACTOR_TYPE_NPC 4

#define ST_IDLE 0
#define ST_WALKING 1
#define ST_JUMPING 2
#define ST_ATTACK 3
#define ST_WALKING_LEFT 4 // only for player
#define ST_WALKING_RIGHT 5

// only for flying entities
#define ST_FL_DESC 6
#define ST_FL_ASC 7
#define ST_FL_FALLING 8

#define ST_DEATH 9


class level;


class actor
{
    public:
        uint type;   // see ACTOR_TYPE

        level *lvl;
        uint id;    //  id in level

        vec position, angle;

		vec bb_min, bb_max;

		vec move_force;

        // Attributes, either read from file or dynamically created
        float max_health;
        float health;

        float move_speed;

        uint target; // either next node to walk to or entity to attack now

        ushort state;

		static const float actor_step;
		static const float actor_gravity;
		static const float actor_friction;
		static const float actor_accel;

        actor(level *, uint type, vec *, vec *);
		actor(level *, uint actor_id, uint type, vec *, vec *);
        virtual ~actor();

        virtual void change_health(float);

        void move(vec *);
        void move_rel(vec *);
		float move_rel_col(vec *);
        void turn(vec *);
        void turn_to(vec *, double time_delta);
		virtual void set_state(uint new_state);

        void state_manager(double time_delta);

        virtual void frame(double time_delta);

    protected:


    private:
};

#endif // ACTOR_H
