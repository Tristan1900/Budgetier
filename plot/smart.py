import numpy as np
import re

def get_smart(name):
	disk_written = []
	count = 0
	with open(name,"r") as f:
		for line in f:
			num = re.findall('\d+', line)
			tmp = ""
			for i in num:
				tmp += i
			disk_written.append(tmp)

	final = disk_written[-1]
	for x in disk_written:
		if x == final:
			break
		else:
			count += 1

	disk_written = disk_written[:count]

	pre  = float(disk_written[0])
	end = float(disk_written[-1])
	total = int(float(end - pre)*512/1000/1000)
	# MB level
	for i, x in enumerate(disk_written):
		if i == 0:
			pre = float(x)
			disk_written[0] = 0
			continue
		else:
			tmp = float(x)
			disk_written[i] = float(x) - pre
			pre = tmp

	disk_written = [float(x) * 512/1000/1000 for x in disk_written]
	#sample every 5 seconds
	disk_written = np.array(disk_written)/5
	return disk_written, total





