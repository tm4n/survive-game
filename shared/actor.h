#ifndef ACTOR_H
#define ACTOR_H

#include "defs.h"
#include "level.h"

#define ACTOR_TYPE_PLAYER 1
#define ACTOR_TYPE_BOX 2
#define ACTOR_TYPE_CONTAINER 3
#define ACTOR_TYPE_NPC 4


class level;

class actor
{
    public:
        uint type;   // see ACTOR_TYPE

        level *lvl;
        uint id;    //  id in level

        vec position, angle;

        // Attributes, either read from file or dynamically created
        float max_health;
        float health;

        float move_speed;

        float move_force;

        uint target; // either next node to walk to or entity to attack now

        ushort state;

        actor(level *, uint type, vec *, vec *);
		actor(level *, uint actor_id, uint type, vec *, vec *);
        virtual ~actor();

        virtual void change_health(float);

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
