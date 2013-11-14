//  Contains implementation of the priority queue

#ifndef PRIORITY_QUEUE_H__
#define PRIORITY_QUEUE_H__

#include <algorithm>
#include <vector>

//  Functions to implement min heap
using std::vector;
using std::pop_heap;
using std::push_heap;

//  Template class implementing priority queue element.
//  First template parameter is value type (unsigned int as the number of vertex in the Dijkstra algo)
//  Second template parameter is priority type (double as the priority of vertex in the Dijkstra algo)
template<typename TVal, typename TPriority>
class PriorityQueueElement
{
private:
	TVal m_Value;
	TPriority m_Priority;
public:
	//  defining methods bodies inside the class because they are simple enough to
	//  also it makes them inline
	PriorityQueueElement(const TVal &val, const TPriority &priority) : m_Value(val), m_Priority(priority){ }
	~PriorityQueueElement() { }

	//  Getters
	TVal GetValue() const { return m_Value; }
	TPriority GetPriority() const { return m_Priority; }

	//  Setters
	void SetValue(const TVal &val) { m_Value = val; }
	void SetPriority(const TPriority &priority) { m_Priority = priority; }
};

//  Function to compare 2 elements in the queue. It is used to form a proper minHeap (thus it uses operator>)
template<typename TVal, typename TPriority>
bool Compare(const PriorityQueueElement<TVal, TPriority> &pqn1, const PriorityQueueElement<TVal, TPriority> &pqn2)
{
	return pqn1.GetPriority() > pqn2.GetPriority();
}

//  Template class implementing priority queue
//  First template parameter is value type (unsigned int as the number of vertice in the Dijkstra algo)
//  Second template parameter is priority type (double as the priority of vertice in the Dijkstra algo)
template<typename TVal, typename TPriority>
class PriorityQueue
{
private:
	//  minHeap itself. Its type also would be a template parameter representing underlying
	//  container like in STL class priority_queue but i decided not to make things harder
	vector<PriorityQueueElement<TVal, TPriority>> m_MinHeap;

	bool GetPriorityIfContains(const TVal &val, TPriority &priority);
public:
	//  methods that have 1 line of code are defined in the class body to make them inline
	//  other methods have separate implementation, otherwise class body would be too hard
	//  to read and understand
	PriorityQueue() { }
	~PriorityQueue() { }

	//  Checks if the element is in the queue
	bool Contains(const TVal &val) const;
	//  Retrieves the element on the top on the queue and returns true if succeeded. Returns false otherwise.
	TVal Top() const;
	//  Get the priority of the top element
	TPriority GetTopPriority() const;
	//  Returns the size of the queue
	size_t Size() const { return m_MinHeap.size(); }
	//  Checks if the queue is empty
	bool Empty() const { return m_MinHeap.size() == 0; }

	//  Changes priority of an element to the given
	void ChangePriority(const TVal &val, const TPriority &priority);
	//  Deletes element on the top from the queue
	void Pop();
	//  Inserts the element to the queue
	void Insert(const TVal &val, const TPriority &priority);
	//  Test have shown that this function makes algorithm work much easier compared to just Insert
	//  (because we don't insert to the close set neighboors of the node we already found a path to
	//  with less priority. Otherwise it would be a lot of useless work. It is cheaper to make this check
	//  then to process close set with much more elements in the Dijkstra algorithm)
	void InsertIfPriorityLess(const TVal &val , const TPriority &priority);
};

//  This method gets the priority of the element if the queue contains this element and returns true.
//  Otherwise returns false
template<typename TVal, typename TPriority>
bool PriorityQueue<TVal, TPriority>::GetPriorityIfContains(const TVal &val, TPriority &priority)
{
	for (auto it = m_MinHeap.begin(); it != m_MinHeap.end(); ++it)
		if (it->GetValue() == val)
		{
			priority = it->GetPriority();
			return true;
		}
	return false;
}

template<typename TVal, typename TPriority>
bool PriorityQueue<TVal, TPriority>::Contains(const TVal &val) const
{
	for (auto it = m_MinHeap.begin(); it != m_MinHeap.end(); ++it)
		if (it->GetValue() == val)
			return true;
	return false;
}

template<typename TVal, typename TPriority>
TVal PriorityQueue<TVal, TPriority>::Top() const
{
	return m_MinHeap.front().GetValue();
}

template<typename TVal, typename TPriority>
TPriority PriorityQueue<TVal, TPriority>::GetTopPriority() const
{
	return m_MinHeap.front().GetPriority();
}

template<typename TVal, typename TPriority>
void PriorityQueue<TVal, TPriority>::ChangePriority(const TVal &value, const TPriority & priority)
{
	for (auto it = m_MinHeap.begin(); it != m_MinHeap.end(); ++it)
		if (it->GetValue() == value)
		{
			it->SetPriority(priority);
			return;
		}
}

template<typename TVal, typename TPriority>
void PriorityQueue<TVal, TPriority>::Pop()
{
	pop_heap(m_MinHeap.begin(), m_MinHeap.end(), Compare<TVal, TPriority>);  //  using STL function to implement heap
	m_MinHeap.pop_back();
}

template<typename TVal, typename TPriority>
void PriorityQueue<TVal, TPriority>::Insert(const TVal &val, const TPriority & priority)
{
	m_MinHeap.push_back(PriorityQueueElement<TVal, TPriority>(val, priority));
	push_heap(m_MinHeap.begin(), m_MinHeap.end(), Compare<TVal, TPriority>);  //  using STL function to implement heap
}

template<typename TVal, typename TPriority>
void PriorityQueue<TVal, TPriority>::InsertIfPriorityLess(const TVal &val, const TPriority & priority)
{
	TPriority tmp_priority;
	bool bContains = GetPriorityIfContains(val, tmp_priority);
	if (!bContains || priority < tmp_priority)
		Insert(val, priority);
}

#endif