# CIS_helpers

A collection of scripts to assist with running the CIS update! 

## C++ files to be run in Root
Run these by:
```
> root
> .L [filename] 
> function()
```

### cis_charge.C
Script to create/mass produce plots in root from CIS scan files. 
* labels: defines plot labels
* cis_charge_new: function to actually create and save the plots
* cis_plot_frame: defines plotting parameters 
* cis_timing(int run, int module=2, int chan=2, const char * xval = 0): produces timing plots for a particular run for the default module 2, channel 2 for all 4 partitions. 
* cis_ampq(int run, int module=2, int chan=2, const char * xval = 0): produces amp/q plots for a particular run for the default module 2, channel 2 for all 4 partitions. 
* cis_pulses(int run, int module=2, int chan=10, const char * xval = 0): plots a random parameter (sample_lo) vs EvtNr for a particular run for all 4 partitions. If there are pulses missing you will see points at 0 at an unusual place. 
* check_run(int run): check both cis_timing and cis_ampq for a particular run. 
* **check_all_runs()**: This will produce timing and amplitude/charge plots for all runs listed in the function body. Currently you have to update the list of runs directly within the function, I don't have an automated way of doing this unfortunately. Most relevant function in this file!

### my_plot_cis.C
Script to examine individual CIS constant fits (ADC Count vs pC injected) 
* **plot(int run, int partition, int module, int channel, int gain)**: Plot one CIS constant fit for a particular run and particular channel. Partition Key: 1=LBA, 2=LBC, 3=EBA, 4=EBC. Module/channel self expalanatory. Gain: 0=lowgain, 1=highgain. (Example: to plot LBC_m23_c40_highgain, run plot(run, 2, 23, 40, 1)). 
* **plot_all_runs(int partition, int module, int channel, int gain)**: Mass produce plots for a single channel according to the list of runs in the function body (again you must directly edit the file to update this list). The method for converting the channel IDs is the same as described in plot().

### plot_cis.C
Script written by Sasha which I adapted into my_plot_cis

## Python scripts

### ReturnRuns.py
Very simple script that will read the terminal output of the CIS_DB_Update.py macro and return+print the list of runs, either separated by spaces or commas. 
* **--file**: Txt file that is the terminal output of running the CIS Update macro. Should contain some lines near the beginning that looks like below which is how we will extract the run numbers. Required!
```
run [399940,'CIS', '2021-08-23 10:13:25,2021-08-23 10:15:03']
```
* -c: Add commas in between the run numbers. Useful to then copy/paste into my_plot_cis.C or cis_charge.C

Example of running: 
```
> python3 ~/CIS_helpers/ReturnRuns.py -f JunJulCIS.txt -c
> 426084, 426286, 426330, 426571, 426701, 426816, 426968, 427100, 427157, 427250, 427419, 427932, 427941
```

### highdev.py 

### StudyFlagCommand.py

### corr.py 

### update_flags.py 

### test_update_flags.py 

