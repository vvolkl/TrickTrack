
#include <cmath>
#include <vector>
#include <functional>

#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"
#include "tricktrack/TripletFilter.h"
#include "tricktrack/CMGraphUtils.h"

using Hit = tricktrack::SpacePoint<size_t>;
using namespace tricktrack;
using namespace std::placeholders;


bool dummyGeometricFilter(const CMCell<Hit>& theInnerCell, const CMCell<Hit>& theOuterCell) {

   auto x1 = theInnerCell.getInnerX();
   auto x2 = theOuterCell.getInnerX();


  std::cout << x1 << "\t" <<  x2 <<  std::endl;
  return std::abs(x1 - x2) < 1;
}



void findTripletsForTest( std::vector<Hit>
                             barrel0,
                         std::vector<CMCell<Hit>::CMntuplet>& foundTracklets) {

  std::vector<HitDoublets<Hit>*> doublets;
  auto doublet1 = new HitDoublets<Hit>(barrel0, barrel0);
  auto doublet2 = new HitDoublets<Hit>(barrel0, barrel0);
  doublets.push_back(doublet1);
  doublets.push_back(doublet2);

  for (const auto& p0 : barrel0) {
    for (const auto& p1 : barrel0) {
      doublets[0]->add(p0.identifier(), p1.identifier());
    }
  }
  for (const auto& p1 : barrel0) {
    for (const auto& p2 : barrel0) {
      doublets[1]->add(p1.identifier(), p2.identifier());
    }
  }
  
  CMGraph g = createGraph({{"innerlayer", "middleLayer", "outerLayer"}});


  
  auto automaton = new HitChainMaker<Hit>(g);

  // create 
  //TripletFilter<Hit> ff = std::bind(defaultGeometricFilter<Hit>, _1, _2,  
  //                                  0.8, // ptmin 
  //                                  0.,  // region_origin_x
  //                                  0.,  // region_origin_y
  //                                  0.002, // region_origin_radius
  //                                  0.2, // phiCut
  //                                  0.8, // hardPtCut
  //                                  0.2  // thetaCut
  //                                  );
  automaton->createAndConnectCells(doublets, dummyGeometricFilter);
  automaton->evolve(3);
  automaton->findNtuplets(foundTracklets, 3);
}

int main() {


    std::vector<Hit> hits;
    hits.push_back(Hit(0, 1, 0, 0));
    hits.push_back(Hit(0, 1.001, 0, 1));
    hits.push_back(Hit(2, 2, 0, 2));
    hits.push_back(Hit(2, 2.002, 0, 3));
    hits.push_back(Hit(4, 3, 0, 4));
    hits.push_back(Hit(4, 3.003, 0, 5));

    std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
    findTripletsForTest(hits, foundTracklets);
    std::cout << "found foundTracklets.size() " << foundTracklets.size() << " hits"  << std::endl;
    return 0;
}
