#ifndef EDGE_TO_POLYGON_IS_INCLUDED
#define EDGE_TO_POLYGON_IS_INCLUDED
/* { */

#include <vector>
#include <ysshellext.h>
#include "hashtable.h"


class ShellEdgeKey
{
public:
	// Edge Vertex Keys
	YSHASHKEY edVtKey[2];

	bool operator==(const ShellEdgeKey &incoming) const
	{
		return (edVtKey[0]==incoming.edVtKey[0] && edVtKey[1]==incoming.edVtKey[1]) ||
		       (edVtKey[0]==incoming.edVtKey[1] && edVtKey[1]==incoming.edVtKey[0]);
	}
	bool operator!=(const ShellEdgeKey &incoming) const
	{
		return (edVtKey[0]!=incoming.edVtKey[0] || edVtKey[1]!=incoming.edVtKey[1]) &&
		       (edVtKey[0]!=incoming.edVtKey[1] || edVtKey[1]!=incoming.edVtKey[0]);
	}
};

template <>
inline unsigned long long int HashTable<ShellEdgeKey,std::vector <YsShell::PolygonHandle> >::HashCode(const ShellEdgeKey &key) const
{
	return key.edVtKey[0]+key.edVtKey[1];
}

class EdgeToPolygonTable : public HashTable <ShellEdgeKey,std::vector <YsShell::PolygonHandle> >
{
public:
	void SetShell(const YsShellExt &shl);
	const std::vector <YsShell::PolygonHandle> *FindPolygonFromEdge(
	    const YsShellExt &shl,
	    YsShell::VertexHandle edVtHd0,YsShell::VertexHandle edVtHd1) const;
};


/* } */
#endif
