MPI_VARIANT = mpich
MPI_CXXFLAGS = `pkg-config --cflags ${MPI_VARIANT}`
MPI_LIBS  = `pkg-config --libs ${MPI_VARIANT}`

HDF5_VARIANT = hdf5-mpich
HDF5_CXXFLAGS = `pkg-config  --cflags  ${HDF5_VARIANT}`
HDF5_LIBS  = `pkg-config --libs ${HDF5_VARIANT}`

libconfigL  = `pkg-config --libs-only-l libconfig`
libconfigID = `pkg-config --variable=includedir libconfig`
libconfigLD = `pkg-config --variable=libdir libconfig`

LIBCONFIG_CXXFLAGS = -I${libconfigID}
LIBCONFIG_LIBS     = -L${libconfigLD} -Wl,-rpath -Wl,${libconfigLD} ${libconfigL}

BOP_CXXFLAGS = `bop-config --cflags`
BOP_LIBS     = `bop-config --libs-mpi`

# Ubuntu 16.04: https://github.com/BVLC/caffe/issues/4046
NVCCFLAGS += -D_FORCE_INLINES
CXXFLAGS += -Wall -Wextra
