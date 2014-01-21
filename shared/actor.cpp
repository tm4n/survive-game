#include "actor.h"
#include <cmath>
#include <algorithm>
#include "defs.h"
#include "stdio.h"


using namespace std;

const float actor::actor_step = 10;
const float actor::actor_gravity = 3;
const float actor::actor_friction = 6;
const float actor::actor_accel = 2;

actor::actor(level* lvl, uint type, vec *pos, vec *pan)
{
    // register in actorlist
    id = lvl->actorlist.add(this);

	printf("freely created actor (type %u) at id=%u \n", type, id);

    this->lvl = lvl;
    this->type = type;

    if (pos != NULL) move(pos); else position.zero();
    if (pan != NULL) turn(pan); else angle.zero();

    move_force = 0;
	move_speed = 1; // default move speed
	state = 0;
}

actor::actor(level* lvl, uint actor_id, uint type, vec *pos, vec *pan)
{
	printf("creating actor (type = %i) with id=%i fixed \n", type, actor_id);

    // register in actorlist at fixed position
    id = lvl->actorlist.add_at(this, actor_id);

    this->lvl = lvl;
    this->type = type;

    if (pos != NULL) move(pos); else position.zero();
    if (pan != NULL) turn(pan); else angle.zero();
    
	move_force = 0;
	move_speed = 1; // default move speed
	state = 0;
}

actor::~actor()
{
    log(LOG_DEBUG_VERBOSE, "actor: deleting from serverlist");
    // remove me from actorlist
    lvl->actorlist.remove(id);
}

/////////////////////////////////////////////////////////
// stats

void actor::change_health(float change)
{
    health += change;
    health = min(health, max_health);
    health = max(health, 0.f);
}

/////////////////////////////////////////////////////////
// movement

void actor::move(vec *newpos)
{
    position.set(newpos);
}

void actor::move_rel(vec *force)
{
    position.x += force->x * (float)cos(angle.x*M_PI/180.);
    position.y += force->x * (float)sin(angle.x*M_PI/180.);
}

