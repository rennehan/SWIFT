/*******************************************************************************
 * This file is part of SWIFT.
 * Copyright (c) 2019 Josh Borrow (joshua.borrow@durham.ac.uk)
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#ifndef SWIFT_GADGET2_HYDRO_PARAMETERS_H
#define SWIFT_GADGET2_HYDRO_PARAMETERS_H

/* Configuration file */
#include <config.h>

/* Global headers */
#if defined(HAVE_HDF5)
#include <hdf5.h>
#endif

/* Local headers */
#include "common_io.h"
#include "error.h"
#include "inline.h"

/**
 * @file Gadget2/hydro_parameters.h
 * @brief Follows the Gadget-2 version of SPH. (default parameters)
 *
 *        This file defines a number of things that are used in
 *        hydro_properties.c as defaults for run-time parameters
 *        as well as a number of compile-time parameters.
 */

/* Viscosity parameters -- FIXED -- MUST BE DEFINED AT COMPILE-TIME */

/* Cosmology default beta=3.0.
 * Alpha can be set in the parameter file.
 * Beta is defined as in e.g. Price (2010) Eqn (103) */
#define const_viscosity_beta 3.0f

/* The viscosity that the particles are reset to after being hit by a
 * feedback event. This should be set to the same value as the
 * hydro_props_default_viscosity_alpha in fixed schemes, and likely
 * to hydro_props_default_viscosity_alpha_max in variable schemes. */
#define hydro_props_default_viscosity_alpha_feedback_reset 0.8f

/* Viscosity paramaters -- Defaults; can be changed at run-time */

/* The "initial" hydro viscosity, or the fixed value for non-variable
 * schemes. This usually takes the value 0.8. */
#define hydro_props_default_viscosity_alpha 0.8f

/* Structs that store the relevant variables */

#ifdef WITH_MHD
struct mhd_global_data {
  float artificial_dissipation_constant;
  float artificial_dissipation_minimum;
  float artificial_dissipation_source;
  float artificial_dissipation_timescale;
  int with_div_B_cleaning;
  float div_B_parabolic_sigma;
  float div_B_over_clean_factor;
}
#endif

/*! Artificial viscosity parameters */
struct viscosity_global_data {
  /*! For the fixed, simple case. Also used to set the initial AV
      coefficient for variable schemes. */
  float alpha;
};

/*! Thermal diffusion parameters */
struct diffusion_global_data {};

/* Functions for reading from parameter file */

/* Forward declartions */
struct swift_params;
struct phys_const;
struct unit_system;

#ifdef WITH_MHD
/**
 * @brief Initialises the MHD parameters in the struct from
 *        the parameter file, or sets them to defaults.
 *
 * @param params: the pointer to the swift_params file
 * @param us: pointer to the internal unit system
 * @param phys_const: pointer to the physical constants system
 * @param mhd: pointer to the mhd_global_data struct to be filled.
 **/
static INLINE void mhd_init(struct swift_params* params,
                            const struct unit_system* us,
                            const struct phys_const* phys_const,
                            struct mhd_global_data* mhd) {

  /* Read the MHD parameters from the file, if they exist,
   * otherwise set them to the defaults defined above. */
  mhd->artificial_dissipation_constant = parser_get_param_float(
    params, "SPH:artificial_dissipation_constant"
  );
  mhd->artificial_dissipation_minimum = parser_get_param_float(
    params, "SPH:artificial_dissipation_minimum"
  );
  mhd->artificial_dissipation_source = parser_get_param_float(
    params, "SPH:artificial_dissipation_source"
  );
  mhd->artificial_dissipation_timescale = parser_get_param_float(
    params, "SPH:artificial_dissipation_timescale"
  );

  mhd->with_div_B_cleaning = parser_get_opt_param_int(
    params, "SPH:with_div_B_cleaning"
  );
  mhd->div_B_parabolic_sigma = parser_get_param_float(
    params, "SPH:div_B_parabolic_sigma"
  );
  mhd->div_B_over_clean_factor = parser_get_opt_param_float(
    params, "SPH:div_B_over_clean_factor", 1.f
  );
  if (mhd->div_B_over_clean_factor < 1.f) {
    error("Cannot have div_B_over_clean_factor < 1.");
  }
}

/**
 * @brief Initialises an MHD struct to sensible numbers for mocking
 *        purposes.
 *
 * @param mhd: pointer to the mhd_global_data struct to be filled.
 **/
static INLINE void mhd_init_no_hydro(
    struct mhd_global_data* mhd) {
  /* TODO: Change these */
  mhd->artificial_dissipation_constant = 0.f;
  mhd->artificial_dissipation_minimum = 0.f;
  mhd->artificial_dissipation_source = 0.f;
  mhd->artificial_dissipation_timescale = 0.f;
  mhd->with_div_B_cleaning = 0;
  mhd->div_B_parabolic_sigma = 0.f;
  mhd->div_B_over_clean_factor = 0.f;
}

/**
 * @brief Prints out the MHD parameters at the start of a run.
 *
 * @param viscosity: pointer to the viscosity_global_data struct found in
 *                   hydro_properties
 **/
static INLINE void mhd_print(
    const struct mhd_global_data* mhd) {
  message("MHD artificial_dissipation_constant = %g",
          mhd->artificial_dissipation_constant);
  message("MHD artificial_dissipation_minimum = %g",
          mhd->artificial_dissipation_minimum);
  message("MHD artificial_dissipation_source = %g",
          mhd->artificial_dissipation_source);
  message("MHD artificial_dissipation_timescale = %g",
          mhd->artificial_dissipation_timescale);

  if (mhd->div_B_cleaning) {
    message("MHD is running with divB cleaning ON.");
    message("MHD div_B_parabolic_sigma = %g",
            mhd->div_B_parabolic_sigma);
    message("MHD div_B_over_clean_factor = %g",
            mhd->div_B_over_clean_factor);
  } else {
    message("MHD is running with divB cleaning OFF.");
  }
}

