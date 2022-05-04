import argparse

parser = argparse.ArgumentParser(description='Generate StudyFlag Commands')
parser.add_argument('--file', '-f', default=None, help=
'Txt file that is the terminal output of running the CIS Update macro. \n \
Should contain some lines near the beginning that looks like: \n \
run  [399940, \'CIS\', \'2021-08-23 10:13:25,2021-08-23 10:15:03\'] \n \
which is how we will extract the run numbers. Required! \n ')
parser.add_argument('-c', action='store_true', help="Add commas in between runs")

args = parser.parse_args()

if args.file is None:
	print('Need to specify your input file!')
	exit()

runs=[]
inBlock = False
with open(args.file) as f:
	for l in f.readlines():
		lst = l.split()
		if len(lst) > 1:
			if lst[0] == "Running" and lst[1] == "ReadCIS":
				# run list ends at: "Running ReadCIS - Read in CIS calibration factors and quality flags from CIS calibration ntuples"
				break
		if inBlock:
			try:
				run = lst[1]
				run = run.strip('[')
				run = run.strip(',')
				runs.append(run)
			except:
				print("NOPE")
				pass
		if len(lst) > 1:
			if lst[0] == "Use:":
				# run list starts at: "Use: Using all the detector"
				inBlock = True

l = len(runs)

for n,r in enumerate(runs):
	if args.c is True:
		if n < (l-1):
			print(r, end=', ')
		else:
			print(r, end='')
	else:
		print(r, end=' ')
print()
