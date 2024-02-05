#!/bin/bash
export LC_NUMERIC="en_US.UTF-8"
unset -v IFS
if [[ $# -eq 0 ]] ; then
    echo 'You can add more than one options'
    echo 'The code assumes there is no conflict with number of generated events in hepmc and inside tuning.h'    
    echo 'bash AustomatedSimulationChain.sh -<option1> -<option2> ...'	
		echo '-N Name initials ak/cc/cn/jd are available options'
    echo '-v G Geometry Visulaization mode # No output file will be saved'
    echo '-v R Running Full simulation chain #It will generate all files'
    echo '-H D Default values in hepmc files'
    echo '-H U User defined momentum and eta range'
    echo '-P PDGCodes Set Particle Types(4 at max)'
    echo '-m <momMin> <MomMax> Lower level of momentum '
    echo '-e <etaMin> <etaMax> Lower level of momentum '
    echo '-b bin size for kinematics first argument for eta second one for momentum'
    echo '-E  Nevent  Number of events to be processed(Default 1000)'
    echo '-A 0 No analysis will be performed'
    echo '-A 1 Analysis will be performed'
    echo '-F S All user defined kinematics ranges will be generated in single  Hepmc files and so for simulation output'
    echo '-F B All user defined kinematics ranges will be generated in different Hepmc files (defined or 0.5 default bins) and so for simulation output'
    echo 'Example: '
    echo '   bash AutomatedSimulationChain.sh -N cc -v R -t 1 -P 211 -P 11 -m 0.05 -m 5 -e -2.5 -e -1.5 -E 1000 -A 1 -F 1'
    echo 'For user cc the code will run the Full simulations and analysis' 
    echo 'for pions and electrons of Momentum 0.05 and 5 GeV/c; at eta -1.5 and -2.5 storing files with 0.01 bins'    
    exit 0
fi

while getopts v:H:P:m:e:b:E:A:F:N: flag
do 
	case "${flag}" in
		v)  Version=${OPTARG};;
		H)  Type=${OPTARG};;
		P)  Part+=($OPTARG);;
		m)  Mom+=($OPTARG);;
		e)  Eta+=($OPTARG);;
		b)  Bin=${OPTARG};;
		E)  Events=${OPTARG};;
		A)  Analysis=${OPTARG};;
		F)  Filing=${OPTARG};;
		N)  Name=${OPTARG};;
	esac	
done
shift $((OPTIND -1))
########## Folder Settings############
if [[ $Name == "" ]];then
	echo "Provide a name initial to create directory to copy files"
  exit 0
elif [[ $Name != "" ]]; then
	MyDir="./scripts/cc"
	LogBook="$MyDir/LogBook.txt"
	HepMC3Dir="./scripts/$Name/HepMC3Files"
	SimulationDir="./scripts/$Name/SimulationFiles"
	OutputDir="./scripts/$Name/OutputFiles"
  echo "'${OutputDir}'"
  mkdir -p ${HepMC3Dir} && mkdir -p ${SimulationDir} && mkdir -p ${OutputDir}
fi
now=$(date)

echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   Start of log Book By $Name at $now@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@">>$LogBook

######################################
if [ $Version == "G" ]; then
	echo "Visulaization Mode is running"
	./build/pfrich -m macro/$Name/vis.mac
