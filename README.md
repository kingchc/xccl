# MCCL - Mellanox Collective Communication Library

The library consists of two layers: 

1. **TCCL** - *teams collective communication layer*, is the lower layer and implements a subset of the *Teams API* under consideration by the **UCX CWG** for the following:
   * A shared memory Team
   * A UCX Team 
   * A SHARP Team
   * A Hardware multicast Team (vmc)

2. **MCCL** - is the upper layer and implements a light-weight, highly scalable framework for expressing hierarchical collectives in terms of the Team abstraction.
   
# Quick Start Guide

The SHARP and hardware multicast teams requires Mellanox's SHARP software library, the hardware multicast team requires Mellanox's VMC software library.

   ### Build and install MCCL and TCCL libraries:

``` bash
# Need for all HPCX_ variable used in examples below
% source /path/to/hpcx/dir/hpcx-init.sh --nompi
% export MCCL_DIR=$PWD/mccl

% git clone https://github.com/openucx/mccl.git $MCCL_DIR
% cd $MCCL_DIR
% ./autogen.sh
% ./configure --prefix=$PWD/install --with-vmc=$HPCX_VMC_DIR \ 
--with-ucx=$HPCX_UCX_DIR --with-sharp=$HPCX_SHARP_DIR
% make -j install
```

   ### Build and install Open MPI :

``` bash
% export OMPI_MCCL_DIR=$PWD/ompi-mccl
% git clone https://github.com/vspetrov/ompi/tree/mccl $OMPI_MCCL_DIR
% cd $OMPI_MCCL_DIR
% ./autogen.pl
% ./configure --prefix=$OMPI_MCCL_DIR/install --with-platform=contrib/platform/mellanox/optimized \
--with-mccl=$MCCL_DIR
% make -j install
```
 
   ### Run :

``` bash
% export OPAL_PREFIX=$OMPI_MCCL_DIR/install
% export PATH=$OMPI_MCCL_DIR/install/bin
% export LD_LIBRARY_PATH="$HPCX_UCX_DIR/lib:$HPCX_SHARP_DIR/lib:$MCCL_DIR/install/lib:$OMPI_MCCL_DIR/install/lib"
% export TCCL_TEAM_LIB_PATH="$MCCL_DIR/install/lib/tccl"
% export nnodes=2 ppn=28

% mpirun -np $((nnodes*ppn)) --map-by ppr:$ppn:node --bind-to core $HPCX_OSU_DIR/osu_allreduce -f
```

# Performance 

### One-level Allreduce: SHARP team  
>Helios Cluster: EDR 16 nodes, 1 process-per-node

**OSU Allreduce**
| msglen	| HCOLL (SHARP) | MCCL |	
|:--- |:---:|---:| 
| 4 |	2.81 | 2.21	|
| 8 | 2.75 | 2.04 |
| 16 | 2.74 | 2.21 |
| 32 | 2.78 | 2.09 |
| 64 | 2.73 | 2.18 |
| 128 |	2.88 | 2.15 |
| 256 |	3.57 | 2.59 |
| 512 |	3.77 | 2.86 |


### Two-level Allreduce: shared memory socket team, shared memory NUMA team 
>Single node POWER9 168 threads  

**OSU Allreduce**
| msglen | 	hcoll | mccl |
|:--- |:---:|---:| 
| 4 | 	4.6 | 5.6 |	
| 8	| 4.53 | 5.6	| 
| 16 | 4.65 |  5.72 |
| 32 |	4.66 | 5.86	|
| 64 |	4.84 | 6.47	|
| 128 |	5.47 | 7.26	|
| 256 |	6.13 | 8.51 |
| 512 |	7.41 | 11.23 |
| 1024 | 9.18 | 15.93 |
| 2048 | 12.5 | 25.18 |


## Three-level Broadcast: UCX team, UCX team, Hardware Multicast team (VMC) :
>Hercules test bed: HDR100 110 nodes, 32 processes-per-node

**OSU Bcast**
| msglen	| hcoll	| mccl |
|:--- |:---:|---:| 
| 1	  | 5.33 | 4.53 |
| 2	  | 4.62 | 4.48 |
| 4	  | 4.51 | 4.33 |
| 8	  | 4.73 | 4.45 |
| 16	| 4.36 | 4.24 |
| 32	| 4.44 | 4.80 |
| 64	| 4.48 | 5.30 |
| 128	| 4.64 | 6.30 |
| 256	| 5.49 | 6.69 |
| 512	| 5.88 | 7.39 |
| 1024| 6.45 | 8.46 |
| 2048 | 7.94 | 9.90 |
| 4096 | 11.30 | 13.42 |
| 8192 | 17.09 | 19.49 |
| 16384 | 30.41	| 30.95 |
| 32768	| 38.37 |	41.54 |




# Publications

This framework is a continuous extension of the advanced research and development on extreme-scale collective communications published in the following scientific papers. The shared memory team is code ported from the HCOLL shared memory BCOL component. The MCCL layer is a "distillate" of the HCOLL framework. The HCOLL framework began its life as the **_Cheetah_** framework:

1. **_Cheetah: A Framework for Scalable Hierarchical Collective Operations_**  
 Date: May 2011  
 Publication description: IEEE/ACM International Symposium on Cluster, Cloud, and Grid Computing (CCGRID)

2. **_ConnectX-2 CORE-Direct Enabled Asynchronous Broadcast Collective Communications_**  
 Date: May 2011      
Publication: First Workshop on Communication Architecture for Scalable Systems (CASS) held in conjunction with the International Parallel and Distributed Processing Symposium (IPDPS)

3. **_Design and Implementation of Broadcast Algorithms for Extreme-Scale Systems_**  
 Date: Sept 2011  
 Publication: IEEE Cluster 2011

4. **_Analyzing the Effect of Multicore Architectures and On-host Communication Characteristics on Collective Communications_**  
 Date: Sept 2011  
 Publication: Workshop on Scheduling and Resource Management for Parallel and Distributed Systems held in conjunction with the International Conference on Parallel Processing (ICPP)

5. **_Assessing the Performance and Scalability of a Novel K-Nomial Allgather on CORE-Direct Systems_**  
 Date: Aug 2012  
 Publication: 18th International Conference, Euro-Par 2012

6. **_Exploring the All-to-All Collective Optimization Space with ConnectX CORE-Direct_**  
 Date: Sept 2012  
 Publication: 41st International Conference on Parallel Processing, ICPP 2012

7. **_Optimizing Blocking and Nonblocking Reduction Operations for Multicore Systems: Hierarchical Design and Implementation_**  
 Date: Sept 2013  
 Publication: IEEE Cluster 2013











 

