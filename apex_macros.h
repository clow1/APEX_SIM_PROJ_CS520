/*
 * apex_macros.h
 * Contains APEX cpu pipeline macros
 *
 * Author:
 * Copyright (c) 2020, Gaurav Kothari (gkothar1@binghamton.edu)
 * State University of New York at Binghamton
 */
#ifndef _MACROS_H_
#define _MACROS_H_

#define FALSE 0x0
#define TRUE 0x1

/* Integers */
#define DATA_MEMORY_SIZE 4096

/* Size of integer register file */
#define REG_FILE_SIZE 16
#define PHYS_REG_FILE_SIZE 20
/* Numeric OPCODE identifiers for instructions */
#define OPCODE_ADD 0x0
#define OPCODE_SUB 0x1
#define OPCODE_MUL 0x2
#define OPCODE_DIV 0x3
#define OPCODE_AND 0x4
#define OPCODE_OR 0x5
#define OPCODE_EXOR 0x6 /*I changed the name from XOR to fit the spec better*/
#define OPCODE_MOVC 0x7
#define OPCODE_LOAD 0x8
#define OPCODE_STORE 0x9
#define OPCODE_BZ 0xa
#define OPCODE_BNZ 0xb
#define OPCODE_HALT 0xc
/*Expanded Instructions*/
#define OPCODE_BP 0xd
#define OPCODE_BNP 0xe
#define OPCODE_ADDL 0xf
#define OPCODE_NOP 0x10
#define OPCODE_SUBL 0x11
#define OPCODE_CMP 0x12
#define OPCODE_JUMP 0X13

/*other expanded instructions*/
#define OPCODE_JALR 0x14
#define OPCODE_RET 0x15

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/* Set this flag to 1 to enable cycle single-step mode */
#define ENABLE_SINGLE_STEP 0


/*VFU Macros will make it easier to see where certain instructions are going -J*/
#define MUL_VFU 0
#define INT_VFU 1
#define BRANCH_VFU 2

#define CC_INDEX 16

/*Clear cut macros of any field that represents src or status bit in IQ or ROB entries -C*/
#define VALID 1
#define INVALID 0

/*Clear cut macros of the rdy bit -C */
#define NOT_READY 0
#define READY 1

/* ITYPES */
#define MEMORY_ITYPE 0
#define ARITHMETIC_ITYPE 1
#define CONTROL_FLOW_ITYPE 2

#endif
