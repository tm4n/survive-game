#include "include/path.h"


#define mdim(i,j) ((i*lvl->size)+j)

// could probably also be done as makros
#define mrev_i(x) ((uint)(x/lvl->size))
#define mrev_j(x) (x - ((uint)(x/lvl->size) * lvl->size))

using namespace std;


path::path(level_sv *mylvl, actor *nme)
{
    lvl = mylvl;
    me = nme;
    progress = -1;

}

path::~path()
{
    unregister_tile();
    //dtor
}

void path::unregister_tile()
{
    // unregister my tile
    if (lvl->tiles_blocked[me->current_tile] == me->id+1) lvl->tiles_blocked[me->current_tile] = 0;
    else log (LOG_ERROR, "ERROR in path::unregister_tile: Tried to unregister actor for tile, but tile was not registered by this actor.");
}

void path::register_tile()
{
    if (lvl->tiles_blocked[me->current_tile] == 0) lvl->tiles_blocked[me->current_tile] = me->id+1;
    else log (LOG_ERROR, "ERROR in path::register_tile: Tried to register new actor for tile, but tile was already blocked.");
}



//////////////////////////////////////////////////////
// PATH CHECKS
//
bool path::pos_valid(uint pos)
{
	// check if in range
	if (pos < 0 || pos >= lvl->num_tiles) return false;

	// check if tiles exist and is walkable
	if (lvl->tiles[pos].g_id <= 0) return false;

	// check for dynamic obstacles within a short range from start tile
	if (pos != pos_target) if ((abs(mrev_i(pos_start) - mrev_i(pos)) + abs (mrev_j(pos_start) - mrev_j(pos)))*10 <= ASTAR_DYNAMIC_OBST_AVOID
	 && (lvl->tiles_blocked[pos] != 0 && lvl->tiles_blocked[pos] != me->id+1)) return false;

	return true;
}

bool path::walkable_up_down(uint pos1, uint pos2)
{
	// check if walls are blocking
	if (lvl->tiles[pos1].w_id[W_ID_UP] != 0 || lvl->tiles[pos2].w_id[W_ID_DOWN] != 0) return false;

	return true;
}

bool path::walkable_right_left(uint pos1, uint pos2)
{
	// check if walls are blocking
	if (lvl->tiles[pos1].w_id[W_ID_RIGHT] != 0 || lvl->tiles[pos2].w_id[W_ID_LEFT] != 0) return false;

	return true;
}

// also tests down right
bool path::walkable_ul_dr(uint pos1, uint pos2)
{

	// check if walls are blocking
	if (lvl->tiles[pos1].w_id[W_ID_LEFT] != 0 || lvl->tiles[pos1].w_id[W_ID_UP] != 0) return false;

	// check if walls are blocking of part 2
	if (lvl->tiles[pos2].w_id[W_ID_RIGHT] != 0 || lvl->tiles[pos2].w_id[W_ID_DOWN] != 0) return false;

	return true;
}

// also tests down left
bool path::walkable_ur_dl(uint pos1, uint pos2)
{
	// check if walls are blocking
	if (lvl->tiles[pos1].w_id[W_ID_RIGHT] != 0 || lvl->tiles[pos1].w_id[W_ID_UP] != 0) return false;

	// check if walls are blocking of part 2
	if (lvl->tiles[pos2].w_id[W_ID_LEFT] != 0 || lvl->tiles[pos2].w_id[W_ID_DOWN] != 0) return false;

	return true;
}



///////////////////////////////////////////////////////
// processes the node like the algorithm says
void path::astar_process_node(s_node* current_node, uint pos, float cost)
{
	s_node* nd;
	set<s_node*>::iterator it;

    // no error! node is just out of dynamic evasion route
	//if (lvl->tiles_blocked[pos] != 0) log (LOG_ERROR, "ERROR in astar_process_node: processing blocked node, omg!");

	// check if it is on the closedset, abort if so
    for (list<s_node*>::iterator it = closedset->begin(); it != closedset->end(); it++)
    {
        if ((*it)->pos == pos) return;
    }

	// check if it is on the openlist
	nd = openset->GetByTile(pos);
	if (nd)
	{
		// check if new path found here is better:
		int new_g = current_node->g + cost;
		if (new_g < nd->g)
		{
			nd->g = new_g;
			//H stays the same
			int new_key = nd->h + nd->g; // F=G+H
			nd->parent = current_node;
			openset->DecreaseKey(nd, new_key);
		}

	}
	else
	{
		// add to open list

		//create s_node data structure
		nd = new s_node();

		// calculate values
		nd->pos = pos;
		nd->g = current_node->g + cost;
		nd->h = (abs(mrev_i(nd->pos) - mrev_i(pos_target)) + abs (mrev_j(nd->pos) - mrev_j(pos_target)))*10;
		nd->key = nd->h + nd->g; // F=G+H
		nd->parent = current_node;

		// and add:

		openset->Insert(nd);
	}

	// show values
	//debug_show(nd->pos, nd->key, 40);

}

