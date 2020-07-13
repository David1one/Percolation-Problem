#include "Point.h"
#include <iostream>
#include <iomanip>
#include <stack>
#include <random>
#include <fstream>

using namespace std;


Point::Point() : x(0), y(0), colour('w'), val(0) {
	/*Assign the point the x & y coordinates of (0,0), the colour white, and the cluster number 0.*/
}

Point::Point(int x, int y) : x(x), y(y), colour('w'), val(0) {
	/*Assign the point the x & y coordinates given, the colour white, and the cluster number 0.*/
}

void Point:: get_pos(int pos[2]) {
	/*Takes in an array and turns it into {x,y}, which can then be read.*/
	pos[0] = this->x;
	pos[1] = this->y;
}

void Point::set_pos(int x, int y) {
	/*Takes in an x & y, and assigns them to the point.*/
	this->x = x;
	this->y = y;
}

void Point::set_colour(char c) {
	/*Set the colour to either 'w' (white), 'g' (grey) or 'b' (black)*/
	this->colour = c;
}

char Point::get_colour() {
	/*Return colour of the point - either 'w', 'g', or 'b'*/
	return this->colour;
}

int Point::get_val() {
	/*Returns the cluster label of the Point.*/
	return val;
}

void Point::set_val(int v) {
	/*Sets the cluster label of the Point.*/
	val = v;
}

void Point::report() {
	/*Outputs position, cluster label, and colour of Point.*/
	cout << "x = " << this->x << endl;
	cout << "y = " << this->y << endl;
	cout << "val = " << this->val << endl;
	cout << "colour = " << this->colour << endl;
}


void rewrite_labels(int* cluster_labels, int old_label, int new_label) {
	cluster_labels[old_label] = -new_label; //Changes old_label into a reference to show it's no longer a proper label.
}



/*
Notes on the stack object:
mystack.size() gives number of elements in stack
mystack.top() gives top element of stack
mystack.push() adds element to stack
mystack.pop() removes element from stack
*/

// Implement Breadth-first search. It assigns the proper label to each Point in the lattice.
void bfs(Point L[MAX_SIZE][MAX_SIZE], const int size, Point start_node, int* cluster_labels) {

	// Declare the stack, and initialise it by appending the start node.
	stack<Point> mystack;
	mystack.push(start_node);

	// x & y will hold the position of the current object whose neighbours are being explored.
	int x, y;

	// As the proper label is always the minimum label in a group, and since the loop goes through the labels in increasing order, 
	// it is guaranteed that the proper label of all the Points explored in this function is equal to the label of the start node.
	// These lines just get the proper label.
	int pos[2];
	start_node.get_pos(pos);
	int proper_label = L[pos[0]][pos[1]].get_val(); 

	// Defensive programming: A sanity check to make sure that we haven't started from an unoccupied element.
	if (start_node.get_val() == 0) {
		cout << "ERROR - Can't do bfs on zero" << endl;
		return;
	}


	while (!mystack.empty()) { // While there are more neighbours to be explored.
		
		// Get Point on top of stack, & save its x & y, then remove it from the top of the stack.
		mystack.top().get_pos(pos);
		x = pos[0];
		y = pos[1];
		mystack.pop();

		//Add all neighbours to list as part of search.

		//If not on left edge, add left neighbour to stack if it's nonzero & still white.
		if (x != 0 && L[x - 1][y].get_val() != 0 && L[x - 1][y].get_colour() == 'w') {
			rewrite_labels(cluster_labels, L[x - 1][y].get_val(), proper_label); // Change the label to the proper label.
			L[x - 1][y].set_colour('g'); // Set colour to grey - it's in the process of being explored.
			mystack.push(L[x - 1][y]); // Add the Point to the stack.
		}
		//If not on right edge, add right neighbour to stack if nonzero & still white.
		if (x != size - 1 && L[x + 1][y].get_val() != 0 && L[x + 1][y].get_colour() == 'w') {
			rewrite_labels(cluster_labels, L[x + 1][y].get_val(), proper_label);
			L[x + 1][y].set_colour('g');
			mystack.push(L[x + 1][y]);
		}
		//If not on top edge, add top neighbour to stack if nonzero & still white.
		if (y != 0 && L[x][y - 1].get_val() != 0 && L[x][y - 1].get_colour() == 'w') {
			rewrite_labels(cluster_labels, L[x][y - 1].get_val(), proper_label);
			L[x][y - 1].set_colour('g');
			mystack.push(L[x][y - 1]);
		}
		//If not on bottom edge, add bottom neighbour to stack if nonzero & still white.
		if (y != size - 1 && L[x][y + 1].get_val() != 0 && L[x][y + 1].get_colour() == 'w') {
			rewrite_labels(cluster_labels, L[x][y + 1].get_val(), proper_label);
			L[x][y + 1].set_colour('g');
			mystack.push(L[x][y + 1]);
		}

		// Set the Point black, now that all of its neighbours have been explored, and we're done with it.
		L[x][y].set_colour('b');

	}

	// At the end of this function, the entire cluster has been given its proper label.
}

