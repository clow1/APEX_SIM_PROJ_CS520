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
        switch(cpu->decode1.vfu){//Checking VFUs -J
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
                    if(cpu->lsq.size() == 6){ //LOAD needs both INT_VFU and MEM Unit -J
                        cpu->fetch.has_insn = FALSE;
                        return;
                    }
                    memory_op = TRUE;
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
            //<dest> <- #<literal> -J
            case OPCODE_MOVC:
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
            //<branch> <src1> #<literal> -J
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
    switch(cpu->decode2.opcode){//Instructions w/ literals -J
        case OPCODE_LOAD:
        case OPCODE_LDI:
        case OPCODE_STORE:
        case OPCODE_STI:
        case OPCODE_JUMP:
        case OPCODE_MOVC:
        case OPCODE_BP:
        case OPCODE_BNP:
        case OPCODE_BZ:
        case OPCODE_BNZ:
            cpu->iq[iq_entry].literal = cpu->decode2.imm;
    }

    switch(cpu->decode2.opcode){//Instructions w/ src1 -J
        case OPCODE_ADD:
        case OPCODE_ADDL:
        case OPCODE_SUB:
        case OPCODE_SUBL:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_EXOR:
        case OPCODE_LOAD:
        case OPCODE_LDI:
        case OPCODE_STORE:
        case OPCODE_STI:
        case OPCODE_JUMP:
        case OPCODE_CMP:
            cpu->iq[entry_index].src1_rdy_bit = cpu->phys_regs[cpu->decode2->rs1].src_bit;
            cpu->iq_entry[entry_index].src1_tag = cpu->decode2->rs1;
            if(cpu->iq[entry_index].src1_rdy_bit){
                cpu->iq[entry_index].src1_val = cpu->phys_regs[cpu->decode2->rs1];
            }
            break;
    }
    switch (cpu->decode2.opcode){//Instructions w/ src2 -J
        case OPCODE_ADD:
        case OPCODE_ADDL:
        case OPCODE_SUB:
        case OPCODE_SUBL:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_EXOR:
        case OPCODE_STORE:
        case OPCODE_STI:
        case OPCODE_CMP:
            cpu->iq[entry_index].src2_rdy_bit = cpu->phys_regs[cpu->decode2->rs2].src_bit;
            cpu->iq_entry[entry_index].src2_tag = cpu->decode2->rs2;
            if(cpu->iq[entry_index].src2_rdy_bit){
                cpu->iq[entry_index].src2_val = cpu->phys_regs[cpu->decode2->rs2];
            }
            break;
    }
    switch (cpu->decode2.opcode){ //Instructions w/ dest -J
        case OPCODE_ADD:
        case OPCODE_ADDL:
        case OPCODE_SUB:
        case OPCODE_SUBL:
        case OPCODE_MUL:
        case OPCODE_AND:
        case OPCODE_OR:
        case OPCODE_EXOR:
        case OPCODE_MOVC:
        case OPCODE_LOAD:
        case OPCODE_LDI:
            cpu->iq[entry_index].dest = cpu->decode2.rd;
            break;

    }
    cpu->iq[iq_entry].pc_value = cpu->decode2.pc;

    switch (cpu->decode2.opcode){//Adding to LSQ if it's a MEM instr -J
        case OPCODE_LOAD:
        case OPCODE_LDI:
        case OPCODE_STORE:
        case OPCODE_STI:
            if(cpu->lsq.empty()){
                cpu->iq[entry_index].lsq_id = 0;
            }else{
                cpu->iq[entry_index].lsq_id = lsq.front().lsq_id + 1; //Give unique id to new entry -J
            }
            cpu->lsq.push(cpu->iq[entry_index]);
            break;
    }

    cpu->decode2.has_insn = FALSE;
    //We don't forward data in pipeline to exec right away like before bc IQ is Out-of-Order -J
}

static int check_LSQ(APEX_CPU* cpu, int entry_index){
    //Returns 100 if LSQ index cannot be used yet
    switch (cpu->iq[entry_index]){
        /*
        Rules for LOAD
        1) Entry for LOAD is at the head of LSQ
        2) All fields are valid (checked prior to tiebreaker_IQ())
        3) Memory is free
        */
        case OPCODE_LOAD:
        case OPCODE_LDI:
            if(!cpu->memory.has_insn && 
                cpu->iq[entry_index].pc_value == cpu->lsq.front().pc_value){
                return a;
            }
            break;
        /*
        Rules for STORE
        1) All inputs are valid (checked prior to tiebreaker_IQ())
        2) When STORE entry is at the head of the ROB and LSQ 
        3) When memory is free
        */
        case OPCODE_STORE:
        case OPCODE_STI:
            if(!cpu->memory.has_insn && 
                cpu->iq[entry_index].pc_value == cpu->lsq.front().pc_value &&
                cpu->iq[entry_index].pc_value == cpu->rob.pc_value){
                return a;
            }
            break; 
    }

    return 100;
}


