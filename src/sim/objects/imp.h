struct Objects;

struct Sdf;
struct Config;
struct Coords;
struct Opt;
struct PFarray;
struct PFarrays;
struct BForce;
struct TimeStepAccel;

void objects_ini(const Config*, const Opt*, MPI_Comm, const Coords*, int maxp, Objects**);
void objects_fin(Objects*);
    
void objects_clear_vel(Objects*);
void objects_update(float dt, Objects*);
void objects_distribute(Objects*);

void objects_clear_forces(Objects*);
void objects_internal_forces(float dt, Objects *o);
void objects_body_forces(const BForce*, Objects *o);

void objects_mesh_dump(Objects*);
void objects_diag_dump(float t, Objects*);
    
void objects_strt_templ(const char *base, Objects*);
void objects_strt_dump(const char *base, long id, Objects*);

void objects_get_particles_all(Objects*, PFarrays*);
void objects_get_particles_mbr(Objects*, PFarrays*);
void objects_get_accel(const Objects*, TimeStepAccel*);

void objects_gen_mesh(Objects*);
void objects_remove_from_wall(const Sdf *sdf, Objects *o);
void objects_gen_freeze(PFarray*, Objects*);

void objects_restart(Objects*);

void objects_bounce(float dt, float flu_mass, const Clist flu_cells, PFarray *flu, Objects*);
void objects_recolor_flu(Objects*, PFarray *flu);


double objects_mbr_tot_volume(const Objects*);

void objects_check_size(const Objects*);
void objects_check_vel(const Objects*, float dt);
void objects_check_forces(const Objects*, float dt);
