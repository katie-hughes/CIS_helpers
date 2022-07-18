import argparse

#Katie Hughes, October 2021

parser = argparse.ArgumentParser('Generate files to be used in updating sqlite values, based on whether they are in the update.')

parser.add_argument('--file', '-f', default='toRecalibrate.txt', help=
'File containing channels to be recalibrated. \n \
Default = toRecalibrate.txt. \n \
Needs to follow this format: \n \
PartitionModule Channel Gain Value \n \
Ex: LBA02    06    1    75.97 \n \ ')

parser.add_argument('--update', default='corr1.txt', help='File to be created of channels in the update. Default=corr1.txt')

parser.add_argument('--not_update', default='corr2.txt', help='File to be created of channels NOT in the update. Default=corr2.txt')

parser.add_argument('--cis', default='CIS_DB_update.txt', help=
'File listing which channels are in the update')

args = parser.parse_args()


# creating a list of the updated channels
updated_channels = []
with open(args.cis, 'r') as f:
	for l in f.readlines():
		if ("TILECAL" in l):
			x = l.split()
			ID = (x[0])[8:]
			updated_channels.append(ID)

print()


def fix_length(str):
	if len(str) == 1:
		return '0'+str
	else:
		return str

corr1 = open(args.update, 'w')
corr2 = open(args.not_update, 'w')
in_update = 0
not_in_update = 0
with open(args.file, 'r') as f:
	lines = f.readlines()
	# check if line matches the ID from the update list
	for l in lines:
		x = l.split()
		partition = fix_length(x[0][:3])
		module = fix_length(x[0][3:])
		channel = fix_length(x[1])
		ch_ID = partition+'_m'+module+'_c'+channel
		if x[2] == '1':
			ch_ID += "_highgain"
		else:
			ch_ID += "_lowgain"
		print('%s:' % ch_ID, end=' ')
		if ch_ID in updated_channels:
			print('in the update')
			in_update += 1
			corr1.write(l)
		else:
			print('NOT in the update')
			not_in_update += 1
			corr2.write(l)

print()

print(in_update, "recalibrated channels are in the update")
print(not_in_update, "recalibrated channels are NOT in the update")
print("\nTxt files", args.update, "and", args.not_update, "successfully written to!\n")