void initialise_lattice(Point L[MAX_SIZE][MAX_SIZE], int size) {
	/* Initialise all the values in the 2D array - set its cluster label to 0, set its colour to white, and tell it its position. */

	for (int i = 0; i < size; i++) { //Every element in row
		for (int j = 0; j < size; j++) { //Every element in column
			L[i][j].set_val(0); //Make it zero
			L[i][j].set_colour('w');
			L[i][j].set_pos(i, j);
		}
	}

}

void print_lattice(Point L[MAX_SIZE][MAX_SIZE], int size) {
	/* This function will basically print the lattice of size "size" to the console.
		We keep i constant as we pass through each column. Once the final column is reached, a newline is printed, and we go to the next row.
	*/

	for (int i = 0; i < size; i++) { // Every element in row i

		for (int j = 0; j < size; j++) { // Every element in column j

			cout << setw(4) << L[i][j].get_val() << " "; // (0,0) (0,1) (0,2) ... (0,9) , (1,0), (1,1)... (9,8), (9,9)

		}

		cout << endl; //Move onto next line

	}

}



int find_proper_label(int* cluster_labels, int c) {
	/* c is the cluster of which we need to find the proper label. 
		It iterates through the list until positive number (signifying the proper label) is found.
		Furthermore, the function optimises cluster_labels at the end of each call, by making all entries it viewed into a "most direct reference".
	*/
	/* Assumptions & Errors:
		Assume cluster_labels are the labels & references that correspond to the assigned labels [0, 1, 2, 3 ...].
		Ideally, 0th position will never be referenced, but it fails safely if it is.
	*/


	// a & i will keep a track of the entries that must be changed at the end of the loop.
	int* a = new int[c + 1]; // Max number of cluster labels that may be viewed is c+1, as the proper label will always be less than or equal to assigned label.
	int i = 0; // Keeps track of how many elements have been viewed.

	// Search until proper label is found 
	while (cluster_labels[c] < 0) { // If entry is negative, then it's not a proper label.

		// Add current cluster number to the list of clusters which need to be given a most direct reference.
		a[i] = c;
		i++;

		// Change c in order to go to referenced location in next iteration.
		// As the value being looked at is actually negative, change it so c will be positive.
		c = -cluster_labels[c];
	}

	// Defensive programming: Verify that c is positive.
	if (c < 0) c = -c;

	// Optimise cluster_labels, by making each reference viewed into a most direct reference.
	// It is known that last value stored is already a most direct reference, so go through all elements in a except last one. ie. j in range [0,i-1)
	// Change the value in cluster_labels for each wanted value in a to -proper_label.
	for (int j = 0; j < i - 1; j++) {
		cluster_labels[a[j]] = -c;
	}

	delete[] a; // Drop dynamic memory

	return c; // Return proper label

}



