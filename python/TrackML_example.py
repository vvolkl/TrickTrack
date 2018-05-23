
from pyTT import TTReco
import trackml
from trackml.dataset import load_event, load_dataset
import sys
import numpy as np
import pandas as pd

filename = '/home/vali/.kaggle/competitions/trackml-particle-identification/train_1/event000001000'

hits, cells, particles, truth = load_event(filename)
#print event_id, len(hits)
hits = pd.merge(hits,truth,on='hit_id')
hits['phi'] = np.arctan2(hits['y'],hits['x'])
hits['r'] = np.sqrt(hits['y']*hits['y']+hits['x']*hits['x'])
hits['tpt'] = np.sqrt(hits['tpx']*hits['tpx']+hits['tpy']*hits['tpy'])
hits['up0'] =0
hits['up1'] =0
hits.sort_values(by=['phi'],inplace=True)
hl1 = hits.query('volume_id ==8 & layer_id==2')
hl2 = hits.query('volume_id ==8 & layer_id==4')
hl3 = hits.query('volume_id ==8 & layer_id==6')
hl4 = hits.query('volume_id ==8 & layer_id==8')
print hl1.size
print hl2.size
print hl3.size

#TODO: use pd directly
d1 =  [list(x) for x in hl1[["x", "y", "z"]].values[:15]]
d2 =  [list(x) for x in hl2[["x", "y", "z"]].values[:15]]
d3 =  [list(x) for x in hl3[["x", "y", "z"]].values[:15]]


print d1
print d2
print d3

# xyz coordinates of hits on three seeding layers
layerPoints = [d1, d2, d3]
result = TTReco(layerPoints, ptMin=0., phiCut=0., thetaCut=100., regionOriginRadius=100)
print len(result)
