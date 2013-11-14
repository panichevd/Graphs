///  Contains Graph related classes implementation
#include "Graph.h"

//  This function generates a random double between dMin and dMax
double GenerateRandomDouble(double dMin, double dMax)
{
	double d = static_cast<double>(rand()) / RAND_MAX;
	return dMin + d * (dMax - dMin);
}

Edge::Edge() : m_startVertex(UINT_MAX), m_endVertex(UINT_MAX), m_Weight(DBL_MAX)
{
}

Edge::Edge(unsigned int startVertex, unsigned int endVertex, double weight) : m_startVertex(startVertex), m_endVertex(endVertex), m_Weight(weight)
{
}

Edge::~Edge()
{
}

unsigned int Edge::GetStartVertexNumber() const
{
	return m_startVertex;
}

unsigned int Edge::GetEndVertexNumber() const
{
	return m_endVertex;
}

double Edge::GetEdgeWeight() const
{
	return m_Weight;
}

void Edge::SetEdgeWeight(double weight)
{
	m_Weight = weight;
}

Graph::Graph(unsigned int size) : m_EdgeList(size), m_EdgesAmount(0)
{
}

//  This construct uses this approach to generate graph with the given density: We generate
//  a random double between 0 and 1. If it is less than the density then create an edge.
//  It means the edge in generated in density cases of 1 (or in density % cases).
//  It equals that graph has the given density
Graph::Graph(unsigned int size, double density, double distance_min, double distance_max) : m_EdgeList(size), m_EdgesAmount(0)
{
	double random_propability, random_distance;
	random_propability = random_distance = 0.0;

	for (size_t i = 0; i < m_EdgeList.size(); ++i)
	{
		m_EdgeList[i].push_back(Edge(i, i, 0));  //  a path to itself always exists
		m_EdgesAmount++;
		for (size_t j = i + 1; j < m_EdgeList.size(); ++j)
		{
			random_propability = GenerateRandomDouble(0.0, 1.0);
			if (random_propability < density)
			{
				random_distance = GenerateRandomDouble(distance_min, distance_max);
				//  graph is undirected so we can go from i to j and from j to i
				m_EdgeList[i].push_back(Edge(i, j, random_distance));
				m_EdgeList[j].push_back(Edge(j, i, random_distance));
				m_EdgesAmount += 2;
			}
		}
	}
}


Graph::Graph(const string &filename) : m_EdgesAmount(0)
{
	ifstream fin(filename, ios_base::in);

	if (fin.good())
	{
		int size, v1, v2, len;
		fin >> size;
		if (fin.good())
		{
			m_EdgeList = vector<list<Edge>>(size);
			while (!fin.eof())
			{
				fin >> v1 >> v2 >> len;
				if (fin.eof())
					break;
				m_EdgeList[v1].push_back(Edge(v1, v2, len));
				m_EdgesAmount++;
			}
		}
	}
	fin.close();
}

Graph::~Graph()
{
}

unsigned int Graph::GetVerticesAmount() const
{
	return m_EdgeList.size();
}

unsigned int Graph::GetEdgesAmount() const
{
	return m_EdgesAmount;
}

unsigned int Graph::GetNodeValue(unsigned int v1) const
{
	return v1;
}

double Graph::GetEdgeValue(unsigned int v1, unsigned int v2) const
{
	if (Adjacent(v1, v2))
	{
		for (auto it = m_EdgeList[v1].begin(); it != m_EdgeList[v1].end(); ++it)
			if (it->GetEndVertexNumber() == v2)
				return it->GetEdgeWeight();
	}

	//  if there is no edge return negative value
	return -1;
}

bool Graph::Adjacent(unsigned int v1, unsigned int v2) const
{
	//  + 1 because of array indexes start with 0 (otherwise v == 50 would pass this check if list size is 50, but the last valid index is 49)
	if (v1 > GetVerticesAmount() + 1 || v2 > GetVerticesAmount() + 1)
		return false;

	//  If v2 is in the list of neighboors of v1 then they are adjacent nodes
	for (auto it = m_EdgeList[v1].begin(); it != m_EdgeList[v1].end(); ++it)
		if (it->GetEndVertexNumber() == v2)
			return true;

	return false;
}

