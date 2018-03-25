import matplotlib.pyplot as plt
from matplotlib import style
import matplotlib.animation as animation
import numpy as np
import sys
from sar import get_sar
from smart import get_smart
from hitratio import get_stat
from admission import get_admission

class Draw:
	def __init__(self):
		style.use('ggplot')
		self.fig = plt.figure(figsize=(10, 8))
		self.ax1 = self.fig.add_subplot(2,2,1)
		self.ax2 = self.fig.add_subplot(2,2,2)
		self.ax3 = self.fig.add_subplot(2,2,3)
		self.ax4 = self.fig.add_subplot(2,2,4)
		self.smooth = 1

	def compress_array(self, array):
		array = np.nanmean(np.pad(array.astype(float), 
			(0, self.smooth - array.size%self.smooth), mode='constant', 
			constant_values=np.NaN).reshape(-1, self.smooth), axis=1)
		return array

	def animate(self, i):
		"""
		sample every 5 seconds, so a compress factor of 12 means 1 miniute
		a compress factor of 240 means every 20 miniutes.
		"""
		sar_name = "../sar.dat"
		stat_name = "../stat.dat"
		smart_name = "../smart.dat"
		admission_name = "../admission.dat"
		# 
		sar_data = get_sar(sar_name)
		# print(len(sar_data))
		sar_data = self.compress_array(sar_data)
		# 
		stat_data = get_stat(stat_name)
		# print(len(stat_data))
		stat_data = self.compress_array(stat_data)
		# 
		smart_data, total = get_smart(smart_name)
		# print(len(smart_data))
		# smart_data = compress_array(smart_data,12 )
		# smart_data = smart_data[:20]
		# row = len(smart_data) % 20
		# smart_data = np.reshape(smart_data[:480*2],(48,-1))
		#
		admission_data = get_admission(admission_name)
		admission_data = self.compress_array(admission_data)

		self.ax1.clear()
		self.ax2.clear()
		self.ax3.clear()
		self.ax4.clear()

		self.ax1.set_title("Disk Utilization")
		self.ax1.set_xlabel("time/min")
		self.ax1.set_ylabel("utilization/percent")

		self.ax2.set_title("hitratio")
		self.ax2.set_xlabel("time/min")
		self.ax2.set_ylabel("hitratio/percent")

		self.ax3.set_title("SMART")
		self.ax3.set_xlabel("time/20min")
		self.ax3.set_ylabel("writeToDisk(MB)/min")

		self.ax4.set_title("Admission")
		self.ax4.set_xlabel("time/min")
		self.ax4.set_ylabel("size(MB)/s")

		self.ax1.plot(sar_data)
		self.ax2.plot(stat_data)
		self.ax3.boxplot(smart_data)
		# ax3.plot(smart_data[:20])
		self.ax4.plot(admission_data)
	def plot(self):
		ani = animation.FuncAnimation(self.fig, self.animate, interval = 10000)
		plt.show()



def main(args):
	draw = Draw()
	if len(args) > 1:
		draw.smooth = int(args[1])
	draw.plot()

if __name__ == '__main__':
	main(sys.argv)

