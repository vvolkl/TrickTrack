#include <cstdio>
#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>


#include "tricktrack/FKDTree.h"
#include "tricktrack/FKDPoint.h"

using namespace tricktrack;

int main() {
  constexpr unsigned rep = 3;
  for(unsigned nhits = 10000; nhits < 100000; nhits+=10000) {
    for(unsigned ccc = 0; ccc < 10; ++ccc ) {
      std::vector<float> xpoints;
      std::vector<float> ypoints;
      FKDTree<FKDPoint<float, 2>, float, 2> tree;
      std::vector<FKDPoint<float, 2>> points;
      unsigned int id = 0;
      float minX = 0.2;
      float minY = 0.1;
      float maxX = 0.7;
      float maxY = 0.9;
      for (unsigned int i = 0; i < nhits; ++i) {
        float x = minX + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / std::fabs(maxX - minX)));
        float y = minY + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / std::fabs(maxY - minY)));
        xpoints.emplace_back(x);
        ypoints.emplace_back(y);
        points.emplace_back(x, y, id);
        id++;
      }
      std::vector<double> durations;
      std::clock_t startcputime = std::clock();
      for (int j = 0; j < rep; ++j) {
        std::vector<unsigned int> result;
        tree.build(points);
        FKDPoint<float, 2> minPoint( minX + (-1*minX+maxX)*0.5, minY + (-1*minY+maxY)*0.5);
        FKDPoint<float, 2> maxPoint(maxX, maxY);
        tree.search(minPoint, maxPoint, result);
        
       }
      double cpu_duration = (std::clock() - startcputime) / rep / (double)CLOCKS_PER_SEC;
      durations.push_back(cpu_duration);
      double average = std::accumulate( durations.begin(), durations.end(), 0.0)/durations.size(); 

      std::vector<double> durations_v;
      std::clock_t startcputime_v = std::clock();
      for (int j = 0; j < rep; ++j) {
        std::vector<float> result;
        result.reserve(nhits*nhits);
        for (auto xx: xpoints) {
          for (auto yy: ypoints) {
            if (xx < maxX && xx > (maxX-minX)*0.5) {
              if (yy < maxY && yy > (maxY-minY)*0.5) {
                result.push_back(xx + yy);
              }
            }
          }
        }

        
       }
      double cpu_duration_v = (std::clock() - startcputime_v) / rep / (double)CLOCKS_PER_SEC;
      durations_v.push_back(cpu_duration_v);
      double average_v = std::accumulate( durations_v.begin(), durations_v.end(), 0.0)/durations_v.size(); 
      std::cout << nhits << "\t" << average << "\t" << average_v  << std::endl; 
    }
  }
  return 0;
}
