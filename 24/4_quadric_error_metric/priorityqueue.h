#ifndef PRIORITYQUEUE_IS_INCLUDED
#define PRIORITYQUEUE_IS_INCLUDED
/* { */

#include "bintree.h"
#include "hashtable.h"

template <class costType,class element>
class PriorityQueue
{
private:
	BinaryTree <costType,element> tree;
	HashTable <element,const typename BinaryTree <costType,element>::Node *> elemToNode;

public:
	PriorityQueue()
	{
	}
	~PriorityQueue()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		tree.CleanUp();
		elemToNode.CleanUp();
	}

	void ResizeHashTable(int tableSize)
	{
		elemToNode.Resize(tableSize);
	}

	void Update(element elem,costType cost)
	{
		auto nodePtr=elemToNode[elem];
		if(nullptr!=nodePtr)
		{
			tree.Delete(*nodePtr);
		}
		auto newNode=tree.Add(cost,elem);
		elemToNode.Update(elem,newNode);
	}
	void Delete(element elem)
	{
		auto nodePtr=elemToNode[elem];
		if(nullptr!=nodePtr)
		{
			tree.Delete(*nodePtr);
			elemToNode.Delete(elem);
		}
	}
	element PopMinimumCost(void)
	{
		element elem;
		auto minNode=tree.GetRoot();
		if(nullptr!=minNode)
		{
			while(nullptr!=minNode->left)
			{
				minNode=minNode->left;
			}
			elemToNode.Delete(minNode->value);
			auto value=minNode->value;
			tree.Delete(minNode);
			return value;
		}
		return elem;
	}
	bool IsEmpty(void) const
	{
		if(nullptr==tree.GetRoot())
		{
			return true;
		}
		return false;
	}
};


/* } */
#endif
