This is for shms pion tracking efficiency. 
 #info taking
  #for D2
  shms_rate_D2.cxx: This scripte find the raw counts and time in scaler tree and write it to results/TE/trackingeff_info.json
  shms_TE_D2runs.cxx: find pion expected and found in data tree and write it to results/TE/trackingeff_info.json
  #for Dummy
  shms_rate_Dummy.cxx: same thing as D2 but for dummy
  shms_TE_Dummyruns.cxx: same thing as D2 but for dummy
 #info ploting
   plot_te_fall.cxx
   plot_te_spring.cxx
   plot_te_both.cxx
 #info passing
   write_te_to_json.cxx: read info from trackingeff_info.json, calculate tracking efficienct, then write it to results/yield/runs_info_te.json. Need to change name runs_info_te.json to runs_info.json, this json is for yield ratio ploting. This json file will need some other info from shuo_analysis/Yield/statistic_runs_D2/Dummy.cxx

#### In slurm, when submit this job, all script will write in one output file trackingeff_info.json, so it will be opened and closed simultaneously, it will loss some infos. 
   This is just for D2 now. I modified Dummy script, now it can loop over all rungroups. (Not so many data for dummy compared with D2)
solution: When submitting jobs in parallel, submit each job with longer interval. Run plot te script, it will alert which rungroup is missing infos. Or, write output separately in a directory, then use another script to combine all output files into one(as in yield) 
