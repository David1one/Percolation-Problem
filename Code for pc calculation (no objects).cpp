/*This is to prettify & remove extraeneous comments. Refer to v2 for many tests and extra comments.*/

#include <iostream>
#include <iomanip>
#include <random> // Contains RNG
#include <algorithm> // Contains sort & binary_search algorithms
#include <fstream> // This is needed for I/O

using namespace std;

//Constants
const int MAX_SIZE = 100;
const int N_NEIGHBOURS = 4; //Number of neighbours each lattice element has. But get_distinct_neighbours must be updated manually.


//Functions
int random(mt19937 &mt_rand, int size) {
	/*This function returns an integer in the range [0, size-1]. Uses Mersenne Twister to do so.*/

	uniform_int_distribution<int> mint(0, size - 1);
	return mint(mt_rand);
}



void print_lattice(int L[MAX_SIZE][MAX_SIZE], int size) {
	/* This function will basically print the lattice of size "size" to the console.*/

	for (int i = 0; i < size; i++) { // Every element in row i

		for (int j = 0; j < size; j++) { // Every element in column j
			// We're keeping i constant as we pass through each column. Once the final column is reached,
			// a newline is printed, and we go through the next row.

			cout << setw(4) << L[i][j] << " "; // (0,0) (0,1) (0,2) ... (0,9) , (1,0), (1,1)... (9,8), (9,9)
		
		}
		
		cout << endl; //Move onto next line

	}

}



void initialise_lattice(int L[MAX_SIZE][MAX_SIZE], int size) {
	/* Initialise all the values in the 2D array to zero. */

	for (int i = 0; i < size; i++) { //Every element in row
		for (int j = 0; j < size; j++) { //Every element in column
			L[i][j] = 0; //Make it zero
		}
	}

}



int find_proper_label(int* cluster_labels, int c) {
	/* c is the cluster of which we need to find the proper label. Keep going deeper into list until positive number found.
	Assume cluster_labels points to [0, 1, 2, 3 ...].
	Furthermore, the function optimises cluster_labels at the end of each call, by making all entries it viewed into a most direct reference.
	Ideally, 0th position will never be referenced, but it fails safely if it is.*/

	// a & i will keep a track of the entries that must be changed at the end of the loop.
	int* a = new int[c + 1]; //Max number of cluster labels that may be viewed, as the proper label will always be less than or equal to assigned label.
	int i = 0; //Keeps track of how many elements viewed.

	while (cluster_labels[c] < 0) { //As long as the proper label is not found

		// Add current cluster number to the list of things to be changed.
		a[i] = c;
		i++;

		// Change to go to referenced location in next iteration.
		// As the value being looked at is actually negative, change it so c will be positive.
		c = -cluster_labels[c];
	}

	// This may be unneeded. But just verifies that c is positive.
	if(c<0) c = -c;

	// Optimise cluster_labels, by making each reference viewed into a most direct reference.
	// It is known that last value stored is already a most direct reference, so go through all elements in a except last one. ie. j in range [0,i-1)
	// Change the value in cluster_labels for each wanted value in a to -proper_label.
	for (int j = 0; j < i - 1; j++) {
		cluster_labels[a[j]] = -c;
	}

	delete[] a; // Drop dynamic memory

	return c; // Return proper label

}



void rewrite_labels(int* cluster_labels, int old_label, int new_label) {
	cluster_labels[old_label] = -new_label; //Changes old_label into a reference to show it's no longer a proper label.
}



