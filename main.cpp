/*
 * main.c
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>

#include "apex_cpu.h"

int
main(int argc, char const *argv[])
{
    APEX_CPU *cpu;

    fprintf(stderr, "APEX CPU Pipeline Simulator v%0.1lf\n", VERSION);
    fprintf(stderr, "%d\n", argc);
    if (argc < 3 || argc > 4)
    {
        fprintf(stderr, "APEX_Help: Usage %s <input_file> <command> <cycles_wanted/memory_address>\n", argv[0]);
        exit(1);
    }

    cpu = APEX_cpu_init(argv[1]);
    if (!cpu)
    {
        fprintf(stderr, "APEX_Error: Unable to initialize CPU\n");
        exit(1);
    }

    if(argc == 3){
        APEX_cpu_run(cpu, argv[2], -1);
    }else if(argc == 4){
        APEX_cpu_run(cpu, argv[2], atoi(argv[3]));
    }
    APEX_cpu_stop(cpu);
    return 0;
}