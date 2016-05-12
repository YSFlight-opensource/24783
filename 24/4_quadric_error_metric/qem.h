#ifndef QEM_IS_INCLUDED
#define QEM_IS_INCLUDED
/* { */

#include <ysshellextedit.h>
#include "priorityqueue.h"

double CalculateQuadricErrorMetric(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd);


class QEMSimplifier
{
public:
	class CollapseTarget
	{
	public:
		YSHASHKEY fromVtKey,toVtKey;
		bool operator==(const CollapseTarget &incoming) const
		{
			return (fromVtKey==incoming.fromVtKey && toVtKey==incoming.toVtKey);
		}
		bool operator!=(const CollapseTarget &incoming) const
		{
			return (fromVtKey!=incoming.fromVtKey || toVtKey!=incoming.toVtKey);
		}
	};
private:
	PriorityQueue <double,CollapseTarget> queue;

public:
	QEMSimplifier();
	~QEMSimplifier();
	void CleanUp(void);

	void Begin(const YsShellExt &shl);
	void Simplify(YsShellExtEdit &shl);
};

template <>
inline unsigned long long int 
    HashTable<QEMSimplifier::CollapseTarget,const BinaryTree <double,QEMSimplifier::CollapseTarget>::Node *>::
        HashCode(const QEMSimplifier::CollapseTarget &key) const
{
	return key.fromVtKey+key.toVtKey;
}

/* } */
#endif
