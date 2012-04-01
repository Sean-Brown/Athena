/*
 *  AthenaMain.cpp
 *  Athena
 *
 *  Created by Sean Brown
 *  Copyright 2011. All rights reserved.
 *
 */

/*
 For this problem, the width will be 48 units and the height will be 10 units, the
 block sizes will be 3 and 4.5. The program will first calcuate a 48"x10" wall, then calculate 
 the number of possible walls for a user-defined width and height.
 
 To run this program (on a Mac and Linux at least), open a terminal, find the directory where "AthenaMain.ccp"
 and "node.h" are located (hopefully in the same directory...), compile both with the command 
 "g++ AthenaMain.cpp node.h -o Athena" (excluding the quotes), then run the program (timed) with the command
 "time ./Athena a b" (replacing a and b with proper parameters, e.g. as 27 and 5).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <math.h>
#include "node.h"

/*
 Declare these functions in order to recursively call later. Used to build the tree representing all
 the possible walls (referred to throughout the project as "bases")
 */
void buildTree(node*, double); // Builds the tree, leaves of the tree are possible bases
void buildTreeRight(node*, double);
void buildTreeLeft(node*, double);
void nodeInit(node*, node*, double, cracks); // Used to initialize the nodes

/*
 Declare these functions in order to calculate base pairs and trees.
 */
void calcBasePairs(double); // Calculates "base pairs", i.e. compatible layers of a wall.
void calcWalls(double, int); // Used to calculate the number of possible walls.

/*
 GLOBALS
 */
long long walls; // Global (possibly very very large...) integer to tally the number of walls
int leafs; // Number of leaves in the tree, one leaf represents a possible row
static node* leaves; // Use as a singly linked list for the leaves (bases) with left = next. Could've/should've used a vector, but this works fine.
std::map<int, node*> bases; // Global map for the bases, used for quick access

/*
 The main method, where user input is parsed.
 */

int main(int argc, char** argv) {
	// Check for only 2 arguments (3 total since the first argument is the program name), those 
	// being the width and height respectively
	if (argc != 3) {
		printf("Usage Error: %s <WALL_WIDTH> <WALL_HEIGHT>\n", argv[0]);
		exit(1);
	}
	else {
		/*
		 First, this program will build a 48"x10" wall. Build two base trees, one with a head of 3.0, 
		 the other with 4.5, the leaves of those trees will be the "bases", i.e. possible wall layers.
		 */
		node *root1 = new node;
		cracks c1;
		cracks c2;
		nodeInit(root1, NULL, 3.0, c1);
		root1->crack.crack_array.push_back(3.0);
		node *root2 = new node;
		nodeInit(root2, NULL, 4.5, c2);
		root2->crack.crack_array.push_back(4.5);
		buildTree(root1, 48.0);
		buildTree(root2, 48.0);
		walls = 0;
		/*
		 Trees built, leaves calculated, now calculate the "base pairs", i.e. compatible layers.
		 */
		calcBasePairs(48.0); 
		/*
		 Finally, calculate the possible 48"x10" panels.
		 */
		calcWalls(48, 10);
		
		/*
		 Now reset the globals.
		 */
		walls = 0; // Reset walls
		leaves = NULL; // Reset leaves
		leafs = 0; // Reset leafs
		bases.clear(); // Reset bases
		
		/*
		 Parse user input, then repeat the process as shown above, with the exception that if the wall is of 
		 height 1, we can instantly just return the number of leaves since those represent possible layers, 
		 so the number of 1" tall walls would be the possible layers.
		 */
		double userWidth = atof(argv[1]);
		int userHeight = atoi(argv[2]);
		if (userHeight > 10 || userHeight <= 0 || userWidth < 3 || userWidth > 48) {
			printf("<INVALID PARAMETERS. Enter a width between 3.0 and 48.0 inclusive which \nis a multiple of .5, and an integer height between 1 and 10 inclusive>\n");
			exit(1);
		}
		cracks c3;
		cracks c4;
		node *root3 = new node;
		nodeInit(root3, NULL, 3.0, c3);
		root3->crack.crack_array.push_back(3.0);
		node *root4 = new node;
		nodeInit(root4, NULL, 4.5, c4);
		root4->crack.crack_array.push_back(4.5);
		buildTree(root3, userWidth);
		buildTree(root4, userWidth);
		if (leaves != NULL) {
			calcBasePairs(userWidth);
			if (userHeight == 1) {		
				walls+=leafs;
				printf("%llu walls built for a %.1fx%d inch panel\n", walls, userWidth, userHeight);
			}
			else {
				calcWalls(userWidth, userHeight);
			}
		}
		else {
			// If the program reaches here, the user 
			printf("0 walls built for a %.1fx%d inch panel, try a different width\n", userWidth, userHeight);
		}
	}
}

