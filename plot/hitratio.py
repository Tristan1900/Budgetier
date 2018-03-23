import matplotlib.pyplot as plt
import numpy as np
import re

def get_stat(name):
	received_raw = []
	hit_raw = []
	received = []
	hit = []
	count = 0
	with open(name,'r') as f:
		for line in f:
			line = line.split(" ")
			if len(line) >= 5 and line[4] == "\"value\":":
				count += 1
				num = re.findall('\d+', line[5])
				tmp = ""
				for i in num:
					tmp += i

				if count % 2 == 1:
					received_raw.append(int(tmp))
				else:
					hit_raw.append(int(tmp))
	tmp = 0
	end = received_raw[-1]
	for i, val in enumerate(received_raw):
		if val == 0:
			continue
		if val == end:
			break
		received.append(val - tmp)
		tmp = val

	tmp = 0
	end = hit_raw[-1]
	for i, val in enumerate(hit_raw):
		if val == 0:
			continue
		if val == end:
			break
		hit.append(val - tmp)
		tmp = val

	received = np.array(received)
	hit = np.array(hit)
	hitratio = hit/received
	return hitratio


