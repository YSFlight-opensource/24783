#include "qem.h"

#include <ysshellext_geomcalc.h>
#include <ysshellext_localop.h>

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

QEMSimplifier::QEMSimplifier()
{
	queue.CleanUp();
}
QEMSimplifier::~QEMSimplifier()
{
	CleanUp();
}
void QEMSimplifier::CleanUp(void)
{
	queue.CleanUp();
}

void QEMSimplifier::Begin(const YsShellExt &shl)
{
	CleanUp();
	queue.ResizeHashTable(shl.GetNumVertex()+1);
	for(auto vtHd : shl.AllVertex())
	{
		for(auto connVtHd : shl.GetConnectedVertex(vtHd))
		{
			auto qem=CalculateQuadricErrorMetric(shl,vtHd,connVtHd);
			CollapseTarget coll;
			coll.fromVtKey=shl.GetSearchKey(vtHd);
			coll.toVtKey=shl.GetSearchKey(connVtHd);
			queue.Update(coll,qem);
		}
	}
}

void QEMSimplifier::Simplify(YsShellExtEdit &shl)
{
	if(true!=queue.IsEmpty())
	{
		YsShellExtEdit::StopIncUndo undoGuard(shl);

		auto coll=queue.PopMinimumCost();
		YsShell::VertexHandle collapseEdVtHd[2]=
		{
			shl.FindVertex(coll.fromVtKey),
			shl.FindVertex(coll.toVtKey)
		};
		if(nullptr!=collapseEdVtHd[0] && nullptr!=collapseEdVtHd[1])
		{
			YsShell_CollapseInfo info;
			if(YSOK==info.MakeInfo(shl.Conv(),collapseEdVtHd))
			{
				auto affected=shl.GetConnectedVertex(collapseEdVtHd[0]);
				for(auto vtHd : affected)
				{
					CollapseTarget coll;
					coll.fromVtKey=shl.GetSearchKey(collapseEdVtHd[0]);
					coll.toVtKey=shl.GetSearchKey(vtHd);
					queue.Delete(coll);
					coll.fromVtKey=shl.GetSearchKey(vtHd);
					coll.toVtKey=shl.GetSearchKey(collapseEdVtHd[0]);
					queue.Delete(coll);
				}
				info.Apply(shl,YSFALSE);
				for(auto vtHd : affected)
				{
					for(auto connVtHd : shl.GetConnectedVertex(vtHd))
					{
						auto qem=CalculateQuadricErrorMetric(shl.Conv(),vtHd,connVtHd);
						CollapseTarget coll;
						coll.fromVtKey=shl.GetSearchKey(vtHd);
						coll.toVtKey=shl.GetSearchKey(connVtHd);
						queue.Update(coll,qem);

						qem=CalculateQuadricErrorMetric(shl.Conv(),connVtHd,vtHd);
						coll.fromVtKey=shl.GetSearchKey(connVtHd);
						coll.toVtKey=shl.GetSearchKey(vtHd);
						queue.Update(coll,qem);
					}
				}
			}
		}
	}
}
