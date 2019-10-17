#These are my scripts. 

#tool scripts: 
readroot*.c: plot variable that are interested in and save in .pdf to download
creat_txt  : creat some annoying repeated txt
makelatexfromttxt.C: write repeated latex format txt
pre steering scripts:
string compare.C: test if string1 is included in string2
testrange.c: test x and y axis range in TGraph
txt_json: unfinished(forgot why)
calorimeter_gain_analysis: to_json.c: convert calorimeter output txt file to json format
help scripts:
json_update.cxx, json_update.py: update json file
print_good_run_number: read all run info from one json file, filter with comments
runnumclassify.c: classify runnumbers by momentum or angle
selected_info: as above

#Calorimeter gain analysis calorimeter_gain_analysis
cal_gain:plot for each block
cal_gain_json:calibration gain for each run in json
csvfallcalib: calibration gain for fall runs in txt
csvspringcalib: calibration gain for spring runs in txt
calorimeter_gain_modifyc: plot gain constant for each block
csvelas.C, csvelasfall.C: compare elastic data runs with simc result
SHMS_CAL_Read_Calparam.h: read txt calgain into json
shms_cal_param_to_json.cxx, read txt calgain into json for shms 
calorimeter_gain_analysis_statistic.cxx:count how mant blocks fires in calorimeter
creat_cal/rdataset: failed try

#elastic runs 
modifyelas.C: compare elastic data runs with some recalculation with simc data

#reference time 
csvreftimecut.c: make plots that are related to reference time cut

#dbase
csv_run_list_extra.json: CSV spring runs comments
kin_group_xq2.json: separated by xbj and Q2 for all runs
run_info_group.json: runs with detailed info eg xbj,Q2,run_group,density
run_list_update.json: spring runs run list
run_list.json: fall runs runlist
ratio_run_group_updated.json: runs group by hms_p and shms_p
#info
dbase files in txt format, generated by cxx in run_info

#run_info
get_run_number.cxx: Enter group number and find out what runs are in that group
print_run_info.cxx: From ratio_run_group.json to generate other style of json files
kin_group.cxx: aborted other style of run groups 
runnumclassify1.cxx: The very first scripts to read run infos from runlist and group them
runnumclassify1_update.cxx: some function between runnumclassify1.cxx and rearange_ratio_group.cxx
make_ratio_kin_group.cxx: upgraded runnumclassify
rearange_ratio_group.cxx: update the order of run groups that is generated by runnumclassify1.cxx
update_ratio_group.cxx: manual input the x,q2,z infor for each rungroup
print_good_run_number.cxx: Very first script that read run_list, and comments and manually decide which run is good run. 

#EPICS
grep_current_to_json.cxx: 1.string to get useful string from raw data file to txt. 2. this scripts can get the values that we are interested in from txt to json
plot_current_ratio.cxx: plot the epics values by pi-runs/pi+runs
plot_target_density.cxx: plot target status by run group
plot_target_density_runs.cxx: plot target status by run numbers
Target_fluid_property: H2 and D2 property from NIST

#kinematics
`plot_kinematics_events.cxx`,plot_kinematics_events_spring.cxx, plot_kinematics_x_Q2.cxx: plot kinematics for each run
`plot_kinematic_kin_group.cxx`: plot kinematics for run group and put them on one graph
`kin_acceptance.cxx`: rotate to shms angle plot pion momentum cross section

