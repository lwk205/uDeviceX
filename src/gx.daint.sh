ini() {
    module load cray-hdf5-parallel cudatoolkit daint-gpu GSL
}

run () {
    n=gx
    
    cat <<-EOF > runme.sh
	#!/bin/bash -l
	#SBATCH --partition=low
	#SBATCH --job-name=$n
	#SBATCH --time=00:30:00
	#SBATCH --ntasks=${NN}
	#SBATCH --ntasks-per-nodes=1
	#SBATCH --output=output.txt
	#SBATCH --error=error.txt
	#SBATCH --constraint=gpu
	:
    module load cray-hdf5-parallel cudatoolkit daint-gpu GSL
	srun --export ALL -u -n ${NN} ./udx ${NX} ${NY} ${NZ}
EOF
    sbatch runme.sh
}