int find_spanning_cluster(Point L[MAX_SIZE][MAX_SIZE], int size, int* cluster_labels) {
	/*Checks the 4 edges for clusters.
	If a cluster is present on al 4 edges, return cluster label.
	Otherwise, return 0.*/

	int label = 0; // The label of the spanning cluster. 0 means there's no spanning cluster.

	// At maximum, an edge has "size" distinct clusters on it.
	// n_X_edge saves the number of clusters on each edge.
	int* top_edge = new int[size]; // Top edge
	int n_top_edge = 0;
	int* bottom_edge = new int[size]; // Bottom edge
	int n_bottom_edge = 0;
	int* left_edge = new int[size]; // Left edge
	int n_left_edge = 0;
	int* right_edge = new int[size]; // Right edge
	int n_right_edge = 0;

	// Collect all non-zero clusters into the lists.
	for (int i = 0; i < size; i++) {

		if (L[0][i].get_val() != 0) { // Top edge
			top_edge[n_top_edge] = find_proper_label(cluster_labels, L[0][i].get_val()); // Save proper label
			n_top_edge++;
		}

		if (L[size - 1][i].get_val() != 0) { // Bottom edge
			bottom_edge[n_bottom_edge] = find_proper_label(cluster_labels, L[size - 1][i].get_val());
			n_bottom_edge++;
		}

		if (L[i][0].get_val() != 0) { // Left edge
			left_edge[n_left_edge] = find_proper_label(cluster_labels, L[i][0].get_val());
			n_left_edge++;
		}

		if (L[i][size - 1].get_val() != 0) { // Right edge
			right_edge[n_right_edge] = find_proper_label(cluster_labels, L[i][size - 1].get_val());
			n_right_edge++;
		}

	}

	// Sort the 4 lists.
	sort(top_edge, top_edge + n_top_edge);
	sort(bottom_edge, bottom_edge + n_bottom_edge);
	sort(left_edge, left_edge + n_left_edge);
	sort(right_edge, right_edge + n_right_edge);


	// Remove duplicate assigned labels in top_edge list
	// Exploits fact that list is sorted.
	// This is just so the same label isn't searched for in all the lists multiple times.

	int distinct_clusters = 0;
	int* distinct_top_edge = new int[n_top_edge]; //At max, all the collected top edge clusters are distinct.
	// Doesn't complain if n_top_edge == 0, so it's ok.

	if (n_top_edge != 0) { // If some elements exist in the list

		// Special case: It is already known that first element is going to be in final list.
		distinct_top_edge[0] = top_edge[0];
		distinct_clusters++;

		// For the rest of the elements, check they're not duplicates.
		for (int i = 1; i < n_top_edge; i++) {

			if (top_edge[i] != top_edge[i - 1]) { // If distinct.
				distinct_top_edge[distinct_clusters] = top_edge[i]; // Next free slot in distinct_top_edge indexed by distinct_clusters.
				distinct_clusters++;
			}
		}

	}

	// If top edge has 0 clusters, then there's no point in doing binary searches - none of the clusters touch the top edge.
	// However, if it's nonzero, then can check for existence of cluster number.
	// binary_search(data, data+size, value) is a function that returns true if the value is in the sorted array, and false if not. It runs in Theta(lg n) time.
	// The use of the binary search function reduces the time complexity of "find_spanning_cluster" from Theta(n^2) to Theta(n lg n).

	int val; // Stores current cluster to check.
	for (int i = 0; i < distinct_clusters; i++) { // For each distinct cluster on top edge

		val = distinct_top_edge[i];

		// Binary search each list for specified value.
		// The short-circuiting of C++ operators also boosts the efficiency.
		if (binary_search(bottom_edge, bottom_edge + n_bottom_edge, val)
			&& binary_search(left_edge, left_edge + n_left_edge, val)
			&& binary_search(right_edge, right_edge + n_right_edge, val)) {

			// If successful, save cluster label & break
			label = val;
			break;
		}
	}

	// Release dynamic memory.
	delete[] top_edge;
	delete[] distinct_top_edge;
	delete[] bottom_edge;
	delete[] left_edge;
	delete[] right_edge;

	return label; // Return spanning cluster label if successful, or 0 if not.
}

double mean(double* data, int size) {
	/* Implements standard formula for calculating the mean of a list. */

	double sum = 0; // Keep a running total
	for (int j = 0; j < size; j++)
		sum += data[j]; // Add together all the terms
	return sum / size; // Divide by number of elements, return.

}

bool F_calculation_to_file(const char* filename, double* data, int n) {
	/*Create a csv file and put data from "data" into it, delimited by newlines.*/

	ofstream outfile(filename, ios::out); // Create output file.

	// Defensive programming: If I can’t make file. IMPORTANT – MAY BE DENIED PERMISSION.
	if (!outfile) {
		cout << "Failed to create file" << endl;
		return EXIT_FAILURE;
	}

	// This loop just prints the ith number in the array, then a newline.
	for (int i = 0; i < n; i++) {
		outfile << data[i]; // Output ith element of data 
		if (i < n - 1)  outfile << endl;  // Add a newline character, unless we're on the last entry.
	}

	outfile.close(); // Close the file

	return EXIT_SUCCESS;
}