static int tiebreaker_IQ(APEX_CPU* cpu, int a, int b){//The lower PC value is the earlier instruction (which in case of tie is issued first) -J
    //If MEM operation, check the LSQ
    if(a != 100 && cpu->iq[a].lsq_id != -1){
        a = check_LSQ(cpu, a);
    }
    if(b != 100 && cpu->iq[b].lsq_id != -1){
        b = check_LSQ(cpu, b);
    }
    return (cpu->iq[a].pc_value < cpu->iq[b].pc_value) ? a : b;
}

static void 
APEX_ISSUE_QUEUE(APEX_CPU *cpu){//Will handle grabbing the correct instructions in the IQ for Exec stage -J
    //I want to make it so there's a comparison between entry 
    int entry_index = 100;
    for(int i = 0; i < 8; i++){
        if(cpu->iq[i].status_bit == 1){//Now check and see if the src_bits are valid (but diff instr wait on diff srcs) -J
            switch(cpu->iq[i].opcode){
                //First look at instr w/ src1 & src2
                case OPCODE_ADD:
                case OPCODE_ADDL:
                case OPCODE_SUB:
                case OPCODE_SUBL:
                case OPCODE_MUL:
                case OPCODE_AND:
                case OPCODE_OR:
                case OPCODE_EXOR:
                case OPCODE_STORE:
                case OPCODE_STI:
                case OPCODE_CMP:
                    if(cpu->iq[i].src1_rdy_bit && iq[i].src2_rdy_bit){
                        entry_index = tiebreaker_IQ(entry_index, i);                        
                    }
                    break;
                //Look at instr with only src1
                case OPCODE_LOAD:
                case OPCODE_LDI:
                case OPCODE_JUMP:
                    if(cpu->iq[i].src1_rdy_bit){
                        entry_index = tiebreaker_IQ(entry_index, i);
                    }
                    break;
                //Look at instr with only literals
                case OPCODE_MOVC:
                case OPCODE_BP:
                case OPCODE_BNP:
                case OPCODE_BZ:
                case OPCODE_BNZ:
                case OPCODE_NOP:
                    entry_index = tiebreaker_IQ(entry_index, i);
                    break;
            }

        }
    }
    if(entry_index == 100){
        //Nothing is ready, so we just don't fill in anything for the EX stage
    }else{
        //We have a valid instruction to issue
        cpu->iq[entry_index].status_bit = 0;
        IQ_Entry issuing_instr = cpu->iq[entry_index];
        if(cpu->iq[entry_index].lsq_id != -1){//If we grabbed an MEM op, make sure to adjust LSQ -J
            cpu->lsq.pop();
            cpu->iq[entry_index].lsq_id = -1;//Reset lsq_id field for later checks -J
        }
        switch (cpu->iq[entry_index].fu_type){
            case MUL_VFU:
                cpu->mul_exec.pc = issuing_instr.pc_value;
                cpu->mul_exec.opcode = issuing_instr.opcode;
                cpu->mul_exec.rs1 = issuing_instr.src1_tag;
                cpu->mul_exec.rs2 = issuing_instr.src2_val;
                cpu->mul_exec.rd = issuing_instr.dest;
                cpu->mul_exec.rs1_value = issuing_instr.src1_val;
                cpu->mul_exec.rs2_value = issuing_instr.src2_val;
                cpu->mul_exec.has_insn = TRUE;
                cpu->mul_exec.stage_delay = 1;
                cpu->mul_exec.vfu = MUL_VFU; 
            case INT_VFU:
                cpu->int_exec.pc = issuing_instr.pc_value;
                cpu->int_exec.opcode = issuing_instr.opcode;
                cpu->int_exec.has_insn = TRUE;
                cpu->int_exec.vfu = INT_VFU;
                switch(issuing_instr.opcode){//Break down INT ops based on instruction syntax -J
                    //dest src1 src2 -J
                    case OPCODE_ADD:
                    case OPCODE_ADDL:
                    case OPCODE_SUB:
                    case OPCODE_SUBL:
                    case OPCODE_AND:
                    case OPCODE_OR:
                    case OPCODE_EXOR:
                        cpu->int_exec.rs1 = issuing_instr.src1_tag;
                        cpu->int_exec.rs2 = issuing_instr.src2_tag;
                        cpu->int_exec.rd = issuing_instr.dest;
                        cpu->int_exec.rs1_value = issuing_instr.src1_val;
                        cpu->int_exec.rs2_value = issuing_instr.src2_val;
                        break;
                    //dest src1 literal -J
                    case OPCODE_LOAD:
                    case OPCODE_LDI:
                        cpu->int_exec.rs1 = issuing_instr.src1_tag;
                        cpu->int_exec.rd = issuing_instr.dest;
                        cpu->int_exec.imm = issuing_instr.literal;
                        cpu->int_exec.rs1_value = issuing_instr.src1_val;
                        break;
                    //src1 src2 literal -J
                    case OPCODE_STI:
                    case OPCODE_STORE:
                        cpu->int_exec.rs1 = issuing_instr.src1_tag;
                        cpu->int_exec.rs2 = issuing_instr.src2_tag;
                        cpu->int_exec.imm = issuing_instr.literal;
                        cpu->int_exec.rs1_value = issuing_instr.src1_val;
                        cpu->int_exec.rs2_value = issuing_instr.src2_val;
                        break;
                    //dest literal -J
                    case OPCODE_MOVC:
                        cpu->int_exec.rd = issuing_instr.dest;
                        cpu->int_exec.imm = issuing_instr.literal;
                        break;
                    //Nothing -J
                    case OPCODE_NOP:

                }
            case BRANCH_VFU: 
                cpu->branch_exec.pc = issuing_instr.pc_value
                cpu->branch_exec.opcode = issuing_instr.opcode;
                cpu->branch_exec.has_insn = TRUE;
                cpu->branch_exec.vfu = BRANCH_VFU;
                switch(issuing_instr.opcode){
                    //Only literal -J
                    case OPCODE_BP:
                    case OPCODE_BNP:
                    case OPCODE_BZ:
                    case OPCODE_BNZ:
                        cpu->branch_exec.imm = issuing_instr.literal;
                        break;
                    //src1 literal -J
                    case OPCODE_JUMP:
                        cpu->branch_exec.rs1 = issuing_instr.src1_tag;
                        cpu->branch_exec.imm = issuing_instr.literal;
                        cpu->branch_exec.rs1_value = issuing_instr.src1_val;
                        break;
                }
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
                        cpu->mult_exec.result_buffer
                            = cpu->mult_exec.rs1_value * cpu->mult_exec.rs2_value;

                        /* Set the zero flag based on the result buffer */
                        if (cpu->mult_exec.result_buffer == 0)
                        {
                            cpu->zero_flag = TRUE;
                        } 
                        else 
                        {
                            cpu->zero_flag = FALSE;
                        }
                        if(cpu->mult_exec.result_buffer > 0){
                            cpu->positive_flag = TRUE;
                        }else{
                            cpu->positive_flag = FALSE;
                        }
                        break;
                }
            }
            cpu->mult_exec.stage_delay = 1;
            cpu->mult_wb = cpu->mult_exec;
            cpu->mul_exec.has_insn = FALSE;
        }else{
            cpu->mult_exec.stage_delay++;
        }
    }
    /*
        Integer section
    */
    if(cpu->int_exec.has_insn){
        int mem_instruction = FALSE;
        switch (cpu->mult_exec.opcode){
            case OPCODE_ADD:
            {
                cpu->int_exec.result_buffer
                    = cpu->int_exec.rs1_value + cpu->int_exec.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }
            case OPCODE_ADDL:
            {
                cpu->int_exec.result_buffer
                    = cpu->int_exec.rs1_value + cpu->int_exec.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUB:
            {
                cpu->int_exec.result_buffer
                    = cpu->int_exec.rs1_value - cpu->int_exec.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_SUBL:
            {
                cpu->int_exec.result_buffer
                    = cpu->int_exec.rs1_value - cpu->int_exec.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_AND:
            {
                cpu->int_exec.result_buffer
                    = cpu->int_exec.rs1_value & cpu->int_exec.rs2_value;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }

            case OPCODE_OR:
            {
                    cpu->int_exec.result_buffer
                        = cpu->int_exec.rs1_value | cpu->int_exec.rs2_value;

                    /* Set the zero flag based on the result buffer */
                    if (cpu->int_exec.result_buffer == 0)
                    {
                        cpu->zero_flag = TRUE;
                    } 
                    else 
                    {
                        cpu->zero_flag = FALSE;
                    }
                    if(cpu->int_exec.result_buffer > 0){
                        cpu->positive_flag = TRUE;
                    }else{
                        cpu->positive_flag = FALSE;
                    }
                    break;
            } 

            case OPCODE_EXOR:
            {
                    cpu->int_exec.result_buffer
                        = cpu->int_exec.rs1_value ^ cpu->int_exec.rs2_value;

                    /* Set the zero flag based on the result buffer */
                    if (cpu->int_exec.result_buffer == 0)
                    {
                        cpu->zero_flag = TRUE;
                    } 
                    else 
                    {
                        cpu->zero_flag = FALSE;
                    }
                    if(cpu->int_exec.result_buffer > 0){
                        cpu->positive_flag = TRUE;
                    }else{
                        cpu->positive_flag = FALSE;
                    }
                    break;
            }
            case OPCODE_MOVC: 
            {
                cpu->int_exec.result_buffer = cpu->int_exec.imm;

                /* Set the zero flag based on the result buffer */
                if (cpu->int_exec.result_buffer == 0)
                {
                    cpu->zero_flag = TRUE;
                } 
                else 
                {
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.result_buffer > 0){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
                break;
            }
            case OPCODE_CMP:
            {
                if(cpu->int_exec.rs1_value == cpu->int_exec.rs2_value){
                    cpu->zero_flag = TRUE;
                }else{
                    cpu->zero_flag = FALSE;
                }
                if(cpu->int_exec.rs1_value > cpu->int_exec.rs2_value){
                    cpu->positive_flag = TRUE;
                }else{
                    cpu->positive_flag = FALSE;
                }
            }

            case OPCODE_LOAD:
            {
                mem_instruction = TRUE;
                cpu->int_exec.memory_address
                    = cpu->int_exec.rs1_value + cpu->int_exec.imm;
                break;
            }

            case OPCODE_STORE:
            {
                mem_instruction = TRUE;
                cpu->int_exec.memory_address
                    = cpu->int_exec.rs2_value + cpu->int_exec.imm;
                break;
            }

            case OPCODE_STI:
            {
                mem_instruction = TRUE;
                cpu->int_exec.memory_address
                    = cpu->int_exec.rs1_value + cpu->int_exec.imm;
                cpu->int_exec.inc_address_buffer = cpu->int_exec.rs1_value + 4;
                break;
            }

            case OPCODE_LDI:
            {
                mem_instruction = TRUE;
                cpu->int_exec.memory_address
                    = cpu->int_exec.rs1_value + cpu->int_exec.imm;
                cpu->int_exec.inc_address_buffer = cpu->int_exec.rs1_value + 4;
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
        //Whoever does branch prediction will have to edit this portion significantly -J
        case OPCODE_BZ:
            {
                if (cpu->zero_flag == TRUE)
                {
                    /* Calculate new PC, and send it to fetch unit */
                    cpu->result_buffer = cpu->branch_exec.pc + cpu->branch_exec.imm;
                    
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
                    cpu->result_buffer = cpu->branch_exec.pc + cpu->branch_exec.imm;
                    
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
                    cpu->result_buffer = cpu->branch_exec.pc + cpu->branch_exec.imm;
                    
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
                    cpu->result_buffer = cpu->branch_exec.pc + cpu->branch_exec.imm;
                    
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
                    cpu->result_buffer = cpu->branch_exec.rs1_value + cpu->branch_exec.imm;
                    
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
            case OPCODE_LDI:
            case OPCODE_LOAD:
            {
                /* Read from data memory */
                cpu->memory.result_buffer
                    = cpu->data_memory[cpu->memory.memory_address];
                cpu->mem_wb = cpu->memory;
                cpu->memory.has_insn = FALSE;
                break;
            }

            case OPCODE_STORE:
            {
                /*Write data into memory*/
                cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs1_value;
                cpu->commitment = cpu->memory;
                cpu->memory.has_insn = FALSE; //Last stop for a STORE, goes straight to commitment -J
                break;
            }

            case OPCODE_STI:
            {
                /*Write data into memory*/
                cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs2_value;
                cpu->mem_wb = cpu->memory;
                cpu->memory.has_insn = FALSE;                
                break;
            }
        }

        /*
        if (ENABLE_DEBUG_MESSAGES || display_state)
        {
            print_stage_content("Memory", &cpu->memory);
        }*/
    }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void 
APEX_forward(APEX_CPU* cpu, CPU_Stage forward){//This is where we'll forward the data to all relevant data structures -J
    //Only forward instr that has a dest reg -J
    switch(forward.opcode){
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
            //Loop through iq and match rd to rs1 or rs2 and fill in and set ready bit -J
            for(int i = 0; i < 8; i++){
                if(cpu->iq[i].status_bit == 1){
                    if(cpu->iq[i].src1_tag == forward.rd){
                        cpu->iq[i].src1_val = forward.result_buffer; 
                        cpu->iq[i].src1_rdy_bit = 1;
                    }
                    if(cpu->iq[i].src2_tag == forward.rd){
                        cpu->iq[i].src2_val = forward.result_buffer; 
                        cpu->iq[i].src2_rdy_bit = 1; 
                    }

                }
            }
            break;
        //LDI has to forward 2 values, rd & src1 -J
        case OPCODE_LDI:
            for(int i = 0; i < 8; i++){
                if(cpu->iq[i].status_bit == 1){
                    if(cpu->iq[i].src1_tag == forward.rd){
                        cpu->iq[i].src1_val = forward.result_buffer; 
                        cpu->iq[i].src1_rdy_bit = 1;
                    }
                    if(cpu->iq[i].src2_tag == forward.rd){
                        cpu->iq[i].src2_val = forward.result_buffer; 
                        cpu->iq[i].src2_rdy_bit = 1; 
                    }
                    if(cpu->iq[i].src1_tag == forward.rs1){
                        cpu->iq[i].src1_val = forward.inc_address_buffer; 
                        cpu->iq[i].src1_rdy_bit = 1;
                    }
                    if(cpu->iq[i].src2_tag == forward.rs1){
                        cpu->iq[i].src2_val = forward.inc_address_buffer; 
                        cpu->iq[i].src2_rdy_bit = 1; 
                    }

                }
            }
        //STI only forwards src1 -J
        case OPCODE_STI:
            for(int i = 0; i < 8; i++){
                if(cpu->iq[i].status_bit == 1){
                    if(cpu->iq[i].src1_tag == forward.rs1){
                        cpu->iq[i].src1_val = forward.inc_address_buffer; 
                        cpu->iq[i].src1_rdy_bit = 1;
                    }
                    if(cpu->iq[i].src2_tag == forward.rs1){
                        cpu->iq[i].src2_val = forward.inc_address_buffer; 
                        cpu->iq[i].src2_rdy_bit = 1; 
                    }

                }
            }
            break;
    }
}
static int
APEX_writeback(APEX_CPU *cpu)
{

    //Handling forwarding write backs -J
    if(cpu->mult_wb.has_insn){
        APEX_forward(cpu->mult_wb);
        mult_wb.has_insn = FALSE;
    }
    if(cpu->int_wb.has_insn){
        APEX_forward(cpu->int_wb);
        int_wb.has_insn = FALSE;
    }
    if(cpu->mem_wb.has_insn){
        APEX_forward(cpu->branch_wb);
        mem_wb.has_insn = FALSE;
    }
    if(cpu->branch_wb.has_insn){
        //This will have to be modified when BTB is added -J
        if(cpu->branch_wb.opcode == OPCODE_HALT){
            return TRUE;
        }else{
            APEX_forward(cpu->branch_wb);
            branch_wb.has_insn = FALSE;            
        }
    }

    /* Default */
    return 0;
}

static void
APEX_commitment(APEX_CPU* cpu){
    cpu->insn_completed++;

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
    cpu->memory.stage_delay = 1;

    //Make sure has_insn is False for all initial vfu checks
    cpu->mult_exec.has_insn = FALSE;
    cpu->int_exec.has_insn = FALSE;
    cpu->branch_exec.has_insn = FALSE;

    for(i = 0; i < 20; i++){//Setting up free list
        cpu->free_list.push(i);
    }
    for(i = 0; i < 8; i++){
        IQ_Entry iq_entry;
        iq_entry.status_bit = 0;
        iq_entry.src1_rdy_bit = 0;
        iq_entry.src2_rdy_bit = 0;
        iq_entry.lsq_id = -1; //Lets us check later on if the IQ entry has corresponding LSQ entry (if -1, then it doesn't) -J
        cpu->iq.[i] = iq_entry;
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
        APEX_commitment(cpu);
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