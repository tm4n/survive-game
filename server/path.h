#ifndef __PATH_H__
#define __PATH_H__

#include <list>
#include <vector>

#include "level_sv.h"
#include "pqueue.h"

//////////////////////////////////////////////////////
// A* Implementation for the MMO

#define ASTAR_COST_STRAIGHT 10
#define ASTAR_COST_DIAG 14
#define ASTAR_DYNAMIC_OBST_AVOID 30



// see pqueue.h for s_node* definition


// predef
class level_sv;

class path
{
    public:
        level_sv *lvl;
        actor *me;
        uint pos_start, pos_target;
        int progress;


        path(level_sv *mylvl, actor *nme);
        ~path();

        void register_tile();
        void unregister_tile();

        bool find_path(uint npos_start, uint npos_target);
        bool has_nodes();
        bool has_several_nodes();
        long next_node(); // negative values are errors!
        void clear();

    protected:
    private:

        std::vector<uint> nodelist;
        pqueue *openset;
        std::list<s_node*> *closedset; // todo: hash_set hash with key value!!!! c++0x..


        void astar_process_node(s_node* current_node, uint pos, float cost);

        bool pos_valid(uint pos);
        bool walkable_up_down(uint pos1, uint pos2);
        bool walkable_right_left(uint pos1, uint pos2);
        bool walkable_ul_dr(uint pos1, uint pos2);
        bool walkable_ur_dl(uint pos1, uint pos2);

};

#endif // __PATH_H__
