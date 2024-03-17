#!/bin/bash

etaMin=(-3.8 -3.5 -3.0 -2.8 -2.0)
etaMax=(-3.5 -3.0 -2.8 -2.0 -1.5)
etaName=(3835 3530 3028 2820 2015)

momMin=(0.05 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0 12.0 13.0 14.0)
momMax=(0.10 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 2.0 3.0 4.0 5.0 6.0 7.0 8.0 9.0 10.0 11.0 12.0 13.0 14.0 15.0)
momAve=(0.075 0.15 0.25 0.35 0.45 0.55 0.65 0.75 0.85 0.95 1.50 2.50 3.50 4.50 5.50 6.50 7.50 8.50 9.50 10.50 11.50 12.50 13.50 14.50)
momName=(00501 0102 0203 0304 0405 0506 0607 0708 0809 0910 1020 2030 3040 4050 5060 6070 7080 8090 90100 100110 110120 120130 130140 140150)

## Run one eta bin at a time
for etaLoop in {0..0}
do

    echo "Eta Range ${etaMin[$etaLoop]} ${etaMax[$etaLoop]}" >> sigmaSummary.txt

    for momLoop in {0..23}
    do
	
	echo "Processing ${etaMin[$etaLoop]} ${etaMax[$etaLoop]}    ${momMin[$momLoop]} ${momMax[$momLoop]}"
	fileNameElectron=electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.hepMC
	fileNamePion=pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.hepMC
	fileNameKaon=kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.hepMC
	fileNameProton=proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.hepMC

	echo ""
	echo "Generating HepMC Files"
	echo ${fileNameElectron}
	echo ${fileNamePion}
	echo ${fileNameKaon}
	echo ${fileNameProton}

	root -b -l -q "hepmc_writer.C+(\"${fileNameElectron}\",0,${etaMin[$etaLoop]},${etaMax[$etaLoop]},${momMin[$momLoop]},${momMax[$momLoop]},1000)" 
	root -b -l -q "hepmc_writer.C+(\"${fileNamePion}\",1,${etaMin[$etaLoop]},${etaMax[$etaLoop]},${momMin[$momLoop]},${momMax[$momLoop]},1000)" 
	root -b -l -q "hepmc_writer.C+(\"${fileNameKaon}\",2,${etaMin[$etaLoop]},${etaMax[$etaLoop]},${momMin[$momLoop]},${momMax[$momLoop]},1000)" 
	root -b -l -q "hepmc_writer.C+(\"${fileNameProton}\",3,${etaMin[$etaLoop]},${etaMax[$etaLoop]},${momMin[$momLoop]},${momMax[$momLoop]},1000)" 

	simuNameElectron=pfRICH-epic_electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.root
	simuNamePion=pfRICH-epic_pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.root
	simuNameKaon=pfRICH-epic_kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.root
	simuNameProton=pfRICH-epic_proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.root

	echo ""
	echo "Generating Simulation Files"
	echo ${simuNameElectron}
	echo ${simuNamePion}
	echo ${simuNameKaon}
	echo ${simuNameProton}

	./build/pfrich-epic -i ${fileNameElectron} -o ${simuNameElectron} -s 1000
	./build/pfrich-epic -i ${fileNamePion} -o ${simuNamePion} -s 1000
	./build/pfrich-epic -i ${fileNameKaon} -o ${simuNameKaon} -s 1000
	./build/pfrich-epic -i ${fileNameProton} -o ${simuNameProton} -s 1000

	root -b -l -q "scripts/reco-epic.C(\"${simuNameElectron}\")" >> tmpOut_electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt
	root -b -l -q "scripts/reco-epic.C(\"${simuNamePion}\")" >> tmpOut_pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt
	root -b -l -q "scripts/reco-epic.C(\"${simuNameKaon}\")" >> tmpOut_kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt
	root -b -l -q "scripts/reco-epic.C(\"${simuNameProton}\")" >> tmpOut_proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt

	tmpElectronNGamma=`grep "!!!!!!!" tmpOut_electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpPionNGamma=`grep "!!!!!!!" tmpOut_pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpKaonNGamma=`grep "!!!!!!!" tmpOut_kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpProtonNGamma=`grep "!!!!!!!" tmpOut_proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`

	if [[ ! -n $tmpElectronNGamma ]]; then
	    tmpElectronNGamma=0.0
	fi

	if [[ ! -n $tmpPionNGamma ]]; then
	    tmpPionNGamma=0.0
	fi

	if [[ ! -n $tmpKaonNGamma ]]; then
	    tmpKaonNGamma=0.0
	fi

	if [[ ! -n $tmpProtonNGamma ]]; then
	    tmpProtonNGamma=0.0
	fi

	tmpElectronMean=`grep "%%%%%%%" tmpOut_electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpPionMean=`grep "%%%%%%%" tmpOut_pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpKaonMean=`grep "%%%%%%%" tmpOut_kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpProtonMean=`grep "%%%%%%%" tmpOut_proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`

	if [[ ! -n $tmpElectronMean ]]; then
	    tmpElectronMean=0.0
	fi

	if [[ ! -n $tmpPionMean ]]; then
	    tmpPionMean=0.0
	fi

	if [[ ! -n $tmpKaonMean ]]; then
	    tmpKaonMean=0.0
	fi

	if [[ ! -n $tmpProtonMean ]]; then
	    tmpProtonMean=0.0
	fi

	tmpElectronSigma=`grep "#######" tmpOut_electron_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpPionSigma=`grep "#######" tmpOut_pion_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpKaonSigma=`grep "#######" tmpOut_kaon_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`
	tmpProtonSigma=`grep "#######" tmpOut_proton_eta${etaName[$etaLoop]}_mom${momName[$momLoop]}.txt | awk '{print $2}'`

	if [[ ! -n $tmpElectronSigma ]]; then
	    tmpElectronSigma=0.0
	fi

	if [[ ! -n $tmpPionSigma ]]; then
	    tmpPionSigma=0.0
	fi

	if [[ ! -n $tmpKaonSigma ]]; then
	    tmpKaonSigma=0.0
	fi

	if [[ ! -n $tmpProtonSigma ]]; then
	    tmpProtonSigma=0.0
	fi

	echo "eta->AddMomentumRange(${momMin[$momLoop]}, ${momMax[$momLoop]}, ${tmpElectronSigma}, ${tmpPionSigma}, ${tmpKaonSigma}, ${tmpProtonSigma});" >> sigmaSummary.txt

	echo "${momAve[$momLoop]} ${tmpElectronNGamma} ${tmpElectronMean} ${tmpPionNGamma} ${tmpPionMean} ${tmpKaonNGamma} ${tmpKaonMean} ${tmpProtonNGamma} ${tmpProtonMean}" >> gammaMeanSummary.dat
	
    done

    echo "" >> sigmaSummary.txt

done

