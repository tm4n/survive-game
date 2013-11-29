#include "include/player_sv.h"
#include "include/net_sv.h"

#include <sstream>

#define PLAYER_AI_STATE_IDLE 1
#define PLAYER_AI_STATE_WALKING 2
#define PLAYER_AI_STATE_PURSUEING 3  // following an enemy or interaction target
#define PLAYER_AI_STATE_PURSUEING_INTERACT 4
#define PLAYER_AI_STATE_ATTACKING 5
//#define PLAYER_AI_STATE_INTERACTING 6


player_sv::player_sv(int zone, int gateway, level_sv *lvl_sv,
               std::list<s_zone> *zonelist, ENetHost *ownhost,
               vec *pos, vec *pan,
               uint account_id,
               ENetPeer *owner,
               uint class_id,
               const char *aname,
               uint level,
               float health,
               float mana,
               uint strength,
               uint agility,
               uint intelligence,
               uint points_remain,
               uint xp,
               uint gold,
               double respawn_timer,
               const char *inv_ch,
               const char *quests_ch,
               const char *skills_ch)
            : player (lvl_sv, pos, pan, class_id, aname, level, strength, agility, intelligence, points_remain, xp, gold, respawn_timer)
{

    this->zone = zone;
    this->gateway = gateway;
    this->zonelist = zonelist;
    this->account_id = account_id;
    this->owner = owner;
    this->ownhost = ownhost;

    this->health = std::min(health, this->max_health);
    this->mana = std::min(mana, this->max_mana);

    this->health_regen_timer = 0;
    this->mana_regen_timer = 0;

    // create inventory
    inv = new inventory(inv_ch, this);

    // create questlog
    quests = new questlog(quests_ch);

    // create spells
    skills = new skill_cont(class_id, PRJ_SKILLS_SIZE, skills_ch);

    // set skillbar as empty
    skbar_ch = NULL;

    this->updater = new actor_updater(this);
    this->pathfinder = new path(lvl_sv, this);

    this->ai_state = PLAYER_AI_STATE_IDLE;

    // register tile (current_tile is set in actor.cpp)
    pathfinder->register_tile();


    std::ostringstream s;
    s << " created Player, actor_id = " << id <<", account=" << account_id <<
        ", name=\"" << name << "\"" <<
        ", class=" << class_id << ", att_range=" << att_range;

    log(LOG_DEBUG_VERBOSE, s.str().c_str());

    log(LOG_DEBUG_VERBOSE, "syncronizing others with this player!");
    net_broadcast_sync_player(id, class_id, aname, ownhost);
}

player_sv::~player_sv()
{
    log(LOG_DEBUG_VERBOSE, "player_sv: deleting player");

    // send deletion
    net_broadcast_remove_player(id, ownhost);

    delete(updater);
    delete(pathfinder);

    delete(quests);
    delete(skills);

    if (skbar_ch != NULL) delete[] skbar_ch;
}