//  Get the list of edges that start in the given node
const list<Edge> &Graph::GetNodeEdges(unsigned int v) const
{
	//  We will assign it to a const list<>& so the temorary object will continue living
	if (v > GetVerticesAmount() + 1)
		return list<Edge>(0);

	return m_EdgeList[v];
}

void Graph::AddEdge(unsigned int v1, unsigned int v2, double distance)
{
	//  don't need to check whether v1 or v2 are more of the edges amount or not because it's done in the beginning of the Adjacent method
	if (!Adjacent(v1, v2))
	{
		m_EdgeList[v1].push_back(Edge(v1, v2, distance));
		m_EdgeList[v2].push_back(Edge(v2, v1, distance));
		m_EdgesAmount++;
	}
}

void Graph::AddEdge(const Edge &edge)
{
	AddEdge(edge.GetStartVertexNumber(), edge.GetEndVertexNumber(), edge.GetEdgeWeight());
}

void Graph::DeleteEdge(unsigned int v1, unsigned int v2)
{
	//  cannot erase path to itself
	if (v1 == v2)
		return;

	//  don't need to check whether v1 or v2 are more of the edges amount or not because it's done in the beginning of the Adjacent method
	if (Adjacent(v1, v2))
	{
		for (auto it = m_EdgeList[v1].begin(); it != m_EdgeList[v1].end(); ++it)
			if (it->GetEndVertexNumber() == v2)
			{
				m_EdgeList[v1].erase(it);
				break;
			}

		for (auto it = m_EdgeList[v2].begin(); it != m_EdgeList[v2].end(); ++it)
			if (it->GetEndVertexNumber() == v1)
			{
				m_EdgeList[v2].erase(it);
				break;
			}

		m_EdgesAmount--;
	}
}

void Graph::SetNodeValue(unsigned int v1, double value)
{
	//  just a stub for now. We don't need any implementation for now cause node number == its value
}

void Graph::SetEdgeValue(unsigned int v1, unsigned int v2, double value)
{
	if (Adjacent(v1, v2))
	{
		for (auto it = m_EdgeList[v1].begin(); it != m_EdgeList[v1].end(); ++it)
			if (it->GetEndVertexNumber() == v2)
			{
				it->SetEdgeWeight(value);
				return;
			}
	}
}

Graph Graph::PrimMST(double &length)
{
	Graph G(GetVerticesAmount());
	list<unsigned int> closeSet;
	PriorityQueue<Edge, double> PQ;
	//  we should check if the Graph is disconnected and return the tree of 0 size in that case
	bool bDisconnected = true;

	length = 0;
	if (GetVerticesAmount() == 0)
		return G;
	
	//  Start with a single vertex
	closeSet.push_back(0);
	for (auto it = m_EdgeList[0].begin(); it != m_EdgeList[0].end(); ++it)
		PQ.Insert(*it, it->GetEdgeWeight());

	//  Algorithm should work while the resulting tree doesn't contain all nodes 
	//  and Graph is not disconnected (second condition)
	while (closeSet.size() != G.GetVerticesAmount() && PQ.Size() > 0)
	{
		bDisconnected = true;
		Edge e;
		//  Get an unvisited vertex with the highest priority
		while (PQ.Size() > 0)
		{
			e =  PQ.Top();
			PQ.Pop();

			if (find(closeSet.begin(), closeSet.end(), e.GetEndVertexNumber()) == closeSet.end())
			{
				bDisconnected = false;
				break;
			}
		}

		if (bDisconnected)
			break;

		//  Add the vertex to the tree and its edges to the queue
		G.AddEdge(e);
		length += e.GetEdgeWeight();
		closeSet.push_back(e.GetEndVertexNumber());
		for (auto it = m_EdgeList[e.GetEndVertexNumber()].begin(); it != m_EdgeList[e.GetEndVertexNumber()].end(); ++it)
			PQ.Insert(*it, it->GetEdgeWeight());
	}

	if (!bDisconnected)
		return G;
	else
	{
		length = DBL_MAX;
		return Graph(0);
	}
}

