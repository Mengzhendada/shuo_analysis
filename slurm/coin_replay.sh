#!/bin/bash

#echo $LD_LIBRARY_PATH

#module list
module purge
module use /lcrc/project/jlab/local/etc/modulefiles
module load singularity
module load hallac_container/1.9.1
#module list

RUN=$1

#export SINGULARITY_BINDPATH="/lcrc,/scratch" 
REPLAYDIR="/lcrc/project/jlab/csv/offline/csv"
TMPDIR="/scratch/replay-$RUN"
ODIR="$REPLAYDIR/ROOTfiles"


echo "================================"
echo "Running replay for COIN run $RUN"
echo "================================"

cd ${REPLAYDIR}
mkdir -p $TMPDIR/full
mkdir -p $TMPDIR/logs
which hcana
echo $LD_LIBRARY_PATH
#/soft/singularity/3.5.3/bin/singularity exec  -B /blues:/blues  -B /lcrc:/lcrc  -B /scratch:/scratch  -B /gpfs:/gpfs /lcrc/project/jlab/local/opt/hallac_container_1.9.1/lib/hallac-1.9.1.sif bash
#/soft/singularity/3.5.3/bin/singularity exec  -B /blues:/blues  -B /lcrc:/lcrc  -B /scratch:/scratch  -B /gpfs:/gpfs /lcrc/project/jlab/local/opt/hallac_container_1.9.1/lib/hallac-1.9.1.sif hcana "scripts/replay_production_coin.cxx+($RUN,-1)" || exit $?
hcana "scripts/replay_production_coin.cxx+($RUN,-1)" || exit $?
rsync -va $TMPDIR/full/* $ODIR/full
mkdir -p $ODIR/log/log-$RUN
rsync -va $TMPDIR/logs/* $ODIR/log/log-$RUN
rm -rf $TMPDIR

echo "================================="
echo "Finished processing COIN run $RUN"
echo "================================="
