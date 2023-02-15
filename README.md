# pfRICH
Standalone ePIC pfRICH GEANT4 simulation codes

  It is assumed that thisroot.sh and geant4.sh were sourced. In the following 
installation under /tmp is assumed. Change the commands accordingly if installing 
in a different directory. Make sure 'cmake' is 3.0 or higher.

IRT installation
----------------

```
cd /tmp 
git clone -b pfrich https://github.com/eic/irt.git
cd irt && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=. -Wno-dev ..
make -j8 install

export LD_LIBRARY_PATH=/tmp/irt/build/lib:${LD_LIBRARY_PATH}
```

BeastMagneticField installation (optional)
------------------------------------------

```
cd /tmp 
git clone https://github.com/eic/BeastMagneticField.git
cd BeastMagneticField && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=. -Wno-dev ..
make -j8 install

export LD_LIBRARY_PATH=/tmp/BeastMagneticField/build/lib:${LD_LIBRARY_PATH}

wget https://wiki.bnl.gov/EPIC/images/f/f5/MARCO_v.6.4.1.1.3_2T_Magnetic_Field_Map_2022_11_14_rad_coords_cm_T.txt
```

HepMC3 installation (optional)
------------------------------

```
cd /tmp 
git clone https://gitlab.cern.ch/hepmc/HepMC3.git
cd HepMC3 && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/home/ayk/eic -DHEPMC3_ENABLE_ROOTIO=ON -DHEPMC3_ENABLE_PYTHON=OFF ..
make -j8 install

export LD_LIBRARY_PATH=/tmp/HepMC3/build/lib:${LD_LIBRARY_PATH}
```

pfRICH installation
-------------------

```
cd /tmp 
git clone https://github.com/alexander-kiselev/pfRICH.git
cd pfRICH && mkdir build && cd build
# 'BMF' and 'HepMC3' are optional;
cmake -DCMAKE_INSTALL_PREFIX=. -DIRT=/tmp/irt/build -DBMF=/tmp/BeastMagneticField/build -DHepMC3=/tmp/HepMC3/build -Wno-dev ..
make -j8 install

export LD_LIBRARY_PATH=/tmp/pfRICH/build/lib:${LD_LIBRARY_PATH}
```

Examples 
--------

```
cd /tmp/pfRICH
./build/pfrich
```

```
cd /tmp/pfRICH
./build/pfrich -m macro/pfrich.mac

root -l 'scripts/pfrich.C("pfrich.root")'

root -l 'scripts/e-eval.C("pfrich.root")'

```