///  Contains Graph related classes declaration

#ifndef GRAPH_H__
#define GRAPH_H__

#include "PriorityQueue.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <list>
#include <utility>
#include <string>
#include <fstream>

using std::vector;
using std::list;
using std::pair;
using std::string;
using std::ifstream;
using std::ios_base;

double GenerateRandomDouble(double dMin, double dMax);

//  This class implements an Edge TO a vertex with a given weight.
//  It doesn't have to store a number of the FROM vertex because these Edges be stored in a list container
//  which is linked with a FROM vertex by the index in the m_EdgeList vector member of the Graph class
class Edge
{
private:
	unsigned int m_startVertex;
	unsigned int m_endVertex;
	double m_Weight;
public:
	//  default constructor needed to use the Edge class in STL containers
	Edge();
	Edge(unsigned int startVertex, unsigned int endVertex, double weight);
	~Edge();

	//  Getters
	unsigned int GetStartVertexNumber() const;
	unsigned int GetEndVertexNumber() const;
	double GetEdgeWeight() const;

	//  Setters
	void SetEdgeWeight(double weight);
};

//  This class implements the Graph
//  It uses adjacency list to represent Graph. Each element in the m_EdgeList vector
//  represents a vertex. Index of the element equals the vertex number.
class Graph
{
private:
	vector<list<Edge>> m_EdgeList;
	unsigned int m_EdgesAmount;
public:
	//  Construct a graph that does not have edges, only nodes.
	//  explicit keyword because we don't want initializations like Graph g = 1; happen
	explicit Graph(unsigned int size);
	//  Construct a graph of the given size, the given density and distance between nodes
	//  ranging from distance_min to distance_max
	//  This constructor is used instead of a generation procedure from the assignment details
	Graph(unsigned int size, double density, double distance_min, double distance_max);
	//  Read graph from a file
	Graph(const string &filename);
	//  The destructor
	~Graph();

	//  Get number of vertices in the Graph
	unsigned int GetVerticesAmount() const;
	//  Get the number of edges between vertices in the Graph
	unsigned int GetEdgesAmount() const;
	//  Isn't useful since we have node value == its number. But can be useful if we change this approach
	unsigned int GetNodeValue(unsigned int v1) const;
	//  Get edge weight by its vertices
	double GetEdgeValue(unsigned int v1, unsigned int v2) const;
	//  Check if vertices are adjacent
	bool Adjacent(unsigned int v1, unsigned int v2) const;
	//  Returns a vertex adjacency list. Return value is a const & because of performance reason
	const list<Edge> &GetNodeEdges(unsigned int v) const;

	//  Since we have node value == its number this function is empty. But it can be changed later
	void SetNodeValue(unsigned int v1, double value);
	//  Change edge weight
	void SetEdgeValue(unsigned int v1, unsigned int v2, double value);
	//  Add an edge to the Graph
	void AddEdge(unsigned int v1, unsigned int v2, double distance);
	void AddEdge(const Edge& edge);
	//  Delete an edge from the Graph
	void DeleteEdge(unsigned int v1, unsigned int v2);
	//  Prim's algorithm. A tree is a graph so the result is of the Graph class
	Graph PrimMST(double &length);
};

//  This class implements a path on the Graph
//  It stores the list of vertices in the Path and
//  its weight is a sum of weights of the containing edges
class Path
{
private:
	list<unsigned int> m_Path;
	double m_Weight;
public:
	//  Construct a path to the vertex itself (weight == 0)
	Path(unsigned int start);
	//  Copy a path
	Path(const Path &path);
	//  Copy a path and add an edge to it (continue path)
	Path(const Path &path, const Edge &edge);

	//  Get the path itself
	const list<unsigned int> &GetPath() const;
	//  Get path weight
	double GetWeight() const;
	//  Get the last vertex of the path
	unsigned int GetFinalVertex() const;
	//  Overloaded operator== to compare paths
	//  IMPORTANT: it compares only paths. Their weights are not used in the comparasion
	bool operator==(const Path &path) const;

	//  Add a vertex to the path
	void AddVertex(const Edge &edge);
};

//  This class implements Dijkstra shortest path algorithm.

//  It stores a priority queue of vertices with their weight as the close set to find shortest path length
//  and a priority queue of paths to get the shortest path itself (Path class)
//  It stores the list of vertices and their respectful weights as the open set.

//  This class has 3 different methods to get the Shortest Path Length, the Average Shortest Path Length and
//  the Shortest Path. Implementation of all of them differs a little because of the performance issues
//  For example, tests have shown that using this implementation of the average path length calculation gives more than 20%
//  of speed upgrade compared to the method of calling GetShortestPathLength for u and each vertex in the Graph
class ShortestPathAlgorithm
{
private:
	vector<unsigned int> m_OpenSet;
	PriorityQueue<unsigned int, double> m_CloseSet;
	PriorityQueue<Path, double> m_PathCloseSet;

	//  Check if the vertex is already is in the open set
	bool OpenSetContains(unsigned int vertex) const;
public:
	ShortestPathAlgorithm();
	~ShortestPathAlgorithm();

	//  Get the shortest Path from the vertex u to the vertex v on the Graph G
	Path GetShortestPath(const Graph &G, unsigned int u, unsigned int v);
	//  Get the shortest Path Length from the vertex u to the vertex v on the Graph G
	double GetShortestPathLength(const Graph& G, unsigned int u, unsigned int v);
	//  Get the average shortest Path Length of the v u of the Graph G
	double AverageShortestPath(const Graph &G, unsigned int u);
};

#endif