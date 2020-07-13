#include "Point.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <string>

using namespace std;


bool randreal(mt19937 &mt_rand, double p) {
	/*Returns true with probability p.*/
	
	// Sanity check: p in range [0,1], as is necessary for a probability.
	if (p<0 || p>1) {
		cout << "ERROR: p must be a double in the range [0,1]" << endl;
		return false;
	}
	
	uniform_real_distribution<double> mint(0, 1);

	if (mint(mt_rand) <= p) return true;
	else return false;
}

double F_calculation(const int size, const double p, mt19937 &mt_rand) {
	/*This creates 1 lattice with occupation probability p, calculates F for the spanning cluster in that lattice, and returns it.*/

	Point L[MAX_SIZE][MAX_SIZE]; // Initialise lattice of points.
	initialise_lattice(L, size); // Initialise lattice as described in "initialise_lattice".

	int cluster_ID = 1; // Holds the value of the highest cluster not yet initialised.
	int spanning_cluster = 0; // Label for spanning cluster

	int* cluster_labels = new int[size*size + 1]; // Max number of labels in a lattice = size*size + 1. Why? It needs a place for 0, and a space for each element in the lattice.
	for (int i = 0; i < size*size + 1; i++) cluster_labels[i] = i; // Initialise cluster_labels, setting the proper label of each cluster to its current assigned label.

	// Make lattice of occuptation probability p. 
	for (int i = 0; i < size; i++) { // Every element in row i

		for (int j = 0; j < size; j++) {

			// Makes a new cluster with probability p.
			if (randreal(mt_rand, p)) {
				L[i][j].set_val(cluster_ID);
				cluster_ID++;
			}

		}
	}

	// Do bfs on each element to do the cluster-relabeling algorithm.
	// This loop leverages the invariant that every subsequent element is either zero, or has a value greater than the value of the current element.
	// This way, the property that the proper label of a cluster is the minimum cluster label found in that cluster is preserved.
	for (int i = 0; i < size; i++) {

		for (int j = 0; j < size; j++) {

			// If a nonzero element still has not been explored, then explore it & its neighbours, and link them together under 1 proper label.
			if (L[i][j].get_val() != 0 && L[i][j].get_colour() == 'w') {
				bfs(L, size, L[i][j], cluster_labels);
			}

		}
	}

	// Get the spanning cluster
	spanning_cluster = find_spanning_cluster(L, size, cluster_labels);

	//Error handling if no spanning cluster was made.
	if (spanning_cluster == 0) {
		//cout << "ERROR: No spanning cluster made with probability " << p << " in lattice." << endl;
		delete[] cluster_labels;
		return -1;
	}


	//Calculate F. F = sites in spanning cluster / occupied sites.
	int total = 0; // Total number of occupied sites.
	int spanning_total = 0; // Total number of occupied sites in spanning cluster.
	for (int i = 0; i < size; i++) { //Every element in row i

		for (int j = 0; j < size; j++) { //Every element in column j

			if (find_proper_label(cluster_labels, L[i][j].get_val()) != 0) total++; // Increment number of total occupied sites if cluster number is nonzero.
			if (find_proper_label(cluster_labels, L[i][j].get_val()) == spanning_cluster) spanning_total++; //Increment number of sites in the spanning cluster if it's part of the spanning cluster.

		}

	}

	// Drop dynamic memory.
	delete[] cluster_labels;

	// Calculate F, and return it.
	return (double)spanning_total / total;
}



void ensemble_F(double* data, int size, double p, int nens, mt19937 &mt_rand) {
	/* Do nens lattice simulations, and store their Fs into an array called data. */

	int i = 0;
	double val;

	// Keep on iterating until we have nens values of F.
	while (i < nens) {
		val = F_calculation(size, p, mt_rand);
		if (val > 0) { //If a spanning cluster has been gotten in this particular case.
			data[i] = val; //Save the associated F
			i++;
		}

	}

}



int main() {

	random_device rt;
	mt19937 mt_rand(rt());

	int size = 80;
	double p;
	const int nens = 20;
	double p_means[50];
	int i = 0;

	//nens = 2000, size = 50, save from .6 to 1.

	double data[nens];

	for (p = .592; p < .6; p += .001) {
		ensemble_F(data, size, p, nens, mt_rand);
		//for (int j = 0; j < nens; j++) cout << data[j] << endl;
		p_means[i] = mean(data, nens);
		i++;
		cout << p << endl;
	}

	for (p = .6; p <= 1; p += .01) {
		ensemble_F(data, size, p, nens, mt_rand);
		//for (int j = 0; j < nens; j++) cout << data[j] << endl;
		p_means[i] = mean(data, nens);
		i++;
	}

	for (int j = 0; j < i; j++) cout << p_means[j] << endl;

	F_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\F-size=80-nens=20.csv", p_means, i);

	system("PAUSE");
	return 0;
}
