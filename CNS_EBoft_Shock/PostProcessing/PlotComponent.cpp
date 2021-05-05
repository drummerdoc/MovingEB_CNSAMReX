/*
  A very simple example of reading a plotfile and calling a function to perform a pointwise transformation
  based on a set of components that are specified by name on the command line.  The transformation is done
  in the accompanying fortran routine.  No grow cells are used, so the transformation cannot involve a 
  stencil operation.

  The output is a new plotfile with a single component set to the output of the transform routine.  This 
  new plotfile has metadata (number of levels, boxarray, grid spacing, etc) that is identical to the original
  plotfile.
 */
#include <string>
#include <iostream>

#include "AMReX_ParmParse.H"
#include <AMReX_ParallelDescriptor.H>
#include <AMReX_DataServices.H>
#include <WritePlotFile.H>
#include <AMReX_PlotFileUtil.H>

#include <AMReX_BLFort.H>

extern "C" {
  void transform (const int* lo, const int* hi,
                  const amrex_real* sIn, const int* sInlo, const int* sInhi, const int* ncIn,
                  amrex_real* sOut, const int* sOutlo, const int* sOuthi, const int* ncOut, double dx[3], double problo[3], double s[64], const int* fileno);
}

using namespace amrex;

std::string
getFileRoot(const std::string& infile)
{
  vector<std::string> tokens = Tokenize(infile,std::string("/"));
  return tokens[tokens.size()-1];
}

int
main (int   argc,
      char* argv[])
{
  amrex::Initialize(argc,argv);
  {
    std::string infile;
    std::string outfile;
    int counter = 0;
    int isize = 64;
    int jsize = 64;
    int ksize = 64;

    double dx[3];
    double problo[3];
    double xvelmean[ksize], xvelmean1[ksize], xvelmean2[ksize], time_1, time_2, time_start, time_end;
    using std::ofstream;
    ofstream outdata;

    for(int k=0;k<ksize;k++){
       xvelmean[k] = 0.0;
    }

    for (int filenum=0; filenum < 710; filenum=filenum+10){
    	if(filenum<10){
	    counter = counter+1;
	    infile = "plt0000" + std::to_string(filenum);
    	    outfile = "pressure0000" + std::to_string(filenum) + ".txt";
	}
	else if(filenum<100){
	    counter = counter+1;
	    infile = "plt000" + std::to_string(filenum);
    	    outfile = "pressure000" + std::to_string(filenum) + ".txt";
	}
	else if(filenum<1000){
	    counter = counter+1;
	    infile = "plt00" + std::to_string(filenum);
    	    outfile = "pressure00" + std::to_string(filenum) + ".txt";
	}
	else if(filenum<10000){
	    counter = counter+1;
	    infile = "plt0" + std::to_string(filenum);
    	    outfile = "pressure0" + std::to_string(filenum) + ".txt";
	}
	else if(filenum<100000){
	    counter = counter+1;
	    infile = "plt" + std::to_string(filenum);
    	    outfile = "pressure" + std::to_string(filenum) + ".txt";
	}

    //outdata.open(outfile); // opens the file
    amrex::Print() << infile << "\n";
    DataServices::SetBatchMode();
    Amrvis::FileType fileType(Amrvis::NEWPLT);
    DataServices dataServices(infile, fileType);
    if( ! dataServices.AmrDataOk()) {
      DataServices::Dispatch(DataServices::ExitRequest, NULL);
    }
    AmrData& amrData = dataServices.AmrDataRef();
    PlotFileData plotfile(infile);

 	int nv = 1;
        Vector<std::string> varNames(nv);
    	varNames[0] = "pressure";

    	const Vector<std::string>& plotVarNames = amrData.PlotVarNames();
    	int nCompIn = varNames.size();
    	Vector<int> destFillComps(nCompIn);
        for (int i=0; i<nCompIn; ++i) {
     	    destFillComps[i] = i;
	    }

        for (int i=0; i<nCompIn; ++i) {
	    int ivar = -1;
      	     for (int j=0; j<plotVarNames.size(); ++j) {
	        if (plotVarNames[j] == varNames[i]) {ivar = j;}
	      }
	      if (ParallelDescriptor::IOProcessor() && ivar<0) {
        	Abort("Cannot find variable="+varNames[i]+" in pltfile");
	      }
        }

    	const int nCompOut = 1;
    	const int nGrow = 0;
    	const int nLev = amrData.FinestLevel() + 1;

    	Array<Real,AMREX_SPACEDIM> prob_lo = plotfile.probLo();
    	Array<Real,AMREX_SPACEDIM> dx0 = plotfile.cellSize(0);
   

    	for (int i=0; i<3; i++){
		dx[i] = dx0[i];
		problo[i] = prob_lo[i];
    	}

      double s[ksize];
for (int i=0; i<ksize; i++){ 
      s[i] = 0.0;
      }

    Vector<MultiFab*> stateOut(nLev);
    for (int lev=0; lev<nLev; ++lev) {
      const BoxArray ba = amrData.boxArray(lev);
      const DistributionMapping dmap(ba);
      MultiFab stateIn(ba,dmap,nCompIn,nGrow);
      stateOut[lev] = new MultiFab(ba,dmap,nCompOut,0);


      // Load input data from pltfile 
      amrData.FillVar(stateIn,lev,varNames,destFillComps);
 
    
      
      // Compute transformation
      for (MFIter mfi(stateIn); mfi.isValid(); ++mfi) {
        const FArrayBox& sIn = stateIn[mfi];
        FArrayBox& sOut = (*stateOut[lev])[mfi];
        const Box& box = mfi.validbox();

        transform(box.loVect(), box.hiVect(),
                  BL_TO_FORTRAN_ANYD(sIn),&nCompIn,
                  BL_TO_FORTRAN_ANYD(sOut),&nCompOut, dx, problo, s, &filenum);
//for (int k=0; k<ksize; k++){
//	outdata << problo[2] + (float(k)+0.5)*dx[2] << " " << s[k] << "\n";
//	amrex::Print() << problo[2] + (float(k)+0.5)*dx[2] << " " << s[k] << "\n";
 //   	}
      }	
      }
      
	
	
//     outdata.close();
     }
   } 
  amrex::Finalize();
  return 0;
}
