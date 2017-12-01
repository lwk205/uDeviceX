/* how wall velocity is changing in time?  */
#if !defined(WVEL_FLAT) && !defined(WVEL_DUPIRE_UP) && !defined(WVEL_DUPIRE_DOWN) && !defined(WVEL_SIN)
  #define WVEL_FLAT
#endif
#ifndef WVEL_LOG_FREQ
  #define WVEL_LOG_FREQ (1000)
#endif

#ifndef WVEL_PAR_A
  #define WVEL_PAR_A (0.0)
  #define WVEL_PAR_Y (false)
  #define WVEL_PAR_Z (false)
#endif

/* magnituted of body force
   see doc/body_force.md */
#ifndef FORCE_PAR_A
  #define FORCE_PAR_A (2.0)
#endif

/* object-object, cell-object, and cell-cell contact force */
#ifndef contactforces
#define contactforces (false)
#endif

/* should solvent have colors? 
   see doc/color.md */
#ifndef multi_solvent
#define multi_solvent (false)
#endif

/* when to re-color rbcs */
#ifndef color_freq
#define color_freq    (0)
#endif

/* recolor solvent crossing periodic boundary ? */
#ifndef RECOLOR_FLUX
#define RECOLOR_FLUX (false)
#endif
#ifndef COL_FLUX_DIR
#define COL_FLUX_DIR (0)
#endif

/* ids for solvent */
#ifndef global_ids
#define global_ids (false)
#endif

/* ids for cell */
#ifndef rbc_ids
#define rbc_ids (false)
#endif

// dump
#ifndef dump_all_fields
#define dump_all_fields (false)
#endif

#ifndef DUMP_BASE
#define DUMP_BASE "."
#endif

#ifndef field_dumps
#define field_dumps (false)
#endif

#ifndef field_freq
#define field_freq (1000)
#endif

#ifndef part_dumps
#define part_dumps (false)
#endif

#ifndef force_dumps
#define force_dumps (false)
#endif

#ifndef part_freq
#define part_freq (1000)
#endif

#ifndef rbc_com_dumps
  #define rbc_com_dumps (false)
#endif

/* stretch cell?  see doc/stretch.md */
#ifndef RBC_STRETCH
  #define RBC_STRETCH (false)
#endif

#ifndef RBC_RND
   #define RBC_RND (false)
#endif

/* dump meshes relative to the domain edge or domain center? */
#if !defined(MESH_SHIFT_EDGE) && !defined(MESH_SHIFT_CENTER)
  #define MESH_SHIFT_EDGE
#endif

/* assert */
#if rbc_com_dumps && !rbc_ids
    #error "Need rbc ids for rbc_com_dumps"
#endif

#ifndef rbc_com_freq
#define rbc_com_freq (1000)
#endif


// dpd

#ifndef dpd_mass
#define dpd_mass 1.0
#endif


// solid

#ifndef solids
#define solids (false)
#endif

#ifndef solid_mass
#define solid_mass dpd_mass
#endif

#ifndef pin_com
#define pin_com (false)
#endif

#ifndef pin_comx
#define pin_comx (false)
#endif

#ifndef pin_comy
#define pin_comy (false)
#endif

#ifndef pin_comz
#define pin_comz (false)
#endif

#ifndef pin_axis
#define pin_axis (false)
#endif

#ifndef sbounce_back
#define sbounce_back (false)
#endif

#ifndef rescue_freq
#define rescue_freq (100)
#endif

#ifndef pushflow
#define pushflow (false)
#endif

#ifndef pushsolid
#define pushsolid (false)
#endif

#ifndef fsiforces
#define fsiforces (false)
#endif

#ifndef empty_solid_particles
#define empty_solid_particles (true)
#endif

// spdir: [s]olid [p]eriodic [dir]ection
// example: an open cylinder along z is periodic along z, so spdir = 2
#ifdef spdir
#undef  pin_com
#define pin_com (true)
#undef  pin_axis
#define pin_axis (true)
#endif

// rbc
#ifndef rbcs
#define rbcs (false)
#endif

#ifndef rbc_mass
#define rbc_mass dpd_mass
#endif

#ifndef RBCnv
#define RBCnv (498)
#endif
/* [n]umber of [t]riangles (Euler formula) */
#define RBCnt ( 2*(RBCnv) - 4 )
/* maximum allowed degree of a vertex */
#define RBCmd 7

#ifndef rbounce_back
#define rbounce_back (false)
#endif

#ifndef pushrbc
#define pushrbc (false)
#endif

// walls
#ifndef walls
#define walls (false)
#endif

#ifndef wall_creation
#define wall_creation (1000)
#endif

// restart

#ifndef RESTART
#define RESTART (false)
#endif

#ifndef BASE_STRT_DUMP
#define BASE_STRT_DUMP "strt"
#endif

#ifndef BASE_STRT_READ
#define BASE_STRT_READ "strt"
#endif

#ifndef strt_dumps
#define strt_dumps (false)
#endif

#ifndef strt_freq
#define strt_freq (1000)
#endif

// time
#ifndef tend
#define tend (10)
#endif