void player_sv::frame(double time_delta)
{
    recalc_values(); // recalc every time to account for buff and inventory changes

    /////////////////////
    // AI part
    if (health > 0)
    {

        if (ai_state == PLAYER_AI_STATE_WALKING)
        {
            vec v;
            lvl->tile_to_world(target, &v);
            if (position.dist2d(&v) < 10)
            {
                if (pathfinder->has_nodes())
                {
                    long tmp_target = pathfinder->next_node();
                    if (tmp_target < 0) set_ai_state(PLAYER_AI_STATE_IDLE); // no new path on block
                    else
                    {
                        target = tmp_target;
                        updater->send_target();
                    }
                }
                else
                {
                    set_ai_state(PLAYER_AI_STATE_IDLE);

                    // check if tile is portal
                        level_sv *lvl_sv = (level_sv*)lvl;
                        // check through gateways list
                        for (std::list<s_gateway>::iterator it = lvl_sv->gateways.begin(); it != lvl_sv->gateways.end(); it++)
                        {
                            if (it->tile == (int)current_tile)
                            {

                                // send connect_to
                                log (LOG_DEBUG, "Hit portal tile, sending connect_to!");
                                printf("Target zone %i, target gateway %i \n", it->to_zone, it->to_id);

                                // getting server data

                                s_zone *z = NULL;
                                for (std::list<s_zone>::iterator zi = zonelist->begin(); zi != zonelist->end(); zi++)
                                {
                                    printf("!Iterating through zone %i \n", zi->id);
                                    if (zi->id == it->to_zone)
                                    {
                                        z = &(*zi);
                                    }
                                }

                                if (z != NULL)
                                {
                                    // save zone
                                    zone = it->to_zone;
                                    gateway = it->to_id;

                                    if (net_send_connect_to(z->ip_address.c_str(), z->port.c_str(), owner) != 0)
                                        log (LOG_ERROR, "ERROR on portal tile: Failed to send redirect message!");
                                }
                                else log (LOG_ERROR, "ERROR on portal tile: Could not find zone data in datalist");
                            }
                    }
                }
            }
        }


        if (ai_state == PLAYER_AI_STATE_PURSUEING || ai_state == PLAYER_AI_STATE_PURSUEING_INTERACT)
        {
            // check if target is valid
            if (!lvl->actorlist.exists(ai_target)
                    || ((ai_state == PLAYER_AI_STATE_PURSUEING)
                        && lvl->actorlist.elem[ai_target]->health <= 0))
            {

                log(LOG_DEBUG, "player_ai: target nonexistent, going back to idle");
                set_ai_state(PLAYER_AI_STATE_IDLE);
            }
        }

        if (ai_state == PLAYER_AI_STATE_PURSUEING || ai_state == PLAYER_AI_STATE_PURSUEING_INTERACT) // still pursueing? enemy is valid now
        {
            // We are walking towards enemy

            // check for targets reached,
            // walk following the path that was set, check for subtargets
            static vec v;
            lvl->tile_to_world(target, &v);
            if (position.dist2d(&v) < 7 || !pathfinder->has_nodes())
            {

                //check if within range of enemy, then we have arrived
                // get position of current enemy
                uint target_tile = lvl->actorlist.elem[ai_target]->current_tile;
                vec target_tile_pos;
                lvl->tile_to_world(target_tile, &target_tile_pos);

                if ((ai_state == PLAYER_AI_STATE_PURSUEING && position.dist2d(&target_tile_pos) <= att_range)
                    || (ai_state == PLAYER_AI_STATE_PURSUEING_INTERACT && position.dist2d(&target_tile_pos) <= PRJ_INTERACT_RANGE))
                {
                    // enemy reached!

                    if (ai_state == PLAYER_AI_STATE_PURSUEING)
                    {
                        // no need to do pathfinding, just attack!
                        log(LOG_DEBUG_VERBOSE, "PLAYER_AI: Going into attack state");
                        set_ai_state(PLAYER_AI_STATE_ATTACKING);
                    }
                    else
                    {
                        // INTERACT!
                        log(LOG_DEBUG_VERBOSE, "PLAYER_AI: Interacting with target now");

                        if (lvl->actorlist.at(ai_target)->type == ACTOR_TYPE_NPC)
                        {
                            npc_sv *np = (npc_sv *)lvl->actorlist.at(ai_target);

                            if (np->looter_id == NPC_LOOTER_ID_NONE)
                            {
                                log(LOG_DEBUG_VERBOSE, "PLAYER_AI: Looting npc");
                                // make me looter
                                np->looter_id = id;
                                np->updater->send_looter_id();
                                // send loot
                                net_send_it_full(ai_target, np->inv, owner);

                            }
                            else
                            {
                                if (np->health > 0)
                                {
                                    std::list<int> q_ids = quests->check_progress_talk(np->npc_type);
                                    for (std::list<int>::iterator it = q_ids.begin(); it != q_ids.end(); it++) net_send_qe_change(quests, *it, owner);

                                    log(LOG_DEBUG_VERBOSE, "PLAYER_AI: talking to npc");

                                    net_send_talk(ai_target, owner);
                                }
                            }
                        }
                        if (lvl->actorlist.at(ai_target)->type == ACTOR_TYPE_CONTAINER)
                        {
                            container_sv *cn = (container_sv *)lvl->actorlist.at(ai_target);

                            if (cn->looter_id == NPC_LOOTER_ID_NONE)
                            {
                                log(LOG_DEBUG_VERBOSE, "PLAYER_AI: Looting container");
                                // make me looter
                                cn->looter_id = id;
                                cn->updater->send_looter_id();
                                // send loot
                                net_send_it_full(ai_target, cn->inv, owner);

                            }
                        }
                        // else TODO

                        set_ai_state(PLAYER_AI_STATE_IDLE);
                    }
                }
                else
                {
                    // not yet in range:
                    // check if a path already exists and if the enemy is still at target position
                    // stop one tile before target
                    if (pathfinder->has_several_nodes() && pathfinder->pos_target == target_tile)
                    {
                        long tmp_target = pathfinder->next_node();
                        if (tmp_target < 0)
                        {
                            set_ai_state(PLAYER_AI_STATE_IDLE);  // blocked, no new path found
                            log (LOG_DEBUG, "PLAYER_AI: pursueing player was blocked, and no new path was found!");
                        }
                        else
                        {
                            target = (uint)tmp_target;
                            updater->send_target();
                        }
                    }
                    else
                    {
                        // find way to use to go to enemy
                        if (!go_to_actor(target_tile))
                        {
                            // to avoid continous pathfinding, set me back to idle state
                            log(LOG_DEBUG, "PLAYER_AI:: target tile seems to be unreachable: canceling pursueing!");
                            set_ai_state(PLAYER_AI_STATE_IDLE);
                        }

                        // target reached, still not in range?
                        //log (LOG_DEBUG, "PLAYER_AI:: reached target, but is still not in range!");
                        //set_ai_state(PLAYER_AI_STATE_IDLE);
                    }
                }
            }

        }


        if (ai_state == PLAYER_AI_STATE_ATTACKING)
        {
            if (state == ACTOR_STATE_ATTACKING)
            {
                //bprintf("currently attacking: %lf\n", me->timer_attack);
                // currently attacking, don't leave the state, do nothing,
                // just look for timer

                // do damage on timer!
                if (damage_done == false && timer_attack >= 50.)
                {
                    damage_done = true;
                    if (lvl->actorlist.elem[ai_target] != NULL)
                    {
                        do_damage(ai_target, get_damage());
                        attract_enemies();
                    }
                    else log(LOG_ERROR, "Player tried to do damage but target was gone??");
                }

                // timer done!
                if (timer_attack >= 100)
                {
                    timer_attack -= 100;
                    set_state(ACTOR_STATE_ATTACK_PAUSE);
                }
            }
            else
            {

                // get position of current enemy
                vec target_tile_pos;
                lvl->tile_to_world(lvl->actorlist.elem[ai_target]->current_tile, &target_tile_pos);

                // check if still within attack range and valid target
                if (lvl->actorlist.elem[ai_target] != NULL
                    && position.dist2d(&target_tile_pos) <= att_range
                    && lvl->actorlist.elem[ai_target]->health > 0)
                {
                    if (timer_attack_cooldown <= 0)
                    {
                        //First time
                        //Start attack
                        if (state != ACTOR_STATE_ATTACKING && state != ACTOR_STATE_ATTACK_PAUSE)
                        {
                            target = ai_target;
                            updater->send_target();
                            // set cooldown
                            timer_attack_cooldown = att_cooldown;
                            set_state(ACTOR_STATE_ATTACKING);
                            log(LOG_DEBUG, "player: starting single attack!");
                        }
                        else
                        {
                            // player specialty: only attack once!
                            log (LOG_DEBUG_VERBOSE, "Already attacked before, done!");
                            set_ai_state(ACTOR_STATE_IDLE);

                        }

                    }
                    else
                    {
                        //printf("cooldown happening: %lf\n", me->timer_attack_cooldown);
                        // cooldown happening.. can't attack again.. just wait..
                    }
                }
                else
                {
                    log(LOG_DEBUG, "player: enemy out of range or nonexistent, leaving attack mode");
                    set_ai_state(NPC_AI_STATE_IDLE);
                }
            }


        }
    }
    else
    {
        ///////////////////////////////////////////////
        // player is dead
        if (respawn_timer == 0)
        {
            // player just died
            respawn_timer = PRJ_RESPAWN_TIME; // set respawn timer

            set_state(ACTOR_STATE_DEAD);
        }
        else
        {
            if (respawn_timer > 0) respawn_timer -= time_delta/16.;
            else
            {
                level_sv *lvl_sv = (level_sv*)lvl;
                s_gateway *gt = lvl_sv->get_gateway(gateway);

                if (gt != NULL)
                {
                    vec vpos;
                    int starttile = lvl_sv->nearest_free_other_tile(gt->tile);
                    lvl_sv->tile_to_world(starttile, &vpos);


                    // reset to position
                    pathfinder->unregister_tile();
                    current_tile = starttile;
                    pathfinder->register_tile();

                    position.set(&vpos);
                    updater->send_pos();
                }
                else log (LOG_ERROR, "ERROR on respawning player: did not find player gateway_id in level");

                // reset player data
                health = max_health/2;
                updater->send_health();
                mana = max_mana/2;
                updater->send_mana();

                set_state(ACTOR_STATE_IDLE);

                respawn_timer = 0;
            }
        }
    }
    ///////////////////////////////////////////////////////////
    // AI end here


    // level up!
    if (xp >= calc_next_xp())
    {
        xp -= calc_next_xp();
        chlevel++;
        // send xp (only to owner)
        net_send_update_xp(id, xp, id, owner);
        updater->send_level();

        // give a new skillpoint
        points_remain++;
        net_send_update_points_remain(id, points_remain, owner);

        // for this version: add fixed values, later: either from b_classes or by user input
        strength += 2;
        net_send_update_strength(id, strength, owner);
        intelligence += 1;
        net_send_update_intelligence(id, intelligence, owner);
        agility += 1;
        net_send_update_agility(id, agility, owner);
    }


    // Usual stuff
    state_manager(time_delta);

    if (health > 0)
    {

        // skills
        skill_cooldown_frame(time_delta);

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

                if (health <= 0 && health_change < 0)
                {
                    // give XP to killer
                }
            }

            float mana_change = buffl->get_mana_modifier();
            if (mana_change != 0)
            {
                change_mana(mana_change);
                updater->send_mana();
            }
        }

        // fixed regeneration rate
        health_regen_timer += time_delta*max_health*0.05;
        if (health_regen_timer > 100)
        {
            health_regen_timer -= 100;
            change_health(1);
            updater->send_health();
        }
        mana_regen_timer += time_delta*max_mana*0.1;
        if (mana_regen_timer > 100)
        {
            mana_regen_timer -= 100;
            change_mana(1);
            updater->send_mana();
        }
    }

    updater->update(time_delta);
}



