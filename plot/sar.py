import numpy as np
import re

def get_sar(name):
	util = []
	count = 0
	with open(name,"r") as f:
		for line in f:
			if len(line) == 1:
				continue
			line = line.split(" ")
			if line[4] == 'nvme0n1':
				count += 1
				util.append(float(line[-1].strip()))

	util = np.array(util)/100
	return util 





