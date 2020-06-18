
from pyTT import TTReco, areAlignedRZ, haveSimilarCurvature
import trackml
from trackml.dataset import load_event, load_dataset
import sys
import numpy as np
import pandas as pd

filename =  '/home/vali/.kaggle/competitions/trackml-particle-identification/train_1/event000001000'

hits, cells, particles, truth = load_event(filename)
hits = pd.merge(hits,truth,on='hit_id')
hits['phi'] = np.arctan2(hits['y'],hits['x'])
hits['r'] = np.sqrt(hits['y']*hits['y']+hits['x']*hits['x'])
hits['tpt'] = np.sqrt(hits['tpx']*hits['tpx']+hits['tpy']*hits['tpy'])
hits['index'] = hits.index
#hits.sort_values(by=['phi'],inplace=True)
hits =hits[hits["particle_id"] > 0]
hl1 = hits.query('volume_id ==8 & layer_id==2')
hl2 = hits.query('volume_id ==8 & layer_id==4')
hl3 = hits.query('volume_id ==8 & layer_id==6')
hl4 = hits.query('volume_id ==8 & layer_id==8')

true_doublets = pd.merge(left=hl1, right=hl2, on=["particle_id"])
true_triplets = pd.merge(left=true_doublets, right=hl3, on=["particle_id"])

true_doublets["zdiff"] =  np.abs(true_doublets["z_x"] - true_doublets["z_y"])
true_doublets["phidiff"] =  np.abs(np.pi - np.abs(np.abs(true_doublets["phi_x"] - true_doublets["phi_y"]) - np.pi))

print true_doublets.shape[0]
print np.sum(true_doublets["zdiff"] < 280)
print np.sum(true_doublets["phidiff"] < 0.3)

for cut in [50.]:#np.linspace(0, 50, 1):
  def check_aligned(row):
    return areAlignedRZ(row["r_x"], row["z_x"], row["r_y"], row["z_y"], row["r"], row["z"], cut, 1.0)
    

  true_triplets["areAlignedRZ"] =  true_triplets.apply(check_aligned, axis=1)
  print cut, ": \t", true_triplets["areAlignedRZ"].sum() , " / ", true_triplets.shape[0]

#for cut in np.linspace(0, 50, 100):
def check_curvature(row):
  return haveSimilarCurvature(row["x_x"], row["y_x"], row["x_y"], row["y_y"], row["x"], row["y"],
       0., # region_origin_x
       0., # region_origin_y
       2., # region_origin_radius
       0.175, # phiCut
       0.2, # hardPtCut
       50. # ptMin
       )

true_triplets["haveSimilarCurvature"] =  true_triplets.apply(check_curvature, axis=1)
print cut, ": \t", true_triplets["haveSimilarCurvature"].sum() , " / ", true_triplets.shape[0]

