# pfRICH
Standalone ePIC pfRICH GEANT4 simulation codes

  It is assumed that thisroot.sh and geant4.sh were sourced. The latter is 
not needed to just import already produced ROOT trees.

  In the following the installation under /tmp/sandbox is assumed. Define SANDBOX 
environment variable accordingly if installing in a different directory. 

  Make sure 'cmake' is version 3.0 or higher. The following combination is known 
to work: SL7, gcc 4.8.5, cmake 3.22.1, ROOT 6.18.04, GEANT 4.10.05.p01 . 

Define installation area
------------------------

```
export SANDBOX=/tmp/sandbox
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
#  TODO: NEED TO UPLOAD NEW ONES
#cd ${SANDBOX}/pfRICH
# A pre-uploaded file with 500 events;
#root -l './examples/pfrich.C("examples/pfrich.root")'
#root -l './examples/multi-eval.C("examples/pfrich.root")'
```

Installation of the actual GEANT simulation environment requires few more steps (make sure 
geant.sh was sourced):


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

# Create links to the default header files; 
pushd share/include  && ln -s hrppd.default.h  hrppd.h && ln -s share.default.h share.h && popd
pushd epic/include   && ln -s epic.default.h   epic.h && popd
pushd ftbf/include   && ln -s ftbf.default.h   ftbf.h && popd
pushd tstand/include && ln -s tstand.default.h tstand.h && popd

mkdir build && cd build
# 'BMF' and 'HepMC3' are optional;
cmake -DCMAKE_INSTALL_PREFIX=${SANDBOX} -DIRT=${SANDBOX} -DBMF=${SANDBOX} -DHEPMC3=${SANDBOX} -DXERCES=${SANDBOX} -Wno-dev ..

make -j8 install
```

Run examples 
------------

```
cd ${SANDBOX}/pfRICH
./build/pfrich-epic -m macro/vis-epic.mac

./build/pfrich-epic -o pfrich-epic.root -s 1000
root -l 'scripts/hit-map-epic.C("pfrich-epic.root")'
root -l 'scripts/multi-eval-epic.C("pfrich-epic.root")'

./build/pfrich-ftbf -m macro/vis-ftbf.mac

# Will take quite some time because of the optical photon tracing in the lens radiator;
./build/pfrich-ftbf -o pfrich-ftbf.root -s 1000
root -l 'scripts/hit-map-ftbf-1x1.C("pfrich-ftbf.root")'
root -l 'scripts/hit-map-ftbf-2x2.C("pfrich-ftbf.root")'
#root -l 'scripts/multi-eval-ftbf.C("pfrich.root")'
```

Customize your environment
--------------------------

```
# Consider creating your local copies of the repository header files, which you may want to change,
# and pointing links like share.h to them, like:
cd ${SANDBOX}/pfRICH
pushd epic/include && rm epic.h && cp epic.default.h epic.xx.h && ln -s epic.xx.h epic.h && popd

# Then you can edit epic.xx.h locally without being affected by possible changes in epic.default.h
# which would overwrite your local changes after next pull
```

Automation
----------

```
# TODO: UPDATE THIS SECTION
#Inside pfRICH directory run
#`bash ./scripts/cc/AutomatedSimulationChain.sh`
#It will display all the features with an example.
```
