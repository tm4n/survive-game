#include "npc_cl.h"

npc_cl::npc_cl(level *lvl, uint actor_id, int npc_type, vec *pos, vec*ang, float health, int target, gameRenderer *arenderer)
	: npc(lvl, actor_id, npc_type, pos, ang, health, target)
{
	renderer = arenderer;

	ro = new RenderObject();
        	
    ro->translation[0] = pos->x;
    ro->translation[1] = pos->y;
	ro->translation[2] = pos->z;
	ro->rotation[0] = ang->x;
	ro->rotation[1] = ang->y;
	ro->rotation[2] = ang->z;
    
	getMesh()->addRenderObject(ro);
    
}


npc_cl::~npc_cl()
{
	getMesh()->removeRenderObject(ro);

	delete ro;
}




void npc_cl::frame(double time_delta)
{
	//if (state == ST_WALKING && rnd(4000) == 0) snd_taunt();

	movement(time_delta);

	animate(time_delta);
	
	ro->translation[0] = position.x; ro->translation[1] = position.y; ro->translation[2] = position.z; 
	ro->rotation[0] = angle.x; ro->rotation[1] = angle.y; ro->rotation[2] = angle.z; 
}
 

void npc_cl::animate(double time_delta)
{
	if (old_anim != state) {anim_prog = 0; old_anim = state;}
		
	float anim_speed = get_anim_speed();

	if (state == ST_IDLE)
	{
		getMesh()->animate(ro, "stand", anim_prog, 1); 
		anim_prog += get_move_speed()/4.f*(float)time_delta*anim_speed;
	}

	if (state == ST_WALKING || state == ST_FL_ASC)
	{
		if (get_move_speed() < 7) {getMesh()->animate(ro, "walk", anim_prog, 1);}
		else {getMesh()->animate(ro, "run", anim_prog, 1);}
		anim_prog += get_move_speed()*1.2f*(float)time_delta*anim_speed;
	}
		
	if (state == ST_JUMPING)
	{
		getMesh()->animate(ro, "jump", anim_prog, 0);
		anim_prog += get_move_speed()*2.f*(float)time_delta*anim_speed;
	}

	if (state == ST_ATTACK)
	{
		getMesh()->animate(ro, "attack", anim_prog, 0);
		anim_prog += get_move_speed()*2.f*(float)time_delta*anim_speed;
	}
		
	if (state == ST_FL_DESC)
	{
		//if (anim_prog == 0) snd_taunt();
		getMesh()->animate(ro, "run", anim_prog, 1);
		anim_prog += get_move_speed()*2.f*(float)time_delta*anim_speed;
	}
	if (state == ST_FL_FALLING)
	{
		getMesh()->animate(ro, "fall", anim_prog, 0);
		anim_prog += get_move_speed()*(float)time_delta*anim_speed;
	}

	if (state == ST_DEATH)
	{
		getMesh()->animate (ro, "death", anim_prog, 0);
		if (get_ai_type() == NPC_AI_PLAYER_FLYING) anim_prog += 17.f*(float)time_delta*anim_speed; else anim_prog += 5.f*(float)time_delta*anim_speed;
		return;
	}

	if (anim_prog > 100.f && (state == 1 || state == 2)) {anim_prog -= 100.f;}
}