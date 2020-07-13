#pragma once
#include <random>

// Constants
const int MAX_SIZE = 100;
const int N_NEIGHBOURS = 4; //Number of neighbours each lattice element has. But get_distinct_neighbours must be updated manually.

class Point
{
private:

	int x; // x coordinate of Point
	int y; // y coordinate of Point
	int val; // Cluster to which Point is associated
	char colour; //white ('w'), grey ('g') or black ('b'). Used for Breadth-first search.
public:
	Point(); // Default constructor.
	Point(int x, int y); // Constructor that allows you to specify x & y.
	void get_pos(int pos[2]); // Makes pos into {x,y}, from which the position may be read.
	void set_pos(int x, int y); // Sets the position of the Point.
	int get_val(); // Obtains the cluster label of the Point.
	void set_val(int v); // Changes the cluster to which the point is associated.
	char get_colour(); // Gets the colour of the point.
	void set_colour(char c); // Sets the colour of the point.
	void report(); // Prints out position, cluster label, and colour 
};

void bfs(Point L[MAX_SIZE][MAX_SIZE], const int size, Point start_node, int* cluster_labels); // Do Breadth-first search on lattice that already has a cluster label assigned to each element, but hasn't created a cluster_list yet.

void initialise_lattice(Point L[MAX_SIZE][MAX_SIZE], int size); //Fill lattice L with points of colour 'w', label 0, and {x,y} their positions in the lattice.

void print_lattice(Point L[MAX_SIZE][MAX_SIZE], int size); // Prints the lattice the stdout.

void rewrite_labels(int* cluster_labels, int old_label, int new_label); // Links 2 clusters by changing the proper label of cluster with label "old_label" to "new_label"

int find_spanning_cluster(Point L[MAX_SIZE][MAX_SIZE], int size, int* cluster_labels); // Finds the spanning cluster of the lattice, and outputs the cluster label. Outputs 0 otherwise.

int find_proper_label(int* cluster_labels, int c); // Gets the proper label of a cluster.

double mean(double* data, int size); // Gets the mean of an array of data of size "size".

bool F_calculation_to_file(const char* filename, double* data, int n); // Basically outputs all the data in the array "data" of size "n" to given filename, delimited by newlines.