bool player_sv::go_to_actor(uint enemy_tile)
{
    log(LOG_DEBUG_VERBOSE, "PLAYER_AI::called go_to_actor");
    // find path
    if (pathfinder->find_path(current_tile, enemy_tile) == true)
    {
        // set first move target
        if (pathfinder->has_several_nodes())
        {
            long tmp_target = pathfinder->next_node();
            if (tmp_target < 0) log (LOG_DEBUG, "player_ai::go_to_actor: pathfinding gave blocked node!");
            else
            {
                target = (uint)tmp_target;
                // transfer move target by setting state_walking
                set_state(ACTOR_STATE_WALKING);

                return true;

            }
        }
        else
        {
            log(LOG_DEBUG, "player_ai::go_to_actor: already in good spot!");
            return true;
        }

    }
    else
    {
        log(LOG_DEBUG, "player_ai::frame: target enemy tile was not reachable!");
        set_ai_state(PLAYER_AI_STATE_IDLE);
    }

    return false;
}

void player_sv::set_state(uint new_state)
{

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

            target = 0;

        break;


    }

    state = new_state;
    updater->send_state();

}


void player_sv::set_ai_state(uint new_ai_state)
{
    // 1. disable old states
    switch(state)
    {
        case PLAYER_AI_STATE_WALKING:
        {

            //ai_target = 0;

            break;
        }

    }

        // enable new state
    switch (new_ai_state)
    {

        case PLAYER_AI_STATE_WALKING:
        {
            set_state(ACTOR_STATE_WALKING);

            // send target
            updater->send_target();

            break;
        }

        case PLAYER_AI_STATE_IDLE:
        {
            set_state(ACTOR_STATE_IDLE);
            pathfinder->clear();

            target = 0;

            break;
        }

        case PLAYER_AI_STATE_ATTACKING:
        {

            pathfinder->clear();

            damage_done = false;

            break;
        }


    }

    ai_state = new_ai_state;
}


