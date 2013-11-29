#include "include/npc_ai.h"

npc_ai::npc_ai(npc_sv *me)
{
    //ctor
    this->me = me;

    me->reset_position();
    me->updater->send_pos();

    ai_state = NPC_AI_STATE_IDLE;

    timer_finder = rand() % TICK_FIND_TARGET_LIMIT;
}

npc_ai::~npc_ai()
{
    //dtor
}

// static variables
float npc_ai::enemy_detection_range = 100;
float npc_ai::cancel_pursue_range = 300;
float npc_ai::stride_probability = 10;
float npc_ai::stride_max_range = 100;


//////////////////////
// frame is called every frame to update AI
void npc_ai::frame(double time_delta)
{
    // increment timer
    timer_finder += time_delta;

    //////////////////////////////////
    // AI, simple statemanager

    // check for death
    if (ai_state != NPC_AI_STATE_DEAD && me->health <= 0)
    {
        set_ai_state(NPC_AI_STATE_DEAD);
        me->set_state(ACTOR_STATE_DEAD);

        // make passable
        me->pathfinder->unregister_tile();

        //reset counter
        timer_respawn = 0;
    }

    if (ai_state == NPC_AI_STATE_DEAD)
    {
        // increment counter
        timer_respawn += time_delta;

        if (timer_respawn >= 360. *16.)
        {
            log(LOG_DEBUG_VERBOSE, "respawning npc");

            // reset position
            me->reset_position();
            me->updater->send_pos();

            me->init_values();
            // send changed values
            me->updater->send_health();

            me->set_state(ACTOR_STATE_IDLE);
            set_ai_state(NPC_AI_STATE_IDLE);

        }
    }

    // check for a new target
    if (ai_state == NPC_AI_STATE_STRIDING || ai_state == NPC_AI_STATE_IDLE)
    {

        if (timer_finder >= TICK_FIND_TARGET_LIMIT)
        {
            timer_finder -= TICK_FIND_TARGET_LIMIT;

            // try to find a target!
            long result = find_best_victim();
            if (result >= 0)
            {
                //if (go_to_actor((uint)result)) // TODO: check if path is working already
                {
                    ai_target = (uint)result;
                    set_ai_state(NPC_AI_STATE_PURSUEING);
                    log(LOG_DEBUG, "FOUND AN REACHABLE ENEMY; GOING FOR IT NOW!");
                }
            }
        }


    }

    // currently idleing
    if (ai_state == NPC_AI_STATE_IDLE)
    {
        // stride back to spawn
        if(me->position.dist2d(&me->spawn_position) > stride_max_range)
        {
            if (rand() % 3000 < stride_probability)
            {
                int sp = ((level_sv*)(me->lvl))->nearest_free_tile(me->lvl->world_to_tile(&me->spawn_position));
                if (go_to_tile(sp)) set_ai_state(NPC_AI_STATE_STRIDING);
            }
        }
        else
        {
            // stride around randomly
            // TODO: make stride independent of frame rate
            if (me->faction == 2 && rand() % 5000 < stride_probability)
            {
                //log (LOG_DEBUG, "NPC_AI: Trying to stride!");

                // find a suitable stride target, go there!

                long target_tile = find_stride_target();
                if (target_tile >= 0)
                {
                    // find path
                    if (go_to_tile((uint)target_tile)) set_ai_state(NPC_AI_STATE_STRIDING);
                }
            }
        }

    }

    // currently striding
    if (ai_state == NPC_AI_STATE_STRIDING)
    {

        // walk following the path that was set
        vec v;
        me->lvl->tile_to_world(me->target, &v);
        if (me->position.dist2d(&v) < 10)
        {
            if (me->pathfinder->has_nodes())
            {
                long tmp_target = me->pathfinder->next_node();
                if (tmp_target < 0)
                {
                    set_ai_state(NPC_AI_STATE_IDLE);  // blocked, no new path found
                }
                else
                {
                    me->target = tmp_target;
                    me->updater->send_target();
                }
            }
            else
            {
                set_ai_state(NPC_AI_STATE_IDLE);  // reached path
            }
        }
    }


    if (ai_state == NPC_AI_STATE_PURSUEING)
    {
        // check if enemy is still with targetting
        // see if target is still valid and in range etc
        if (decide_cancel_enemy())
        {
            // cancel attack
            log(LOG_DEBUG, "NPC: canceling attack!");
            set_ai_state(NPC_AI_STATE_IDLE);
        }
        else
        {
            //TODO: AGGRO, see if better enemy is there
        }



        if (ai_state == NPC_AI_STATE_PURSUEING) // still pursueing? enemy is valid now
        {
            // We are walking towards enemy


            // check for targets reached,
            // walk following the path that was set, check for subtargets
            static vec v;
            me->lvl->tile_to_world(me->target, &v);
            if (me->position.dist2d(&v) < 7 || !me->pathfinder->has_nodes())
            {

                // get position of current enemy
                uint enemy_tile = me->lvl->actorlist.elem[ai_target]->current_tile;
                vec enemy_tile_pos;
                me->lvl->tile_to_world(enemy_tile, &enemy_tile_pos);

                //check if within range of enemy, then we have arrived
                if (me->position.dist2d(&enemy_tile_pos) <= me->att_range)
                {
                    // switch into AI attack state:
                    set_ai_state(NPC_AI_STATE_ATTACKING);
                }
                else
                {


                    // check if a path already exists and if the enemy is still at target position
                    if (me->pathfinder->has_several_nodes() && me->pathfinder->pos_target == enemy_tile)
                    {
                        long tmp_target = me->pathfinder->next_node();
                        if (tmp_target < 0)
                        {
                            log (LOG_DEBUG, "NPC_AI: pursueing npc was blocked, and no new path was found!");
                            timer_finder = -TICK_FIND_TARGET_LIMIT;
                            set_ai_state(NPC_AI_STATE_IDLE);  // blocked, no new path found
                        }
                        else
                        {
                            me->target = (uint)tmp_target;
                            me->updater->send_target();
                        }

                    }
                    else
                    {
                         // find way to use to go to enemy
                        if (!go_to_actor(ai_target))
                        {
                            // to avoid continous pathfinding, set me back to idle state
                            log(LOG_DEBUG, "NPC_AI:: enemy tile seems to be unreachable: canceling attack!");
                            timer_finder = -TICK_FIND_TARGET_LIMIT;
                            set_ai_state(NPC_AI_STATE_IDLE);
                        }
                    }
                }
            }

        }


    }


    if (ai_state == NPC_AI_STATE_ATTACKING)
    {
        if (me->state == ACTOR_STATE_ATTACKING)
        {
            //printf("currently attacking: %lf\n", me->timer_attack);
            // currently attacking, don't leave the state, do nothing,
            // just look for timer

            // do damage on timer!
            if (damage_done == false && me->timer_attack >= 50.)
            {
                damage_done = true;
                if (me->lvl->actorlist.elem[ai_target] != NULL)
                {
                    me->do_damage(ai_target, me->get_damage());
                }
                else log(LOG_ERROR, "Player tried to do damage but target was gone??");
            }

            // timer done!
            if (me->timer_attack >= 100.)
            {
                me->timer_attack -= 100;
                me->set_state(ACTOR_STATE_ATTACK_PAUSE);
                //printf("STATE set to ACTOR_STATE_ATTACK_PAUSE");
            }
        }
        else
        {
            damage_done = false;
            // check if enemy is still with targetting
            // see if target is still valid and in visibility range
            if (decide_cancel_enemy())
            {
                // cancel attack
                set_ai_state(NPC_AI_STATE_IDLE);
                log(LOG_DEBUG, "npc: decided to 'cancel enemy', leaving attack mode");
            }
            else
            {
                // check if still within attack range
                if (me->position.dist2d(&me->lvl->actorlist.elem[ai_target]->position) <= me->att_range)
                {
                    if (me->timer_attack_cooldown <= 0.)
                    {
                        //First time, or last attack done!
                        //Start attack
                        // set cooldown
                        me->timer_attack_cooldown = me->att_cooldown;
                        me->set_state(ACTOR_STATE_ATTACKING);
                        log(LOG_DEBUG, "npc: starting single attack!");

                        if (me->target != ai_target)
                        {
                            me->target = ai_target;
                            me->updater->send_target();
                        }
                        //printf("timer_attack_cooldown set to %f, timer_attack is %f", me->timer_attack_cooldown, me->timer_attack);

                    }
                    else
                    {
                        //printf("cooldown happening: %lf\n", me->timer_attack_cooldown);
                        // cooldown happening.. can't attack again.. just wait..
                    }
                }
                else
                {
                    log(LOG_DEBUG, "npc: enemy out of range, leaving attack mode");
                    set_ai_state(NPC_AI_STATE_PURSUEING);
                }
            }
        }


    }

}


