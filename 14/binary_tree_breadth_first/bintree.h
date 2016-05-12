#ifndef BINTREE_IS_INCLUDED
#define BINTREE_IS_INCLUDED
/* { */

template <class KeyClass,class ValueClass>
class BinaryTree
{
public:
	enum
	{
		POSITION_ROOT,
		POSITION_LEFT,
		POSITION_RIGHT
	};

	class Node
	{
	public:
		KeyClass key;
		ValueClass value;
		Node *parent,*left,*right;
		Node()
		{
			parent=nullptr;
			left=nullptr;
			right=nullptr;
		}
	};

protected:
	Node *rootNode;

public:
	BinaryTree()
	{
		rootNode=nullptr;
	}
	~BinaryTree()
	{
		CleanUp();
	}
	const Node *GetRoot(void) const
	{
		return rootNode;
	}
private:
	void Free(Node *ptr)
	{
		if(nullptr!=ptr)
		{
			Free(ptr->left);
			Free(ptr->right);
			delete ptr;
		}
	}
public:
	void CleanUp(void)
	{
		Free(rootNode);
		rootNode=nullptr;
	}

private:
	void Add(Node *newNode);
public:
	const Node *Add(KeyClass key,const ValueClass &value);

public:
	class Position
	{
	public:
		Node *node;
		int position;
	};
	void Delete(const Node *toDel);
private:
	Position SimpleDetach(Node *toDetach);
	void SimpleAttach(Node *toAttach,Position pos);

public:
	bool RotateLeft(const Node *nodePtr);
};

template <class KeyClass,class ValueClass>
void BinaryTree <KeyClass,ValueClass>::Add(
    typename BinaryTree <KeyClass,ValueClass>::Node *newNode)
{
	if(nullptr==rootNode)
	{
		rootNode=newNode;
	}
	else
	{
		auto *seeker=rootNode;
		for(;;)
		{
			if(newNode->key<seeker->key)
			{
				if(nullptr==seeker->left)
				{
					seeker->left=newNode;
					break;
				}
				seeker=seeker->left;
			}
			else
			{
				if(nullptr==seeker->right)
				{
					seeker->right=newNode;
					break;
				}
				seeker=seeker->right;
			}
		}
		newNode->parent=seeker;
	}
}

template <class KeyClass,class ValueClass>
const typename BinaryTree<KeyClass,ValueClass>::Node *BinaryTree<KeyClass,ValueClass>::Add(KeyClass key,const ValueClass &value)
{
	auto newNode=new BinaryTree<KeyClass,ValueClass>::Node;
	newNode->key=key;
	newNode->value=value;
	Add(newNode);
	return newNode;
}

template <class KeyClass,class ValueClass>
void BinaryTree<KeyClass,ValueClass>::Delete(const Node *cToDel)
{
	Node *toDel=(Node *)cToDel;

	// Easy case?
	if(nullptr==toDel->left || nullptr==toDel->right)
	{
		auto pos=SimpleDetach(toDel);
		if(nullptr!=toDel->left)
		{
			SimpleAttach(toDel->left,pos);
		}
		else if(nullptr!=toDel->right)
		{
			SimpleAttach(toDel->right,pos);
		}
		delete toDel;
	}
	else
	{
		// Find right-most of the left sub-tree.
		Node *rightMostOfLeft=toDel->left;
		while(nullptr!=rightMostOfLeft->right)
		{
			rightMostOfLeft=rightMostOfLeft->right;
		}

		{
			auto pos=SimpleDetach(rightMostOfLeft);
			if(nullptr!=rightMostOfLeft->left)
			{
				SimpleAttach(rightMostOfLeft->left,pos);
			}
		}

		{
			auto pos=SimpleDetach(toDel);
			SimpleAttach(rightMostOfLeft,pos);
			rightMostOfLeft->right=toDel->right;
			if(nullptr!=rightMostOfLeft->right)
			{
				rightMostOfLeft->right->parent=rightMostOfLeft;
			}
			rightMostOfLeft->left=toDel->left;
			if(nullptr!=rightMostOfLeft->left)
			{
				rightMostOfLeft->left->parent=rightMostOfLeft;
			}
		}

		delete toDel;
	}
}

template <class KeyClass,class ValueClass>
typename BinaryTree<KeyClass,ValueClass>::Position
    BinaryTree<KeyClass,ValueClass>::SimpleDetach(Node *toDetach)
{
	Position pos;
	if(nullptr!=toDetach->parent)
	{
		if(toDetach->parent->left==toDetach)
		{
			pos.node=toDetach->parent;
			pos.position=POSITION_LEFT;
			toDetach->parent->left=nullptr;
			toDetach->parent=nullptr;
		}
		else // if(toDetach->parent->right==toDetach)
		{
			pos.node=toDetach->parent;
			pos.position=POSITION_RIGHT;
			toDetach->parent->right=nullptr;
			toDetach->parent=nullptr;
		}
	}
	else
	{
		pos.node=nullptr;
		pos.position=POSITION_ROOT;
		rootNode=nullptr;
	}
	return pos;
}

template <class KeyClass,class ValueClass>
void BinaryTree<KeyClass,ValueClass>::SimpleAttach(Node *toAttach,Position pos)
{
	if(POSITION_ROOT==pos.position || nullptr==pos.node)
	{
		rootNode=toAttach;
		toAttach->parent=nullptr;
	}
	else if(POSITION_LEFT==pos.position)
	{
		pos.node->left=toAttach;
		toAttach->parent=pos.node;
	}
	else // if(POSITION_RIGHT==position)
	{
		pos.node->right=toAttach;
		toAttach->parent=pos.node;
	}
}

template <class KeyClass,class ValueClass>
bool BinaryTree<KeyClass,ValueClass>::RotateLeft(const Node *nodePtr)
{
	if(nullptr!=nodePtr->right)
	{
		Node *A=(Node *)nodePtr;
		auto B=nodePtr->right;

		auto Apos=SimpleDetach(A);

		A->right=B->left;
		if(nullptr!=A->right)
		{
			A->right->parent=A;
		}
		B->left=A;
		A->parent=B;

		SimpleAttach(B,Apos);
		return true;
	}
	return false;
}

/* } */
#endif
