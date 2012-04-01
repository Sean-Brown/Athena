/*
 *  node.h
 *  Athena
 *
 *  used by Sean Brown
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>

/*
 Define a struct for the cracks in the walls. This keeps a running tally to see where the 
 cracks are. This is just another way of representing the wall. For example, a crack 
 array could look like "[3, 6, 9, 12, 15, 18, 21, 24, 27, 31.5, 34.5, 37.5, 42, 45, 48]"
 */
struct cracks 
{
	std::vector<double> crack_array; // max cracks is width/3 (row of all 3s)
};

/*
 Define a struct for the nodes of the tree to be built. Each leaf node will represent a
 complete "base layer", i.e. a valid layer (height 1) of a possible wall.
 */
struct node 
{
	bool isLeaf; // A boolean to tell if this node is a leaf
	double block_value; // A double value of this leaf's block (either 3.0 or 4.5) 
	cracks crack; // A variable to keep a tally of the cracks
	double total; // A variable to determine the total width of this node
	
	/*
	 Used in building the tree
	 */
	node *left; // A pointer to the left child node in the tree
	node *right; // A pointer to the right child node in the tree (Note: only used during tree building)
	node *parent; // A pointer to the parent node in the tree
	std::vector<int> combos; // A list of all the possible combinations, i.e. base layers that have no cracks in common with this node
	int numbcombos; // An integer value of the number of combinations with this node
	
	int id; // Identifies the leaf in the map, useful during wall calculations and for inserting into the map
	
	/*
	 Used exclusively in calculating walls, make long long ints since they can get really big.
	 */
	long long int numbh; // Represents the number of occurrences of this base node at a height h
	long long int numbhp; // Represents the number of this base node at a height h-1
};