// debug
/* dbg macros */
#if !defined(DBG_NONE)    && !defined(DBG_TRACE) && \
    !defined(DBG_SILENT)  && !defined(DBG_PEEK)
#define DBG_NONE
#endif

/* [k]ernel [l]aunch macros */
#if !defined(KL_RELEASE)    && !defined(KL_TRACE)  && \
    !defined(KL_PEEK)       && !defined(KL_UNSAFE) && \
    !defined(KL_TRACE_PEEK) && !defined(KL_NONE)   && \
    !defined(KL_CPU)        && !defined(KL_SYNC)
#define KL_RELEASE
#endif

/* [m]pi [c]heck macro */
#if !defined(MC_RELEASE)
  #define MC_RELEASE
#endif

/* [c]uda [c]heck macro */
#if !defined(CC_RELEASE) && !defined(CC_SYNC) && !defined(CC_TRACE) && !defined(CC_TRACE_PEEK)
  #define CC_RELEASE
#endif

/* [te]xture macros */
#if !defined(TE_RELEASE) && !defined(TE_TRACE)
  #define TE_RELEASE
#endif

/* who plays as device? */
#if !defined(DEV_CUDA) && !defined(DEV_CPU)
  #define DEV_CUDA
#endif

/* body force */
#if !defined(FORCE_NONE) && !defined(FORCE_DOUBLE_POISEUILLE) && \
    !defined(FORCE_4ROLLER) && !defined(FORCE_CONSTANT) && !defined(FORCE_SHEAR)
  #define FORCE_NONE
#endif

/* a radius of the spherical drop */
#ifndef BANGLADESH_R
  #define BANGLADESH_R (4)
#endif

/* make a center of mass velocity zero? */
#if !defined(RESTRAIN_RED_VEL) && !defined(RESTRAIN_RBC_VEL) && \
    !defined(RESTRAIN_NONE)
  #define RESTRAIN_NONE
#endif

/* RBC membrain parameter sets */
#if !defined(RBC_PARAMS_TEST) && !defined(RBC_PARAMS_LINA)
  #define RBC_PARAMS_TEST
#endif

#ifndef RBC_STRESS_FREE
  #define RBC_STRESS_FREE (false)
#endif

/* MSG frequency */
#ifndef RESTRAIN_REPORT_FREQ
  #define RESTRAIN_REPORT_FREQ (1000)
#endif

/*           Velocity controller           */
#ifndef VCON
  #define VCON (false)
#endif

#ifndef VCON_SAMPLE_FREQ
  #define VCON_SAMPLE_FREQ (10)
#endif

#ifndef VCON_ADJUST_FREQ
  #define VCON_ADJUST_FREQ (500)
#endif

#ifndef VCON_LOG_FREQ
  #define VCON_LOG_FREQ (0)
#endif

#ifndef VCON_VX
  #define VCON_VX (1.f)
#endif

#ifndef VCON_VY
  #define VCON_VY (0.f)
#endif

#ifndef VCON_VZ
  #define VCON_VZ (0.f)
#endif

#ifndef VCON_FACTOR
  #define VCON_FACTOR (0.08f)
#endif

/* outflow */

#ifndef OUTFLOW
  #define OUTFLOW (false)
#endif

#ifndef OUTFLOW_CIRCLE
#define OUTFLOW_CIRCLE (true)
#endif

#ifndef OUTFLOW_CIRCLE_R
#define OUTFLOW_CIRCLE_R (8.f)
#endif

#ifndef INFLOW
  #define INFLOW (false)
#endif



/* how mass affects dpd forces: like "gravity" ~ mi*mj or like
   "charge" -- no dependencies on mass */
#if !defined(DPD_GRAVITY) && !defined(DPD_CHARGE)
  #define DPD_CHARGE
#endif

/* DPD kernel envelop parameter: random and dissipative kernels (wd = wr^2)
   0: wr = 1 - r
   1: wr = (1 - r)^(1/2)
   2: wr = (1 - r)^(1/4) */
#ifndef S_LEVEL
  #define S_LEVEL (2)
#endif

/*** TODO ***/
#ifndef gdpd_s
  #define gdpd_s gdpd_b
#endif

#ifndef adpd_s
  #define adpd_s adpd_b
#endif
/**********/

/*** see poc/color */
#ifndef gdpd_bw
  #define gdpd_bw gdpd_b
#endif

#ifndef adpd_bw
  #define adpd_bw adpd_b
#endif

#ifndef gdpd_bs
  #define gdpd_bs gdpd_b
#endif

#ifndef adpd_bs
  #define adpd_bs adpd_b
#endif

#ifndef gdpd_rw
  #define gdpd_rw gdpd_r
#endif

#ifndef adpd_rw
  #define adpd_rw adpd_r
#endif

#ifndef gdpd_rs
  #define gdpd_rs gdpd_r
#endif

#ifndef adpd_rs
  #define adpd_rs adpd_r
#endif

#ifndef gdpd_sw
  #define gdpd_sw gdpd_b
#endif

#ifndef adpd_sw
  #define adpd_sw adpd_b
#endif
/*** */
