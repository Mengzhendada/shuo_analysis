This make run groups
The CSV rungroup are made by runnumberclassify, 

April2022
Re-Group the ptsidis runs, scripts are copied and done Nov2021(I guess). 
Steps:
1. run runnumberclassify_ptsidis.cxx
2. run runnumberclassify_ptsidis_updata.cxx
3. run rearange_ratio_group_ptsidis.cxx
The output from step 3, rearange_ratio_run_group_ptsidis.json is the one wanted
Don't ask why, I don't remember. The target is labeled wrong, so I changed in runnumberclassify_ptsidis.cxx. Then it took another two steps to get the similar file used as CSV
