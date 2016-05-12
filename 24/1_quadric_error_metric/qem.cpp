#include "qem.h"

#include <ysshellext_geomcalc.h>

double CalculateQuadricErrorMetric(
   const YsShellExt &shl,
   YsShell::VertexHandle fromVtHd,
   YsShell::VertexHandle toVtHd)
{
	double sum=0.0;
	auto plHdToDel=shl.FindPolygonFromEdgePiece(fromVtHd,toVtHd);
	auto fromPos=shl.GetVertexPosition(fromVtHd);
	auto toPos=shl.GetVertexPosition(toVtHd);
	for(auto plHd : shl.FindPolygonFromVertex(fromVtHd))
	{
		auto nom=YsShell_CalculateNormal(shl.Conv(),plHd);
		auto d=(toPos-fromPos)*nom;
		sum+=d*d;
	}
	return sum;
}

