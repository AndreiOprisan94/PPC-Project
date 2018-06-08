# Parallel Matrix Multiplication using Open MPI

The project is already configured to build an MPI application. All you have to do is to configure the
``MPIROOT`` environment variable, this variable needs to point to the installation directory of openmpi.

After the project is build. You can run the program as:

``$mpirun --oversubscribe -np NUMBER_OF_WORKERS /path/to/ParallelMatrixMultiplication``

In order to save the results please specify the path to a file via the ``PPC_PARALLEL_RESULT_PATH`` environment variable. Note that
the destination file does not need to be present, but if any exists it`s content will be overwritten. 