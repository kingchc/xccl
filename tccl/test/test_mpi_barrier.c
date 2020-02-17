/**
* Copyright (C) Mellanox Technologies Ltd. 2001-2020.  ALL RIGHTS RESERVED.
*
* See file LICENSE for terms.
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "api/tccl.h"

static int oob_allgather(void *sbuf, void *rbuf, size_t len, void *coll_context) {
    MPI_Comm comm = (MPI_Comm)coll_context;
    MPI_Allgather(sbuf, len, MPI_BYTE, rbuf, len, MPI_BYTE, comm);
    return 0;
}

static inline void
do_barrier(tccl_team_h team) {
    tccl_coll_req_h request;
    tccl_coll_op_args_t coll = {
        .coll_type = TCCL_BARRIER,
        .alg.set_by_user = 0,
        .tag  = 123, //todo
    };
    tccl_collective_init(&coll, &request, team);
    tccl_collective_post(request);
    tccl_collective_wait(request);
    tccl_collective_finalize(request);
}

int main (int argc, char **argv) {
    int rank, size;
    char *var;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    var = getenv("TCCL_TEST_TEAM");
    tccl_team_lib_params_t lib_params = {
        .tccl_model_type = TCCL_MODEL_TYPE_MPI,
        .team_lib_name = var ? var : "ucx",
        .ucp_context = NULL
    };
    tccl_team_lib_h lib;
    tccl_team_lib_init(&lib_params, &lib);

    tccl_team_context_config_t team_ctx_config = {
        .thread_support = TCCL_THREAD_MODE_PRIVATE,
        .completion_type = TCCL_TEAM_COMPLETION_BLOCKING
    };
    tccl_team_lib_attr_t team_lib_attr;
    team_lib_attr.field_mask = TCCL_ATTR_FIELD_CONTEXT_CREATE_MODE;
    tccl_team_lib_query(lib, &team_lib_attr);
    if (team_lib_attr.context_create_mode == TCCL_TEAM_LIB_CONTEXT_CREATE_MODE_GLOBAL) {
        tccl_oob_collectives_t oob_ctx = {
            .allgather  = oob_allgather,
            .coll_context = (void*)MPI_COMM_WORLD,
            .rank = rank,
            .size = size
        };

        team_ctx_config.oob = oob_ctx;
    }
    tccl_team_context_h team_ctx;
    tccl_create_team_context(lib, &team_ctx_config, &team_ctx);
    {
        /* Create TEAM for comm world */
        tccl_team_config_t team_config = {
            .team_size = size,
            .team_rank = rank,
        };

        tccl_oob_collectives_t oob = {
            .allgather  = oob_allgather,
            .coll_context = (void*)MPI_COMM_WORLD,
            .rank = rank,
            .size = size
        };

        tccl_team_h world_team;
        tccl_team_create_post(team_ctx, &team_config, oob, &world_team);
        int i;
        srand(time(NULL));
        for (i=0; i<size; i++) {
            int sleep_us = rand() % 1000;
            usleep(sleep_us);
            if (i == rank) {
                printf("Rank %d checks in\n", rank);
                fflush(stdout);
                usleep(100);
            }
            do_barrier(world_team);
        }
        tccl_team_destroy(world_team);
    }


    MPI_Finalize();
}
