#include "include/npc_sv.h"
#include "backends/b_dropsets.h"
#include <sstream>

npc_sv::npc_sv(level_sv* lvl_sv, uint npc_type, vec *pos, vec *pan) : npc(lvl_sv, npc_type, pos, pan)
{

    this->updater = new actor_updater(this);
    this->pathfinder = new path(lvl_sv, this);

    // save position of spawn
    spawn_position.set(&position);

    // TODO: create AI depending on npc values

    std::ostringstream s;
    s << " created NPC, type=" << npc_type << " tilt=" << pan->y << " roll=" << pan->z;

    log(LOG_DEBUG_VERBOSE, s.str().c_str());

    //ctor
}

npc_sv::~npc_sv()
{
	
}

void npc_sv::frame(double time_delta)
{
	// and more
    state_manager(time_delta);
}




bool npc_sv::do_damage(uint target_actor, float damage)
{
   if (!lvl->actorlist.exists(target_actor)) return false;

    actor *ac = lvl->actorlist.elem[target_actor];
    if (ac == NULL) return false;

    //Differentiate for different actor types
    if (ac->type == ACTOR_TYPE_NPC)
    {
        npc_sv *np = (npc_sv *)ac;
        np->change_health(-damage);
        //np->updater->send_health();

        return true;
    }
    if (ac->type == ACTOR_TYPE_PLAYER)
    {
        player_sv *pl = (player_sv *)ac;
        pl->change_health(-get_damage());
        //pl->updater->send_health();

        return true;
    }

    return false;
}
