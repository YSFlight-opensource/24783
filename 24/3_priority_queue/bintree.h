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
	int nNode;

public:
	BinaryTree()
	{
		rootNode=nullptr;
		nNode=0;
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
		nNode=0;
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

	bool RotateRight(const Node *nodePtr);
	void TreeToVine(void);
	void Compression(int n);
	void VineToTree(void);
};

template <class KeyClass,class ValueClass>
void BinaryTree <KeyClass,ValueClass>::Add(
    typename BinaryTree <KeyClass,ValueClass>::Node *newNode)
{
	int depth=0;
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
				++depth;
			}
			else
			{
				if(nullptr==seeker->right)
				{
					seeker->right=newNode;
					break;
				}
				seeker=seeker->right;
				++depth;
			}
		}
		newNode->parent=seeker;
	}
	++nNode;

	int minDepth=(int)(log2((double)nNode));
	if(minDepth*2<depth)
	{
		printf("Rebalance! (minDepth=%d depth=%d)\n",minDepth,depth);
		TreeToVine();
		VineToTree();
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
		--nNode;
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
		--nNode;
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

template <class KeyClass,class ValueClass>
bool BinaryTree<KeyClass,ValueClass>::RotateRight(const Node *nodePtr)
{
	if(nullptr!=nodePtr->left)
	{
		Node *A=(Node *)nodePtr;
		auto B=nodePtr->left;

		auto Apos=SimpleDetach(A);

		A->left=B->right;
		if(nullptr!=A->left)
		{
			A->left->parent=A;
		}
		B->right=A;
		A->parent=B;

		SimpleAttach(B,Apos);
		return true;
	}
	return false;
}

template <class KeyClass,class ValueClass>
void BinaryTree<KeyClass,ValueClass>::TreeToVine(void)
{
	auto ptr=GetRoot();
	while(nullptr!=ptr)
	{
		if(nullptr!=ptr->left)
		{
			RotateRight(ptr);
			ptr=ptr->parent;
		}
		else
		{
			ptr=ptr->right;
		}
	}
}

template <class KeyClass,class ValueClass>
void BinaryTree<KeyClass,ValueClass>::Compression(int n)
{
	auto node=GetRoot();
	while(nullptr!=node && 0<n)
	{
		if(nullptr!=node->right)
		{
			RotateLeft(node);
			node=node->parent->right;
		}
		else
		{
			break;
		}
		--n;
	}
}

template <class KeyClass,class ValueClass>
void BinaryTree<KeyClass,ValueClass>::VineToTree(void)
{
	int n=0;

	for(auto node=GetRoot(); nullptr!=node; node=node->right)
	{
		++n;
	}

	int k=1;
	while(k<=n)
	{
		k*=2;
	}
	k/=2;

	int lc=n+1-k;
	Compression(lc);

	n-=lc;
	while(1<n)
	{
		Compression(n/2);
		n/=2;
	}
}

/* } */
#endif

