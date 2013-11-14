//  Example of using Graph library
#include "Graph.h"

int main()
{
	srand(time(NULL));
	ShortestPathAlgorithm spa;

	Graph G(50, 0.5, 1.0, 10.0);

	double len;
	auto mST = G.PrimMST(len);
	
	double average = spa.AverageShortestPath(G, 0);

	return 0;
}