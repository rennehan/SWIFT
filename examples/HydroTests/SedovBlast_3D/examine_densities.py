import h5py
import numpy as np

f = h5py.File("sedov_0005.hdf5")

balsara = np.array(f["PartType0/BalsaraFactors"])

print("Balsara factors")
print(np.histogram(balsara))

C_means = np.array(f["PartType0/ContraharmonicMeanDensities"])

print("Contraharmonic means")
print(np.histogram(C_means))

rho = np.array(f["PartType0/Densities"])

print("Densities")
print(np.histogram(rho))

f.close()