int get_distinct_neighbours(int L[MAX_SIZE][MAX_SIZE], int size, int* cluster_labels, int x, int y, int distinct_neighbours[N_NEIGHBOURS]) {
	/*This looks up, down, left and right, and saves the cluster numbers in an array.
	Then it sorts the list.
	Then it removes any zeros, and removes repetition.
	It saves these assigned labels, and returns the number of these labels.*/

	// neighbours has N-NEIGHBOURS elements.
	// Structure: {left_x, right_x, bottom_y, top_y}
	int neighbours[N_NEIGHBOURS];

	// As a placeholder, if our node is on the edge of the lattice, its non-existent neighbour will be assigned 0.
	// Otherwise, get the proper label of surrounding clusters.

	if (x == 0) neighbours[0] = 0; //If on left edge
	else neighbours[0] = find_proper_label(cluster_labels, L[x - 1][y]);
	
	if (x == size - 1) neighbours[1] = 0; //If on right edge
	else neighbours[1] = find_proper_label(cluster_labels, L[x + 1][y]);

	if (y == 0) neighbours[2] = 0; //If on bottom edge
	else neighbours[2] = find_proper_label(cluster_labels, L[x][y - 1]);

	if (y == size - 1) neighbours[3] = 0; //If on top edge
	else neighbours[3] = find_proper_label(cluster_labels, L[x][y + 1]);


	sort(neighbours, neighbours + N_NEIGHBOURS); // Sorts the list

	// Remove duplicate labels in list, exploting the fact that the list is sorted.
	// Saves the cluster and increments the number of clusters if its distinct & nonzero.
	int distinct_clusters = 0;

	if (neighbours[0] != 0) { //Special case: No previous element to check against.
		distinct_neighbours[0] = neighbours[0]; // Save element.
		distinct_clusters++; // Increment number of clusters
	}

	for (int i = 1; i < N_NEIGHBOURS; i++) {

		//Same as above, but extra check.
		if (neighbours[i] != 0 && neighbours[i] != neighbours[i - 1]) { //If nonzero, and distinct.
			distinct_neighbours[distinct_clusters] = neighbours[i]; //Save element. Next free slot in distinct_neighbours indexed by distinct_clusters.
			distinct_clusters++; // Increment number of clusters
		}
	}

	//Array of distinct neighbours is now sorted & saved.
	//Return number of distinct assigned cluster labels
	return distinct_clusters; //Return the number of distinct clusters.
}