fi	
if [ $Version == "R" ]; then  
	echo "Full Simulation Mode activated "
	if [[ $Type == "" ]]; then
	echo "You need to specify Hepmc Type"
	elif [[ $Type == "D" ]]; then
		echo "Default hepmc Values are in use"
		root -l "${MyDir}/hepmc_writer_two_particles.C+($Events)"
		./build/pfrich -i out.hepmc
		root -l "$MyDir/multi-eval.C"
	elif [[ $Type == "U" ]]; then
		echo "You are feeding momentum, particle pdg, eta and number of events"
	  echo "Momentum: '${Mom[@]}' Particles: '${Part[@]}' Eta:'${Eta[@]}'"
		if [[ $Filing == "S" ]]; then
			moml=${Mom[0]}
			momh=${Mom[1]}
			etal=${Eta[0]}
			etah=${Eta[1]}
			if [[ $Events == "" ]]; then 
			Events=1000 
			echo "$Events set to ${Events}"
			fi
			echo "${#Part[@]} Particles are passed" 
			for part in  "${Part[@]}"
			 do 
			   echo "$part"
			   PName+=$part","
			done
			echo "$PName"
			echo "root -l './scripts/$Name/hepmc_writer_two_particles_full.C+($moml,$momh,$etal,$etah",${PName}"$Events)' evoked">>$LogBook
			root -l "./scripts/$Name/hepmc_writer_two_particles_full.C+($moml,$momh,$etal,$etah",${PName}"$Events)"
			echo "./build/pfrich -i out.hepmc -s $Events Evoked">>$LogBook
			./build/pfrich -i out.hepmc -s $Events
			HepMC3File="$HepMC3Dir/out.p.$moml_$momh.eta.$etal_$etah.${PName}.hepmc"
			SimulationFile="$SimulationDir/pfrich.p.$moml_$momh.eta.$etal_$etah.${PName}.root"
			cp out.hepmc $HepMC3File && cp pfrich.root $SimulationFile
			if [[ $Analysis == 1 ]]; then
				echo "root -l $MyDir/multi-eval.C Evoked">>$LogBook
				root -l "$MyDir/multi-eval.C"				
				OutputFile="$OutputDir/result.p.$moml_$momh.eta.$etal_$etah.${PName}.root"
				cp output.root $OutputFile
			else echo "No Anlysis Argument passed">>$LogBook
			fi				
			now=$(date)
			echo "############################################################">>$LogBook
      echo "                   Copy of Tuning file used                 ">>$LogBook
      echo "############################################################">>$LogBook
			cat include/tuning.h>>$LogBook
				echo "!!!!!!!!!!!!!!!!File Location!!!!!!!!!!!!">>$LogBook
			ls -ltrh $HepMC3File $SimulationFile $OutputFile >> $LogBook
			echo "$HepMC3File $SimulationFile $OutputFile"		
			echo "@@@@@@@@@@@@@@@@@@@@@Log Book Closed: $now by $Name ">>$LogBook
		  elif [[ $Filing == "B" ]]; then
				moml=${Mom[0]}
				momh=${Mom[1]}
				etal=${Eta[0]}
				etah=${Eta[1]}
				if [[ $Events == "" ]]; then 
				Events=1000 
				echo "$Events set to ${Events}"
				fi
				echo "${#Part[@]} Particles are passed" 
				for part in  "${Part[@]}"
			 		do 
			   		echo "$part"
			   		PName+=$part","
					done
				if [[ $Bin == "" ]]; then
					Bin=1.5
					for et in $(seq $etal $Bin $etah);
						do
							ee=`echo $et+$Bin|bc`
							echo "Eta bins: $et $ee"
							for pp in $(seq $moml $Bin $momh)
								do
									mom=`echo $pp+$Bin|bc`
									echo "Mom Bins: $pp $mom"
									echo "root -l '$MyDir/hepmc_writer_two_particles_user.C+($pp,$et",${PName}"$Events)' evoked">>$LogBook
									root -l "${MyDir}/hepmc_writer_two_particles_user.C+($pp,$et",${PName}"$Events)"
							
									HepMC3File="$HepMC3Dir/out.p.$pp.eta.$et.${PName}.hepmc"
									echo "./build/pfrich -i out.hepmc evoked">>$LogBook
									./build/pfrich -i out.hepmc -s $Events
									cp out.hepmc $HepMC3File && cp pfrich.root $SimulationFile
									SimulationFile="$SimulationDir/pfrich.p.$moml_$momh.eta.$etal_$etah.${PName}.root"
									echo "root -l $MyDir/multi-eval.C  evoked "
									if [ $Analysis == 1 ]; then 
										root -l "$MyDir/multi-eval.C"
										OutputFile="$OutputDir/result.p.$moml_$momh.eta.$etal_$etah.${PName}.root"
										cp output.root $OutputFile
									else echo "You have not switched ON analysis Command"
									fi
								done
					done
					now=$(date)
					echo "############################################################">>$LogBook
					echo "                   Copy of Tuning file used                 ">>$LogBook
					echo "############################################################">>$LogBook
					cat include/tuning.h>>$LogBook
					echo "!!!!!!!!!!!!!!!!File Location!!!!!!!!!!!!">>$LogBook
					ls -ltrh $HepMC3File $SimulationFile $OutputFile >> $LogBook
					ls -ltrh $HepMC3File $SimulationFile $OutputFile >> $LogBook
					now=$(date)
					echo "@@@@@@@@@@@@@@@@@@@@@Log Book Closed: $now by $Name ">>$LogBook
					elif [[ $Bin != "" ]]; then
			  	BinEta=${Bin[0]}
			  	BinMom=${Bin[1]} 
					for et in $(seq $etal $BinEta $etah)
						do
							for pp in $(seq $moml $BinMom $momh)
							do
								echo "root -l '$MyDir/hepmc_writer_two_particles_user.C+("out.hepmc",$pp,$et",${PName}"$Events)'   Evoked">>$LogBook
								root -l "$MyDir/hepmc_writer_two_particles_user.C+($pp,$et",${PName}"$Events)"
								HepMC3File="$HepMC3Dir/out.p.$pp.eta.$et.${PName}.hepmc"
								echo "./build/pfrich -i out.hepmc   Evoked">>$LogBook
								./build/pfrich -i out.hepmc -s $Events
								cp out.hepmc $HepMC3File && cp pfrich.root $SimulationFile
								SimulationFile="$SimulationDir/pfrich.p.$pp.eta.$et.${PName}.root"
								echo "root -l $MyDir/mult-eval.C evoked">>$LogBook
								if [[ $Analysis == 1 ]]; then
									echo 'root -l "$MyDir/multi-eval.C" Evoked'
									root -l "$MyDir/multi-eval.C"
									OutputFile="$OutputDir/result.p.$pp.eta.$et.${PName}.root"
									cp output.root $OutputFile
									else echo "You have not switched ON analysis Command"
								fi
							done
				done
				now=$(date)
				echo "############################################################">>$LogBook
				echo "                   Copy of Tuning file used                 ">>$LogBook
				echo "############################################################">>$LogBook
				cat include/tuning.h>>$LogBook
				echo "!!!!!!!!!!!!!!!!File Location!!!!!!!!!!!!">>$LogBook
				ls -ltrh $HepMC3File $SimulationFile $OutputFile >> $LogBook
				now=$(date)
				echo "@@@@@@@@@@@@@@@@@@@@@Log Book Closed: $now by $Name ">>$LogBook
			fi						
		fi				
	else echo "Put a correct argument to run this mode"			
 fi	
fi
