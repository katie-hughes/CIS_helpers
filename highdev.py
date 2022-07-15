import numpy as np
import argparse

parser = argparse.ArgumentParser(description='Return High Deviation Channels')
parser.add_argument('--file', '-f', default='CIS_DB_update.txt', help=
'CIS Update .txt file ')

args = parser.parse_args()

if args.file is None:
	print('Need to specify your input file!')
	exit()



count = 0

with open(args.file) as f:
	for l in f.readlines():
		if 'TILECAL' in l:
			spl = l.split()
			module = spl[0]
			old = float(spl[1])
			new = float(spl[2])
			percent = float(spl[3])
			if np.abs(percent) > 0.05:
				print('%s \t %.2f \t %.2f \t %.2f%% '%((module.strip('TILECAL')).strip('_'), old, new, percent*100))
				count += 1

print(count)
