import matplotlib.pyplot as plt
from matplotlib import style
import matplotlib.animation as animation
import numpy as np
import sys
from sar import get_sar
from smart import get_smart
from hitratio import get_stat

style.use('ggplot')
fig = plt.figure(figsize=(10, 8))
ax1 = fig.add_subplot(2,2,1)
ax2 = fig.add_subplot(2,2,2)
ax3 = fig.add_subplot(2,2,3)

def compress_array(array, smooth):
	array = np.nanmean(np.pad(array.astype(float), (0, smooth - array.size%smooth), mode='constant', constant_values=np.NaN).reshape(-1, smooth), axis=1)
	return array

def animate(i):
	sar_name = "../sar.dat"
	stat_name = "../stat.dat"
	smart_name = "../smart.dat"
	# 
	sar_data = get_sar(sar_name)
	sar_data = compress_array(sar_data,12)
	# 
	stat_data = get_stat(stat_name)
	stat_data = compress_array(stat_data,12)
	# 
	smart_data, total = get_smart(smart_name)
	# smart_data = compress_array(smart_data,12)
	row = len(smart_data) % 120
	smart_data = np.reshape(smart_data[:-row],(120,-1))

	ax1.clear()
	ax2.clear()
	ax3.clear()

	ax1.set_title("Disk Utilization")
	ax1.set_xlabel("time/min")
	ax1.set_ylabel("utilization/percent")

	ax2.set_title("hitratio")
	ax2.set_xlabel("time/min")
	ax2.set_ylabel("hitratio/percent")

	ax3.set_title("SMART")
	ax3.set_xlabel("time/min")
	ax3.set_ylabel("writeToDisk/MB")

	ax1.plot(sar_data)
	ax2.plot(stat_data)
	ax3.boxplot(smart_data, 0, '')


def main(args):
	ani = animation.FuncAnimation(fig, animate, interval = 10000)
	plt.show()

if __name__ == '__main__':
	main(sys.argv)

