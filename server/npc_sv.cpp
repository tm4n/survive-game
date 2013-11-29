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
    // ai needs to be created last, depends on updater and pathfinder
    this->ai = new npc_ai(this);

    std::ostringstream s;
    s << " created NPC, type=" << npc_type << " tilt=" << pan->y << " roll=" << pan->z;

    log(LOG_DEBUG_VERBOSE, s.str().c_str());

    // create merchant inventory
    int dropset_id = b_np->at(npc_type)->bdropset_id;
    if (faction == 1 && dropset_id > 0)
    {
        printf("creating shop inventory, dropset_id=%i \n", dropset_id);
        inv = new item_cont(PRJ_SHOP_SIZE);

        // fill with items
        b_drp->fill_loot(inv, dropset_id);
    }

    //ctor
}

npc_sv::~npc_sv()
{
    delete(updater);
    delete(pathfinder);
    delete(ai);
    //dtor
}

void npc_sv::reset_position()
{
    vec p;
    level_sv *lvl_sv = (level_sv*)lvl;
    // find unblocked spot
    int nearest_tile = lvl_sv->nearest_free_tile(lvl_sv->world_to_tile(&spawn_position));
    lvl_sv->tile_to_world(nearest_tile, &p);

    position.set(&p);
    current_tile = nearest_tile;
    pathfinder->register_tile();
}

void npc_sv::frame(double time_delta)
{
    ai->frame(time_delta);

    state_manager(time_delta);

    // buffs
    buffl->expire_list(time_delta);
    // lower health and mana on counter
    if (buffl->buff_timer >= 16)
    {
        buffl->buff_timer -= 16;

        float health_change = buffl->get_health_modifier();
        if (health_change != 0)
        {
            change_health(health_change);
            updater->send_health();
        }

        float mana_change = buffl->get_mana_modifier();
        if (mana_change != 0)
        {
            change_mana(mana_change);
            updater->send_mana();
        }
    }
    // change buff values
    move_speed = calc_move_speed();


    updater->update(time_delta);

    // check loot
    if (looter_id != NPC_LOOTER_ID_NOLOOT && looter_id != NPC_LOOTER_ID_NONE)
    {
        if (lvl->actorlist.exists(looter_id))
        {
            actor *ac = lvl->actorlist.at(looter_id);
            vec v;
            lvl->tile_to_world(current_tile, &v);
            if (ac->type != ACTOR_TYPE_PLAYER || ac->position.dist2d(&v) > PRJ_INTERACT_RANGE) loot_disassociate();
        }
        else loot_disassociate();
    }
}

void npc_sv::loot_disassociate()
{
    log(LOG_DEBUG_VERBOSE, "NPC_SV:: disassociating player with loot");
    if (inv->filled == 0) loot_remove();
    else
    {
        looter_id = NPC_LOOTER_ID_NONE;
        updater->send_looter_id();
        // TODO: especially send to looting player of existant
    }
}

void npc_sv::loot_remove()
{
    log (LOG_DEBUG_VERBOSE, "NPC_SV:: removing loot completely");
    looter_id = NPC_LOOTER_ID_NOLOOT;
    updater->send_looter_id();
    delete inv;
    inv = NULL;
}

// override actor set_state!
void npc_sv::set_state(uint new_state)
{
    // disable old state
    switch(state)
    {
        case ACTOR_STATE_DEAD:
        {
            if (looter_id != NPC_LOOTER_ID_NOLOOT) loot_remove();

            break;
        }
    }

    // enable new state
    switch (new_state)
    {

        case ACTOR_STATE_WALKING:
            // send target
            updater->send_target();

        break;


        case ACTOR_STATE_IDLE:

            target = 0;

        break;


        case ACTOR_STATE_DEAD:


            // create loot!
            int dropset_id = b_np->at(npc_type)->bdropset_id;

            if (faction == 2 && dropset_id > 0)
            {
                log(LOG_DEBUG_VERBOSE, "creating loot!");
                inv = new item_cont(PRJ_LOOT_SIZE);

                // fill with loot
                b_drp->fill_loot(inv, dropset_id);

                // if not empty, update to players
                if (inv->filled > 0)
                {
                    looter_id = NPC_LOOTER_ID_NONE;
                    updater->send_looter_id();
                    log(LOG_DEBUG_VERBOSE, "created loot!");
                }
                else
                {
                    log(LOG_DEBUG_VERBOSE, "no loot created!");
                    // no loot generated
                    delete inv;
                    inv = NULL;
                }

            }

        break;


    }

    state = new_state;
    updater->send_state();

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
        np->updater->send_health();

        return true;
    }
    if (ac->type == ACTOR_TYPE_PLAYER)
    {
        player_sv *pl = (player_sv *)ac;
        pl->change_health(-get_damage());
        pl->updater->send_health();

        return true;
    }

    return false;
}