/* 
 Recursively build all possible bases, with the help of two helper functions
 */
void buildTree(node* root, double userWidth) {
	buildTreeLeft(root, userWidth); // build left subtree (subtree with root 3)
	buildTreeRight(root, userWidth); // build right subtree (subtree with root 4.5)
}

void buildTreeRight(node *root, double userWidth) {
	// Check if this node is part of a base layer
	if (root->total + 4.5 <= userWidth) { 
		node *r = new node;
		nodeInit(r, root, 4.5, root->crack);
		r->crack.crack_array.push_back(root->total+4.5); // Add this value to the array of cracks
		r->total = root->total + 4.5;
		if (r->total == userWidth) { // The node is the end of a possible wall, add it to the leaf list
			r->isLeaf = true;
			if (leaves != NULL) { // Add to the list of leaves at the end
				node *prev = leaves;				
				r->left = prev;
				leaves = r;
			}
			else { // This is the first leaf, add it to the list of leaves
				leaves = r;
			}
			bases[leafs] = r;
			r->id = leafs;
			leafs++;
		}
		else if (r->total + 3.0 > userWidth) { // Node's a leaf that's an invalid wall, delete it to free the memory
			delete r;
			return;
		}
		
		else { // This node is part of the wall, keep building walls recursively
			r->parent = root;
			buildTree(r, userWidth);
		}
 	}
}

// See commenting above, the idea is the same except this root is 3 instead of 4.5
void buildTreeLeft(node *root, double userWidth) { 
	if ((root->total + 3.0) <= userWidth) {
		node *l = new node;
		nodeInit(l, root, 3.0, root->crack);
		l->crack.crack_array.push_back(root->total+3.0);
		l->total = root->total + 3.0;
		if (l->total == userWidth) {
			l->isLeaf = true;
			if (leaves!=NULL) { 
				node *prev = leaves;								
				l->left = prev;
				leaves = l;
			}			
			else {
				leaves = l;
			}
			bases[leafs] = l;
			l->id = leafs;
			leafs++;
		}
		else if (l->total + 3.0 > userWidth){ 
			delete l;
			return;
		}
		else {
			l->parent = root;
			buildTree(l, userWidth);
		}
	}
}


/*
 Function to initialize a node, "root" is the node to initialize, "parent" is the 
 parent of this root, val is the length of the block (only 3.0 and 4.5 are possible), and"c" is the array to be 
 used to represent the cracks that this possible row has.
 */
void nodeInit(node *root, node *parent, double val, cracks c) {
	root->isLeaf = NULL;
	root->block_value = val;
	root->total = (parent!=NULL) ? parent->total : val;
	root->left = NULL; // Initialize the left and right children to null
	root->right = NULL;
	root->parent = parent;
	root->crack = c;
	root->numbcombos = 0;
	root->id = -1; // ID is negative, will only be positive if this node becomes a leaf
	root->numbh = 1;
	root->numbhp = 1;
}

/*
 A simple function to determine if a node has a child. Used in building the tree.
 */
