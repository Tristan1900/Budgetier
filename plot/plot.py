import matplotlib.pyplot as plt
from matplotlib import style
import matplotlib.animation as animation
import numpy as np
import sys
import optparse
from sar import get_sar
from smart import get_smart
from hitratio import get_stat
from admission import get_admission

class Draw:
	def __init__(self):
		self.style = style.use('ggplot')
		self.fig = plt.figure(figsize=(10, 8))
		self.ax1 = self.fig.add_subplot(2,2,1)
		self.ax2 = self.fig.add_subplot(2,2,2)
		self.ax3 = self.fig.add_subplot(2,2,3)
		self.ax4 = self.fig.add_subplot(2,2,4)
		self.fontsize = 8
		self.smooth = 1
		self.file=""

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
		sar_name = "../result4/"
		stat_name = "../result4/"
		smart_name = "../result4/"
		admission_name = "../result4/"
		self.ax1.clear()
		self.ax2.clear()
		self.ax3.clear()
		self.ax4.clear()
		self.plot_sar(sar_name)
		self.plot_stat(stat_name)
		self.plot_smart(smart_name)
		self.plot_admission(admission_name)

	def plot_sar(self, filename):
		path = filename + "/sar.dat"
		sar_data = get_sar(path)
		sar_data = self.compress_array(sar_data)
		self.ax1.set_title("Disk Utilization",fontsize=self.fontsize)
		self.ax1.set_xlabel("time",fontsize=self.fontsize)
		self.ax1.set_ylabel("utilization/percent",fontsize=self.fontsize)
		self.ax1.tick_params(axis='both', which='major', labelsize=self.fontsize)
		self.ax1.plot(sar_data)

	def plot_stat(self, filename):
		path = filename + "/stat.dat"
		stat_data = get_stat(path)
		stat_data = self.compress_array(stat_data)
		self.ax2.set_title("hitratio",fontsize=self.fontsize)
		self.ax2.set_xlabel("time",fontsize=self.fontsize)
		self.ax2.set_ylabel("hitratio/percent",fontsize=self.fontsize)
		self.ax2.tick_params(axis='both', which='major', labelsize=self.fontsize)
		self.ax2.plot(stat_data)

	def plot_smart(self, filename):
		path = filename + "/smart.dat"
		smart_data, total = get_smart(path)
		row = len(smart_data) % 80
		smart_data = np.reshape(smart_data[:-row],(80,-1))
		self.ax3.set_title("SMART",fontsize=self.fontsize)
		self.ax3.set_xlabel("time",fontsize=self.fontsize)
		self.ax3.set_ylabel("writeToDisk(MB)/min",fontsize=self.fontsize)
		self.ax3.tick_params(axis='both', which='major', labelsize=self.fontsize)
		self.ax3.boxplot(smart_data)

	def plot_admission(self, filename):
		path = filename + "/admission.dat"
		admission_data = get_admission(path)
		admission_data = self.compress_array(admission_data)
		self.ax4.set_title("Admission",fontsize=self.fontsize)
		self.ax4.set_xlabel("time",fontsize=self.fontsize)
		self.ax4.set_ylabel("size(MB)/s",fontsize=self.fontsize)
		self.ax4.tick_params(axis='both', which='major', labelsize=self.fontsize)
		self.ax4.plot(admission_data)

	def plot_all(self):
		prefix = "/Users/Tristan/Desktop/research_code/"
		for file in self.file:
			self.plot_sar(prefix+file)
			self.plot_smart(prefix+file)
			self.plot_stat(prefix+file)
			self.plot_admission(prefix+file)
		plt.show()

	def plot(self):
		ani = animation.FuncAnimation(self.fig, self.animate, interval = 10000)
		plt.show()



def main(args):
	# parse arguments
	parser = optparse.OptionParser()
	parser.add_option('-m','--minute', action="store", dest="minute", help="granularity", default=1)
	parser.add_option('-b', '--box', action="store", dest="box", help="box granularity", default=1)
	parser.add_option('-f', '--file', action="store", dest="file", help="file name", default="")

	options, args = parser.parse_args()
	minute = int(options.minute)*12
	box = int(options.box)*12
	file = options.file.split(",")
	draw = Draw()
	draw.smooth=minute
	draw.box=box
	draw.file=file
	draw.plot_all()
	# draw.plot()

if __name__ == '__main__':
	main(sys.argv)

