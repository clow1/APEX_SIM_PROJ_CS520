/*
 * apex_cpu.h
 * Contains APEX cpu pipeline declarations
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_

#include "apex_macros.h"
#include <vector>
#include <queue>

using namespace std;

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
    char opcode_str[128];
    int opcode;
    int rd;
    int rs1;
    int rs2;
    int imm;
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
    int pc;
    char opcode_str[128];
    int opcode;
    int rs1;
    int rs2;
    int rd;
    int imm;
    int rs1_value;
    int rs2_value;
    int result_buffer;
    int memory_address;
    int has_insn;
    int stage_delay; //Counter to delay MUL by four cycles -J
    int status_bit; //To help Decode1 check VFUs (1 for busy, 0 for available) -J
} CPU_Stage;


/*branch predicution unit struct*/

//the new branches = explicitly taken each time
typedef struct Branch_Unit
{
    int bnz_last;
    int bnp_last;
    int bz_last;
    int bp_last;

    vector<int> btb;

    int branch_in_pipe_flag;

} Branch_Unit;

typedef struct IQ_Entry
{
  int status_bit;
  int fu_type; // 0,1,2,3     0 = mult, 1 = int, 2 = branch, ETC;
                              //make a cool enUM
  int literal;
  int src1_rdy_bit;
  int src1_tag;
  int src1_val;

  int src2_rdy_bit;
  int src2_tag;
  int src2_val;

  int dest;
  int lsq_id;

}IQ_Entry;


//REORDER BUFFER WITH 16 ENTRIES

typedef struct ROB_Entry
{
    int pc_value;
    int ar_addr;
    int result;

    int sval;
    int sval_valid;

    int excodes;
    int status_bit;
    int itype;
}ROB_Entry;

typedef struct Rename_Entry
{
    int id; //can del
    int src_bit;
    int phys_reg_idx;
} Rename_Entry;

typedef struct RF_Entry
{
    int value;  //whatever supposed 2 be stored in the RF
    int cc; //2 bit extension;

} RF_Entry;


/* Model of APEX CPU */
typedef struct APEX_CPU
{
    int pc;                        /* Current program counter */
    int clock;                     /* Clock cycles elapsed */
    int insn_completed;            /* Instructions retired */
    int arch_regs[REG_FILE_SIZE];       /* Integer register file */
    int phys_regs[20];
    int code_memory_size;          /* Number of instruction in the input file */
    APEX_Instruction *code_memory; /* Code Memory */
    int data_memory[DATA_MEMORY_SIZE]; /* Data Memory */
    int single_step;               /* Wait for user input after every cycle */
    int zero_flag;                 /* {TRUE, FALSE} Used by BZ and BNZ to branch */
    int fetch_from_next_cycle;

    /* Pipeline stages */
    CPU_Stage fetch;
    CPU_Stage decode1;
    CPU_Stage decode2;
    CPU_Stage mult_exec;   //MULTIPLICATION UNIT
    CPU_Stage int_exec;    //INTEGER UNIT
    CPU_Stage branch_exec; //BRANCH UNIT EXECUTION
    CPU_Stage memory;

    CPU_Stage mult_wb;
    CPU_Stage int_wb;
    CPU_Stage branch_wb;

    Branch_Unit branch_predictor;
    int rename_table[REG_FILE_SIZE+1];  /*last element in CC is the
                                        most recently allocated phys. reg*/

  //earlier dispatch instruction = tie breaker
    vector<IQ_Entry> iq; //8 entries; use a vector so we can remove out of order
                        //Can check business of FUs by has_insn

    queue<int> free_list; //nums 0-19 for the # reg

    queue<ROB_Entry> rob; /*check the size whenever
                            we need to add to this queue
                            maximum size 16 entries */

    queue<IQ_Entry> lsq; /*LSQ entry has the same
                          structure as an IQ entry.
                          use queue because in order*/
} APEX_CPU;

/*functional unit struct*/





APEX_Instruction *create_code_memory(const char *filename, int *size);
APEX_CPU *APEX_cpu_init(const char *filename);
void APEX_cpu_run(APEX_CPU *cpu);
void APEX_cpu_stop(APEX_CPU *cpu);
#endif
