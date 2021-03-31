#!/bin/bash

REPLAYDIR="/lcrc/project/jlab/csv/offline/online_csv"


echo "================================"
echo "Get Live time"
echo "================================"

cd ${REPLAYDIR}
root -q -b "shuo_analysis/LT/Computer_live_time.cxx+(10*$1)" || exit$?
root -q -b "shuo_analysis/LT/Total_live_time.cxx+(10*$1)" || exit$?
#for(( i = 1;i<59;i++))
#do
#root -q -b "shuo_analysis/LT/Computer_live_time.cxx+(10*$i)" || exit$?
#root -q -b "shuo_analysis/LT/Total_live_time.cxx+(10*$i)" || exit$?
#done
echo "================================="
echo "end of do something $RUNGROUP"
echo "================================="
