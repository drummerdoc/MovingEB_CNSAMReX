# MovingEB_CNSAMReX
Implementation of moving embedded boundary algorithm for compressible flow in the CNS code in AMReX.
Currently only prescribed motion can be done. i.e. the simulations are not two-way coupled - the 
EB does not move in accordance with the force exerted by the fluid on it. 

## How to run a case 

To run a case, the prescribed motion of the EB and its velocity are to be specified. Follow the steps below.

1. The prescribed motion of the EB is specified as a case in one of the `if` loops in `CNS_init_eb2.cpp`. For eg.,
   for a vertically oscillating cylinder with `y(t)=A cos(2 pi f t)`, create an `if` loop for a `geom_type` of `moving_cylinder`, and this 
   will be specified in the `inputs` file.
```
	if(geom_type == "moving_cylinder"){
	EB2::CylinderIF cf1(0.2, 10.0, 0, {0.0,0.2*0.4*cos(2.0*3.14159265359*24.375*1.0*time),7.0}, false);
	auto polys = EB2::makeUnion(cf1);
	auto gshop = EB2::makeShop(polys);
	EB2::Build(gshop, geom, max_coarsening_level, max_coarsening_level, 4,false);
	}
```
2. Specify the velocity of the EB in `hydro/cns_eb_hyp_wall.f90`. This routine calculates the invsicid fluxes.
   For the above vertically oscillating cylinder the vertical velocity is `v = -2 pi f A sin(2 pi f t)`.
```
	ublade = 0.0d0
	vblade = -0.2d0*0.4d0*2.0d0*3.14159265359d0*24.375d0*1.0d0*sin(2.0d0*3.14159265359d0*24.375d0*1.0d0*time)
	wblade = 0.0d0
``` 
3. Specify the velocity of the EB (same as in 2) in `diffusion/cns_eb_diff_wall.F90` as well. This routine calcuates the viscous 
   fluxes.
```
	ublade = 0.0d0
	vblade = -0.2d0*0.4d0*2.0d0*3.14159265359d0*24.375d0*1.0d0*sin(2.0d0*3.14159265359d0*24.375d0*1.0d0*time)
	wblade = 0.0d0
```
4. The initial condition is specified in `Exec/cns_prob.F90`.
5. Modify `inputs` located in `Exec/<casename>` for domain size, number of cells, refinement, viscosity, and also specify the `geom_type`
   `eb2.geom_type = moving_cylinder` for the above example.

## Notes

1. When there are parts of the EB which move and parts which are stationary, `if` loops are used to specify which part of the EB 
   moves. For eg., the case of a moving piston in a stationary cylinder `CNS_EBoft_ICE`, the velocity in `hydro/cns_eb_hyp_wall.f90` is 
```
	if(x.le.0.075d0 .and. abs(anrmx).gt.1e-6)then
	ublade=-0.02d0*3000.0d0/8.0d0*sin(3000.0d0/8.0d0*time)
	vblade = 0.0d0
	wblade = 0.0d0
	else
	ublade = 0.0d0
	vblade = 0.0d0
	wblade = 0.0d0
	endif
```
