#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cmath>
#include <vector>
#include <array>
// Header for interface we want to test
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"

using Hit = tricktrack::SpacePoint<size_t>;
using namespace tricktrack;

template <typename Hit>
  bool customizedGeometricFilter(const CMCell<Hit>& theInnerCell, const CMCell<Hit>& theOuterCell) {
  return defaultGeometricFilter(theInnerCell,theOuterCell, 0.8, 0., 0., 0.002, 0.2, 0.8, 0.2 );

    }



void TTReco(std::array<std::vector<std::array<double, 3>>, 3> theHits, double thetaCut = 0.002,
                                                        double phiCut = 0.2,
                                                        double ptMin = 0.8,
                                                        double regionOriginRadius = 0.02,
                                                        double hardPtCut = 0.0  ) {

    // geometric information used for cuts
    const TrackingRegion region(0, 0, regionOriginRadius, ptMin);

    // create Tri
    std::vector<Hit> inner_hits;
    for (auto e: theHits[0]) {
      inner_hits.push_back(Hit(e[0], e[1], e[2], 0));
    }
    std::vector<Hit> middle_hits;
    for (auto e: theHits[1]) {
      middle_hits.push_back(Hit(e[0], e[1], e[2], 0));
    }
    std::vector<Hit>  outer_hits;
    for (auto e: theHits[2]) {
      outer_hits.push_back(Hit(e[0], e[1], e[2], 0));
    }

    std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
    std::vector<HitDoublets<Hit>*> doublets;
    auto doublet1 = new HitDoublets<Hit>(inner_hits, middle_hits);
    auto doublet2 = new HitDoublets<Hit>(middle_hits, outer_hits);
    doublets.push_back(doublet1);
    doublets.push_back(doublet2);

    for (const auto& p0 : inner_hits) {
      for (const auto& p1 : middle_hits) {
        doublets[0]->add(p0.identifier(), p1.identifier());
      }
    }
    for (const auto& p1 : middle_hits) {
      for (const auto& p2 : outer_hits) {
        doublets[1]->add(p1.identifier(), p2.identifier());
      }
    }

    auto l1 = CMLayer("innerLayer", 100000);
    auto l2 = CMLayer("middleLayer", 100000);
    auto l3 = CMLayer("outerLayer", 100000);

    auto lp1 = CMLayerPair(0, 1);
    auto lp2 = CMLayerPair(1, 2);
    l1.theOuterLayers.push_back(1);
    l2.theInnerLayers.push_back(0);
    l2.theOuterLayers.push_back(2);
    l3.theInnerLayers.push_back(1);
    l1.theOuterLayerPairs.push_back(0);
    l2.theInnerLayerPairs.push_back(0);
    l2.theOuterLayerPairs.push_back(1);
    l3.theInnerLayerPairs.push_back(1);

    auto g = CMGraph();
    g.theLayers.push_back(l1);
    g.theLayers.push_back(l2);
    g.theLayers.push_back(l3);
    g.theLayerPairs.push_back(lp1);
    g.theLayerPairs.push_back(lp2);
    g.theRootLayers.push_back(0);

    auto automaton = new HitChainMaker<Hit>(g);
    automaton->createAndConnectCells(doublets, customizedGeometricFilter);
    automaton->evolve(3);
    automaton->findNtuplets(foundTracklets, 3);
    std::cout << "found Tracklets: " << foundTracklets.size() << std::endl;
}

PYBIND11_MODULE(pyTT, m) {
      m.doc() = "pybind11 example plugin"; // optional module docstring

          m.def("TTReco", &TTReco, "Top level function for trick track reco",  pybind11::arg("hardPtCut") = 0.0, pybind11::arg("ptMin") = 0.8, pybind11::arg("regionOriginRadius") = 0.02, pybind11::arg("phiCut") = 0.2, pybind11::arg("thetaCut") = 0.002, pybind11::arg("theHits") = 0);
          m.def("areAlignedRZ", &areAlignedRZ, "geometric hit filter");
          m.def("haveSimilarCurvature", &haveSimilarCurvature, "geometric hit filter");
          m.def("haveSimilarCurvature", &haveSimilarCurvature, "geometric hit filter");
}