bool hasChildren(node * n) {
	return (n->right != NULL || n->left != NULL);
}

/*
 Function that calculates if two nodes have a crack that line up. Calculated by iterating
 over the crack array to see if they are equal at any point that's not an end point. Since 
 crack arrays can differ in lengths, this function checks within 2 cracks, since they can't differ by more than
 2 and line up (since the widths of the blocks are 3.0 and 4.5).
 */
bool lineUp(node* leaf1, node*leaf2, double userWidth) {
	double c1 = leaf1->crack.crack_array[0];
	double c3 = leaf1->crack.crack_array[1];
	double c2 = leaf2->crack.crack_array[0];
	double c4 = leaf2->crack.crack_array[1];
	for (int i = 0; i < (int)userWidth/3; i++) { // The max number of cracks is the user width/3 (a row of 3s)
		if (c1 == userWidth || c2 == userWidth) { 
			return false; // The end of one array has been reached, can return "false", i.e. they don't line up.
		}
		if (c1 == c2 || c3 == c2 || c4 == c1) {
			return true; // A crack is found, return "true", i.e. these walls line up.
		}
		else { // Keep iterating along the crack arrays.
			c1 = leaf1->crack.crack_array[i];
			c2 = leaf2->crack.crack_array[i+1];
			if (i+1 < (int)userWidth/3) { // Bounds check, to make sure it doesn't go off the edge.
				c3 = leaf1->crack.crack_array[i+1];
				c4 = leaf2->crack.crack_array[i+1];
				if (c3 >= userWidth) {
					c3 = c1;
				}
				if (c4 >= userWidth) {
					c4 = c2;
				}
			}
			else {
				c3 = c1;
				c4 = c2;
			}
			
		}
	}
	return false; // Ultimately, should all else fail, it returns false. I don't think it should reach this part ever.
}

/* 
 Calculate number of possible combinations for each leaf
 1. Calculate possible combinations with first leaf, from first to last leaf
 2. Calculate possible combinations with second leaf, from second leaf to last leaf
 3. Repeat until at the last leaf.
 
 Note that this function adds each possible base combination to the other one, so it's
 not necessary to check previous leaves (base layers).
 */
void calcBasePairs(double userWidth) {
	node *leaf1 = leaves;
	node *leaf2;
	// A saftey check, it shouldn't be null at this stage
	if (leaf1->left != NULL) { 
		leaf2 = leaf1->left;
	}
	else {
		return;
	}
	// Start at the first leaf. 
	while (true) {
		// Cycle through subsequent leaves.
		while (true) {
			bool lu = lineUp(leaf1, leaf2, userWidth);
			if (!lu) {
				// The leaves have no cracks in common, add them to each other's combo list.
				leaf1->combos.push_back(leaf2->id);
				leaf2->combos.push_back(leaf1->id);
				leaf1->numbcombos++;
				leaf2->numbcombos++;
			}
			if (leaf2->left != NULL) {
				leaf2 = leaf2->left;
			}
			else {
				break;
			}
		}
		// Check if there are more leaves, if so, keep iterating through them.
		if (leaf1->left != NULL) { 
			leaf1 = leaf1->left;
			if (leaf1->left != NULL) {
				leaf2 = leaf1->left;
			}
			else {
				break;
			}
		}
		else { // Otherwise we're at the end of the list, just return.
			break;
		}
	}
}

/*
 THE HEART AND SOUL OF THE PROGRAM, WHERE THE REAL WORK IS DONE!
 
 This function calculates the number of walls. The idea is to keep track of how many of each layer are possible
 at each height of the tree. This function takes advantage of the combo list (possible base combinations) and
 some simple observations about how some bases will end up, no matter what.
 
 For instance, if a base has only one combination (one compatible layer), and that compatible layer only has the 
 other leaf as a compatible layer, then those two layers will ultimately yield EXACTLY 2 walls. One starting with
 one layer and alternating, the other starting with the other layer and alternating. 
 
 This function also takes advantage of information saved in each node about previous possible numbers of layers
 at the last height and the second to last height.
 */
