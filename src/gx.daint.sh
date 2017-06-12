# where are googleX geoms?
#googlex=/scratch/snx3000/lisergey/googlex
googlex=/scratch/snx3000/amlucas/geom

ini() {
    module load cray-hdf5-parallel cudatoolkit daint-gpu GSL
    cp .cache.Makefile.amlucas.daint .cache.Makefile
}

run () {
    n=gx-big
    
    cat <<-EOF > runme.sh
#!/bin/bash -l
#SBATCH --partition=low
#SBATCH --job-name=$n
#SBATCH --time=06:00:00
#SBATCH --nodes=${NN}
#SBATCH --ntasks-per-node=1
#SBATCH --output=output.txt
#SBATCH --error=error.txt
#SBATCH --constraint=gpu
:
module load cray-hdf5-parallel cudatoolkit daint-gpu GSL
export HEX_COMM_FACTOR=2
srun --export=HEX_COMM_FACTOR -u ./udx ${NX} ${NY} ${NZ}
EOF
    sbatch runme.sh
}