Path::Path(unsigned int start) : m_Weight(0.0)
{
	m_Path.push_back(start);
}

Path::Path(const Path &path) : m_Path(path.m_Path), m_Weight(path.m_Weight)
{
}

//  Continue the path
Path::Path(const Path &path, const Edge &edge) : m_Path(path.m_Path)
{
	m_Path.push_back(edge.GetEndVertexNumber());
	m_Weight = path.m_Weight + edge.GetEdgeWeight();
}

double Path::GetWeight() const
{
	return m_Weight;
}

unsigned int Path::GetFinalVertex() const
{
	return m_Path.back();
}

const list<unsigned int> &Path::GetPath() const
{
	return m_Path;
}

//  Two paths are equal if their final vertices. This means we can use this function to understand 
//  if there already is a path to this vertex in the close set. It helps for the algo to work faster.
bool Path::operator==(const Path& path) const
{
	if (m_Path.size() != path.m_Path.size())
		return false;

	return GetFinalVertex() == path.GetFinalVertex();
}

void Path::AddVertex(const Edge &edge)
{
	m_Path.push_back(edge.GetEndVertexNumber());
	m_Weight += edge.GetEdgeWeight();
}

ShortestPathAlgorithm::ShortestPathAlgorithm() : m_CloseSet(), m_OpenSet()
{
}

ShortestPathAlgorithm::~ShortestPathAlgorithm()
{
}

//  Check if the open set contains the vertex
bool ShortestPathAlgorithm::OpenSetContains(unsigned int vertex) const
{
	for (auto it = m_OpenSet.begin(); it != m_OpenSet.end(); ++it)
		if (*it == vertex)
			return true;
	return false;
}

//  Get Shortest Path LENGTH from u to v
double ShortestPathAlgorithm::GetShortestPathLength(const Graph& G, unsigned int u, unsigned int v)
{
	m_CloseSet = PriorityQueue<unsigned int, double>();
	m_OpenSet.resize(0);

	const list<Edge> &firstVNeighbors = G.GetNodeEdges(u);
	//  Add u to the open set
	m_OpenSet.push_back(u);
	//  Add all the neighboors of u to the close set
	for (auto it = firstVNeighbors.begin(); it != firstVNeighbors.end(); ++it)
		m_CloseSet.Insert(it->GetEndVertexNumber(), it->GetEdgeWeight());

	//  While we can find a path from u to v
	while (!m_CloseSet.Empty())
	{
		//  Get the vertex with the best weight
		unsigned int vertex = m_CloseSet.Top();
		double priority = m_CloseSet.GetTopPriority();
		m_CloseSet.Pop();

		//  If it is v we're over (Dijkstra algoritm guarantees this path's the shortest)
		if (vertex == v)
			return priority;

		//  If this vertex is not in the open set (it equals there is no shorter path to this vertex)
		if (!OpenSetContains(vertex))
		{
			m_OpenSet.push_back(vertex);

			const list<Edge> & neighbors = G.GetNodeEdges(vertex);
			for (auto it = neighbors.begin(); it != neighbors.end(); ++it)
			{
				if (!OpenSetContains(it->GetEndVertexNumber()))
				{
					//  Inserting the vertex if it is not in the queue already or
					//  if its weight is less than a weight of another path to this vertex
					//  (i.e. this path is better than any prior paths to this vertex found)
					m_CloseSet.InsertIfPriorityLess(it->GetEndVertexNumber(), priority + it->GetEdgeWeight());
				}
			}
		}
	}

	return -1;
}