void calcWalls(double userWidth, int maxHeight) {
	// Start with h=2 since h=1 is just the leaves (possible base layers).
	int h = 2;
	while (h <= maxHeight) { 
		// This is the start of calculating the number of times each layer can appear in possible walls at height h.
		for (int a = 0; a < bases.size(); a++) {
			// This iterates through each base layer.
			/* 
			 This first check is to see if the number of combinations with this leaf is 1 and that this leaf's 
			 first combination has more than one combination. If this is true, then this leaf can only show up
			 as many times as its combination showed up in the previous layer
			 */
			if (bases[a]->numbcombos == 1 && bases[bases[a]->combos[0]]->numbcombos > 1) {
				bases[a]->numbhp = bases[a]->numbh; // Set the previous number to the current number of this base at height h.
				/* 
				 Check to see if this is the first round, if so, set the number of possible layers at height 2 to the
				 combinations of this layer, since each of those combinations will fork off into a possible wall containing
				 this layer.
				 */
				if (h == 2) { 
					bases[a]->numbh = bases[a]->numbcombos;
				}
				/*
				 Otherwise, the height is greater than 2, so use previous knowledge of possible numbers of this layer, along
				 with layer ids. The ids are useful because in this function, some layers have their current number (numbh) set 
				 before others. The ids tell whether to use numbh or numbhp to set this layer's possible combinations at height h.
				 */
				else {					
					if (bases[a]->id < bases[bases[a]->combos[0]]->id) {
						// This base is being set before the combination, can use that combination's numbh value.
						bases[a]->numbh = (bases[bases[a]->combos[0]]->numbh);
					}
					else {
						// This base is being set after the combination, use the combination's numbhp value.
						bases[a]->numbh = (bases[bases[a]->combos[0]]->numbhp);
					}
				}
				continue; // Continue on to the next base. Maybe could've used a switch statement...
			}
			else if (bases[a]->numbcombos == 1 && bases[bases[a]->combos[0]]->numbcombos == 1) {
				/* 
				 This is the boring base, which only has one compatible base. Can immeditely set it's numbh value
				 to 1 since it will only have 1 wall ending in this base (the same can be said of its combination).
				 */
				bases[a]->numbh = 1;
				continue; // Continue on to the next base.
			}
			else if (bases[a]->numbcombos == 0) {
				// Another boring layer, this has no compatible layers so it doesn't get counted at the end.
				bases[a]->numbh = 0;
				continue; // Continue on to the next base.
			}
			else if (bases[a]->numbcombos > 1) {
				/*
				 If the layer has more than one combination, set the previous number of this base at height h to the
				 current, then recalculate the current by iterating through all of the combinations and adding that 
				 combination's current number to this base's number, since the next layer will have one fork from the 
				 combination which will contain this layer. Again, use of the base's id is important to see if this 
				 base is being set before or after its combination.
				 */
				bases[a]->numbhp = bases[a]->numbh;
				bases[a]->numbh = 0;
				for (int b = 0 ; b < bases[a]->combos.size(); b++) {
					int k = bases[a]->combos[b];
					if (h == 2) {
						bases[a]->numbh = bases[a]->numbcombos;
					}
					else if (bases[a]->id < bases[k]->id) {
						bases[a]->numbh += bases[k]->numbh;
					}
					else {
						bases[a]->numbh += bases[k]->numbhp;
					}
				}
			}
		}
		h++; // Increment the layer height.
	}
	/*
	 Finally, we can compute the number of walls, based on the number times each base will appear in a final wall.
	 */
	while (true) {
		walls += leaves->numbh;
		if (leaves->left != NULL) {
			leaves = leaves->left;
		}
		else {
			break;
		}
	}	
	printf("%llu walls built for a %.1fx%d inch panel\n", walls, userWidth, maxHeight);
}
