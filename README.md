# pfRICH
Standalone ePIC pfRICH GEANT4 simulation codes

  It is assumed that thisroot.sh and geant.sh were sourced. In the following 
installation under /tmp is assumed. Change the commands accordingly if installing 
in a different directory. Make sure 'cmake' is 3.0 or higher.

IRT installation
----------------

```
cd /tmp 
git clone -b pfrich https://github.com/eic/irt.git
cd irt && mkdir build && cd build
make -j8 install

export LD_LIBRARY_PATH=/tmp/irt/build/lib:${LD_LIBRARY_PATH}
```

pfRICH installation
-------------------

```
cd /tmp 
git clone https://github.com/alexander-kiselev/pfRICH.git
cd pfRICH && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=. -DIRT=/tmp/irt/build -Wno-dev ..
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