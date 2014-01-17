#ifndef __PLAYER_SV_H__
#define __PLAYER_SV_H__

#include "gameServer.h"
#include "player.h"
#include <enet/enet.h>
#include "actor_updater.h"
#include "level_sv.h"
#include "path.h"
#include "enet/enet.h"

class path;
class actor_updater;
class level_sv;
class s_zone;

class player_sv : public player
{
    public:
        int zone, gateway;
        uint account_id;
        ENetPeer *owner;
        ENetHost *ownhost;

        actor_updater *updater;
        path *pathfinder;

        uint ai_state;
        uint ai_target;

        char *skbar_ch;


        player_sv(int zone, int gateway, level_sv *lvl,
               std::list<s_zone> *zonelist, ENetHost *ownhost, vec *pos, vec *pan,
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
               const char* inv_ch,
               const char* quests_ch,
               const char* skills_ch);

        virtual ~player_sv();

        virtual void frame (double time_delta);

        virtual void set_state(uint new_state);
        void set_ai_state(uint new_ai_state);

        virtual bool do_damage(uint target_actor, float damage);

        virtual void attract_enemies();

        void give_xp(int amount, uint giver_id);
        void give_gold(int amount, uint giver_id);
        void give_gold(uint giver_id);

        bool go_to_actor(uint enemy_tile);

        void recalc_equip();

        void order_move(uint target_tile);
        void order_attack(uint target_actor);
        void order_interact(uint target_actor);
    protected:

        bool damage_done;

        double health_regen_timer, mana_regen_timer;

        std::list<s_zone> *zonelist;

    private:
};

#endif // PLAYER_SV_H
