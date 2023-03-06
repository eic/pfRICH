# pfRICH
Standalone ePIC pfRICH GEANT4 simulation codes

  It is assumed that thisroot.sh and geant4.sh were sourced. The latter is 
not needed to just import already produced ROOT trees.

  In the following installation under /tmp is assumed. Define SANDBOX 
environment variable accordingly if installing in a different directory. 
Make sure 'cmake' is version 3.0 or higher.

Define installation area
------------------------

```
export SANDBOX=/tmp
export LD_LIBRARY_PATH=${SANDBOX}/lib:${SANDBOX}/lib64:${LD_LIBRARY_PATH}
```

Install IRT library
-------------------

```
cd ${SANDBOX}
git clone -b pfrich https://github.com/eic/irt.git
cd irt && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -Wno-dev ..
make -j8 install
```

Download pfRICH repository
--------------------------

```
cd ${SANDBOX}
git clone https://github.com/alexander-kiselev/pfRICH.git
```

This is sufficient to import and analyze ROOT trees already produced after pfRICH 
GEANT simulation pass elsewhere (as long as there were no drastic changes to the 
C++ class variable description):


Try out example 
---------------

```
cd ${SANDBOX}/pfRICH
# A pre-uploaded file with 500 events;
root -l './examples/pfrich.C("examples/pfrich.root")'
root -l './examples/multi-eval.C("examples/pfrich.root")'
```

Installation of the actual GEANT simulation environment requires few more steps:


Install Xerces-c
----------------

```
cd ${SANDBOX} 
# Copy over xerces-c-3.2.4.tar.gz (or a similar version) from the Web, unpack;
cd xerces-c-3.2.4 && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -Wno-dev ..
make -j8 install
```


Install BeastMagneticField (optional)
-------------------------------------

```
cd ${SANDBOX} 
git clone https://github.com/eic/BeastMagneticField.git
cd BeastMagneticField && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -Wno-dev ..
make -j8 install
```

Install HepMC3 (optional)
-------------------------

```
cd ${SANDBOX}
git clone https://gitlab.cern.ch/hepmc/HepMC3.git
cd HepMC3 && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -DHEPMC3_ENABLE_ROOTIO=ON -DHEPMC3_ENABLE_PYTHON=OFF ..
make -j8 install
```

Install pfRICH simulation codes
-------------------------------

```
cd ${SANDBOX}/pfRICH

# Create a custom tuning.xx.h file (aka configuration file); use tuning.ak.h as a template;
cd include && cp tuning.ak.h  tuning.xx.h && ln -s tuning.xx.h tuning.h && cd ..

mkdir build && cd build
# 'BMF' and 'HepMC3' are optional;
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -DIRT=${SANDBOX} -DBMF=${SANDBOX} -DHEPMC3=${SANDBOX} -DXERCES=${SANDBOX} -Wno-dev ..

make -j8 install
```

Run examples 
------------

```
cd ${SANDBOX}/pfRICH
./build/pfrich -s 1000

root -l 'scripts/ak/pfrich.C("pfrich.root")'

./build/pfrich -m macro/ak/vis.mac

```
Automation
----------
```
Inside pfRICH directory run
`bash ./scripts/cc/AutomatedSimulationChain.sh`
It will display all the features with an example.
```
