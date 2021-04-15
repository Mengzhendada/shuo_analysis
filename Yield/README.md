#This is for yield calculating. 
  
  //statistic_runs_D2/Dummy.cxx: main script. It founds cuts in db2/all_cuts.json, apply all cuts on data, write histograms into results/yield/kinematics/yield_runnumber.root, print check plots in results/yield/check/runnumber_neg/pos/.png. Print out statistic numbers in results/yield/run_info/"rungroup.json
  Now the cuts is done in check/skim_check.cxx, this script is not necessary anymore, however, the combine_json is needed after skim_check.cxx 
  combine_json: after the first script, it prints all infos in separated json file for each rungroup, this one will combine all json files into /results/yield/run_info/runs_info.json. After this, need copy runs_info.json back to result/s/yield/ directory. 
#before actually plot the yield, need to go back to TE directory to write all tracking efficiencies into runs_info.json. This is done by shuo_analysis/TE/write_te_to_json.cxx. This script will read both runs_info.json and tracking_info.json, write TEs into results/Yield/runs_info_te.json. Need to change name to runs_info.json for further ploting
  plot_Q2x_June.cxx: this one will read all infos in runs_info.json, then plot yield and yield ratio for each kinematic settings. It will create one for each Q2x settings, then on each Q2x settings, plot all different z
  plot_Q2x_ratio_new.cxx: This one will read histograms in results/yield/kinematics/yield_runnumber, then rebin histograms and plot the yield ratio. Out plots are in results/yield/statistics/x_Q2_0.50_5.000_ratio.pdf

2021Jan update
  plot_kin_Q2x.cxx should be the correct one to use to plot all pions kin settings in one plot. It is written after plot_kin.cxx. It uses db2/kin_group_xq2.json, I already forgot which script is used to generate this, however it is generated from ratio_run_group_

  2021 March
  plot_Q2x_ratio_raw.cxx
  changed plot_Q2x_ratio_new.cxx to plot_Q2x_ratio_raw.cxx. This script has updates on coin time cut and rftime cut. It plots counts yield and ratio in results/yield/statistics

  plot_Q2x_ratio_corr.cxx
  copied plot_Q2x_ratio_raw.cxx, need to add efficiencies correction

  statistics_runs_D2.cxx : updated weight, which applies pi eff and pi purity on 1d histogram. Need to continue for 2d histogram

2021 April
  for pos runs in slurm/redo.txt, their coin time peak is at wrong position, don't know why
  deleted 6081, it's a short 8 mins run 