#if defined(HAVE_HDF5)
/**
 * @brief Prints the MHD information to the snapshot when writing.
 *
 * @param h_grpsph: the SPH group in the ICs to write attributes to.
 * @param mhd: pointer to the mhd_global_data struct.
 **/
static INLINE void mhd_print_snapshot(
    hid_t h_grpsph, const struct mhd_global_data* mhd) {

  io_write_attribute_f(
    h_grpsph, "Artificial dissipation constant", 
    mhd->artificial_dissipation_constant
  );
  io_write_attribute_f(
    h_grpsph, "Artificial dissipation minimum", 
    mhd->artificial_dissipation_minimum
  );
  io_write_attribute_f(
    h_grpsph, "Artificial dissipation source", 
    mhd->artificial_dissipation_source
  );
  io_write_attribute_f(
    h_grpsph, "Artificial dissipation timescale", 
    mhd->artificial_dissipation_timescale
  );

  io_write_attribute_i(
    h_grpsph, "divB cleaning turned on", 
    mhd->with_div_B_cleaning
  );

  if (mhd->with_div_B_cleaning) {
    io_write_attribute_f(
      h_grpsph, "divB parabolic sigma", 
      mhd->div_B_parabolic_sigma
    );
    io_write_attribute_f(
      h_grpsph, "divB over-cleaning factor", 
      mhd->div_B_over_clean_factor
    );
  }
}
#endif
#endif

/* Viscosity */

/**
 * @brief Initialises the viscosity parameters in the struct from
 *        the parameter file, or sets them to defaults.
 *
 * @param params: the pointer to the swift_params file
 * @param us: pointer to the internal unit system
 * @param phys_const: pointer to the physical constants system
 * @param viscosity: pointer to the viscosity_global_data struct to be filled.
 **/
static INLINE void viscosity_init(struct swift_params* params,
                                  const struct unit_system* us,
                                  const struct phys_const* phys_const,
                                  struct viscosity_global_data* viscosity) {

  /* Read the artificial viscosity parameters from the file, if they exist,
   * otherwise set them to the defaults defined above. */

  viscosity->alpha = parser_get_opt_param_float(
      params, "SPH:viscosity_alpha", hydro_props_default_viscosity_alpha);
}

/**
 * @brief Initialises a viscosity struct to sensible numbers for mocking
 *        purposes.
 *
 * @param viscosity: pointer to the viscosity_global_data struct to be filled.
 **/
static INLINE void viscosity_init_no_hydro(
    struct viscosity_global_data* viscosity) {
  viscosity->alpha = hydro_props_default_viscosity_alpha;
}

/**
 * @brief Prints out the viscosity parameters at the start of a run.
 *
 * @param viscosity: pointer to the viscosity_global_data struct found in
 *                   hydro_properties
 **/
static INLINE void viscosity_print(
    const struct viscosity_global_data* viscosity) {
  message("Artificial viscosity parameters set to alpha: %.3f",
          viscosity->alpha);
}

#if defined(HAVE_HDF5)
/**
 * @brief Prints the viscosity information to the snapshot when writing.
 *
 * @param h_grpsph: the SPH group in the ICs to write attributes to.
 * @param viscosity: pointer to the viscosity_global_data struct.
 **/
static INLINE void viscosity_print_snapshot(
    hid_t h_grpsph, const struct viscosity_global_data* viscosity) {

  io_write_attribute_f(h_grpsph, "Alpha viscosity", viscosity->alpha);
  io_write_attribute_f(h_grpsph, "Beta viscosity", const_viscosity_beta);
}
#endif

/* Diffusion */

/**
 * @brief Initialises the diffusion parameters in the struct from
 *        the parameter file, or sets them to defaults.
 *
 * @param params: the pointer to the swift_params file
 * @param us: pointer to the internal unit system
 * @param phys_const: pointer to the physical constants system
 * @param diffusion: pointer to the diffusion struct to be filled.
 **/
static INLINE void diffusion_init(struct swift_params* params,
                                  const struct unit_system* us,
                                  const struct phys_const* phys_const,
                                  struct diffusion_global_data* diffusion) {}

/**
 * @brief Initialises a diffusion struct to sensible numbers for mocking
 *        purposes.
 *
 * @param diffusion: pointer to the diffusion_global_data struct to be filled.
 **/
static INLINE void diffusion_init_no_hydro(
    struct diffusion_global_data* diffusion) {}

/**
 * @brief Prints out the diffusion parameters at the start of a run.
 *
 * @param diffusion: pointer to the diffusion_global_data struct found in
 *                   hydro_properties
 **/
static INLINE void diffusion_print(
    const struct diffusion_global_data* diffusion) {}

#ifdef HAVE_HDF5
/**
 * @brief Prints the diffusion information to the snapshot when writing.
 *
 * @param h_grpsph: the SPH group in the ICs to write attributes to.
 * @param diffusion: pointer to the diffusion_global_data struct.
 **/
static INLINE void diffusion_print_snapshot(
    hid_t h_grpsph, const struct diffusion_global_data* diffusion) {}
#endif

#endif /* SWIFT_GADGET2_HYDRO_PARAMETERS_H */
