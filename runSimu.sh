#!/bin/bash

RUN=$1
NUM=$2

date

fileNameElectron=electronEvents_${RUN}.hepMC
fileNamePion=pionEvents_${RUN}.hepMC
fileNameKaon=kaonEvents_${RUN}.hepMC
fileNameProton=protonEvents_${RUN}.hepMC

echo ""
echo "Generating HepMC Files"
echo ${fileNameElectron}
echo ${fileNamePion}
echo ${fileNameKaon}
echo ${fileNameProton}

## Supply absolute path to the hepmc_writer.C script
root -b -l -q "/yourDir/hepmc_writer.C+(\"${fileNameElectron}\",0,2.65,3.10,0.1,15.0,${NUM})" 
root -b -l -q "/yourDir/hepmc_writer.C+(\"${fileNamePion}\",1,2.65,3.10,0.1,15.0,${NUM})" 
root -b -l -q "/yourDir/hepmc_writer.C+(\"${fileNameKaon}\",2,2.65,3.10,0.1,15.0,${NUM})" 
root -b -l -q "/yourDir/hepmc_writer.C+(\"${fileNameProton}\",3,2.65,3.10,0.1,15.0,${NUM})" 

simuNameElectron=pfRICH-epic_electron_${RUN}.root
simuNamePion=pfRICH-epic_pion_${RUN}.root
simuNameKaon=pfRICH-epic_kaon_${RUN}.root
simuNameProton=pfRICH-epic_proton_${RUN}.root

echo ""
echo "Generating Simulation Files"
echo ${simuNameElectron}
echo ${simuNamePion}
echo ${simuNameKaon}
echo ${simuNameProton}

## Supply absolute path to the pfRICH build dir
/yourDir/build/pfrich-epic -i ${fileNameElectron} -o ${simuNameElectron} -s ${NUM}
/yourDir/build/pfrich-epic -i ${fileNamePion} -o ${simuNamePion} -s ${NUM}
/yourDir/build/pfrich-epic -i ${fileNameKaon} -o ${simuNameKaon} -s ${NUM}
/yourDir/build/pfrich-epic -i ${fileNameProton} -o ${simuNameProton} -s ${NUM}

rootNameElectron=outElectron_${RUN}.root
rootNamePion=outPion_${RUN}.root
rootNameKaon=outKaon_${RUN}.root
rootNameProton=outProton_${RUN}.root

echo ""
echo "Generating Root Trees"
echo ${rootNameElectron}
echo ${rootNamePion}
echo ${rootNameKaon}
echo ${rootNameProton}

root -b -l -q "scripts/reco-epic-LUT.C(\"${simuNameElectron}\",\"${rootNameElectron}\")"
root -b -l -q "scripts/reco-epic-LUT.C(\"${simuNamePion}\",\"${rootNamePion}\")"
root -b -l -q "scripts/reco-epic-LUT.C(\"${simuNameKaon}\",\"${rootNameKaon}\")"
root -b -l -q "scripts/reco-epic-LUT.C(\"${simuNameProton}\",\"${rootNameProton}\")"

date
