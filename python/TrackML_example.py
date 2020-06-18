
from pyTT import TTReco
import trackml
from trackml.dataset import load_event, load_dataset
import sys
import numpy as np
import pandas as pd

filename =  '/home/vali/.kaggle/competitions/trackml-particle-identification/train_1/event000001000'

hits, cells, particles, truth = load_event(filename)
print hits.dtypes
#print event_id, len(hits)
hits = pd.merge(hits,truth,on='hit_id')
print hits.dtypes
hits['phi'] = np.arctan2(hits['y'],hits['x'])
hits['r'] = np.sqrt(hits['y']*hits['y']+hits['x']*hits['x'])
#hits['tpt'] = np.sqrt(hits['tpx']*hits['tpx']+hits['tpy']*hits['tpy'])
hits['index'] = hits.index
hits = hits[hits["particle_id"] > 0]
#hits.sort_values(by=['phi'],inplace=True)
hl1 = hits.query('volume_id ==8 & layer_id==2')
hl2 = hits.query('volume_id ==8 & layer_id==4')
hl3 = hits.query('volume_id ==8 & layer_id==6')
hl4 = hits.query('volume_id ==8 & layer_id==8')
print hl1.size
print hl2.size
print hl3.size

i1 = hl1.index

d1 =  [list(x) for x in hl1[["x", "y", "z"]].values]
d2 =  [list(x) for x in hl2[["x", "y", "z"]].values]
d3 =  [list(x) for x in hl3[["x", "y", "z"]].values]


id1 =  [list(x) for x in hl1[["particle_id", "index"]].values]
id2 =  [list(x) for x in hl2[["particle_id", "index"]].values]
id3 =  [list(x) for x in hl3[["particle_id", "index"]].values]


print len(d1)
print len(d2)
# xyz coordinates of hits on three seeding layers
layerPoints = [d1, d2, d3]
layerIds = [id1, id2, id3]
#result = TTReco(layerPoints, ptMin=0.8,  regionOriginRadius=20)
result = TTReco(theHits=layerPoints, theIds=layerIds, thetaCut=1., phiCut=0.175, phiCut_d = 0.3, ptMin=10., regionOriginRadius=2.0, hardPtCut=0.2)
print len(result)
#for indices in result[:4]:
#  particle_id = hits[hit_indices[0]
#  for i in indices:
#    print hits[hits["hit_id"] == i],
#  print "\n\n\n"
