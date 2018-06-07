# Parallel Matrix Multiplication using Open MPI

The project is already configured to build an MPI application. All you have to do is to configure the
``MPIROOT`` environment variable, this variable needs to point to the installation directory of openmpi.

After the project is build. You can run the program as:

``$mpirun --oversubscribe -np NUMBER_OF_WORKERS /path/to/ParallelMatrixMultiplication``