#include "edge_to_polygon.h"

void EdgeToPolygonTable::SetShell(const YsShellExt &shl)
{
	CleanUp();
	Resize(shl.GetNumPolygon());

	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);

		for(int edIdx=0; edIdx<plVtHd.GetN(); ++edIdx)
		{
			ShellEdgeKey hashKey;
			hashKey.edVtKey[0]=shl.GetSearchKey(plVtHd[edIdx]);
			hashKey.edVtKey[1]=shl.GetSearchKey(plVtHd.GetCyclic(edIdx+1));

			auto edPlPtr=(*this)[hashKey];
			if(nullptr!=edPlPtr)
			{
				edPlPtr->push_back(plHd);
			}
			else
			{
				std::vector <YsShell::PolygonHandle> edPl;
				edPl.push_back(plHd);
				this->Update(hashKey,edPl);
			}
		}
	}
}

const std::vector <YsShell::PolygonHandle> *EdgeToPolygonTable::FindPolygonFromEdge(
	    const YsShellExt &shl,
	    YsShell::VertexHandle edVtHd0,YsShell::VertexHandle edVtHd1) const
{
	ShellEdgeKey hashKey;
	hashKey.edVtKey[0]=shl.GetSearchKey(edVtHd0);
	hashKey.edVtKey[1]=shl.GetSearchKey(edVtHd1);
	return (*this)[hashKey];
}
