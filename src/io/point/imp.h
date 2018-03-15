struct IOPointConf;
struct IOPoint;

// tag::interface[]
void io_point_conf_ini(/**/ IOPointConf**);
void io_point_conf_push(int nv, const char *keys);
void io_point_conf_fin(IOPointConf*);

void io_point_ini(int maxn, const char *path, IOPointConf*, /**/ IOPoint**);
void io_point_push(int n, double*, const char *keys);
void io_point_write(IOPoint*, MPI_Comm comm, int id);
void io_point_fin(IOPoint*);
// end::interface[]
