/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apex_cpu.h"
#include "apex_macros.h"

int display_state = FALSE;
FILE * fp;
/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */
static int
get_code_memory_index_from_pc(const int pc)
{
    return (pc - 4000) / 4;
}

static void
print_instruction(const CPU_Stage *stage)
{
    switch (stage->opcode)
    {
        case OPCODE_ADD:
        case OPCODE_MUL:
        case OPCODE_DIV:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_EXOR:
        case OPCODE_SUB:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                       stage->rs2);

            }
            if(display_state){
                fprintf(fp, "%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
                       stage->rs2);

            }
            break;
        }

        case OPCODE_MOVC:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
            }
            if(display_state){
                fprintf(fp, "%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
            }
            break;
        }

        case OPCODE_ADDL:
        case OPCODE_LOAD:
        case OPCODE_SUBL:
        case OPCODE_LDI:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                       stage->imm);
            }
            if(display_state){
                fprintf(fp, "%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
                       stage->imm);
            }
            break;
        }

        case OPCODE_STORE:
        {
            if(ENABLE_DEBUG_MESSAGES){
            printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                   stage->imm);
            }
            if(display_state){
                fprintf(fp, "%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
                   stage->imm);
            }
            break;
        }

        case OPCODE_STI:
        {
            if(ENABLE_DEBUG_MESSAGES){
                  printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs2, stage->rs1,
                   stage->imm);          
            }
            if(display_state){
                  fprintf(fp, "%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs2, stage->rs1,
                   stage->imm);          
            }
            break;
        }

        case OPCODE_BP:
        case OPCODE_BNP:
        case OPCODE_BZ:
        case OPCODE_BNZ:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,#%d ", stage->opcode_str, stage->imm);
            }
            if(display_state){
                  fprintf(fp, "%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs2, stage->rs1,
                   stage->imm);          
            }
            break;
        }

        case OPCODE_JUMP:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,R%d,#%d ", stage->opcode_str, stage->rs1, stage->imm);
            }
            if(display_state){
                fprintf(fp, "%s,R%d,#%d ", stage->opcode_str, stage->rs1, stage->imm);
            }
            break;
        }

        case OPCODE_NOP:
        case OPCODE_HALT:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s", stage->opcode_str);
            }
            if(display_state){
                fprintf(fp, "%s", stage->opcode_str);
            }
            break;
        }

        case OPCODE_CMP:
        {
            if(ENABLE_DEBUG_MESSAGES){
                printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
            }
            if(display_state){
                fprintf(fp, "%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
            }
            break;
        }
    }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void
