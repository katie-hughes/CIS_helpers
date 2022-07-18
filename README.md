# CIS_helpers

A collection of scripts to assist with running the CIS update! 

My guide: https://docs.google.com/document/d/1j4nPQhlyucGZnkZhU7JdNx6T5cwepf6vH0LONC1STCg/edit?usp=sharing

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
* **--file, -f**: Txt file that is the terminal output of running the CIS Update macro. Should contain some lines near the beginning that looks like below which is how it will extract the run numbers. Required!
```
run [399940,'CIS', '2021-08-23 10:13:25,2021-08-23 10:15:03']
```
* -c: Add commas in between the run numbers. Useful to then copy/paste into my_plot_cis.C or cis_charge.C

Example of running: 
```
> python3 ~/CIS_helpers/ReturnRuns.py -f JunJulCIS.txt -c
426084, 426286, 426330, 426571, 426701, 426816, 426968, 427100, 427157, 427250, 427419, 427932, 427941
```

### highdev.py 
Simple script to read the CIS_DB_Update.txt and return the channels which have a greater than 5% deviation change from DB value. If you run this in the same directory that contains CIS_DB_Update.txt, you do not need to include any additional arguments. If not, you can include --file or -f and specify the path to the file. 

Example of running: 
```
> python3 ~/CIS_helpers/highdev.py 
EBC_m22_c16_lowgain 	 1.24 	 1.02 	 -17.56% 
EBC_m30_c11_highgain 	 74.23 	 78.71 	 6.04% 
EBC_m30_c11_lowgain 	 1.19 	 1.26 	 5.98% 
LBA_m62_c26_highgain 	 100.62 	 69.13 	 -31.30% 
LBC_m52_c18_highgain 	 94.86 	 105.24 	 10.95% 
5 channels greater than 5.0% change
```

### StudyFlagCommand.py
Script to assist with "manual" channel recalibration (aims to make as little of it manual as possible!). I usually run only specifying --recal_file, --cistxt, and --cisupdate. 

