#! /bin/env csh

## Variabe Exec should be set to the absolute path of the runSimu.sh script (should be in same dir as this script)
## set Exec = /yourDIR/runSimu.sh
set Exec = 

####### Initialize condor file
echo ""  > CondorFile
echo "Universe     = vanilla" >> CondorFile
echo "Executable   = ${Exec}" >> CondorFile
echo "getenv = true" >> CondorFile

## Output Directory
## Output should point to the dir you wan the error and stdOut files to go (again, likely the same dir as this script)
set Output = 

####### Set number of jobs and events per job
set NUMBER = 1
set LIMIT = 10
set NEVENTS = 100000

####### Loop over jobs
while ( "$NUMBER" <= "$LIMIT" )

    set OutFile = ${Output}/submitSimu_$NUMBER.out
    set ErrFile = ${Output}/submitSimu_$NUMBER.err

    set Args = ( $NUMBER $NEVENTS )
    echo "" >> CondorFile
    echo "Output       = ${OutFile}" >> CondorFile
    echo "Error        = ${ErrFile}" >> CondorFile
    echo "Arguments    = ${Args}" >> CondorFile
    echo "Queue" >> CondorFile   

    echo Submitting:
    echo $Exec $Args
    echo "Logging output to " $OutFile
    echo "Logging errors to " $ErrFile

    @ NUMBER++

end


#condor_submit CondorFile