void npc_ai::set_ai_state(uint new_ai_state)
{
    // first: reset old state data
    switch (ai_state)
    {

    }

    // second: set new state data
    switch(new_ai_state)
    {
        case NPC_AI_STATE_STRIDING:
        {
            // get path
            // set actor states
            me->set_state(ACTOR_STATE_WALKING);

            break;
        }

        case NPC_AI_STATE_IDLE:
        {
            // set fitting actor state
            me->set_state(ACTOR_STATE_IDLE);
            me->pathfinder->clear();
            ai_target = 0;

            break;

        }

        case NPC_AI_STATE_ATTACKING:
        {
            log(LOG_DEBUG_VERBOSE, "Switching into attack mode!");
            me->pathfinder->clear();

            damage_done = false;

            break;
        }

    }

    ai_state = new_ai_state;
}


//////////////////////////////
// return the best victim
// in standard AI its determined by range only within range, none if any
// TODO: AGGRO value!

long npc_ai::find_best_victim()
{

    double min_dist = 99999999; long min_id = -1;

    // loop though all actors
    for (uint i = 0; i < me->lvl->actorlist.size; i++)
    {

        if (me->lvl->actorlist.elem[i] != NULL
            && is_enemy(me->lvl->actorlist.elem[i])
            && me->lvl->actorlist.elem[i]->health > 0
            && me->lvl->actorlist.elem[i]->faction != me->faction)
        {
            // is possible target, check distance
            double dist = me->position.dist2d(&me->lvl->actorlist.elem[i]->position);
            if (dist < min_dist && dist <= me->att_range + enemy_detection_range)
            {
                min_dist = dist;
                min_id = i;
            }

        }

    }

    return min_id;
}

