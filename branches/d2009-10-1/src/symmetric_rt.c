/****************************************************************************
 *
 *  symmetric_rt.c
 *
 *  Run time initialisation for the symmetric phi^4 free energy.
 *
 *  $Id: symmetric_rt.c,v 1.1.2.2 2009-11-04 18:35:08 kevin Exp $
 *
 *  Edinburgh Soft Matter and Statistical Physics Group
 *  and Edinburgh Parallel Computing Centre
 *
 *  Kevin Stratford (kevin@epcc.ed.ac.uk)
 *  (c) The University of Edinburgh (2009)
 *
 ****************************************************************************/

#include <assert.h>

#include "pe.h"
#include "phi.h"
#include "coords.h"
#include "runtime.h"
#include "free_energy.h"
#include "symmetric.h"

/****************************************************************************
 *
 *  symmetric_run_time
 *
 ****************************************************************************/

void symmetric_run_time(void) {

  int n;
  double a;
  double b;
  double kappa;

  /* Single order parameter, del^2 phi required. */

  /* There's a slight complication in that halo width one is enough
   * at the moment when using full LB. */

  assert(phi_nop() == 1);
  phi_gradient_level_set(2);
  coords_nhalo_set(2);

  info("Symmetric phi^4 free energy selected.\n");
  info("Single conserved order parameter nop = 1\n");
  info("Requires up to del^2 derivatives so setting nhalo = %1d\n", nhalo_);
  info("\n");

  /* Parameters */

  n = RUN_get_double_parameter("A", &a);
  n = RUN_get_double_parameter("B", &b);
  n = RUN_get_double_parameter("K", &kappa);

  info("Parameters:\n");
  info("Bulk parameter A      = %12.5e\n", a);
  info("Bulk parameter B      = %12.5e\n", b);
  info("Surface penalty kappa = %12.5e\n", kappa);

  symmetric_free_energy_parameters_set(a, b, kappa);

  info("Surface tension       = %12.5e\n", symmetric_interfacial_tension());
  info("Interfacial width     = %12.5e\n", symmetric_interfacial_width());

  /* For the symmetric... */

  assert(kappa > 0.0);

  /* Set free energy function pointers. */

  fe_density_set(symmetric_free_energy_density);
  fe_chemical_potential_set(symmetric_chemical_potential);
  fe_isotropic_pressure_set(symmetric_isotropic_pressure);
  fe_chemical_stress_set(symmetric_chemical_stress);

  return;
}
