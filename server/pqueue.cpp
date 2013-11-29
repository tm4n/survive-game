#include "include/pqueue.h"
#include "stdio.h"

pqueue::pqueue()
{
	size = 100; // default size
	filled = 0;

	array = new s_node*[size];
	for (int i = 0; i < size; i++) array[i] = 0;
}

pqueue::~pqueue()
{
	//assert(queue);
	//delete all priorityQueueEntries:
	int i;
	for (i = 0; i < size; i++)
	{
		if (array[i]) delete((array)[i]);
	}

	// free memory of array
	delete[] (array);
}


/*int pqParent(int n) {return((n-1)/2);}
int pqLeft(int n) {return(2*n+1);}
int pqRight(int n) {return(2*n+2);}*/


// Helper makros:
#define pqParent(n) ((n-1)/2)
#define pqLeft(n) (2*n+1)
#define pqRight(n) (2*n+2)

/* Swap the elements with the given indices
Complexity: O(1) */
void pqueue::Swap(int s1, int s2)
{
	s_node* tmp;

	tmp = (array)[s1];
	(array)[s1] = (array)[s2];
	(array)[s2] = tmp;

}

/*
Move the Element up until the heap is sorted again.
Complexity (worst case): O( log(n) )
*/
int pqueue::BubbleUp(int m)
{
	int n;

	n = pqParent(m);
	// while there is still a parent and parent has bigger value
	while(n != m && (((array)[m])->key < ((array)[n])->key))
	{
		// exchange parent and current node
		Swap(n, m);
		m = n;

		n = pqParent(m);
	}
	return (m);
}


// recursive implementation from
// http://www.csua.berkeley.edu/~ranga/school/cs161/index.html
void pqueue::BubbleDown(int i)
{
	int l,r,smallest;

	l = pqLeft(i);
	r = pqRight(i);

  // check the left child

	if (l < filled)
	{
		if(((array)[l])->key <= (((array)[i])->key)) smallest = l;
		else smallest = i;
	}
	else smallest = i;

  // check the right child

	if (r < filled)
	{
		if(((array)[r])->key <= (((array)[smallest])->key)) smallest = r;
	}

  if (smallest != i)
  {

    // swap nodes smallest and i, then repeat

    Swap(i, smallest);
    BubbleDown(smallest);
  }
}


//DEBUG function for printing out the internal array that holds the priorityQueue.
//Complexity: (O(n))
/*void pqPrint(s_priorityQueue* queue)
{
	int i;

	STRING* output = str_create("Print list:");

	//if (mx == 0) {puts("FEHLER in pqPrint: Liste scheint leer");}
	for (i = 0; i < queue->size; i++)
	{
		if ((array)[i])
		{
			str_cat_num(output, "%.0f, ", ((array)[i])->key);
		}
		else
		{
			str_cat(output, "x, ");
		}
	}
	error(output);

	str_remove(output);
}

var pqValid(s_priorityQueue* queue, int pos)
{
}*/

// End of helper functions.

// Start of functions defined in header:
// A short description of their function is given in the header.



s_node* pqueue::Insert(s_node* pte)
{
	int m;

	//assert(queue);
	//assert(data);

	/* O(1) */

	// test if internal structure is full and resize if necessary
	if (filled == size)
	{
	    puts("resizing pqueue!");

		//error ("increasing queue size!");
		// double the size
		size *= 2;
		s_node** array_old = array;

		array = new s_node*[size];
		for (int i = 0; i < filled; i++) array[i] = array_old[i];
		delete [](array_old);
		// clear new array members
		for (int i = filled; i < size; i++) array[i] = 0;
	}

	// insert at last point in heap
	m = filled;
	if ((array)[m] != 0) puts("FATAL ERROR in pqueue::Insert: Memory at last position in Heap was not empty!");
	((array)[m]) = pte;
	filled++;

	/* O(1) */

	// bubble up until at right position
	BubbleUp(m);

	/* O (log (n)), see function description above */

	return (pte);

	/* Complexity of this function: O(1) + O(1) + O(n) + O(log (n)) = O(n) */
}

void pqueue::DecreaseKey(s_node* entry, int newKey)
{
	//find entry in queue
	int p = 0;
	while (p < size && (array)[p] != entry) p++;
	if (p == size) {puts("ERROR in pqDecreaseKey: entry not found in queue!"); return;}

	//change key value
	entry->key = newKey;

	//resort (bubble up) the changed entry
	BubbleUp(p);
}

//
s_node* pqueue::Extract()
{
	s_node *elem;

	//assert(queue);

	elem = (array)[0];
	// Test for empty queue element
	if (elem == 0) {return(0);}


	// Copy the last element at the place of the first
	filled--;
	(array)[0] = (array)[filled];
	(array)[filled] = NULL;


	// Don't delete the element this time


	// Bubble down the new first element until queue is sorted again
	if (filled > 0) BubbleDown(0);

	return(elem);

}

// maybe this could be done faster?
s_node* pqueue::GetByTile(uint tile)
{
	int i;
	for (i = 0; i < filled; i++)
	{
		if ((array)[i]->pos == tile) return ((array)[i]);
	}
	return 0;
}

int pqueue::Filled() {return (filled);}
