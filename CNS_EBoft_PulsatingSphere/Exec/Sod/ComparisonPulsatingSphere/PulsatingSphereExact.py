import numpy as np
from math import *
#from pycse import bvp
import matplotlib.pyplot as plt

dt=5e-6;
gamma=1.4;
rho0=1.226;
a0=0.01;
p0=101325.0;
c0=(gamma*p0/rho0)**0.5;
#a1(t)=amp*cos(2*pi*freq*t)
#a1dotdot(t)=-(2*pi*freq)**2*amp*cos(2*pi*freq*t)
amp=1e-6;
freq=1000.0;
rmin=a0;
n=1000;
r=np.zeros(n);
pprime=np.zeros(n);


for filenum in np.arange(370,570,100):
	print filenum
	time=filenum*1*dt;
	rmax=c0*time;
	dr=(rmax-rmin)/(n-1)

	for i in np.arange(0,n,1):
		r[i]=a0+i*dr;
		
		if(rmax >= rmin and r[i]-a0<c0*time):
			adotdot=-(2*pi*freq)**2*amp*cos(2*pi*freq*(time-r[i]/c0));
			pprime[i]=rho0*a0**2*adotdot/r[i];

	ps_num_filename="pressure%05d.txt"%filenum
	ps_num=np.loadtxt(ps_num_filename);
	filesize=np.size(ps_num[:,0])	

	counter=0
	for i in np.arange(0,filesize,1):
		if(ps_num[i,0] > rmax):
			break
		counter=counter+1		

	imgfilename="./Images/PulsatingSphere%04d.png"%(filenum/10)

	import matplotlib.pyplot as plt
	plt.xlim([0,0.5]);
	#plt.ylim([101324,101325.8]);
	plt.plot(r-a0,pprime,linewidth=2)
	plt.plot(ps_num[:,0]-a0,ps_num[:,1]-101325.0,linestyle='none',marker='o',color='k')
	plt.xlabel('$r-R_s (m)$',fontsize=20)
	plt.ylabel('$p\' (N/m^2)$',fontsize=20)
	plt.legend(('Exact','Present'),loc="best")
	plt.savefig(imgfilename)
	plt.close()
	#plt.show()