bool player_sv::do_damage(uint target_actor, float damage)
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

        // give XP, check quest progress
        if (np->health <= 0)
        {
            give_xp(b_np->at(np->npc_type)->bbounty_xp, target_actor);
            give_gold(target_actor);

            std::list<int> q_ids = quests->check_progress_kill(np->npc_type);
            for (std::list<int>::iterator it = q_ids.begin(); it != q_ids.end(); it++) net_send_qe_change(quests, *it, owner);
        }

        return true;
    }
    if (ac->type == ACTOR_TYPE_PLAYER)
    {
        player_sv *pl = (player_sv *)ac;
        pl->change_health(-get_damage());
        pl->updater->send_health();

        // TODO: XP?

        return true;
    }

    return false;
}

void player_sv::attract_enemies()
{
    level_sv *lvl_sv = (level_sv*)lvl;

    // go through near enemies
    std::list<uint> l;
    lvl_sv->get_actors_within(&l, &position, 320);

    for (std::list<uint>::iterator ui = l.begin(); ui != l.end(); ui++)
    {
        if (lvl_sv->actorlist.exists(*ui))
        {
            actor *ac = lvl_sv->actorlist.at(*ui);

            if (ac->type == ACTOR_TYPE_NPC)
            {
                npc_sv *np = (npc_sv*)ac;

                if (np->state == ACTOR_STATE_IDLE && np->faction != this->faction && np->att_dmg_min > 0)
                {
                    std::cout << "attracked enemy: " << *ui << "\n";
                    np->ai->ai_target = id;
                    np->ai->set_ai_state(NPC_AI_STATE_PURSUEING);
                }
            }
        }
    }

}



