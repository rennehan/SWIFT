#!/bin/bash

# Generate the initial conditions if they are not present.
if [ ! -e glassPlane_128.hdf5 ]
then
    echo "Fetching initial glass file for the Sod shock example..."
    ./getGlass.sh
fi
if [ ! -e brioWuShock.hdf5 ]
then
    echo "Generating initial conditions for the Sod shock example..."
    python3 makeIC.py
fi

# Configure as: ./configure --with-hydro=gadget2 --enable-mhd --with-hydro-dimension=2 --with-adiabatic-index=2 --disable-hand-vec
# Run SWIFT
../../../swift --hydro --threads=1 brioWuShock.yml 2>&1 | tee output.log

python3 plotSolution.py 1