bool npc_ai::go_to_tile(uint ttile)
{
    // find path
    if (me->pathfinder->find_path(me->current_tile, ttile) == true)
    {
        // set first move target
        long tmp_target = me->pathfinder->next_node();
        if (tmp_target < 0) log (LOG_DEBUG, "npc_ai::go_to_tile: pathfinder gave blocked node!");
        else
        {
            ai_target = (uint)ttile;
            me->target = tmp_target;
            // transfer move target by setting state_walking
            me->set_state(ACTOR_STATE_WALKING);
            return true;

        }

    }
    else log(LOG_DEBUG, "npc_ai::go_to_tile: tile was not reachable");

    return false;
}

bool npc_ai::go_to_actor(uint enemy_id)
{
    log(LOG_DEBUG, "NPC_AI::called go_to_actor");

    // get actor
    if (!me->lvl->actorlist.exists(enemy_id)) {log(LOG_ERROR, "ERROR: npc_ai::go_to_actor called with invalid id!"); return false;}
    uint enemy_tile = me->lvl->actorlist.at(enemy_id)->current_tile;

    // find path
    if (me->pathfinder->find_path(me->current_tile, enemy_tile) == true)
    {
        // set first move target
        if (me->pathfinder->has_several_nodes())
        {
            long tmp_target = me->pathfinder->next_node();
            if (tmp_target < 0) log (LOG_DEBUG, "npc_ai::go_to_actor: pathfinding gave blocked node!");
            else
            {
                me->target = (uint)tmp_target;
                // transfer move target by setting state_walking
                me->set_state(ACTOR_STATE_WALKING);

                return true;

            }
        }
        else
        {
            log(LOG_DEBUG, "npc_ai::go_to_actor: already in good spot!");
            return true;
        }

    }
    else
    {
        log(LOG_DEBUG, "npc_ai::frame: target enemy tile was not reachable!");
        set_ai_state(NPC_AI_STATE_IDLE);
    }

    return false;
}


////////////////////////////////
// decide when to no longer attack
bool npc_ai::decide_cancel_enemy()
{
    // invalid
    if (me->lvl->actorlist.elem[ai_target] == NULL
            || me->lvl->actorlist.elem[ai_target]->health <= 0) return true;

    // get distance
    if (me->position.dist2d(&me->lvl->actorlist.elem[ai_target]->position) > me->att_range + cancel_pursue_range) return true;

    return false;
}

uint npc_ai::find_stride_target()
{
    return ((level_sv*)(me->lvl))->nearest_free_other_tile(me->current_tile);
}

bool npc_ai::is_enemy(actor *act)
{
    return(act->faction > 0 && act->faction != me->faction);
}
