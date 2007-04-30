/*****************************************************************************
 *
 *  model.h
 *
 *  $Id: model.h,v 1.8 2007-04-30 14:23:21 kevin Exp $
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *
 *****************************************************************************/

#ifndef _MODEL_H
#define _MODEL_H

#include "d3q15.h"
#include "d3q19.h"

/* Always three dimensions at the moment */
enum {ND = 3};
enum {NHYDRO = 10};

typedef struct {
  double f[NVEL], g[NVEL];
} Site;

extern const double cs2;
extern const double rcs2;
extern const double d_[3][3];

void   init_site(void);
void   finish_site(void);
void   halo_site(void);

int    index_site(const int, const int, const int);
double get_f_at_site(const int, const int);
double get_g_at_site(const int, const int);
double get_rho_at_site(const int);
double get_phi_at_site(const int);
void   set_rho(const double, const int);
void   set_phi(const double, const int);
void   set_f_at_site(const int, const int, const double);
void   set_g_at_site(const int, const int, const double);
void   set_rho_u_at_site(const double, const double [], const int);
void   get_momentum_at_site(const int, double[ND]);

#endif
