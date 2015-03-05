/****************************************************************************
 *
 *  phi_lb_coupler.c
 *
 *  In cases where the order parameter is via "full LB", this couples
 *  the scalar order parameter field to the 'g' distribution.
 *
 *  Edinburgh Soft Matter and Statistical Physics Group and
 *  Edinburgh Parallel Computing Centre
 *
 *  (c) 2010-2014 The University of Edinburgh
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 ****************************************************************************/

#include <assert.h>

#include "pe.h"
#include "coords.h"
#include "model.h"
#include "lb_model_s.h"
#include "field_s.h"
#include "phi_lb_coupler.h"

/*****************************************************************************
 *
 *  phi_lb_to_field
 *
 *****************************************************************************/

//TODO declare these somewhere sensible.
extern __targetConst__ int tc_nSites; //declared in collision.c
extern __targetConst__ int tc_Nall[3]; //declared in gradient routine
extern __targetConst__ int tc_nhalo;
extern __targetConst__ int tc_ndist;


__target__ int phi_lb_to_field_site(double * phi, double * f, const int baseIndex) {

  double phi0=0.;

  int coords[3];
  targetCoords3D(coords,tc_Nall,baseIndex);
  
  // if not a halo site:
  if (coords[0] >= tc_nhalo &&
      coords[1] >= tc_nhalo &&
      coords[2] >= tc_nhalo &&
      coords[0] < tc_Nall[X]-tc_nhalo &&
      coords[1] < tc_Nall[Y]-tc_nhalo &&
      coords[2] < tc_Nall[Z]-tc_nhalo){
    
    
    //lb_0th_moment(lb, baseIndex, LB_PHI, &phi0);
    
    int p;
     for (p = 0; p < NVEL; p++) {
      phi0 += f[LB_ADDR(tc_nSites, tc_ndist, NVEL, baseIndex, LB_PHI, p)];
    }
    
     //field_scalar_set(phi, baseIndex, phi0);
     phi[baseIndex]=phi0;

    
    
  }
  

  return 0;
}


__targetEntry__ void phi_lb_to_field_lattice(double * phi, double * f) {

  int baseIndex=0;

  __targetTLPNoStride__(baseIndex,tc_nSites){	  
    phi_lb_to_field_site(phi, f, baseIndex);
  }


  return;
}



__targetHost__ int phi_lb_to_field(field_t * phi, lb_t  *lb) {

  int ic, jc, kc, index;
  int nlocal[3];
  coords_nlocal(nlocal);
  int nhalo = coords_nhalo();

  double phi0;

  int Nall[3];
  Nall[X]=nlocal[X]+2*nhalo;  Nall[Y]=nlocal[Y]+2*nhalo;  Nall[Z]=nlocal[Z]+2*nhalo;

  int nSites=Nall[X]*Nall[Y]*Nall[Z];
  int nFields=NVEL*lb->ndist;


  assert(phi);
  assert(lb);
  coords_nlocal(nlocal);

  //start constant setup
  copyConstToTarget(&tc_nSites,&nSites, sizeof(int)); 
  copyConstToTarget(&tc_nhalo,&nhalo, sizeof(int)); 
  copyConstToTarget(tc_Nall,Nall, 3*sizeof(int)); 
  copyConstToTarget(&tc_ndist,&lb->ndist, sizeof(int)); 
  //end constant setup

#ifndef TARGETFAST   //temporary optimisation specific to GPU code for benchmarking
  copyToTarget(lb->t_f,lb->f,nSites*nFields*sizeof(double)); 
#endif

  phi_lb_to_field_lattice __targetLaunchNoStride__(nSites) (phi->t_data, lb->t_f);

  copyFromTarget(phi->data,phi->t_data,nSites*sizeof(double)); 

  return 0;
}


/*****************************************************************************
 *
 *  phi_lb_from_field
 *
 *  Move the scalar order parameter into the non-propagating part
 *  of the distribution, and set other elements of distribution to
 *  zero.
 *
 *****************************************************************************/

__targetHost__ int phi_lb_from_field(field_t * phi, lb_t * lb) {

  int p;
  int ic, jc, kc, index;
  int nlocal[3];

  double phi0;

  assert(phi);
  assert(lb);
  coords_nlocal(nlocal);

  for (ic = 1; ic <= nlocal[X]; ic++) {
    for (jc = 1; jc <= nlocal[Y]; jc++) {
      for (kc = 1; kc <= nlocal[Z]; kc++) {

	index = coords_index(ic, jc, kc);

	field_scalar(phi, index, &phi0);

	lb_f_set(lb, index, 0, 1, phi0);
	for (p = 1; p < NVEL; p++) {
	  lb_f_set(lb, index, p, 1, 0.0);
	}

      }
    }
  }

  return 0;
}