print_stage_content(const char *name, const CPU_Stage *stage)
{
    if(ENABLE_DEBUG_MESSAGES){
        printf("%-15s: pc(%d) ", name, stage->pc);
    }
    if(display_state){
        fprintf(fp, "%-15s: pc(%d) ", name, stage->pc);
    }
    print_instruction(stage);
    if(ENABLE_DEBUG_MESSAGES){
        printf("\n");
    }
    if(display_state){
        fprintf(fp, "\n");
    }
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
static void
print_reg_file(const APEX_CPU *cpu)
{
    int i;

    printf("----------\n%s\n----------\n", "Registers:");

    for (int i = 0; i < REG_FILE_SIZE / 2; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");

    for (i = (REG_FILE_SIZE / 2); i < REG_FILE_SIZE; ++i)
    {
        printf("R%-3d[%-3d] ", i, cpu->regs[i]);
    }

    printf("\n");
}

/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    APEX_Instruction *current_ins;

    if (cpu->fetch.has_insn)
    {
        /* This fetches new branch target instruction from next cycle */
        if (cpu->fetch_from_next_cycle == TRUE)
        {
            cpu->fetch_from_next_cycle = FALSE;

            /* Skip this cycle*/
            return;
        }

        /* Store current PC in fetch latch */
        cpu->fetch.pc = cpu->pc;

        /* Index into code memory using this pc and copy all instruction fields
         * into fetch latch  */
        current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
        strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
        cpu->fetch.opcode = current_ins->opcode;
        cpu->fetch.rd = current_ins->rd;
        cpu->fetch.rs1 = current_ins->rs1;
        cpu->fetch.rs2 = current_ins->rs2;
        cpu->fetch.imm = current_ins->imm;

        /* Update PC for next instruction */
        cpu->pc += 4;

        /* Copy data from fetch latch to decode latch*/
        cpu->decode = cpu->fetch;

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Fetch", &cpu->fetch);
        }

        /* Stop fetching new instructions if HALT is fetched */
        if (cpu->fetch.opcode == OPCODE_HALT)
        {
            cpu->fetch.has_insn = FALSE;
        }
    }
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode(APEX_CPU *cpu)
{
    char wait_for_data = 0;
    if (cpu->decode.has_insn)
    {
        /* Read operands from register file based on the instruction type */
        switch (cpu->decode.opcode)
        {
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_EXOR:
            case OPCODE_MUL:
            {
                if(cpu->available_regs[cpu->decode.rs1] + cpu->available_regs[cpu->decode.rs2] == 0){//Neither source needs to wait

                    cpu->available_regs[cpu->decode.rd] += 1; //Writing to destination register so we flag dependency
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];   
                }else{
                    wait_for_data = 1;
                }

                break;
            }

            case OPCODE_STORE:
            case OPCODE_CMP:
            {
                if(cpu->available_regs[cpu->decode.rs1] + cpu->available_regs[cpu->decode.rs2] == 0){//Neither source needs to wait

                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];   
                }else{
                    wait_for_data = 1;
                }

                break;
            }

            case OPCODE_STI:
            {
                if(cpu->available_regs[cpu->decode.rs1] + cpu->available_regs[cpu->decode.rs2] == 0){//Neither source needs to wait
                    cpu->available_regs[cpu->decode.rs1] += 1; //Writing to src1 register so we flag dependency
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                    cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];   
                }else{
                    wait_for_data = 1;
                }

                break;
            }

            case OPCODE_LOAD:
            case OPCODE_ADDL:
            case OPCODE_SUBL:
            {
                if(cpu->available_regs[cpu->decode.rs1] == 0){
                    cpu->available_regs[cpu->decode.rd] += 1; //Writing to destination register so we flag dependency
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                }else{
                    wait_for_data = 1;
                }

                break;
            }


            case OPCODE_MOVC:
            {
                cpu->available_regs[cpu->decode.rd] += 1; //Writing to destination register so we flag dependency
                /* MOVC doesn't have register operands */
                break;
            }

            case OPCODE_JUMP:
            {
                if(cpu->available_regs[cpu->decode.rs1] == 0){
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                }else{
                    wait_for_data = 1;
                }
                break;
            }

            case OPCODE_LDI:
            {
                if(cpu->available_regs[cpu->decode.rs1] == 0){
                    cpu->available_regs[cpu->decode.rd] += 1; //Writing to destination register so we flag dependency
                    cpu->available_regs[cpu->decode.rs1] += 1; //Writing to src1 register so we flag dependency
                    cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
                }else{
                    wait_for_data = 1;
                }

                break;     
            }

        }

        /* Copy data from decode latch to execute latch*/
        if(wait_for_data){
            cpu->fetch.has_insn = FALSE;
        }else{
            if(cpu->decode.opcode != OPCODE_HALT){
                cpu->fetch.has_insn = TRUE;
            }
            cpu->execute = cpu->decode;
            cpu->decode.has_insn = FALSE;
        }

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Decode/RF", &cpu->decode);
        }
    }
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute(APEX_CPU *cpu)
{
    if (cpu->execute.has_insn)
    {
        /* Execute logic based on instruction type */
        switch (cpu->execute.opcode)
        {
            case OPCODE_ADD:
            {
                cpu->execute.result_buffer
                    = cpu->execute.rs1_value + cpu->execute.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_LOAD:
            {
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                break;
            }

            case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNZ:
            {
                if (cpu->zero_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BP:
            {
                if (cpu->positive_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_BNP:
            {
                if (cpu->positive_flag == FALSE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.pc + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
                }
                break;
            }

            case OPCODE_JUMP:
            {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->pc = cpu->execute.rs1_value + cpu->execute.imm;
                    
                    /* Since we are using reverse callbacks for pipeline stages, 
                     * this will prevent the new instruction from being fetched in the current cycle*/
                    cpu->fetch_from_next_cycle = TRUE;

                    /* Flush previous stages */
                    cpu->decode.has_insn = FALSE;

                    /* Make sure fetch stage is enabled to start fetching from new PC */
                    cpu->fetch.has_insn = TRUE;
            }

            case OPCODE_MOVC: 
            {
                cpu->execute.result_buffer = cpu->execute.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_ADDL:
            {
                cpu->execute.result_buffer
                    = cpu->execute.rs1_value + cpu->execute.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUB:
            {
                cpu->execute.result_buffer
                    = cpu->execute.rs1_value - cpu->execute.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->execute.result_buffer
                    = cpu->execute.rs1_value - cpu->execute.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_AND:
            {
                cpu->execute.result_buffer
                    = cpu->execute.rs1_value & cpu->execute.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->execute.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_OR:
            {
                    cpu->execute.result_buffer
                        = cpu->execute.rs1_value | cpu->execute.rs2_value;

                    /* Set the zero flag based on the result buffer */
                    if (cpu->execute.result_buffer == 0)
                    {
                        cpu->zero_flag = TRUE;
                    } 
                    else 
                    {
                        cpu->zero_flag = FALSE;
                    }
                    if(cpu->execute.result_buffer > 0){
                        cpu->positive_flag = TRUE;
                    }else{
                        cpu->positive_flag = FALSE;
                    }
                    break;
            } 

            case OPCODE_EXOR:
            {
                    cpu->execute.result_buffer
                        = cpu->execute.rs1_value ^ cpu->execute.rs2_value;

                    /* Set the zero flag based on the result buffer */
                    if (cpu->execute.result_buffer == 0)
                    {
                        cpu->zero_flag = TRUE;
                    } 
                    else 
                    {
                        cpu->zero_flag = FALSE;
                    }
                    if(cpu->execute.result_buffer > 0){
                        cpu->positive_flag = TRUE;
                    }else{
                        cpu->positive_flag = FALSE;
                    }
                    break;
            } 

            case OPCODE_MUL:
            {
                    cpu->execute.result_buffer
                        = cpu->execute.rs1_value * cpu->execute.rs2_value;

                    /* Set the zero flag based on the result buffer */
                    if (cpu->execute.result_buffer == 0)
                    {
                        cpu->zero_flag = TRUE;
                    } 
                    else 
                    {
                        cpu->zero_flag = FALSE;
                    }
                    if(cpu->execute.result_buffer > 0){
                        cpu->positive_flag = TRUE;
                    }else{
                        cpu->positive_flag = FALSE;
                    }
                    break;
            }

            case OPCODE_STORE:
            {
                cpu->execute.memory_address
                    = cpu->execute.rs2_value + cpu->execute.imm;
                break;
            }

            case OPCODE_STI:
            {
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                cpu->execute.inc_address_buffer = cpu->execute.rs1_value + 4;
                break;
            }

            case OPCODE_LDI:
            {
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                cpu->execute.inc_address_buffer = cpu->execute.rs1_value + 4;
                break;
            }

            case OPCODE_CMP:
            {
                if(cpu->execute.rs1_value == cpu->execute.rs2_value){
                    cpu->zero_flag = TRUE;
                }else{
                    cpu->zero_flag = FALSE;
                }
                if(cpu->execute.rs1_value > cpu->execute.rs2_value){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
            }

        }



        /* Copy data from execute latch to memory latch*/
        cpu->memory = cpu->execute;
        cpu->execute.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Execute", &cpu->execute);
        }
    }
}

/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_memory(APEX_CPU *cpu)
{
    if (cpu->memory.has_insn)
    {
        switch (cpu->memory.opcode)
        {
            case OPCODE_ADD:
            {
                /* No work for ADD */
                break;
            }

            case OPCODE_LDI:
            case OPCODE_LOAD:
            {
                /* Read from data memory */
                cpu->memory.result_buffer
                    = cpu->data_memory[cpu->memory.memory_address];
                break;
            }

            case OPCODE_STORE:
            {
                /*Write data into memory*/
                cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs1_value;
                break;
            }

            case OPCODE_STI:
            {
                /*Write data into memory*/
                cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs2_value;
                break;
            }
        }

        /* Copy data from memory latch to writeback latch*/
        cpu->writeback = cpu->memory;
        cpu->memory.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Memory", &cpu->memory);
        }
    }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int
APEX_writeback(APEX_CPU *cpu)
{
    if (cpu->writeback.has_insn)
    {
        /* Write result to register file based on instruction type */
        switch (cpu->writeback.opcode)
        {


            case OPCODE_ADD:
            case OPCODE_LOAD:
            case OPCODE_MOVC: 
            case OPCODE_ADDL:
            case OPCODE_SUB:
            case OPCODE_SUBL:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_EXOR:
            case OPCODE_MUL:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                cpu->available_regs[cpu->writeback.rd] -= 1; //Finished Writing to destination register so we unflag dependency
                break;
            }
            
            case OPCODE_LDI:
            {
                cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
                cpu->regs[cpu->writeback.rs1] = cpu->writeback.inc_address_buffer;
                cpu->available_regs[cpu->writeback.rd] -= 1; //Finished Writing to destination register so we unflag dependency
                cpu->available_regs[cpu->writeback.rs1] -= 1; //Finished Writing to destination register so we unflag dependency
                break;

            }
            
            case OPCODE_STI:
            {
                cpu->regs[cpu->writeback.rs1] = cpu->writeback.inc_address_buffer;
                cpu->available_regs[cpu->writeback.rs1] -= 1; //Finished Writing to destination register so we unflag dependency
                break;
            }

        }

        cpu->insn_completed++;
        cpu->writeback.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Writeback", &cpu->writeback);
        }

        if (cpu->writeback.opcode == OPCODE_HALT)
        {
            /* Stop the APEX simulator */
            return TRUE;
        }
    }

    /* Default */
    return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *
APEX_cpu_init(const char *filename)
{
    int i;
    APEX_CPU *cpu;

    if (!filename)
    {
        return NULL;
    }

    cpu = calloc(1, sizeof(APEX_CPU));

    if (!cpu)
    {
        return NULL;
    }

    /* Initialize PC, Registers and all pipeline stages */
    cpu->pc = 4000;
    memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->available_regs, 0, sizeof(int) * REG_FILE_SIZE);
    memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
    cpu->single_step = ENABLE_SINGLE_STEP;
    cpu->filename = (char *) malloc(128);
    strcpy(cpu->filename, filename);

    /* Parse input file and create code memory */
    cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
    if (!cpu->code_memory)
    {
        free(cpu);
        return NULL;
    }

    if (ENABLE_DEBUG_MESSAGES)
    {
        fprintf(stderr,
                "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
                cpu->code_memory_size);
        fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
        fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
        printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
               "imm");

        for (i = 0; i < cpu->code_memory_size; ++i)
        {
            printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
                   cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
                   cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
        }
    }

    /* To start fetch stage */
    cpu->fetch.has_insn = TRUE;
    cpu->clock = 1;
    return cpu;
}

APEX_CPU * user_init(APEX_CPU * cpu){
    /*I just leverage the given init function (with a modification to keep track of the instruction filename)*/
    char* filename = (char *) malloc(128);
    strcpy(filename, cpu->filename);
    APEX_cpu_stop(cpu);
    return APEX_cpu_init(filename);
}

void simulate_display(APEX_CPU * cpu){
    printf("************************************************************\n");
    print_reg_file(cpu);

    printf("FLAGS: Z = %d, P = %d\n", cpu->zero_flag, cpu->positive_flag);

    printf("----------\n%s\n----------\n", "MEMORY:");
    for(int i = 0; i < 99; i++){
        printf("mem[%d] = %d, ", i, cpu->data_memory[i]);
    }
    printf("mem[%d] = %d\n", 99, cpu->data_memory[99]);


    printf("************************************************************\n");
}
/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_run(APEX_CPU *cpu, const char * arg, int cycles_wanted)
{
    int mem_address_wanted = -1;
    if(strcmp(arg, "single_step") == 0){
        cpu->single_step = TRUE;
    }else if(strcmp(arg, "show_mem") == 0){
        mem_address_wanted = cycles_wanted; 
    }else if(strcmp(arg, "display") == 0){
        display_state = TRUE;
        fp = fopen("temp.txt", "w");
    }
    char user_prompt_val;
    /*Cycles_wanted = -1 means single_step
     * mem_address_wanted != -1 means show_mem
     * Otherwise it is display or simulate
     */
    while (mem_address_wanted != -1 || cycles_wanted == -1 || cpu->clock < cycles_wanted)
    {
        if (ENABLE_DEBUG_MESSAGES)
        {
            printf("--------------------------------------------\n");
            printf("Clock Cycle #: %d\n", cpu->clock);
            printf("--------------------------------------------\n");
        }

        if (display_state)
        {
            fprintf(fp,"--------------------------------------------\n");
            fprintf(fp,"Clock Cycle #: %d\n", cpu->clock);
            fprintf(fp,"--------------------------------------------\n");
        }

        if (APEX_writeback(cpu))
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }
        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);
        print_reg_file(cpu);

        if (cpu->single_step)
        {
            printf("Press any key to advance CPU Clock or <q> to quit:\n");
            scanf("%c", &user_prompt_val);

            if ((user_prompt_val == 'Q') || (user_prompt_val == 'q'))
            {
                printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
                break;
            }
        }

        cpu->clock++;
    }
    //Do display stuff
    printf("Display %s stuff goes here!!\n", arg);
    if(strcmp(arg, "simulate") == 0 || strcmp(arg, "single_step") == 0){
        simulate_display(cpu);
    }else if(strcmp(arg, "display") == 0){
        simulate_display(cpu);
        fclose(fp);
        fp = fopen("temp.txt", "r");
        char c = fgetc(fp);        
        while(c != EOF){
            printf("%c", c);
            c = fgetc(fp);
        }
        fclose(fp);
        remove("temp.txt");
        simulate_display(cpu);
    }else if(strcmp(arg, "show_mem") == 0){
        int data = cpu->data_memory[mem_address_wanted];
        printf("mem[%d] = %d\n", mem_address_wanted, data);
    }
}

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void
APEX_cpu_stop(APEX_CPU *cpu)
{
    free(cpu->code_memory);
    free(cpu->filename);
    free(cpu);
}