//  Get the AVERAGE of shortest paths from u to other vertices
//  This method computes a shortest path from u to any vertex in the Graph
//  Then it computes the average
double ShortestPathAlgorithm::AverageShortestPath(const Graph &G, unsigned int u)
{
	m_CloseSet = PriorityQueue<unsigned int, double>();
	m_OpenSet.resize(0);

	double sum = 0.0;
	const list<Edge> &firstVNeighbors = G.GetNodeEdges(u);
	//  Add u to the open set
	m_OpenSet.push_back(u);
	//  Add all the neighboors of u to the close set
	for (auto it = firstVNeighbors.begin(); it != firstVNeighbors.end(); ++it)
		m_CloseSet.Insert(it->GetEndVertexNumber(), it->GetEdgeWeight());

	//  While we can find a path from u to v
	while (!m_CloseSet.Empty())
	{
		//  Get the path with the best priority
		unsigned int vertex = m_CloseSet.Top();
		double priority = m_CloseSet.GetTopPriority();
		m_CloseSet.Pop();

		//  If this vertex is not in the open set (it equals there is no shorter path to this vertex)
		if (!OpenSetContains(vertex))
		{
			m_OpenSet.push_back(vertex);
			//  Add this weight to the sum
			sum += priority;

			const list<Edge> & neighbors = G.GetNodeEdges(vertex);
			for (auto it = neighbors.begin(); it != neighbors.end(); ++it)
			{
				if (!OpenSetContains(it->GetEndVertexNumber()))  //  1. find 2. second check
				{
					//  Inserting the vertex if it is not in the queue already or
					//  if its weight is less than a weight of another path to this vertex
					//  (i.e. this path is better than any prior paths to this vertex found)
					m_CloseSet.InsertIfPriorityLess(it->GetEndVertexNumber(), priority + it->GetEdgeWeight());
				}
			}
		}
	}

	if (m_OpenSet.size() > 1)
		return sum / (m_OpenSet.size() - 1);
	else
		return -1.0;
}

//  Get Shortest PATH from u to v
Path ShortestPathAlgorithm::GetShortestPath(const Graph &G, unsigned int u, unsigned int v)
{
	m_PathCloseSet = PriorityQueue<Path, double>();
	m_OpenSet.resize(0);

	Path currentPath(u);
	const list<Edge> &firstVNeighbors = G.GetNodeEdges(u);
	//  Add u to the open set
	m_OpenSet.push_back(u);
	//  Add all the neighboors of u to the close set
	for (auto it = firstVNeighbors.begin(); it != firstVNeighbors.end(); ++it)
	{
		Path nextPath(currentPath, *it);
		m_PathCloseSet.Insert(nextPath, nextPath.GetWeight());
	}

	//  While we can find a path from u to v
	while (!m_PathCloseSet.Empty())
	{
		//  Get the path with the best priority
		currentPath = m_PathCloseSet.Top();
		double priority = m_PathCloseSet.GetTopPriority();
		m_PathCloseSet.Pop();

		//  If it ends in v we're over (Dijkstra algoritm guarantees this path's the shortest)
		if (currentPath.GetFinalVertex() == v)
			return currentPath;

		//  If this vertex is not in the open set (it equals there is no shorter path to this vertex)
		if (!OpenSetContains(currentPath.GetFinalVertex()))
		{
			//  Add vertex to the open set
			m_OpenSet.push_back(currentPath.GetFinalVertex());

			const list<Edge> & neighbors = G.GetNodeEdges(currentPath.GetFinalVertex());
			for (auto it = neighbors.begin(); it != neighbors.end(); ++it)
			{
				if (!OpenSetContains(it->GetEndVertexNumber()))
				{
					//  Inserting the path if it is not in the queue already or
					//  if its weight is less than a weight of another path to this vertex
					//  (i.e. this path is better than any prior paths to this vertex found)
					Path nextPath(currentPath, *it);
					m_PathCloseSet.InsertIfPriorityLess(nextPath, nextPath.GetWeight());
				}
			}
		}
	}

	return Path(u);
}