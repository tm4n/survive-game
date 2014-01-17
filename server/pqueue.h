#ifndef _PQUEUE_H_
#define _PQUEUE_H_

#include "defs.h"

struct s_node;

struct s_node {

	int key; // F value: g + h
	uint pos; // position in array
	int g; // movement cost to move from the starting point to the given node
	int h; // heuristic movement cost to goal
	s_node* parent; // parent node from which we are coming

};


class pqueue
{
    public:
        pqueue();
        ~pqueue();

        s_node* Insert(s_node* pte);
        void DecreaseKey(s_node* entry, int newKey);
        s_node* Extract();
        s_node* GetByTile(uint tile);
        int Filled();

    protected:
    private:

        int filled;
        int size;
        s_node **array;

        int BubbleUp(int m);
        void BubbleDown(int i);
        void Swap(int s1, int s2);
};

#endif // _PQUEUE_H_
