#include "player.h"

#include "string.h"
#include "helper.h"

player::player(level *lvl, vec *pos, vec *ang, float ahealth,
               const char *name)

               : actor(lvl, ACTOR_TYPE_PLAYER, pos, ang)
{
	this->health = health;
	this->input = 0;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';
}


player::player(level *lvl, 
			   uint actor_id, vec *pos, vec *ang, float health,
               const char *name, int state, int input)

               : actor(lvl, actor_id, ACTOR_TYPE_PLAYER, pos, ang)
{
	this->health = health;

	this->state = state;
	this->input = input;

    strncpy(this->name, name, 32);
    this->name[31] = '\0';

	bb_max.x = 1;
}




player::~player()
{
    //dtor
}



// does movement based in input, same on client and server
void player::movement(float time_step)
{
	if (health > 0.f)
	{		
		// forces to stop movement
		if (move_force.x > 0.f)
		{
			if (move_force.x > actor_friction*time_step) {move_force.x -= actor_friction*time_step;} else {move_force.x = 0.f;}
		}
		if (move_force.x < 0.f)
		{
			if (move_force.x < -actor_friction*time_step) {move_force.x += actor_friction*time_step;} else {move_force.x = 0.f;}
		}
		if (move_force.y > 0.f)
		{
			if (move_force.y > actor_friction*time_step) {move_force.y -= actor_friction*time_step;} else {move_force.y = 0.f;}
		}
		if (move_force.y < 0.f)
		{
			if (move_force.y < -actor_friction*time_step) {move_force.y += actor_friction*time_step;} else {move_force.y = 0.f;}
		}

		// forces to accelerate
		if (input & INPUT_FORW || input & INPUT_BACK || input & INPUT_LEFT || input & INPUT_RIGHT)
		{
			if (state != ST_JUMPING)
			{
				state = ST_WALKING;
				if (input & INPUT_LEFT && !(input & INPUT_RIGHT)) state = ST_WALKING_LEFT;
				if (input & INPUT_RIGHT && !(input & INPUT_LEFT)) state = ST_WALKING_RIGHT;
				
			}
		}
		else {if (state != ST_JUMPING) state = ST_IDLE;}
		
		move_force.x += actor_accel*(input & INPUT_FORW)*time_step;
		move_force.x -= actor_accel*(input & INPUT_BACK)*time_step;
		
		move_force.y += actor_accel*(input & INPUT_LEFT)*time_step;
		move_force.y -= actor_accel*(input & INPUT_RIGHT)*time_step;
		
		// jumping
		if (input & INPUT_JUMP && state != ST_JUMPING)
		{
			state = ST_JUMPING;
			move_force.z += 19;
		}

		// gravity
		move_force.z -= actor_gravity*time_step;
		move_force.z = clamp(move_force.z, -30.f, 30.f);
	
		vec v1(0.f, 0.f, move_force.z*time_step);
		float result = move_rel_col(&v1);
		if (result <= 0)
		{
			move_force.z = 0;
			if (state == ST_JUMPING) state = ST_WALKING;
		}
		
	
		// limit vectors
		if (!(input & INPUT_SPRINT))
		{
			if (input & INPUT_RIGHT || input & INPUT_LEFT) move_force.x = clamp(move_force.x, -6.f, 6.f); 
				else move_force.x = clamp(move_force.x, -13.f, 13.f); 
			move_force.y = clamp(move_force.y, -6.f, 6.f);
		}
		else
		{
			if (input & INPUT_RIGHT || input & INPUT_LEFT) move_force.x = clamp (move_force.x, -9.f, 9.f); 
				else move_force.x = clamp(move_force.x, -19.5f, 19.5f); 
			move_force.y = clamp(move_force.y, -9.f, 9.f);
		}

		vec v2(move_force.x*time_step, move_force.y*time_step, 0.f);
		move_rel_col(&v2);
	}
}