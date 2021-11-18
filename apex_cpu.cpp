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
/* Commenting out old prints (They have to be modified) - J
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
*/
/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_fetch(APEX_CPU *cpu)
{
    //I think fetch can go completely unchanged - J
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

        switch (cpu->fetch.opcode){
            case OPCODE_MUL:
                cpu->fetch.vfu = MUL_VFU;
                break;

            case OPCODE_ADD:
            case OPCODE_ADDL:
            case OPCODE_SUB:
            case OPCODE_SUBL:
            case OPCODE_MOVC:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_EXOR:
            case OPCODE_LOAD:
            case OPCODE_LDI:
            case OPCODE_STI:
            case OPCODE_STORE:
            case OPCODE_NOP:
                cpu->fetch.vfu = INT_VFU;
                break;

            case OPCODE_BZ:
            case OPCODE_BNZ:
            case OPCODE_BP:
            case OPCODE_BNP:
            case OPCODE_JUMP:
            case OPCODE_HALT:
                cpu->fetch.vfu = BRANCH_VFU;
                break;

        }



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

static char available_ROB(APEX_CPU* cpu){
    if(cpu->rob.size() == 16){
        return FALSE;
    }else{
        return TRUE;
    }
}

static char available_IQ(APEX_CPU* cpu){
    for(int i = 0; i < 8; i++){
        if(cpu->iq[i].status_bit == 0){
            return TRUE;
        }
    }
    return FALSE;
}

static char index_IQ(APEX_CPU* cpu){//Finds the first valid index to write into -J
    for(char i = 0; i < 8; i++){
        if(cpu->iq[i].status_bit == 0){
            return i;
        }
    }
    return -1;
}
/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_decode1(APEX_CPU *cpu){
/*
Rj <-- Rk <op> Rl

1) Check for availability of given VFU & free phys regs

2) Find corresponding physical registers for given arch registers, and set them

3) Grab free register from the list and assign it to Rj

Stall if free list isn't empty

- J
*/

    if(!available_ROB(cpu) || !available_IQ(cpu)){//All instructions need a slot in the ROB & IQ -J
        cpu->fetch.has_insn = FALSE; //Stall -J
        return;
    }else{
        char memory_op = FALSE;   
        switch(cpu->decode1.vfu){//Another switch...but this time for checking VFUs -J
            //Check MUL VFU -J
            case MUL_VFU:
                if(cpu->mult_exec.has_insn){
                    cpu->fetch.has_insn = FALSE; 
                    return;
                }
                break;
            //Check INT VFU -J
            case INT_VFU:
                if(cpu->int_exec.has_insn){
                    cpu->fetch.has_insn = FALSE; 
                    return;
                }                        
                break;
            //Check BRANCH VFU -J
            case BRANCH_VFU:
                if(cpu->branch_exec.has_insn){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
            break;
        }

        switch (cpu->decode1.opcode){//This switch is for checking IQ, LSQ, & Free List -J
            case OPCODE_ADD:
            case OPCODE_ADDL:
            case OPCODE_SUB:
            case OPCODE_SUBL:
            case OPCODE_MUL:
            case OPCODE_MOVC:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_EXOR:
            case OPCODE_LOAD:
            case OPCODE_LDI:
                if(cpu->decode1.opcode == OPCODE_LOAD || cpu->decode1.opcode == OPCODE_LDI){
                    //LSQ check -J
                    if(cpu->lsq.size() == 6){
                        cpu->fetch.has_insn = FALSE;
                        return;
                    }else{
                        memory_op = TRUE;
                    }
                }
               if(cpu->free_list.empty()){
                    //Free List check -J
                    cpu->fetch.has_insn = FALSE;
                    return;
                }else{
                    break;
                }
            case OPCODE_STORE:
            case OPCODE_STI:
                //LSQ check -J
                memory_op = TRUE;
                if(cpu->lsq.size() == 6){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
        }
        //Do decode1 stuff, but check instruction types
        cpu->fetch.has_insn = TRUE; //Might have to change this, not sure how this might interact with branches/HALTs -J
        
        
        cpu->decode2 = cpu->decode1;
        cpu->decode1.has_insn = FALSE;
    }


}

static void
APEX_decode2(APEX_CPU *cpu){
/*
Rj <-- Rk <op> Rl

4) Fill in fields with values from physical registers

5) Try to read from phys reg file, wait if the data is invalid

6) Dispatch to IQ (& LSQ if need be)

*/
   int free_reg = -1; //If it stays -1, then we know that it's an instruction w/o a destination
   int memory_op = FALSE;
   ROB_Entry rob_entry;
   rob_entry.pc_value = cpu->decode2.pc;
   rob_entry.ar_addr = cpu->decode2.rd;
   rob_entry.status_bit = 0;
   rob_entry.opcode = cpu->decode2.opcode;
   cpu->rob.push(rob_entry);
   switch(cpu->decode2.opcode){//Handling the instruction renaming -J
            //<dest> <- <src1> <op> <src2> -J
            case OPCODE_ADD:
            case OPCODE_ADDL:
            case OPCODE_SUB:
            case OPCODE_SUBL:
            case OPCODE_MUL:
            case OPCODE_AND:
            case OPCODE_OR:
            case OPCODE_EXOR:
                //Both sources must be valid before we can grab the data (Stall if not) -J
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0 || cpu->phys_regs[cpu->rename_table[cpu->decode2.rs2].phys_reg_id].src_bit== 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id; //Take arch reg and turn it to phys through lookup -J
                cpu->decode2.rs2 = cpu->rename_table[cpu->decode2.rs2].phys_reg_id;
                free_reg = cpu->free_list.front();
                cpu->free_list.pop();
                cpu->rename_table[cpu->rd].phys_reg_id = free_reg;
                cpu->rd = free_reg;
                cpu->phys_regs[cpu->rd].src_bit = 0; //Have to set dest src_bit to zero since we'll now be in the process of setting that value -J
                break;
            //<dest> <- <src1> -J
            case OPCODE_MOVC:
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id;
                free_reg = cpu->free_list.front();
                cpu->free_list.pop();
                cpu->rename_table[cpu->rd].phys_reg_id = free_reg;
                cpu->rd = free_reg;
                cpu->phys_regs[cpu->rd].src_bit = 0;
                break;
            //<dest> <- <src1> <op> #<literal> -J
            case OPCODE_LOAD:
            case OPCODE_LDI:
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id;
                free_reg = cpu->free_list.front();
                cpu->free_list.pop();                
                cpu->rename_table[cpu->rd].phys_reg_id = free_reg;
                cpu->decode2.rd = free_reg;
                cpu->phys_regs[cpu->rd].src_bit = 0;
                break;
            //<src1> <src2> #<literal> -J
            case OPCODE_STORE:
            case OPCODE_STI:
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0 || cpu->phys_regs[cpu->rename_table[cpu->decode2.rs2].phys_reg_id].src_bit== 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id;
                cpu->decode2.rs2 = cpu->rename_table[cpu->decode2.rs2].phys_reg_id;
                break;
            //<branch> <src1> -J
            case OPCODE_JUMP:
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                }
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id;
                break;
            //<op> <src1> <src2> -J
            case OPCODE_CMP:
                if(cpu->phys_regs[cpu->rename_table[cpu->decode2.rs1].phys_reg_id].src_bit == 0 || cpu->phys_regs[cpu->rename_table[cpu->decode2.rs2].phys_reg_id].src_bit== 0){
                    cpu->fetch.has_insn = FALSE;
                    return;
                } 
                cpu->decode2.rs1 = cpu->rename_table[cpu->decode2.rs1].phys_reg_id;
                cpu->decode2.rs2 = cpu->rename_table[cpu->decode2.rs2].phys_reg_id;
                break;
        }
    char entry_index = index_IQ(cpu);
    //Filling out IQ entry -J
    cpu->iq[entry_index].status_bit = 1;
    cpu->iq[entry_index].fu_type = cpu->decode2.vfu;
    cpu->iq[entry_index].opcode = cpu->decode2.opcode;
    cpu->iq[entry_index].src1_rdy_bit = cpu->phys_regs[cpu->decode2->rs1].src_bit;
    cpu->iq_entry[entry_index].src1_tag = cpu->decode2->rs1;
    if(cpu->iq[entry_index].src1_rdy_bit){
        cpu->iq[entry_index].src1_val = cpu->phys_regs[cpu->decode2->rs1];
    }
    cpu->iq[entry_index].src2_rdy_bit = cpu->phys_regs[cpu->decode2->rs2].src_bit;
    cpu->iq_entry[entry_index].src2_tag = cpu->decode2->rs2;
    if(cpu->iq[entry_index].src2_rdy_bit){
        cpu->iq[entry_index].src2_val = cpu->phys_regs[cpu->decode2->rs2];
    }
    cpu->iq[entry_index].dest = cpu->decode2.rd;
    cpu->iq[iq_entry].pc_value = cpu->decode2.pc;

    switch (cpu->decode2.opcode){//Adding to LSQ if it's a MEM instr -J
        case OPCODE_LOAD:
        case OPCODE_LDI:
        case OPCODE_STORE:
        case OPCODE_STI:
            cpu->iq[entry_index].lsq_id = cpu->lsq.size();
            cpu->lsq.push(cpu->iq[entry_index]);
            break;
    }

    cpu->decode2.has_insn = FALSE;
    //We don't forward data in pipeline to exec right away like before bc IQ is Out-of-Order -J
}

static void 
APEX_ISSUE_QUEUE(APEX_CPU *cpu){//Will handle grabbing the correct instructions in the IQ for Exec stage
}


/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void
APEX_execute(APEX_CPU *cpu)
{   
    //Grab instruction from issue queue
    APEX_ISSUE_QUEUE(cpu);

    /*
        Multiplication section
    */
    if(cpu->mult_exec.has_insn){
        if(cpu->mult_exec.stage_delay == 4){
            switch (cpu->mult_exec->opcode){
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
            }
            cpu->mult_exec.stage_delay = 1;
            cpu->mult_wb = cpu->mult_exec;
        }else{
            cpu.mult_exec_delay++;
        }
        cpu->mul_exec.has_insn = FALSE;
    }
    /*
        Integer section
    */
    if(cpu->int_exec.has_insn){
        int mem_instruction = FALSE;
        switch (cpu->mult_exec.opcode){
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

            case OPCODE_LOAD:
            {
                mem_instruction = TRUE;
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                break;
            }

            case OPCODE_STORE:
            {
                mem_instruction = TRUE;
                cpu->execute.memory_address
                    = cpu->execute.rs2_value + cpu->execute.imm;
                break;
            }

            case OPCODE_STI:
            {
                mem_instruction = TRUE;
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                cpu->execute.inc_address_buffer = cpu->execute.rs1_value + 4;
                break;
            }

            case OPCODE_LDI:
            {
                mem_instruction = TRUE;
                cpu->execute.memory_address
                    = cpu->execute.rs1_value + cpu->execute.imm;
                cpu->execute.inc_address_buffer = cpu->execute.rs1_value + 4;
                break;
            }
        }
        if(mem_instruction){
            cpu->memory = cpu->int_exec; //Memory has its own stage
        }else{
            cpu->int_wb = cpu->int_exec;
        }
        cpu->int_exec.has_insn = FALSE;
        
    }
    /*
        Branch section
    */
    if(cpu->branch_exec.has_insn){
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
        cpu->branch_wb = cpu->branch_exec;
        cpu->branch_exec.has_insn = FALSE;
    }


    /*Have to remove this guy, but want to preserve the MEM stuff*/
    /*if (cpu->execute.has_insn)
    {
        // Execute logic based on instruction type
        switch (cpu->execute.opcode)
        {
            



        }



        // Copy data from execute latch to memory latch
        cpu->memory = cpu->execute;
        cpu->execute.has_insn = FALSE;

        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Execute", &cpu->execute);
        }
    }*/
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

    /*
        I think we want to handle ROB commits here
    */
    if(cpu->mult_wb.has_insn){

    }
    if(cpu->int_wb.has_insn){

    }
    if(cpu->branch_wb.has_insn){

    }

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

    /*Initialization Additions*/
    cpu->mult_exec.stage_delay = 1;

    for(i = 0; i < 20; i++){//Setting up free list
        cpu->free_list.push(i);
    }
    for(i = 0; i < 8; i++){
        IQ_Entry iq_entry;
        iq_entry.status_bit = 0;
        iq_entry.src1_rdy_bit = 0;
        iq_entry.src2_rdy_bit = 0;
        cpu->iq.push_back(iq_entry);
    }
    //Don't need to init LSQ or ROB bc they are both proper queues -J

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

        /*if (display_state)
        {
            fprintf(fp,"--------------------------------------------\n");
            fprintf(fp,"Clock Cycle #: %d\n", cpu->clock);
            fprintf(fp,"--------------------------------------------\n");
        }*/

        if (APEX_writeback(cpu))
        {
            /* Halt in writeback stage */
            printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n", cpu->clock, cpu->insn_completed);
            break;
        }
        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode2(cpu);
        APEX_decode1(cpu);
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
/*  Old display code. Not sure if any of it will remain relevant.  
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
*/
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