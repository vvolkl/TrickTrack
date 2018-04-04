
from pyTT import TTReco

# xyz coordinates of hits on three seeding layers
layerPoints = [
              [ [0,0,0], [0.1,0.1,0.1] ] , #layer1
              [ [1,1,1] ], #layer2
              [ [2,2,2] ], #layer3
              ]
TTReco(layerPoints)