Arguments: 
* --recal_file: Txt file of channels to be recalibrated. Required! Format: 
```
[PartitionModule channel gain]
LBA02 06 0 7/22
```
* --end_date: final date for recalibration. Default is today's date. Irrelevant if you specify cistxt, which will work with run numbers instead of dates. 
* --output: output folder for plots, if StudyFlag macro is used. Default=ExampleFolder (~/Tucs/plots/latest/cis/ExampleFolder)
* --cistxt: cis.txt file (output of running the CIS_DB_Update macro), the same as in ReturnRuns. This will extract the list of runs with the dates and make the plots slightly more accurate. If separate_partitions is used, it is required. 
* --cisupdate: generate the recalibrate commands using the CIS_DB_Update macro instead of the StudyFlag macro (preferred as constants will be more accurate, and constants will be saved to a txt file instead of reading them off the plot!). The one caveat to this is that if channels are determined to be "not in the update" with this new date range, then they will not be present in the sqlite and thus won't be added in the final txt file of new values. Not sure how to get around this but it seems like a not super common case. If the channels that fit this scenario were not in the udpate originally, it doesn't matter as the "recalibrated" value is essentailly the same as the DB value (they don't need to be recalibrated!). But if the channel IS in the original update but is NOT in the recalibrated update, the best you can do at the moment is to just read the value off the recalibrated plot (which was the default before this option was added). 
* --result: if --cisupdate is used, use this if you want to specify the name of the txt file containing new constants. The default is toRecalibrate.txt which will be located in the results file. 
* --separate_partitions: use if there are different valid runs for different partitions
* --valid_file: List of valid runs for each partition+gain. Only needed if separate_partitions is used. 
* --bashfile: Specify the name of the bash file to be created. Default: RecalCommands.sh
Format: 
```
LBA_lowgain  396441 397150 397165 397192 397291 ....
```

Example of Running: 
```
> python3 ~/CIS_helpers/StudyFlagCommand.py --recal_file toRecalibrate.txt --cistxt JunJulCIS.txt --cisupdate
Run: 426084 	 Date: 2022-06-23
Run: 426286 	 Date: 2022-06-24
Run: 426330 	 Date: 2022-06-24
Run: 426571 	 Date: 2022-06-28
Run: 426701 	 Date: 2022-06-29
Run: 426816 	 Date: 2022-06-30
Run: 426968 	 Date: 2022-07-01
Run: 427100 	 Date: 2022-07-03
Run: 427157 	 Date: 2022-07-03
Run: 427250 	 Date: 2022-07-04
Run: 427419 	 Date: 2022-07-06
Run: 427932 	 Date: 2022-07-12
Run: 427941 	 Date: 2022-07-12
Channel ID: EBC_m61_c08_lowgain 	 Recal Date: 2022-06-28
Channel ID: LBA_m51_c12_highgain 	 Recal Date: 2022-06-30
Channel ID: LBC_m13_c15_lowgain 	 Recal Date: 2022-07-05
Channel ID: LBC_m13_c15_highgain 	 Recal Date: 2022-07-05

Starting from RUN # 426701 


macros/cis/CIS_DB_Update.py --date '28 days' --region 'EBC_m61_c08_lowgain' --ldate 426701 426816 426968 427100 427157 427250 427419 427932 427941 


Starting from RUN # 426968 


macros/cis/CIS_DB_Update.py --date '28 days' --region 'LBA_m51_c12_highgain' --ldate 426968 427100 427157 427250 427419 427932 427941 


Starting from RUN # 427419 


macros/cis/CIS_DB_Update.py --date '28 days' --region 'LBC_m13_c15_lowgain' 'LBC_m13_c15_highgain' --ldate 427419 427932 427941 

> cd ~/Tucs
> ./results/JunJul2022/RecalCommands.sh
............
```
#### How does the bash file work? 
Let's look at one "block" of the bash file created by the previous example. 
```
1. macros/cis/CIS_DB_Update.py --date '28 days' --region 'EBC_m61_c08_lowgain' --ldate 426701 426816 426968 427100 427157 427250 427419 427932 427941 
2. cd ~/Tucs/results
3. ReadCalibFromCool.py --schema="sqlite://;schema=tileSqlite.db;dbname=CONDBR2" --folder=/TILE/OFL02/CALIB/CIS/LIN --tag=UPD1 | grep -v miss > Recal.txt
4. grep 'EBC61  8 0' Recal.txt >> toRecalibrate.txt
5. rm tileSqlite.db CIS_DB_update.txt Recal.txt
6. cd ~/Tucs
```
1. We run the first CIS_DB_Update macro command produced by StudyFlagCommand. This is complete with the appropriate region(s) and run range. 
2. Once the macro is finished running, we navigate to results where the sqlite file produced by the macro is located. 
3. We read the sqlite into a txt file called Recal.txt. This txt file looks something like this: 
```
...
EBC52 40 0    1.295349
EBC52 40 1    84.076927
EBC52 41 0    1.301266
EBC52 41 1    81.951797
...
```
4. We search for the channel(s) defined previously in the region area within Recal.txt (what is in the sqlite). We then read the entire line of this txt into the file toRecalibrate.txt (or whatever is specified by the --result flag). If there are multiple channels specified by region, we grep multiple times. This adds the channels and their new constants to this result file. 
5. We remove all of the files created by the CIS Update macro as we don't want things to get overwritten in weird ways. As we have saved the new constants, there is no other use for them. The plots that were created by the macro are still there in the plots/latest/cis/CIS_Update folder. 
6. We return to the Tucs directory and repeat for all of the other commands over different run ranges. 
### corr.py 
This will generate two files to be used in updating the sqlite values, one of channels in the update and one of channels NOT in the update. You can tell which channels are in the update by looking at CIS_DB_Update.txt.

* --file, -f: File containing the channels to be recalibrated. Default is toRecalibrate.txt. It needs to follow this format: 
```
[PartitionModule Channel Gain Value]
LBA02    06    1    75.97
```
* --update: Ouptut file of channels in the update, default=corr1.txt
* --not_update: Output file of channels NOT in the update, default=corr2.txt
* --cis: default=CIS_DB_Update.txt file. This contains all of the channels that are in the update and is used to separate the channels to recalibrate. 

After this runs, verify that corr1.txt and corr2.txt look ok and follow the right format of PartitionModule Channel Gain Value. 

### update_flags.py 
This will be used as an execfile in creating a new sqlite that describes any flag changes. As it cannot take any arguments, you must prefill two txt files that the script will read. Create and fill 2 files, RemoveBadCIS.txt and AddBadCIS.txt and fill appropriately. Example:
```
EBA_m19_c41_highgain
EBC_m20_c10_lowgain
```

### test_update_flags.py 
As update_flags.py cannot be directly run, you can instead run test_update_flags.py once you have your two txt files filled in to make sure there are no typos or errors or anything. For example, if you misspell highgain or lowgain, you will see the gain printed as -1 instead of 0 or 1. 