bool path::find_path(uint npos_start, uint npos_target)
{

	uint npos;
	s_node* current_node = NULL;

    // delete old path
    nodelist.clear();

    // save values
    pos_start = npos_start;
    pos_target = npos_target;


	// check if tile target exist and is walkable
	if (lvl->tiles[npos_target].g_id <= 0) return false;
	//if (lvl->tiles_blocked[npos_target] != me->id+1) return false;


	//DEBUG:
	//	timer();

	// initialize data structures needed

	// openset as priority_queue
	openset = new pqueue();

	// closedset as set
	closedset = new list<s_node*>();

	//add start node to openset
	//create s_node data structure
	s_node* nd_start = new s_node();
	nd_start->pos = pos_start;
	nd_start->g = 0;
	nd_start->h = (abs(mrev_i(pos_start) - mrev_i(pos_target)) + abs (mrev_j(pos_start) - mrev_j(pos_target)))*10;
	nd_start->key = nd_start->h;
	nd_start->parent = NULL;

	// add start node to openset
	openset->Insert(nd_start);

	while (openset->Filled() > 0) // as long as there are nodes in the open set
	{
		//get current node
		current_node = openset->Extract();

		//put it on the closed set
		closedset->push_back(current_node);
		//debug_show_c(current_node->pos, 0);

		// if current node is target: done!!
		if (current_node->pos == pos_target) {break;}

		// straight nodes
		// check node above:
		npos = current_node->pos - lvl->size;
		if (pos_valid(npos) && walkable_up_down(current_node->pos, npos)) astar_process_node(current_node, npos, ASTAR_COST_STRAIGHT);

		// check node to the right:
		npos = current_node->pos + 1;
		if (pos_valid(npos) && walkable_right_left(current_node->pos, npos)) astar_process_node(current_node, npos, ASTAR_COST_STRAIGHT);

		// check node below
		npos = current_node->pos + lvl->size;
		if (pos_valid(npos) && walkable_up_down(npos, current_node->pos)) astar_process_node(current_node, npos, ASTAR_COST_STRAIGHT);

		// check node to the left
		npos = current_node->pos - 1;
		if (pos_valid(npos) && walkable_right_left(npos, current_node->pos)) astar_process_node(current_node, npos, ASTAR_COST_STRAIGHT);

		// diagonal nodes
		// check node above left:
		npos = current_node->pos - lvl->size -1;
		if (pos_valid(npos) && walkable_ul_dr(current_node->pos, npos)) astar_process_node(current_node, npos, ASTAR_COST_DIAG);

		// check node above right:
		npos = current_node->pos - lvl->size +1;
		if (pos_valid(npos) && walkable_ur_dl(current_node->pos, npos)) astar_process_node(current_node, npos, ASTAR_COST_DIAG);

		// check node below right
		npos = current_node->pos + lvl->size +1;
		if (pos_valid(npos) && walkable_ul_dr(npos, current_node->pos)) astar_process_node(current_node, npos, ASTAR_COST_DIAG);

		// check node to the left
		npos = current_node->pos + lvl->size -1;
		if (pos_valid(npos) && walkable_ur_dl(npos, current_node->pos)) astar_process_node(current_node, npos, ASTAR_COST_DIAG);

	}

	//double time = dtimer();
	//error (str_for_float(0, time));
	bool way_found;

	if (current_node->pos != pos_target)
	{
		// no way found
		way_found = false;
	}
	else
	{
		// way found
		way_found = true;
		// create nodelist backwards
		s_node* ppos = current_node;
		while (ppos != 0)
		{
		    nodelist.push_back(ppos->pos);
			//create_marker(ppos->pos); 		// create ents on the way for debugging
			ppos = ppos->parent;
		}
		if (nodelist.size() > 1) progress = nodelist.size() - 2; else progress = 0;
	}

	// deinitialize everything
	delete(openset);

	// remove all s_nodes, is done automatically for openset
    for (list<s_node*>::iterator it = closedset->begin(); it != closedset->end(); it++) delete(*it);
    delete(closedset);

	return way_found;
}

long path::next_node()
{
    // check for obstacles!
    // get next tile to go to
    uint next_tile = nodelist[progress];

    if (lvl->tiles_blocked[next_tile] == 0 || lvl->tiles_blocked[next_tile] == me->id+1)
    {
        // mark me as standing on the tile
        lvl->tiles_blocked[next_tile] = me->id+1;
    }
    else
    {
        if (next_tile == pos_target)
        {
            log(LOG_DEBUG, "Target of path was blocked, cannot move to that tile");
            clear();
            return -1;
        }

        log(LOG_DEBUG, "Path was blocked, trying to find new one..!");
        // find a new path
        if(find_path(me->current_tile, pos_target) == false)
        {
            log(LOG_DEBUG, "No new path found :(");
            // no new path found
            clear();
            // path user has to double check now, else he will walk to 0!
            return -1;
        }
        else
        {
            log(LOG_DEBUG, "New Path found!");

            // mark new tile
            next_tile = nodelist[progress];

            if (lvl->tiles_blocked[next_tile] == 0)
            {
                // mark me as standing on the tile
                lvl->tiles_blocked[next_tile] = me->id+1;
            }
            else
            {
                log(LOG_ERROR, "ERROR in path::next_node: Pathfinding gave blocked node, FIX ME!");
                clear();
                return -1;
            }

        }
    }


    if (me->current_tile != next_tile)
    {
        if (lvl->tiles_blocked[me->current_tile] == me->id+1) lvl->tiles_blocked[me->current_tile] = 0;
        else log(LOG_ERROR, "ERROR in path::next_node: could not reset last own tile!");
    }

    me->current_tile = next_tile;

    // DEBUG
    /*int num_blocked_nodes = 0;
    for (int j = 0; j < lvl->num_tiles; j++) if (lvl->tiles_blocked[j] != 0) num_blocked_nodes++;

    printf("DEBUG: next_node done, number of nodes currently blocked in lvl = %i \n", num_blocked_nodes);
    */

    return nodelist[progress--];
}

bool path::has_nodes()
{
    return nodelist.size() > 0 && progress >= 0;
}

bool path::has_several_nodes() // more than one node remaining
{
    return nodelist.size() > 0 && progress >= 1;
}


//////////////////////////////////////////////////////
void path::clear()
{
    progress = -1;
    nodelist.clear();
    pos_start = 0; pos_target = 0;
}
