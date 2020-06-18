#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cmath>
#include <vector>
#include <array>
// Header for interface we want to test
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
//#include "tricktrack/SpacePoint.h"

struct Vector3D {

  Vector3D(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}

  double m_x;
  double m_y;
  double m_z;

  double x() const { return m_x; }
  double y() const { return m_y; }
  double z() const { return m_z; }
};

/// @brief 3D space point for track seeding.
///
class MLSpacePoint {
public:
  MLSpacePoint(double x, double y, double z, long unsigned int truth_id, unsigned dataframe_index, size_t id) : m_position(x, y, z), m_truth_id(truth_id), m_dataframe_index(dataframe_index), m_identifier(id)  {}

  const Vector3D& position() const { return m_position; }
  double x() const { return m_position.x(); }
  double y() const { return m_position.y(); }
  double z() const { return m_position.z(); }
  double rho() const { return std::sqrt(std::pow(m_position.x(), 2) + std::pow(m_position.y(), 2)); }
  double phi() const { return std::atan2(m_position.y(), m_position.x()); }
  const size_t& identifier() const { return m_identifier; }
  long unsigned int truth_id() const {return m_truth_id;};
  long unsigned int dataframe_index() const {return m_dataframe_index;};

private:
  Vector3D m_position;

  size_t m_identifier;
  long unsigned int m_truth_id;
  unsigned int m_dataframe_index;
};

using Hit = MLSpacePoint;
using namespace tricktrack;
using namespace std::placeholders;

template <typename Hit>
  bool customizedGeometricFilter(const CMCell<Hit>& theInnerCell, const CMCell<Hit>& theOuterCell) {
  return defaultGeometricFilter(theInnerCell,theOuterCell, 0.8, 0., 0., 0.002, 0.2, 0.8, 0.2 );

    }



