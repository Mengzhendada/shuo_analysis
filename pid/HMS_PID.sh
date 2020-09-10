#!/bin/bash

module load singularity
module use /lcrc/project/jlab/local/etc/modulefiles
module load hallac_container/1.5.5

RUNGROUP=$1

export SINGULARITY_BINDPATH="/lcrc,/scratch" 
REPLAYDIR="/lcrc/project/jlab/csv/offline/online_csv"
TMPDIR="/scratch/kin-$RUNGROUP"
ODIR="$REPLAYDIR/results"


echo "================================"
echo "HMS PID for $RUNGROUP"
echo "================================"

cd ${REPLAYDIR}
root -q -b "shuo_analysis/pid/HMS_PID_RunGroup_calcut.cxx+($RUNGROUP)" || exit $?
root -q -b "shuo_analysis/pid/HMS_PID_RunGroup_cercut.cxx+($RUNGROUP)" || exit $?
root -q -b "shuo_analysis/pid/HMS_PID_plots.cxx+($RUNGROUP)" || exit $?
root -q -b "shuo_analysis/pid/HMS_cuts_compare.cxx+($RUNGROUP)" || exit $?

echo "================================="
echo "end of do something $RUNGROUP"
echo "================================="
