import numpy as np

def get_admission(name):
	"""
	Admission in unit of bytes, and sample every 5 seconds.
	So element/5/1024/1024 = MB/s
	"""
	res = []
	with open(name,"r") as f:
		for line in f:
			res.append(int(line))
	res = np.array(res)/(5*1024*1024)
	return res 