float actor::move_rel_col(vec *reldir)
{
	vec ch_old_pos, col_move_world, col_move_border_min, col_move_border_max, my_max, my_min;
	actor *col_move_nearest[3];
	ch_old_pos.set(&position);
	
	// convert to world space
	col_move_world.x = cos(angle.x)*reldir->x + cos(angle.x+90.f)*reldir->y;
	col_move_world.y = sin(angle.x)*reldir->x + sin(angle.x+90.f)*reldir->y;
	col_move_world.z = reldir->z;
	
	col_move_border_min.set(lvl->border_min, lvl->border_min, lvl->border_ground);
	col_move_border_max.set(lvl->border_max, lvl->border_max, lvl->border_height);
	col_move_nearest[0] = NULL; col_move_nearest[1] = NULL; col_move_nearest[2] = NULL;
	
	// precalculate my hull
	my_max.x = position.x + bb_max.x;
	my_max.y = position.y + bb_max.y;
	my_max.z = position.z + bb_max.z;
	
	my_min.x = position.x + bb_min.x;
	my_min.y = position.y + bb_min.y;
	my_min.z = position.z + bb_min.z;
	
	//////////////////////////
	// do z collision
	if (col_move_world.z != 0)
	{
		for (uint i = 0; i < lvl->actorlist.size; i++)
		{
			actor *ac = lvl->actorlist.at(i);
			if (ac != NULL)
			{
				if (ac->health > 0.f)
				{	
					if ((my_min.x < ac->position.x + ac->bb_max.x && my_max.x > ac->position.x + ac->bb_min.x) // collision in x plane
						&& (my_min.y < ac->position.y + ac->bb_max.y && my_max.y > ac->position.y + ac->bb_min.y)) // collision in y plane
					{
						if (col_move_world.z < 0 && ac->position.z+ac->bb_max.z > col_move_border_min.z && ac->position.z+ac->bb_max.z <= position.z+bb_min.z)
						{
							col_move_border_min.z = ac->position.z + ac->bb_max.z;
							col_move_nearest[2] = ac;
						}
					
						if (col_move_world.z > 0 && ac->position.z+ac->bb_min.z < col_move_border_max.z && ac->position.z+ac->bb_min.z >= position.z+bb_max.z)
						{
							col_move_border_max.z = ac->position.z + ac->bb_min.z;
							col_move_nearest[2] = ac;
						}
					}
				}
			}
		}
		if (col_move_world.z < 0) 
			if (col_move_border_min.z-bb_min.z > position.z + col_move_world.z) position.z = col_move_border_min.z-bb_min.z;
				else {position.z += col_move_world.z; col_move_nearest[2] = NULL;} 
				
		if (col_move_world.z > 0) 
			if (col_move_border_max.z-bb_max.z < position.z + col_move_world.z) position.z = col_move_border_max.z-bb_max.z;
				else {position.z += col_move_world.z; col_move_nearest[2] = NULL;} 
		
		//if (col_move_world.z < 0) my.z = maxv(col_move_border_min.z-my.min_z, my.z + col_move_world.z);
		//if (col_move_world.z > 0) my.z = minv(col_move_border_max.z-my.max_z, my.z + col_move_world.z);
	}
	
	// do x collision
	if (col_move_world.x != 0)
	{
		for (uint i = 0; i < lvl->actorlist.size; i++)
		{
			actor *ac = lvl->actorlist.at(i);
			if (ac != NULL)
			{
				if (ac->health > 0.f)
				{	
					if ((my_min.z < ac->position.z + ac->bb_max.z && my_max.z > ac->position.z + ac->bb_min.z) // collision in t plane
						&& (my_min.y < ac->position.y + ac->bb_max.y && my_max.y > ac->position.y + ac->bb_min.y)) // collision in y plane
					{
						if (col_move_world.x < 0 && ac->position.x+ac->bb_max.x > col_move_border_min.x && ac->position.x+ac->bb_max.x <= position.x+bb_min.x)
						{
							col_move_border_min.x = ac->position.x + ac->bb_max.x;
							col_move_nearest[0] = ac;
						}
					
						if (col_move_world.x > 0 && ac->position.x+ac->bb_min.x < col_move_border_max.x && ac->position.x+ac->bb_min.x >= position.x+bb_max.x)
						{
							col_move_border_max.x = ac->position.x + ac->bb_min.x;
							col_move_nearest[0] = ac;
						}
					}
				}
			}
		}
		if (col_move_world.x < 0) 
			if (col_move_border_min.x-bb_min.x > position.x + col_move_world.x) position.x = col_move_border_min.x-bb_min.x;
				else {position.x += col_move_world.x; col_move_nearest[0] = NULL;} 
				
		if (col_move_world.x > 0) 
			if (col_move_border_max.x-bb_max.x < position.x + col_move_world.x) position.x = col_move_border_max.x-bb_max.x;
				else {position.x += col_move_world.x; col_move_nearest[0] = NULL;} 
		
		//if (col_move_world.x < 0) my.x = maxv(col_move_border_min.x-my.min_x, my.x + col_move_world.x);
		//if (col_move_world.x > 0) my.x = minv(col_move_border_max.x-my.max_x, my.x + col_move_world.x);
	}
	
	// y collision
	if (col_move_world.y != 0)
	{
		for (uint i = 0; i < lvl->actorlist.size; i++)
		{
			actor *ac = lvl->actorlist.at(i);
			if (ac != NULL)
			{
				if (ac->health > 0.f)
				{	
					if ((my_min.z < ac->position.z + ac->bb_max.z && my_max.z > ac->position.z + ac->bb_min.z) // collision in z plane
						&& (my_min.x < ac->position.x + ac->bb_max.x && my_max.x > ac->position.x + ac->bb_min.x)) // collision in x plane
					{
						if (col_move_world.y < 0 && ac->position.y+ac->bb_max.y > col_move_border_min.y && ac->position.y+ac->bb_max.y <= position.y+bb_min.y)
						{
							col_move_border_min.y = ac->position.y + ac->bb_max.y;
							col_move_nearest[1] = ac;
						}
					
						if (col_move_world.y > 0 && ac->position.y+ac->bb_min.y < col_move_border_max.y && ac->position.y+ac->bb_min.y >= position.y+bb_max.y)
						{
							col_move_border_max.y = ac->position.y + ac->bb_min.y;
							col_move_nearest[1] = ac;
						}
					}
				}
			}
		}
		if (col_move_world.y < 0) 
			if (col_move_border_min.y-bb_min.y > position.y + col_move_world.y) position.y = col_move_border_min.y-bb_min.y;
				else {position.y += col_move_world.y; col_move_nearest[1] = NULL;} 
				
		if (col_move_world.y > 0) 
			if (col_move_border_max.y-bb_max.y < position.y + col_move_world.y) position.y = col_move_border_max.y-bb_max.y;
				else {position.y += col_move_world.y; col_move_nearest[1] = NULL;} 
		
		//if (col_move_world.y < 0) my.y = maxv(col_move_border_min.y-my.min_y, my.y + col_move_world.y);
		//if (col_move_world.y > 0) my.y = minv(col_move_border_max.y-my.max_y, my.y + col_move_world.y);
	}
	
	float ch_moved_dist = position.dist(&ch_old_pos);
	
	/*if (my.emask & ENABLE_ENTITY)
	{
		// start event
		ent = NULL;
		if (col_move_nearest[2] != NULL) ent = col_move_nearest[2];
		if (col_move_nearest[1] != NULL) ent = col_move_nearest[1];
		if (col_move_nearest[0] != NULL) ent = col_move_nearest[0];
		
		if (ent != NULL && my->event != NULL)
		{
			ch_event_type = EVENT_ENTITY;
			fnc_event_prototype = my->event;
			fnc_event_prototype(ent);
		}
	}*/
	
	return ch_moved_dist;
}
void actor::turn(vec *newpan)
{
    angle.set(newpan);
}

void actor::turn_to(vec *pos, double time_delta)
{
    float ang_pan = 90.0f - vec::angle(atan2(pos->x - position.x, pos->y - position.y))*(float)(180.0/M_PI);

    angle.x += vec::angle(vec::angle(ang_pan) - vec::angle(angle.x))*(float)(0.5*time_delta);

    // old code
	/*float n = pos->y - position.y;
	if (n == 0) n += 0.0001;
    angle.x = atan((pos->x - position.x)/(n));
	while (angle.x >= 360) angle.x -= 360;
	while (angle.x < 0) angle.x += 360;*/
}


/////////////////////////////////////////////////////////
// statemanager
void actor::set_state(uint new_state)
{
    // WARNING: this code is overwritten on server and client!

	state = new_state;
}

void actor::state_manager(double time_delta)
{
	// WARNING: this code may be overwritten on server and client!

}

// FRAME
void actor::frame(double time_delta)
{
	// WARNING: this code is overwritten on server and client!
}
