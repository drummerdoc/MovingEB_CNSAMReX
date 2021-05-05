import numpy as np
from math import *
#from pycse import bvp
import matplotlib.pyplot as plt

#The velocity of the piston is up=Ut/t0

gam=1.4
U=-100.0
t0=2e-4
c0=340.15
xmin=-0.5
xmax=0.5
n=1000
delx=(xmax-xmin)/(n-1)
x=np.zeros(n)
u=np.zeros(n)

delt=1e-6

for filenum in np.arange(0,100,1):
	t=filenum*10*delt
	for i in np.arange(0,n,1):
		x[i]=xmin+i*delx
		up=U*t/t0
		if(x[i] > 0.5*U/t0*t**2 and 1/gam**2*(c0-(gam+1)/2*up)**2-2/gam*U/t0*(x[i]-c0*t) >= 0 and x[i]<c0*t):
			u[i]=-1.0/gam*(c0-(gam+1)/2*up)+(1/gam**2*(c0-(gam+1)/2*up)**2-2/gam*U/t0*(x[i]-c0*t))**0.5
			#if(u[i]<0):
			#u[i]=0.0

	expfan_num_filename="temp%04d.curve"%filenum
	expfan_num=np.loadtxt(expfan_num_filename);

        imgfilename="./Images/ExpansionFan%04d.png"%filenum

	#plt.xlim([0,0.5]);
	#plt.ylim([101324,101325.8]);
	plt.plot(x,u,linewidth=2)
	plt.plot(expfan_num[:,0]-0.5,expfan_num[:,1],'k',linewidth=2)
	plt.xlabel('$x (m)$')
	plt.ylabel('$u (m/s)$')
	plt.gca().legend(('Exact','CNS'))
	plt.savefig(imgfilename)
	plt.close()
	#plt.show()