void player_sv::give_xp(int amount, uint giver_id)
{
    xp += amount;
    net_send_update_xp(id, xp, giver_id, owner);
}

void player_sv::give_gold(int amount, uint giver_id)
{
    gold += amount;
    net_send_update_gold(id, gold, giver_id, owner);
}

void player_sv::give_gold(uint giver_id)
{
    actor* ac_giver = lvl->actorlist.at(giver_id);

    if (ac_giver->type == ACTOR_TYPE_NPC)
    {
        npc_sv *npc_giver = (npc_sv*)ac_giver;

        s_npcs *nd = b_np->at(npc_giver->npc_type);
        if (nd == NULL) log(LOG_ERROR, "FATAL ERROR in player_sv::give_gold: giver npc had invalid npc_type");

        // see if any gold is given
        if (nd->bbounty_gold_probability < (float)random()/(float)RAND_MAX)
        {
            // calculate how much is given from giver

            int amount = nd->bbounty_gold_min + (random() % nd->bbounty_gold_max);

            give_gold(amount, giver_id);
        }
    }
}

void player_sv::recalc_equip()
{
    int eq_new;

    for (int i = 0; i < PRJ_INV_EQUIPMENT; i++)
    {
        item *it = ((inventory*)inv)->at((PRJ_INV_SIZE - PRJ_INV_EQUIPMENT)+i);
        if (it == NULL) eq_new = 0;
        else eq_new = it->id;

        // send if any chance happend
        if (eq_new != equip[i])
        {
            equip[i] = eq_new;
            updater->send_equip((char)i, eq_new);
        }
    }
}

void player_sv::order_move(uint target_tile)
{
    log(LOG_DEBUG_VERBOSE, "player: movement order received");

    if (pathfinder->pos_target == target_tile && state == ACTOR_STATE_WALKING) return; // same position

    // get true current tile
    uint true_current_tile = lvl->world_to_tile(&position);


    if (pathfinder->find_path(true_current_tile, target_tile) == true)
    {

        // set first move target
        long tmp_target = pathfinder->next_node();
        if (tmp_target < 0) log (LOG_DEBUG, "order_move: gave blocked/invalid node!");
        else
        {

            target = tmp_target;

            ai_target = target_tile;

            set_ai_state(PLAYER_AI_STATE_WALKING);

        }

    }
    else log(LOG_DEBUG, "player: order move tile was not reachable");
}


void player_sv::order_attack(uint target_actor)
{
    log(LOG_DEBUG_VERBOSE, "player: attack order received");

    // TODO: another sanity check

    ai_target = target_actor;

    if (ai_state != PLAYER_AI_STATE_ATTACKING) set_ai_state(PLAYER_AI_STATE_PURSUEING);

}

void player_sv::order_interact(uint target_actor)
{
    log(LOG_DEBUG_VERBOSE, "player: interact order received");

    // TODO: another sanity check

    ai_target = target_actor;

    set_ai_state(PLAYER_AI_STATE_PURSUEING_INTERACT);

}