int find_spanning_cluster(int L[MAX_SIZE][MAX_SIZE], int size, int* cluster_labels) {
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

		if (L[0][i] != 0) { // Top edge
			top_edge[n_top_edge] = find_proper_label(cluster_labels, L[0][i]); // Save proper label
			n_top_edge++;
		}

		if (L[size - 1][i] != 0) { // Bottom edge
			bottom_edge[n_bottom_edge] = find_proper_label(cluster_labels, L[size - 1][i]);
			n_bottom_edge++;
		}

		if (L[i][0] != 0) { // Left edge
			left_edge[n_left_edge] = find_proper_label(cluster_labels, L[i][0]);
			n_left_edge++;
		}

		if (L[i][size - 1] != 0) { // Right edge
			right_edge[n_right_edge] = find_proper_label(cluster_labels, L[i][size - 1]);
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
	int distinct_clusters = 0;
	int* distinct_top_edge = new int[n_top_edge]; //At max, all the collected top edge clusters are distinct.
	// Doesn't complain if n_top_edge == 0, so it's ok.

	if (n_top_edge != 0) { // If some elements exist in the list

		// Special case: It is already known that first element is going to be in final list.
		distinct_top_edge[0] = top_edge[0];
		distinct_clusters++;

		// For the rest of the elements, check they're not duplicates.
		for (int i = 1; i < n_top_edge; i++) {

			if (top_edge[i] != top_edge[i - 1]) { //If distinct.
				distinct_top_edge[distinct_clusters] = top_edge[i]; //Next free slot in distinct_top_edge indexed by distinct_clusters.
				distinct_clusters++;
			}
		}

	}

	// If top edge has 0 clusters, then there's no point in doing binary searches - none of the clusters touch the top edge.
	// However, if it's nonzero, then can check for existence of cluster number.
	// binary_search(data, data+size, value) is a function that returns true if the value is in the sorted array, and false if not, and runs in lg(size) time.

	int val; // Stores current cluster to check.
	for (int i = 0; i < distinct_clusters; i++) { // For each distinct cluster on top edge
		val = distinct_top_edge[i];

		// Binary search each list for specified value.
		// Note that the cluster numbers are not distinct here, but that's not a problem.
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

	return label; // Return spanning cluster label
}



bool on_edge(int x, int y, int size) { //Just says if it's on an edge or not.
	if (x == 0 || x == size - 1 || y == 0 || y == size - 1) return true;
	else return false;
}



double pc_calculation(int L[MAX_SIZE][MAX_SIZE], int size) {
	/* Basically does a running total of all occupied sites, then returns the ratio. */
	
	int total = 0;
	for (int i = 0; i < size; i++) { //Every element in row i

		for (int j = 0; j < size; j++) { //Every element in column j

			if (L[i][j] != 0) total++; //Increment number of occupied sites

		}

	}

	// Calculate pc, and return it.
	// pc = occupied sites / total sites.
	return (double)total / (size*size);
}



bool pc_calculation_to_file(const char* filename, double* data, int n) {
	/*Create a csv file and put data from pc array into it, delimited by newlines.*/

	ofstream outfile(filename, ios::out); // Create output file.

	if (!outfile) { // If I can’t make file. IMPORTANT – MAY BE DENIED PERMISSION.
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

//pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile.txt", data, n);



bool print_lattice_to_file(const char* filename, int L[MAX_SIZE][MAX_SIZE], int size) {
	/* Same as print_lattice, except to a file. */

	ofstream outfile(filename, ios::out); // Create output file

	if (!outfile) { // If I can’t make file. IMPORTANT – MAY BE DENIED PERMISSION.
		cout << "Failed to create file" << endl;
		return EXIT_FAILURE;
	}

	// Replace cout in print_lattice with outfile
	for (int i = 0; i < size; i++) { // Every element in row i

		for (int j = 0; j < size; j++) { // Every element in column j
			outfile << setw(4) << L[i][j] << " "; // Output the element, and a space.
		}
		outfile << endl;
	}

	outfile.close(); // Close the file

	return EXIT_SUCCESS;
}

//Associated command:
//print_lattice_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\basic_outfile.txt", L, size);
//Since you forget to leave it open: C:\Mindmaps\UCC_Lectures\Computational_physics\Percolation\Lattices



double generate_lattice(int size, mt19937 &mt_rand) {

	int L[MAX_SIZE][MAX_SIZE]; // Our lattice has to be the same size as the indexes declared in header file.
	initialise_lattice(L, size); // Set all our lattice values to zero.

	int x, y; // Placeholders for randomly generated indexes
	int cluster_ID = 1; // Holds the value of the highest cluster not yet initialised.
	int neighbours[4]; // Array to hold the cluster labels of all neighbouring clusters.
	int n_neighbours; // n_neighbours is number of distinct clusters surrounding a lattice element.
	int spanning_cluster = 0; // Label for spanning cluster
	int new_label; // For rewriting the labels of the clusters if a bridge is formed.

	int* cluster_labels = new int[size*size+1]; // Max labels in a lattice. Needs 0, and a space for each element in the lattice.
	for (int i = 0; i < size*size + 1; i++) cluster_labels[i] = i; // Initialise all the clusters to their proper label.

	// Iterate until a spanning cluster found.
	while (true) {

		do { x = random(mt_rand, size); y = random(mt_rand, size); } while (L[x][y] != 0); // Keeps generating random x & y until we find an unoccupied site.

		n_neighbours = get_distinct_neighbours(L, size, cluster_labels, x, y, neighbours); //Saves the neighbouring clusters into neighbours, and returns the number of them.
		
		// Decide on behaviour depending on number of neighbours.
		if (n_neighbours == 0) { // It's a new cluster
			L[x][y] = cluster_ID; // Assign a new cluster number to that element.
			cluster_ID++; // Increment the new max cluster number.
		}

		else if (n_neighbours == 1) { // Link it with its sole neighbour, and check if spanning cluster exists iff on an edge.

			L[x][y] = neighbours[0]; // Set the cluster id to the only neighbouring cluster

			if (on_edge(x, y, size)) { // First check if it's on the edge.
				
				spanning_cluster = find_spanning_cluster(L, size, cluster_labels); // Save possible spanning cluster to spanning_cluster
				
				// Terminate loop if a spanning cluster is found.
				if (spanning_cluster != 0) { //If spanning cluster found.
					break;
				}

			}

		}

		else if (n_neighbours >= 2) { // Bridge between 2 clusters

			new_label = neighbours[0]; // Proper label of conjoined clusters.
			L[x][y] = new_label; // Label newest occupied site with correct label.

			// Relabel each neighbour with its new proper label.
			for (int i = 1; i < n_neighbours; i++) rewrite_labels(cluster_labels, neighbours[i], new_label); 

			// Check for spanning cluster
			spanning_cluster = find_spanning_cluster(L, size, cluster_labels); //Save possible spanning cluster to spanning_cluster
			if (spanning_cluster != 0) { //If spanning cluster found.
				break;
			}

		}

	}

	// Release dynamic memory.
	delete[] cluster_labels;

	//Calculate pc, & return it.
	return pc_calculation(L, size);

}



void ensemble_lattice(double* data, int size, int nens, mt19937 &mt_rand) {
	/* Do nens lattice simulations, and store their pcs into an array called data. */

	for (int i = 0; i < nens; i++) {
		data[i] = generate_lattice(size, mt_rand); //Save the associated pc
	}

}



double mean(double* data, int size) {
	/* Implements standard formula for calculating the mean of a list. */

	double sum = 0; // Keep a running total
	for (int j = 0; j < size; j++)
		sum += data[j]; // Add together all the terms
	return sum / size; // Divide by number of elements, return.

}









int main() {

	random_device rt;
	mt19937 mt_rand(rt()); //Initialise the RNG: The Mersenne Twister.
	//const int size = 20; //Assume size>1. size=1 fails.
	const int nens = 1;
	const int n_pc_means = 1000;

	double* pcs = new double[nens];
	double* pc_means = new double[n_pc_means];
	int it = 0;

	cout << generate_lattice(5, mt_rand) << endl; //For some reason, size = 200 won't work.
	//generate_lattice()

//	for (int size = 200; size <= 6400; size *= 2) {
//		ensemble_lattice(pcs, size, nens, mt_rand);
//		pc_means[it] = mean(pcs, nens);
//		cout << size << endl;
//		cout << pc_means[it] << endl;
//		it++;
//	}

	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_advanced_outfile.txt", pc_means, 20);

	
	delete[] pcs;
	delete[] pc_means;

	system("PAUSE");
	return 0;
}


/*ensemble_lattice(pcs, 5, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_5.csv", pcs, nens);
	pcs_means[0] = mean(pcs, nens);

	ensemble_lattice(pcs, 10, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_10.csv", pcs, nens);
	pcs_means[1] = mean(pcs, nens);

	ensemble_lattice(pcs, 15, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_15.csv", pcs, nens);
	pcs_means[2] = mean(pcs, nens);

	ensemble_lattice(pcs, 20, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_20.csv", pcs, nens);
	pcs_means[3] = mean(pcs, nens);

	ensemble_lattice(pcs, 25, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_25.csv", pcs, nens);
	pcs_means[4] = mean(pcs, nens);

	ensemble_lattice(pcs, 50, nens, mt_rand);
	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfile_50.csv", pcs, nens);
	pcs_means[5] = mean(pcs, nens);

	pc_calculation_to_file("C:\\Mindmaps\\UCC_Lectures\\Computational_physics\\Percolation\\Lattices\\pc_basic_outfiles_means.csv", pcs_means, 10);*/

/*for (int size = 5; size <= 100; i+5) {
		ensemble_lattice(pcs, size, nens, mt_rand);
		pc_means[i] = mean(pcs, nens);
		cout << i << endl;
		i++;
	}

	for (int i = 0; i < 20; i++) {
		cout << pc_means[i] << endl;
	}
*/