std::vector<std::vector<long unsigned int>> TTReco(std::array<std::vector<std::array<double, 3>>, 3> theHits, std::array<std::vector<std::array<long unsigned int, 2 >>, 3> theIds, double thetaCut = 0.002,
                                                        double phiCut = 0.2,
                                                        double phiCut_d = 0.2,
                                                        double ptMin = 0.8,
                                                        double regionOriginRadius = 0.02,
                                                        double hardPtCut = 0.0  ) {

    std::cout << "arguments: "  << std::endl
    << "\tthetaCut: " <<  thetaCut << std::endl
    << "\tphiCut: " <<  phiCut<< std::endl 
    << "\tphiCut_d: " << phiCut_d << std::endl
    << "\tptMin: " << ptMin << std::endl
    << "\tregionOriginRadius: " <<  regionOriginRadius << std::endl
    << "\thardPtCut: " << hardPtCut << std::endl; 

    // geometric information used for cuts
    const TrackingRegion region(0, 0, regionOriginRadius, ptMin);

    std::cout << "hit vector sizes: " << theHits[0].size() << "\t" << theHits[1].size() << "\t" << theHits[2].size() << std::endl;
    std::cout << "create TrickTrack Points ... " << std::endl;
    std::vector<Hit> inner_hits;
    for (int count1 = 0; count1 < theHits[0].size(); ++count1) {
      auto & e = theHits[0][count1];
      std::array<long unsigned int, 2> _id = theIds[0][count1];
      inner_hits.push_back(Hit(e[0], e[1], e[2], _id[0], _id[1], count1));
    }
    std::vector<Hit> middle_hits;
    for (int count2 = 0; count2 < theHits[1].size(); ++count2) {
      auto & e = theHits[1][count2];
      std::array<long unsigned int, 2> _id = theIds[1][count2];
      middle_hits.push_back(Hit(e[0], e[1], e[2], _id[0], _id[1], count2));
    }
    std::vector<Hit>  outer_hits;
    for (int count3 = 0; count3 < theHits[2].size(); ++count3) {
      auto & e = theHits[2][count3];
      std::array<long unsigned int, 2> _id = theIds[2][count3];
      outer_hits.push_back(Hit(e[0], e[1], e[2], _id[0], _id[1], count3));
    }
    std::cout << "hit vector sizes: " << inner_hits.size() << "\t" << middle_hits.size() << "\t" << outer_hits.size() << std::endl;
    std::cout << "create doublets ... " << std::endl;
    std::vector<CMCell<Hit>::CMntuplet> foundTracklets;
    std::vector<HitDoublets<Hit>*> doublets;
    auto doublet1 = new HitDoublets<Hit>(inner_hits, middle_hits);
    auto doublet2 = new HitDoublets<Hit>(middle_hits, outer_hits);
    doublets.push_back(doublet1);
    doublets.push_back(doublet2);

    unsigned int numGoodDoublets = 0;
    for (const auto& p0 : inner_hits) {
      for (const auto& p1 : middle_hits) {
       	double phi0 = p0.phi();
        double phi1 = p1.phi();
        double dPhi = std::fabs(M_PI - std::fabs(std::fabs(phi1 - phi0) - M_PI));
        if(dPhi < phiCut_d && std::abs(p0.z() - p1.z()) < 280) {
        	doublets[0]->add(p0.identifier(), p1.identifier());
          if (p0.truth_id() == p1.truth_id()) {
            numGoodDoublets++;
          }
        }
      }
    }
    std::cout << "found " << numGoodDoublets << " / " << doublets[0]->size() << "good doublets" << std::endl;
    numGoodDoublets = 0;
    for (const auto& p1 : middle_hits) {
      for (const auto& p2 : outer_hits) {
       	double phi1 = p1.phi();
        double phi2 = p2.phi();
        double dPhi = std::abs(M_PI - std::fabs(std::fabs(phi2 - phi1) - M_PI));
        if(dPhi < phiCut_d && std::abs(p1.z() - p2.z())<280.) {
                doublets[1]->add(p1.identifier(), p2.identifier());
            if (p1.truth_id() == p2.truth_id()) {
              numGoodDoublets++;
            }
        }
      }
    }
 
    std::cout << "found " << numGoodDoublets << " / " << doublets[0]->size() << "good doublets" << std::endl;
    std::cout<<"d1: "<<doublets[0]->size()<<", d2: "<<doublets[1]->size()<<std::endl;

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
    
    TripletFilter<Hit> ff = std::bind(defaultGeometricFilter<Hit>, _1, _2,  ptMin, 0., 0., regionOriginRadius, phiCut, hardPtCut, thetaCut );

    auto automaton = new HitChainMaker<Hit>(g);
    std::cout << "createAndConnectCells ..." << std::endl;
    automaton->createAndConnectCells(doublets, ff);
    std::cout << "evolve..." << std::endl;
    automaton->evolve(3);
    automaton->findNtuplets(foundTracklets, 3);
    std::cout << "found Tracklets: " << foundTracklets.size() << std::endl;
    std::vector<std::vector<long unsigned int>> result;

    auto cells = automaton->getAllCells();
    unsigned int numcorrect = 0;
    unsigned int numDuplicates = 0;
    unsigned int numFalse = 0;
    std::set<long unsigned int> foundParticles;
    for (const auto& tracklet : foundTracklets) {
      result.push_back(std::vector<long unsigned int>());
      result.back().push_back(cells[tracklet[0]].getInnerHit().dataframe_index());
      long unsigned int correct = cells[tracklet[0]].getInnerHit().truth_id();
      for(const auto& t: tracklet) {
        result.back().push_back(cells[t].getOuterHit().dataframe_index());
        if (cells[t].getOuterHit().truth_id() != correct ) {
          correct = -1;
        } 
      }
      if (correct == -1) {
        numFalse += 1;
      } else { if( foundParticles.find( correct) != foundParticles.end()) {
            std::cout << correct << std::endl;
            numDuplicates++;
          } else {
           foundParticles.insert(correct); 
           numcorrect += 1;
          }
        }
      }
		
    std::cout << "numberOfTracklets: " << foundTracklets.size() << "\tnumber of true tracklets: " << numcorrect << "\t number of fakes: " << numFalse << "\t: number of duplicate correct: " << numDuplicates <<  std::endl;
    return result;

}

PYBIND11_MODULE(pyTT, m) {
      m.doc() = "pybind11 example plugin"; // optional module docstring

          m.def("TTReco", &TTReco, "Top level function for trick track reco", 
          pybind11::arg("theHits"), 
          pybind11::arg("theIds"), 
          pybind11::arg("thetaCut") = 0.002, 
          pybind11::arg("phiCut") = 0.2, 
          pybind11::arg("phiCut_d") = 0.2, 
          pybind11::arg("ptMin") = 0.8,  
          pybind11::arg("regionOriginRadius") = 0.02, 
          pybind11::arg("hardPtCut") = 0.0);
          m.def("areAlignedRZ", &areAlignedRZ, "geometric hit filter");
          m.def("haveSimilarCurvature", &haveSimilarCurvature, "geometric hit filter");
}
