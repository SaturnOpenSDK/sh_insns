#include "build_instructions.h"

#include <list>
#include <string>
#include <algorithm>
#include <regex>

using namespace std::literals::string_literals;

constexpr bool operator ==(const environment_t& a, const environment_t& b)
  { return a.cpus == b.cpus && a.property == b.property; }


std::list<insns> build_insn_blocks (void)
{

  struct instruction_info_t
  {
    std::string mnemonic;
    std::string name;
    std::string classification;
    std::list<environment_t> environments;
    std::list<citation_t> citations;
  };

  std::list<instruction_info_t> name_data =
  {
    { "STS", "Store System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 231 }, { SH4A_DOC, 425 } } },
    { "STS", "Store from FPU System Register", "System Control Instruction", {}, { { SH4A_DOC, 453 } } },

    { "FMOV", "Floating-point Move", "Floating-Point Instruction", {}, { { SH4A_DOC, 497 } } },
    { "FMOV", "Floating-point Move Extension", "Floating-Point Instruction", {}, { { SH4A_DOC, 501 } } },

    { "LDC", "Load to Control Register", "System Control Instruction", {}, { { SH4A_DOC, 337 } } },
    { "LDC", "Load to Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 165 }, { SH4A_DOC, 449 } } },

    { "LDS", "Load to System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 172 }, { SH4A_DOC, 342 } } },
    { "LDS", "Load to FPU System register", "System Control Instruction", {}, { { SH4A_DOC, 450 } } },

    { "BRAF", "Branch Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 133 }, { SH4A_DOC, 307 } } },
    { "BRA", "Branch", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 131 }, { SH4A_DOC, 305 } } },
    { "BSRF", "Branch to Subroutine Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 137 }, { SH4A_DOC, 445 } } },
    { "BSR", "Branch to Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 135 }, { SH4A_DOC, 443 } } },
    { "JMP", "Jump", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 162 }, { SH4A_DOC, 336 } } },
    { "JSR", "Jump to Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 163 }, { SH4A_DOC, 447 } } },
    { "RTE", "Return from Exception", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" }, { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 212 }, { SH4A_DOC, 401 } } },
    { "RTS", "Return from Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 214 }, { SH4A_DOC, 403 } } },
    { "STC", "Store Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 228 }, { SH4A_DOC, 420 }, { SH4A_DOC, 452 } } },
    { "SLEEP", "Sleep", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 227 }, { SH4A_DOC, 419 } } },
    { "LDTLB", "Load PTEH/PTEL to TLB", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH4A_DOC, 344 } } },

    { "MOV", "Move Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 183 }, { SH4A_DOC, 353 } } },
    { "MOV", "Move Constant Value", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 189 }, { SH4A_DOC, 359 } } },
    { "MOV", "Move Global Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 191 }, { SH4A_DOC, 362 } } },
    { "MOV", "Move Structure Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 194 }, { SH4A_DOC, 365 } } },
    { "ADDC", "Add with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 124 }, { SH4A_DOC, 296 } } },
    { "ADDV", "ADD with `V Flag` Overflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 125 }, { SH4A_DOC, 297 } } },
    { "ADD", "Add binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 123 }, { SH4A_DOC, 294 } } },
    { "AND", "AND Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 126 }, { SH4A_DOC, 299 } } },
    { "BF/S", "Branch if False with Delay Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 129 }, { SH4A_DOC, 303 } } },
    { "BF", "Branch if False", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 128 }, { SH4A_DOC, 301 } } },
    { "BT/S", "Branch if True with Delay Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 140 }, { SH4A_DOC, 310 } } },
    { "BT", "Branch if True", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 139 }, { SH4A_DOC, 308 } } },
    { "CLRMAC", "Clear MAC Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 142 }, { SH4A_DOC, 312 } } },
    { "CLRS", "Clear S Bit", "System Control Instruction", {}, { { SH4A_DOC, 313 } } },
    { "CLRT", "Clear T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 143 }, { SH4A_DOC, 314 } } },
    { "CMP/EQ", "Compare If Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/GE", "Compare If Signed Greater Than or Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/GT", "Compare If Signed Greater Than", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/HI", "Compare If Unsigned Greater Than", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/HS", "Compare If Unsigned Greater Than or Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/PL", "Compare If Signed Greater Than Zero", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/PZ", "Compare If Signed Greater Than or Equal To Zero", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/STR", "Compare If Strings Equal", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "DIV0S", "Divide `Step 0` as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 148 }, { SH4A_DOC, 319 } } },
    { "DIV0U", "Divide `Step 0` as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 149 }, { SH4A_DOC, 320 } } },
    { "DIV1", "Divide 1 Step", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 150 }, { SH4A_DOC, 321 } } },
    { "DMULS\\.L", "Double-length Multiply as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 155 }, { SH4A_DOC, 326 } } },
    { "DMULU\\.L", "Double-length Multiply as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 157 }, { SH4A_DOC, 328 } } },
    { "DT", "Decrement and Test", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 159 }, { SH4A_DOC, 330 } } },
    { "EXTS", "Extend as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 160 }, { SH4A_DOC, 331 } } },
    { "EXTU", "Extend as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 161 }, { SH4A_DOC, 333 } } },
    { "ICBI", "Instruction Cache Block Invalidate", "Data Transfer Instruction", {}, { { SH4A_DOC, 335 } } },
    { "LDRE", "Load Effective Address to RE Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 168 } } },
    { "LDRS", "Load Effective Address to RS Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 170 } } },
    { "MAC\\.L", "Multiply and Accumulate Long", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 177 }, { SH4A_DOC, 346 } } },
    { "MAC\\.W", "Multiply and Accumulate Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 180 }, { SH4A_DOC, 350 } } },
    { "MOVA", "Move Effective Address", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 197 }, { SH4A_DOC, 369 } } },
    { "MOVT", "Move T Bit", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 198 }, { SH4A_DOC, 376 } } },
    { "MUL\\.L", "Multiply Long", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 199 }, { SH4A_DOC, 379 } } },
    { "MULS\\.W", "Multiply as Signed Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 200 }, { SH4A_DOC, 380 } } },
    { "MULU\\.W", "Multiply as Unsigned Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 201 }, { SH4A_DOC, 381 } } },
    { "MOVCA\\.L", "Move with Cache Block Allocation", "Data Transfer Instruction", {}, { { SH4A_DOC, 371 } } },
    { "MOVCO", "Move Conditional", "Data Transfer Instruction", {}, { { SH4A_DOC, 372 } } },
    { "MOVLI", "Move Linked", "Data Transfer Instruction", {}, { { SH4A_DOC, 374 } } },
    { "MOVUA", "Move Unaligned", "Data Transfer Instruction", {}, { { SH4A_DOC, 377 } } },
    { "NEGC", "Negate with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 203 }, { SH4A_DOC, 383 } } },
    { "NEG", "Negate", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 202 }, { SH4A_DOC, 382 } } },
    { "NOP", "No Operation", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 204 }, { SH4A_DOC, 384 } } },
    { "NOT", "NOT-Logical Complement", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 205 }, { SH4A_DOC, 385 } } },
    { "OR", "OR Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 206 }, { SH4A_DOC, 389 } } },
    { "PREFI", "Prefetch Instruction Cache Block", "Data Transfer Instruction", {}, { { SH4A_DOC, 394 } } },
    { "PREF", "Prefetch Data to Cache", "Data Transfer Instruction", {}, { { SH4A_DOC, 391 } } },
    { "ROTCL", "Rotate with Carry Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 208 }, { SH4A_DOC, 397 } } },
    { "ROTCR", "Rotate with Carry Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 209 }, { SH4A_DOC, 398 } } },
    { "ROTL", "Rotate Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 210 }, { SH4A_DOC, 399 } } },
    { "ROTR", "Rotate Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 211 }, { SH4A_DOC, 400 } } },
    { "SETRC", "Set Repeat Count to RC", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 216 } } },
    { "SETS", "Set S Bit", "System Control Instruction", {}, { { SH4A_DOC, 405 } } },
    { "SETT", "Set T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 218 }, { SH4A_DOC, 406 } } },
    { "SHAD", "Shift Arithmetic Dynamically", "Shift Instruction", {}, { { SH4A_DOC, 407 } } },
    { "SHAL", "Shift Arithmetic Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 219 }, { SH4A_DOC, 409 } } },
    { "SHAR", "Shift Arithmetic Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 220 }, { SH4A_DOC, 410 } } },
    { "SHLD", "Shift Logical Dynamically", "Shift Instruction", {}, { { SH4A_DOC, 411 } } },
    { "SHLL([281][6]?)", "Shift Logical Left \\1 Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 222 }, { SH4A_DOC, 414 } } },
    { "SHLL", "Shift Logical Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 221 }, { SH4A_DOC, 413 } } },
    { "SHLR([281][6]?)", "Shift Logical Right \\1 Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 225 }, { SH4A_DOC, 417 } } },
    { "SHLR", "Shift Logical Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 224 }, { SH4A_DOC, 416 } } },
    { "SUBC", "Subtract with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 237 }, { SH4A_DOC, 428 } } },
    { "SUBV", "Subtract with `V Flag` Underflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 238 }, { SH4A_DOC, 429 } } },
    { "SUB", "Subtract Binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 236 }, { SH4A_DOC, 427 } } },
    { "SWAP", "Swap Register Halves", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 239 }, { SH4A_DOC, 431 } } },
    { "SYNCO", "Synchronize Data Operation", "Data Transfer Instruction", {}, { { SH4A_DOC, 433 } } },
    { "TAS", "Test and Set", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 241 }, { SH4A_DOC, 434 } } },
    { "TRAPA", "Trap Always", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 242 }, { SH4A_DOC, 436 } } },
    { "TST", "Test Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 243 }, { SH4A_DOC, 438 } } },
    { "XOR", "Exclusive OR Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 245 }, { SH4A_DOC, 440 } } },
    { "XTRCT", "Extract", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 247 }, { SH4A_DOC, 442 } } },
    { "MOVS", "Move Single Data between Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 255 } } },
    { "MOVX", "Move between X Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 257 } } },
    { "MOVY", "Move between Y Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 258 } } },
    { "NOPX", "No Access Operation for X Memory", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 260 } } },
    { "PABS", "Absolute", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 278 } } },
    { "DC[TF] PADD", "Addition with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 282 } } },
    { "PADDC", "Addition with Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 291 } } },
    { "PADD PMULS", "Addition & Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 286 } } },
    { "DC[TF] PAND", "Logical AND", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 294 } } },
    { "DC[TF] PCLR", "Clear", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 298 } } },
    { "PCMP", "Compare Two Data", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 301 } } },
    { "DC[TF] PCOPY", "Copy with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 303 } } },
    { "DC[TF] PDEC", "Decrement by 1", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 307 } } },
    { "DC[TF] PDMSB", "Detect MSB with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 312 } } },
    { "DC[TF] PINC", "Increment by 1 with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 317 } } },
    { "DC[TF] PLDS", "Load System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 322 } } },
    { "PMULS", "Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 326 } } },
    { "DC[TF] PNEG", "Negate", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 329 } } },
    { "DC[TF] POR", "Logical OR", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 334 } } },
    { "PRND", "Rounding", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 338 } } },
    { "DC[TF] PSHA", "Shift Arithmetically with Condition", "DSP Arithmetic Shift Instruction", {}, { { SH1_2_DSP_DOC, 342 } } },
    { "DC[TF] PSHL", "Shift Logically with Condition", "DSP Logical Shift Instruction", {}, { { SH1_2_DSP_DOC, 350 } } },
    { "DC[TF] PSTS", "Store System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 357 } } },
    { "DC[TF] PSUB", "Subtract with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 362 } } },
    { "PSUB PMULS", "Subtraction & Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 367 } } },
    { "PSUBC", "Subtraction with Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 372 } } },
    { "DC[TF] PXOR", "Logical Exclusive OR", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 375 } } },
    { "OCBI", "Operand Cache Block Invalidate", "Data Transfer Instruction", {}, { { SH4A_DOC, 386 } } },
    { "OCBP", "Operand Cache Block Purge", "Data Transfer Instruction", {}, { { SH4A_DOC, 387 } } },
    { "OCBWB", "Operand Cache Block Write Back", "Data Transfer Instruction", {}, { { SH4A_DOC, 388 } } },
    { "FABS", "Floating-point Absolute Value", "Floating-Point Instruction", {}, { { SH4A_DOC, 467 } } },
    { "FADD", "Floating-point ADD", "Floating-Point Instruction", {}, { { SH4A_DOC, 468 } } },
    { "FCMP", "Floating-point Compare", "Floating-Point Instruction", {}, { { SH4A_DOC, 471 } } },
    { "FCNVDS", "Floating-point Convert Double to Single Precision", "Floating-Point Instruction", {}, { { SH4A_DOC, 475 } } },
    { "FCNVSD", "Floating-point Convert Single to Double Precision", "Floating-Point Instruction", {}, { { SH4A_DOC, 478 } } },
    { "FDIV", "Floating-point Divide", "Floating-Point Instruction", {}, { { SH4A_DOC, 480 } } },
    { "FIPR", "Floating-point Inner Product", "Floating-Point Instruction", {}, { { SH4A_DOC, 484 } } },
    { "FLDI0", "Floating-point Load Immediate 0.0", "Floating-Point Instruction", {}, { { SH4A_DOC, 486 } } },
    { "FLDI1", "Floating-point Load Immediate 1.0", "Floating-Point Instruction", {}, { { SH4A_DOC, 487 } } },
    { "FLDS", "Floating-point Load to System register", "Floating-Point Instruction", {}, { { SH4A_DOC, 488 } } },
    { "FLOAT", "Floating-point Convert from Integer", "Floating-Point Instruction", {}, { { SH4A_DOC, 489 } } },
    { "FMAC", "Floating-point Multiply and Accumulate", "Floating-Point Instruction", {}, { { SH4A_DOC, 491 } } },
    { "FMUL", "Floating-point Multiply", "Floating-Point Instruction", {}, { { SH4A_DOC, 504 } } },
    { "FNEG", "Floating-point Negate Value", "Floating-Point Instruction", {}, { { SH4A_DOC, 507 } } },
    { "FPCHG", "Pr-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 508 } } },
    { "FRCHG", "FR-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 509 } } },
    { "FSCA", "Floating Point Sine And Cosine Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 510 } } },
    { "FSCHG", "Sz-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 512 } } },
    { "FSQRT", "Floating-point Square Root", "Floating-Point Instruction", {}, { { SH4A_DOC, 513 } } },
    { "FSRRA", "Floating Point Square Reciprocal Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 516 } } },
    { "FSTS", "Floating-point Store System Register", "Floating-Point Instruction", {}, { { SH4A_DOC, 518 } } },
    { "FSUB", "Floating-point Subtract", "Floating-Point Instruction", {}, { { SH4A_DOC, 519 } } },
    { "FTRC", "Floating-point Truncate and Convert to integer", "Floating-Point Instruction", {}, { { SH4A_DOC, 522 } } },
    { "FTRV", "Floating-point Transform Vector", "Floating-Point Instruction", {}, { { SH4A_DOC, 525 } } },
  };

  auto to_lowercase = [](unsigned char c) -> unsigned char { return std::tolower(c); };
  for(instruction_info_t& info : name_data)
  {
    info.mnemonic.insert(0, 1, '$'); // add regex string start
    std::transform(info.mnemonic.begin(), info.mnemonic.begin(), info.mnemonic.end(), to_lowercase); // convert to lowercase
  }

  std::list<insns> insn_blocks;

insn_blocks.push_back
(insns { "Data Transfer Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → Rn" },
  code { "0110nnnnmmmm0011" },

  group { SH4, "MT", SH4A, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH2A, "0", SH4, "0", SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← op1;
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOV (int m, int n)
{
  R[n] = R[m];
  PC += 2;
}
)"},

  example
{R"(
MOV R0,R1 ;Before execution: R0 = H'FFFFFFFF, R1 = H'00000000
;After execution: R1 = H'FFFFFFFF
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov	#imm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "imm → sign extension → Rn" },
  code { "1110nnnniiiiiiii" },

  group { SH4, "EX", SH4A, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  name { "Move Constant Value" },
  brief
{R"(
imm ← SignExtend 8(s);
op2 ← imm;
R n ← Register(op2);
)"},
  description
{R"(
Stores immediate data, sign-extended to longword, in general register Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVI (int i, int n)
{
  if ((i & 0x80) == 0)
    R[n] = (0x000000FF & i);
  else
    R[n] = (0xFFFFFF00 | i);

  PC += 2;
}
)"},

  example
{R"(
1000 MOV #H'80,R1 ;R1 = H'FFFFFF80
1002 MOV.W IMM,R2 ;R2 = H'FFFF9ABC, IMM means @(H'08,PC)
1004 ADD #–1,R0 ;
1006 TST R0,R0 ;← PC location used for address calculation for the
MOV.W instruction
1008 MOVT R13 ;
100A BRA NEXT ;Delayed branch instruction
100C MOV.L @(4,PC),R3 ;R3 = H'12345678
100E IMM
.data.w H'9ABC ;
1010
.data.w H'1234 ;
1012 NEXT JMP @R3 ;Branch destination of the BRA instruction
1014 CMP/EQ #0,R0 ;← PC location used for address calculation for the
;MOV.L instruction
.align 4 ;
1018
.data.l H'12345678 ;
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movi20	#imm20,Rn",
  SH2A,
  abstract { "imm → sign extension → Rn" },
  code { "0000nnnniiii0000 iiiiiiiiiiiiiiii" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Stores immediate data that has been sign-extended to longword in general
register Rn.
<br /><img alt="Move Immediate Data operation" src="movi20.svg" height="140" />
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVI20 (int i, int n)
{
  if (i & 0x00080000) == 0)
    R[n] = (0x000FFFFF & (long)i);
  else
    R[n] = (0xFFF00000 | (long)i);

  PC += 4;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movi20s	#imm20,Rn",
  SH2A,
  abstract { "imm << 8 → sign extension → Rn" },
  code { "0000nnnniiii0001 iiiiiiiiiiiiiiii" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Shifts immediate data 8 bits to the left and performs sign extension to
longword, then stores the resulting data in general register Rn. Using an OR or
ADD instruction as the next instruction enables a 28-bit absolute address to be
generated.
<br /><img alt="Move Immediate Data operation" src="movi20s.svg" height="150" />
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVI20S (int i, int n)
{
  if (i & 0x00080000) == 0)
    R[n] = (0x000FFFFF & (long)i);
  else
    R[n] = (0xFFF00000 | (long)i);

  R[n] <<= 8;
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mova	@(disp,PC),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 4) + (PC & 0xFFFFFFFC) + 4 → R0" },
  code { "11000111dddddddd" },

  group { SH4, "EX", SH4A, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
pc ← SignExtend 32 (PC);
disp ← ZeroExtend 8(i) << 2;
IF (IsDelaySlot())
THROW ILLSLOT;
r0 ← disp + ((pc + 4) ∧ (~ 0x3));
R 0 ← Register(r0);
)"},
  description
{R"(
Stores the effective address of the source operand into general
register R0.  The 8-bit displacement is zero-extended and quadrupled.
Consequently, the relative interval from the operand is PC + 1020 bytes.  The PC
is the address four bytes after this instruction, but the lowest two bits of the
PC are fixed at 00.
)"},

  note
{R"(
SH1*, SH2*, SH3*:<br />
If this instruction is placed immediately after a delayed branch instruction,
the PC must point to an address specified by (the starting address of the branch
destination) + 2.<br /><br />

SH4*:<br />
If this instruction is executed in a delay slot, a slot illegal instruction
exception will be generated.
)"},

  operation
{R"(
void MOVA (int d)
{
  unsigned int disp;
  disp = (unsigned int)(0x000000FF & d);
  R[0] = (PC & 0xFFFFFFFC) + 4 + (disp << 2);
  PC += 2;
}
)"},

  example
{R"(
.org H'1006
1006 MOVA STR,R0 ;Address of STR → R0
1008 MOV.B @R0,R1 ;R1 = “X” ← PC location after correcting the lowest two bits
100A ADD R4,R5 ;← Original PC location for address calculation for the MOVA instruction
.align 4
100C STR:
.sdata “XYZP12”
...............
2002 BRA TRGET ;Delayed branch instruction
2004 MOVA @(0,PC),R0 ;Address of TRGET + 2 → R0
2006 NOP ;
)"},

  exceptions
{R"(
Slot illegal instruction
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@(disp,PC),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 2 + PC + 4) → sign extension → Rn" },
  code { "1001nnnndddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Constant Value" },
  brief
{R"(
pc ← SignExtend 32 (PC);
disp ← ZeroExtend 8(i) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
address ← ZeroExtend 32(disp + (pc + 4));
op2 ← SignExtend 16 (ReadMemory 16(address));
R n ← Register(op2);
)"},
  description
{R"(
Stores immediate data, sign-extended to longword, in general register Rn.
The data is stored from memory address (PC + 4 + displacement × 2).
The 8-bit displacement is multiplied by two after zero-extension, and so the
relative distance from the table is in the range up to PC + 4 + 510 bytes. The
PC value is the address of this instruction.
)"},

  note
{R"(
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void MOVWI (int d, int n)
{
  unsigned int disp = (0x000000FF & d);
  R[n] = Read_16 (PC + 4 + (disp << 1));
  if ((R[n] & 0x8000) == 0)
    R[n] &= 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Slot illegal instruction exception
Data TLB miss exception
Data TLB protection violation exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@(disp,PC),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 4 + (PC & 0xFFFFFFFC) + 4) → sign extension → Rn" },
  code { "1101nnnndddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Constant Value" },
  brief
{R"(
pc ← SignExtend 32 (PC);
disp ← ZeroExtend 8(i) << 2;
IF (IsDelaySlot())
THROW ILLSLOT;
address ← ZeroExtend 32(disp + ((pc + 4) ∧ (~ 0x3)));
op2 ← SignExtend 32 (ReadMemory 32(address));
R n ← Register(op2);
)"},
  description
{R"(
Stores immediate data, sign-extended to longword, in general register Rn.
The data is stored from memory address (PC + 4 + displacement × 4).
The 8-bit displacement is multiplied by four after zero-extension, and so the
relative distance from the operand is in the range up to PC + 4 + 1020 bytes.
The PC value is the address of this instruction. A value with the lower 2 bits
adjusted to 00 is used in address calculation.
)"},

  note
{R"(
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void MOVLI (int d, int n)
{
  unsigned int disp = (0x000000FF & d);
  R[n] = Read_32 ((PC & 0xFFFFFFFC) + 4 + (disp << 2));
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Slot illegal instruction exception
Data TLB miss exception
Data TLB protection violation exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → sign extension → Rn" },
  code { "0110nnnnmmmm0000" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 8(ReadMemory8 (address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(
MOV.B @R0,R1 ;Before execution: @R0 = H'80, R1 = H'00000000
;After execution: R1 = H'FFFFFF80
)"},

  operation
{R"(
void MOVBL (int m, int n)
{
  R[n] = Read_8 (R[m]);
  if ((R[n] & 0x80) == 0)
    R[n] &= 0x000000FF;
  else
    R[n] |= 0xFFFFFF00;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → sign extension → Rn" },
  code { "0110nnnnmmmm0001" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 16 (ReadMemory 16(address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWL (int m, int n)
{
  R[n] = Read_16 (R[m]);
  if ((R[n] & 0x8000) == 0)
    R[n] &= 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → Rn" },
  code { "0110nnnnmmmm0010" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 32 (ReadMemory 32(address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLL (int m, int n)
{
  R[n] = Read_32 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	Rm,@Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (Rn)" },
  code { "0010nnnnmmmm0000" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2);
WriteMemory 8(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBS (int m, int n)
{
  Write_8 (R[n], R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	Rm,@Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (Rn)" },
  code { "0010nnnnmmmm0001" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2);
WriteMemory 16(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWS (int m, int n)
{
  Write_16 (R[n], R[m]);
  PC += 2;
}
)"},

  example
{R"(
MOV.W R0,@R1 ;Before execution: R0 = H'FFFF7F80
;After execution: @R1 = H'7F80
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	Rm,@Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (Rn)" },
  code { "0010nnnnmmmm0010" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2);
WriteMemory 32(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLS (int m, int n)
{
  Write_32 (R[n], R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@Rm+,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → sign extension → Rn, Rm+1 → Rm" },
  code { "0110nnnnmmmm0100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "1/2" },

  brief
{R"(
m_field ← ZeroExtend 4(m);
n_field ← ZeroExtend 4(n);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 8(ReadMemory8 (address));
IF (m_field = n_field)
op1 ← op2;
ELSE
op1 ← op1 + 1;
R m ← Register(op1);
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBP (int m, int n)
{
  R[n] = Read_8 (R[m]);
  if ((R[n] & 0x80) == 0)
    R[n] &= 0x000000FF;
  else
    R[n] |= 0xFFFFFF00;

  if (n != m)
    R[m] += 1;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@Rm+,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → sign extension → Rn, Rm+2 → Rm" },
  code { "0110nnnnmmmm0101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "1/2" },

  brief
{R"(
m_field ← ZeroExtend 4(m);
n_field ← ZeroExtend 4(n);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 16 (ReadMemory 16(address));
IF (m_field = n_field)
op1 ← op2;
ELSE
op1 ← op1 + 2;
R m ← Register(op1);
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWP (int m, int n)
{
  R[n] = Read_16 (R[m]);
  if ((R[n] & 0x8000) == 0)
    R[n] &= 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  if (n != m)
    R[m] += 2;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@Rm+,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → Rn, Rm+4 → Rm" },
  code { "0110nnnnmmmm0110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "1/2" },

  brief
{R"(
m_field ← ZeroExtend 4(m);
n_field ← ZeroExtend 4(n);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
op2 ← SignExtend 32 (ReadMemory 32(address));
IF (m_field = n_field)
op1 ← op2;
ELSE
op1 ← op1 + 4;
R m ← Register(op1);
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(
MOV.L @R0+,R1 ;Before execution: R0 = H'12345670
;After execution: R0 = H'12345674, R1 = @H'12345670
)"},

  operation
{R"(
void MOVLP (int m, int n)
{
  R[n] = Read_32 (R[m]);

  if (n != m)
    R[m] += 4;

  PC += 2;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	Rm,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn-1 → Rn, Rm → (Rn)" },
  code { "0010nnnnmmmm0100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2 - 1);
WriteMemory 8(address, op1);
op2 ← address;
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBM (int m, int n)
{
  Write_8 (R[n] - 1, R[m]);
  R[n] -= 1;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	Rm,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn-2 → Rn, Rm → (Rn)" },
  code { "0010nnnnmmmm0101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2 - 2);
WriteMemory 16(address, op1);
op2 ← address;
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWM (int m, int n)
{
  Write_16 (R[n] - 2, R[m]);
  R[n] -= 2;
  PC += 2;
}
)"},

  example
{R"(
MOV.W R0,@–R1 ;Before execution: R0 = H'AAAAAAAA, R1 = H'FFFF7F80
;After execution: R1 = H'FFFF7F7E, @R1 = H'AAAA
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	Rm,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn-4 → Rn, Rm → (Rn)" },
  code { "0010nnnnmmmm0110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2 - 4);
WriteMemory 32(address, op1);
op2 ← address;
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLM (int m, int n)
{
  Write_32 (R[n] - 4, R[m]);
  R[n] -= 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@-Rm,R0",
  SH2A,
  abstract { "Rm-1 → Rm, (Rm) → sign extension → R0" },
  code { "0100mmmm11001011" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSBM (int m)
{
  R[m] -= 1;
  R[0] = Read_16 (R[m]);

  if ((R[0] & 0x80) == 0)
    R[0] &= 0x000000FF;
  else
    R[0] |= 0xFFFFFF00;

  PC+=2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@-Rm,R0",
  SH2A,
  abstract { "Rm-2 → Rm, (Rm) → sign extension → R0" },
  code { "0100mmmm11011011" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSWM (int m)
{
  R[m]-= 2;
  R[0] = Read_16 (R[m]);

  if ((R[0] & 0x8000) == 0)
    R[0] &= 0x0000FFFF;
  else
    R[0] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@-Rm,R0",
  SH2A,
  abstract { "Rm-4 → Rm, (Rm) → R0" },
  code { "0100mmmm11101011" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSLM (int m)
{
  R[m] -= 4;
  R[0] = Read_32 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	R0,@Rn+",
  SH2A,
  abstract { "R0 → (Rn), Rn+1 → Rn" },
  code { "0100nnnn10001011" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSBP (int n)
{
  Write_8 (R[n], R[0]);
  R[n] += 1;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	R0,@Rn+",
  SH2A,
  abstract { "R0 → (Rn), Rn+2 → Rn" },
  code { "0100nnnn10011011" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSWP (int n)
{
  Write_16 (R[n], R[0]);
  R[n] += 2;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	R0,@Rn+",
  SH2A,
  abstract { "R0 → (Rn), Rn+4 → Rn" },
  code { "0100nnnn10101011" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRSLP (int n)
{
  Write_32 (R[n], R[0]);
  R[n] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@(disp,Rm),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp + Rm) → sign extension → R0" },
  code { "10000100mmmmdddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Structure Data" },
  brief
{R"(
disp ← ZeroExtend 4(i);
op2 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(disp + op2);
r0 ← SignExtend 8(ReadMemory 8 (address));
R 0 ← Register(r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is only zero-extended, so a range up to +15 bytes
can be specified. If a memory operand cannot be reached, the @(R0,Rn) mode can
be used instead.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBL4 (int m, int d)
{
  long disp = (0x0000000F & (long)d);
  R[0] = Read_8 (R[m] + disp);

  if ((R[0] & 0x80) == 0)
    R[0] &= 0x000000FF;
  else
    R[0] |= 0xFFFFFF00;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@(disp12,Rm),Rn",
  SH2A,
  abstract { "(disp + Rm) → sign extension → Rn" },
  code { "0011nnnnmmmm0001 0100dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBL12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  R[n] = Read_8 (R[m] + disp);

  if ((R[n] & 0x80) == 0)
    R[n] &= 0x000000FF;
  else
    R[n] |= 0xFFFFFF00;

  PC += 4;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movu.b	@(disp12,Rm),Rn",
  SH2A,
  abstract { "(disp + Rm) → zero extension → Rn" },
  code { "0011nnnnmmmm0001 1000dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers a source operand to a destination, performing unsigned data transfer.
This instruction is ideal for data access in a structure or the stack.
The loaded data is zero-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBUL12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  R[n] = Read_8 (R[m] + disp);
  R[n] &= 0x000000FF;
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@(disp,Rm),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 2 + Rm) → sign extension → R0" },
  code { "10000101mmmmdddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Structure Data" },
  brief
{R"(
disp ← ZeroExtend 4(i) << 1;
op2 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(disp + op2);
r0 ← SignExtend 16 (ReadMemory 16(address));
R 0 ← Register(r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is multiplied by two after zero-extension, enabling a
range up to +30 bytes to be specified.  If a memory operand cannot be reached,
the @(R0,Rn) mode can be used instead.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWL4 (int m, int d)
{
  long disp = (0x0000000F & (long)d);
  R[0] = Read_16 (R[m] + (disp << 1));

  if ((R[0] & 0x8000) == 0)
    R[0] &= 0x0000FFFF;
  else
    R[0] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@(disp12,Rm),Rn",
  SH2A,
  abstract { "(disp × 2 + Rm) → sign extension → Rn" },
  code { "0011nnnnmmmm0001 0101dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWL12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  R[n] = Read_16 (R[m] + (disp << 1));

  if ((R[n] & 0x8000) == 0)
    R[n] &= 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movu.w	@(disp12,Rm),Rn",
  SH2A,
  abstract { "(disp × 2 + Rm) → zero extension → Rn" },
  code { "0011nnnnmmmm0001 1001dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers a source operand to a destination, performing unsigned data transfer.
This instruction is ideal for data access in a structure or the stack.
The loaded data is zero-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWUL12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  R[n] = Read_16 (R[m] + (disp << 1));
  R[n] &= 0x0000FFFF;
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@(disp,Rm),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 4 + Rm) → Rn" },
  code { "0101nnnnmmmmdddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Structure Data" },
  brief
{R"(
disp ← ZeroExtend 4(i) << 2;
op2 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(disp + op2);
op3 ← SignExtend 32 (ReadMemory 32(address));
R n ← Register(op3);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is multiplied by four after zero-extension, enabling a
range up to +60 bytes to be specified.  If a memory operand cannot be reached,
the @(R0,Rn) mode can be used instead.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLL4 (int m, int d, int n)
{
  long disp = (0x0000000F & (long)d);
  R[n] = Read_32 (R[m] + (disp << 2));
  PC += 2;
}
)"},

  example
{R"(
MOV.L @(2,R0),R1 ;Before execution: @(R0 + 8) = H'12345670
;After execution: R1 = H'12345670
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@(disp12,Rm),Rn",
  SH2A,
  abstract { "(disp × 4 + Rm) → Rn" },
  code { "0011nnnnmmmm0001 0110dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "2" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLL12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  R[n] = Read_32 (R[m] + (disp << 2));
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	R0,@(disp,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 → (disp + Rn)" },
  code { "10000000nnnndddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Structure Data" },
  brief
{R"(
r0 ← SignExtend 32 (R0);
disp ← ZeroExtend 4(i);
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(disp + op2);
WriteMemory 8(address, r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is only zero-extended, so a range up to +15 bytes
can be specified. If a memory operand cannot be reached, the @(R0,Rn) mode can
be used instead.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBS4 (int d, int n)
{
  long disp = (0x0000000F & (long)d);
  Write_8 (R[n] + disp, R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	Rm,@(disp12,Rn)",
  SH2A,
  abstract { "Rm → (disp + Rn)" },
  code { "0011nnnnmmmm0001 0000dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "0" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBS12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  Write_8 (R[n] + disp, R[m]);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	R0,@(disp,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 → (disp × 2 + Rn)" },
  code { "10000001nnnndddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Structure Data" },
  brief
{R"(
r0 ← SignExtend 32 (R0);
disp ← ZeroExtend 4(i) << 1;
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(disp + op2);
WriteMemory 16(address, r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is multiplied by two after zero-extension, enabling a
range up to +30 bytes to be specified.  If a memory operand cannot be reached,
the @(R0,Rn) mode can be used instead.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWS4 (int d, int n)
{
  long disp = (0x0000000F & (long)d);
  Write_16 (R[n] + (disp << 1), R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	Rm,@(disp12,Rn)",
  SH2A,
  abstract { "Rm → (disp × 2 + Rn)" },
  code { "0011nnnnmmmm0001 0001dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "0" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWS12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  Write_16 (R[n] + (disp << 1), R[m]);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	Rm,@(disp,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (disp × 4 + Rn)" },
  code { "0001nnnnmmmmdddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Structure Data" },
  brief
{R"(
op1 ← SignExtend 32 (Rm );
disp ← ZeroExtend 4(i) << 2;
op3 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(disp + op3);
WriteMemory 32(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 4-bit displacement is multiplied by four after zero-extension, enabling a
range up to +60 bytes to be specified.  If a memory operand cannot be reached,
the @(R0,Rn) mode can be used instead.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLS4 (int m, int d, int n)
{
  long disp = (0x0000000F & (long)d);
  Write_32 (R[n] + (disp << 2), R[m]);
  PC += 2;
}
)"},

  example
{R"(
MOV.L R0,@(H'F,R1) ;Before execution: R0 = H'FFFF7F80
;After execution: @(R1 + 60) = H'FFFF7F80
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	Rm,@(disp12,Rn)",
  SH2A,
  abstract { "Rm → (disp × 4 + Rn)" },
  code { "0011nnnnmmmm0001 0010dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "0" },



  description
{R"(
Transfers the source operand to the destination.  This
instruction is ideal for data access in a structure or the stack.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLS12 (int d, int m, int n)
{
  long disp = (0x00000FFF & (long)d);
  Write_32 (R[n] + (disp << 2), R[m]);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@(R0,Rm),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + Rm) → sign extension → Rn" },
  code { "0000nnnnmmmm1100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(r0 + op1);
op2 ← SignExtend 8(ReadMemory8 (address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBL0 (int m, int n)
{
  R[n] = Read_8 (R[m] + R[0]);

  if ((R[n] & 0x80) == 0)
    R[n] &= 0x000000FF;
  else R[n] |= 0xFFFFFF00;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@(R0,Rm),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + Rm) → sign extension → Rn" },
  code { "0000nnnnmmmm1101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(r0 + op1);
op2 ← SignExtend 16 (ReadMemory 16(address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWL0 (int m, int n)
{
  R[n] = Read_16 (R[m] + R[0]);

  if ((R[n] & 0x8000) == 0)
    R[n] &= 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(
MOV.W @(R0,R2),R1 ;Before execution: R2 = H'00000004, R0 = H'10000000
;After execution: R1 = @H'10000004
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@(R0,Rm),Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + Rm) → Rn" },
  code { "0000nnnnmmmm1110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(r0 + op1);
op2 ← SignExtend 32 (ReadMemory 32(address));
R n ← Register(op2);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLL0 (int m, int n)
{
  R[n] = Read_32 (R[m] + R[0]);
  PC += 2;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	Rm,@(R0,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (R0 + Rn)" },
  code { "0000nnnnmmmm0100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(r0 + op2);
WriteMemory 8(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBS0 (int m, int n)
{
  Write_8 (R[n] + R[0], R[m]);
  PC += 2;
}
)"},

  example
{R"(
MOV.B R1,@(R0,R2) ;Before execution: R2 = H'00000004, R0 = H'10000000
;After execution: R1 = @H'10000004
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	Rm,@(R0,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (R0 + Rn)" },
  code { "0000nnnnmmmm0101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(r0 + op2);
WriteMemory 16(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWS0 (int m, int n)
{
  Write_16 (R[n] + R[0], R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	Rm,@(R0,Rn)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → (R0 + Rn)" },
  code { "0000nnnnmmmm0110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(r0 + op2);
WriteMemory 32(address, op1);
)"},
  description
{R"(
Transfers the source operand to the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLS0 (int m, int n)
{
  Write_32 (R[n] + R[0], R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	@(disp,GBR),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp + GBR) → sign extension → R0" },
  code { "11000100dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
disp ← ZeroExtend 8(i);
address ← ZeroExtend 32(disp + gbr);
r0 ← SignExtend 8(ReadMemory 8 (address));
R 0 ← Register(r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is only zero-extended, so a range up to +255 bytes can be
specified.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBLG (int d)
{
  unsigned int disp = (0x000000FF & d);
  R[0] = Read_8 (GBR + disp);

  if ((R[0] & 0x80) == 0)
    R[0] &= 0x000000FF;
  else
    R[0] |= 0xFFFFFF00;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	@(disp,GBR),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 2 + GBR) → sign extension → R0" },
  code { "11000101dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
disp ← ZeroExtend 8(i) << 1;
address ← ZeroExtend 32(disp + gbr);
r0 ← SignExtend 16 (ReadMemory 16(address));
R 0 ← Register(r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is multiplied by two after zero-extension, enabling a
range up to +510 bytes to be specified.
The loaded data is sign-extended to 32 bit before being stored in the
destination register.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWLG (int d)
{
  unsigned int disp = (0x000000FF & d);
  R[0] = Read_16 (GBR + (disp << 1));

  if ((R[0] & 0x8000) == 0)
    R[0] &= 0x0000FFFF;
  else
    R[0] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	@(disp,GBR),R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(disp × 4 + GBR) → R0" },
  code { "11000110dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
disp ← ZeroExtend 8(i) << 2;
address ← ZeroExtend 32(disp + gbr);
r0 ← SignExtend 32 (ReadMemory 32(address));
R 0 ← Register(r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is multiplied by four after zero-extension, enabling a
range up to +1020 bytes to be specified.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLLG (int d)
{
  unsigned int disp = (0x000000FF & d);
  R[0] = Read_32 (GBR + (disp << 2));
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.b	R0,@(disp,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 → (disp + GBR)" },
  code { "11000000dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
r0 ← SignExtend 32 (R0);
disp ← ZeroExtend 8(i);
address ← ZeroExtend 32(disp + gbr);
WriteMemory 8(address, r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is only zero-extended, so a range up to +255 bytes can be
specified.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVBSG (int d)
{
  unsigned int disp = (0x000000FF & d);
  Write_8 (GBR + disp, R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.w	R0,@(disp,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 → (disp × 2 + GBR)" },
  code { "11000001dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
r0 ← SignExtend 32 (R0);
disp ← ZeroExtend 8(i) << 1;
address ← ZeroExtend 32(disp + gbr);
WriteMemory 16(address, r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is multiplied by two after zero-extension, enabling a
range up to +510 bytes to be specified.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVWSG (int d)
{
  unsigned int disp = (0x000000FF & d);
  Write_16 (GBR + (disp << 1), R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mov.l	R0,@(disp,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 → (disp × 4 + GBR)" },
  code { "11000010dddddddd" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  name { "Move Global Data" },
  brief
{R"(
gbr ← SignExtend 32 (GBR);
r0 ← SignExtend 32 (R0);
disp ← ZeroExtend 8(i) << 2;
address ← ZeroExtend 32(disp + gbr);
WriteMemory 32(address, r0);
)"},
  description
{R"(
Transfers the source operand to the destination.
The 8-bit displacement is multiplied by four after zero-extension, enabling a
range up to +1020 bytes to be specified.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLSG (int d)
{
  unsigned int disp = (0x000000FF & (long)d);
  Write_32 (GBR + (disp << 2), R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movco.l	R0,@Rn",
  SH4A,
  abstract { "LDST → T\nIf (T == 1): R0 → Rn\n0 → LDST" },
  code { "0000nnnn01110011" },
  flags { "LDST → T" },

  group { SH4A, "CO" },
  issue { SH4A, "1" },
  latency { SH4A, "1" },

  description
{R"(
MOVCO is used in combination with MOVLI to realize an atomic read-modify-write
operation in a single processor.<br /><br />
This instruction copies the value of the LDST flag to the T bit. When the T bit
is set to 1, the value of R0 is stored at the address in Rm. If the T bit is
cleared to 0, the value is not stored at the address in Rm. Finally, the LDST
flag is cleared to 0. Since the LDST flag is cleared by an instruction or
exception, storage by the MOVCO instruction only proceeds when no interrupt or
exception has occurred between the execution of the MOVLI and MOVCO instructions.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVCO (int n)
{
  T = LDST;
  if (T == 1)
    Write_32 (R[n], R[0]);

  LDST = 0;
  PC += 2
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movli.l	@Rm,R0",
  SH4A,
  abstract { "1 → LDST\n(Rm) → R0\nWhen interrupt/exception occured: 0 → LDST" },
  code { "0000mmmm01100011" },

  group { SH4A, "CO" },
  issue { SH4A, "1" },
  latency { SH4A, "1" },



  description
{R"(
MOVLI is used in combination with MOVCO to realize an atomic read-modify-write
operation in a single processor.<br /><br />
This instruction sets the LDST flag to 1 and reads the four bytes of data
indicated by Rm into R0. If, however, an interrupt or exception occurs, LDST is
cleared to 0. Storage by the MOVCO instruction only proceeds when the
instruction is executed after the LDST bit has been set by the MOVLI instruction
and not cleared by an interrupt or other exception.  When LDST has been cleared
to 0, the MOVCO instruction clears the T bit and does not proceed with storage.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLINK (int m)
{
  LDST = 1;
  R[0] = Read_32 (R[m]);
  PC += 2
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movua.l	@Rm,R0",
  SH4A,
  abstract { "(Rm) → R0\nLoad non-boundary alignment data" },
  code { "0100mmmm10101001" },

  group { SH4A, "LS" },
  issue { SH4A, "2" },
  latency { SH4A, "2" },



  description
{R"(
Loads the longword of data from the effective address indicated
by the contents of Rm in memory to R0. The address is not restricted to longword
boundaries address (4n).  This instruction allows loading from
non-longword-boundary addresses (4n + 1, 4n + 2, and 4n + 3). Data address error
exceptions do not occur when access is to non-longword-boundary addresses
(4n + 1, 4n + 2, and 4n + 3).
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVUAL (int m)
{
  Read_Unaligned_32 (R0, R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error (when the privileged area is accessed from user mode)
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movua.l	@Rm+,R0",
  SH4A,
  abstract { "(Rm) → R0, Rm + 4 → Rm\nLoad non-boundary alignment data" },
  code { "0100mmmm11101001" },

  group { SH4A, "LS" },
  issue { SH4A, "2" },
  latency { SH4A, "2" },



  description
{R"(
Loads the longword of data from the effective address indicated
by the contents of Rm in memory to R0. The address is not restricted to longword
boundaries address (4n).  This instruction allows loading from
non-longword-boundary addresses (4n + 1, 4n + 2, and 4n + 3). Data address error
exceptions do not occur when access is to non-longword-boundary addresses
(4n + 1, 4n + 2, and 4n + 3).
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVUALP (int m)
{
  Read_Unaligned_32 (R0,R[m]);

  if (m != 0)
    R[m] += 4;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error (when the privileged area is accessed from user mode)
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movml.l	Rm,@-R15",
  SH2A,
  abstract
{ R"(R15-4 → R15, Rm → (R15)
R15-4 → R15, Rm-1 → (R15)
...
...
R15 - 4 → R15, R0 → (R15)
Note: When Rm = R15, read Rm as PR)"},

  code { "0100mmmm11110001" },
  issue { SH2A, "1-16" },
  latency { SH2A, "1-16" },



  description
{R"(
Transfers a source operand to a destination. This instruction performs transfer
between a number of general registers (R0 to Rn/Rm) not exceeding the specified
register number and memory with the contents of R15 as its address.
<br /><br />
If R15 is specified, PR is transferred instead of R15. That is, when
nnnn(mmmm) = 1111 is specified, R0 to R14 and PR are the general registers
subject to transfer.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLMML (int m)
{
  for (int i = m; i >= 0; i--)
  {
    if (i == 15)
      Write_32 (R[15] - 4, PR);
    else
      Write_32 (R[15] - 4, R[i]);

    R[15] -= 4;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movml.l	@R15+,Rn",
  SH2A,
  abstract
{R"((R15) → R0, R15+4 → R15
(R15) → R1, R15+4 → R15
...
...
(R15) → Rn
Note: When Rn = R15, read Rn as PR)"},

  code { "0100nnnn11110101" },
  issue { SH2A, "1-16" },
  latency { SH2A, "2-17" },



  description
{R"(
Transfers a source operand to a destination. This instruction performs transfer
between a number of general registers (R0 to Rn/Rm) not exceeding the specified
register number and memory with the contents of R15 as its address.
<br /><br />
If R15 is specified, PR is transferred instead of R15. That is, when
nnnn(mmmm) = 1111 is specified, R0 to R14 and PR are the general registers
subject to transfer.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLPML (int n)
{
  for (int i = 0; i <= n; i++)
  {
    if (i == 15)
      PR = Read_32 (R[15]);
    else
      R[i] = Read_32 (R[15]);

    R[15] += 4;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movmu.l	Rm,@-R15",
  SH2A,
  abstract
{R"(R15-4 → R15, PR → (R15)
R15-4 → R15, R14 → (R15)
...
...
R15-4 → R15, Rm → (R15)
Note: When Rm = R15, read Rm as PR)"},

  code { "0100mmmm11110000" },
  issue { SH2A, "1-16" },
  latency { SH2A, "1-16" },



  description
{R"(
Transfers a source operand to a destination. This instruction performs transfer
between a number of general registers (Rn/Rm to R14, PR) not lower than the
specified register number and memory with the contents of R15 as its address.
<br /><br />
If R15 is specified, PR is transferred instead of R15.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLMMU (int m)
{
  Write_32 (R[15] - 4, PR);
  R[15] -= 4;

  for (int i = 14; i >= m; i--)
  {
    Write_32 (R[15] - 4, R[i]);
    R[15] -= 4;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movmu.l	@R15+,Rn",
  SH2A,
  abstract
{R"((R15) → Rn, R15+4 → R15
(R15) → Rn+1, R15+4 → R15
...
...
(R15) → R14, R15+4 → R15
(R15) → PR
Note: When Rn = R15, read Rn as PR)"},

  code { "0100nnnn11110100" },
  issue { SH2A, "1-16" },
  latency { SH2A, "2-17" },



  description
{R"(
Transfers a source operand to a destination. This instruction performs transfer
between a number of general registers (Rn/Rm to R14, PR) not lower than the
specified register number and memory with the contents of R15 as its address.
<br /><br />
If R15 is specified, PR is transferred instead of R15.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVLPMU (int n)
{
  for (int i = n; i <= 14; i++)
  {
    R[i] = Read_32 (R[15]);
    R[15] += 4;
  }

  PR = Read_32 (R[15]);
  R[15] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movrt	Rn",
  SH2A,
  abstract { "~T → Rn" },
  code { "0000nnnn00111001" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Reverses the T bit and then stores the resulting value in general register Rn.
The value of Rn is 0 when T = 1 and 1 when T = 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVRT (int n)
{
  if (T == 1)
    R[n] = 0x00000000;
  else
    R[n] = 0x00000001;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movt	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T → Rn" },
  code { "0000nnnn00101001" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← t;
R n ← Register(op1);
)"},
  description
{R"(
Stores the T bit in general register Rn.
The value of Rn is 1 when T = 1 and 0 when T = 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVT (int n)
{
  if (T == 1)
    R[n] = 0x00000001;
  else
    R[n] = 0x00000000;
  PC += 2;
}
)"},

  example
{R"(
XOR R2,R2 ;R2 = 0
CMP/PZ R2 ;T = 1
MOVT R0 ;R0 = 1
CLRT ;T = 0
MOVT R1 ;R1 = 0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "nott",
  SH2A,
  abstract { "~T → T" },
  code { "0000000001101000" },
  flags { "~T → T" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Inverts the T bit, then stores the resulting value in the T bit.
)"},

  note
{R"(

)"},

  operation
{R"(
void NOTT (void)
{
  if (T == 1)
    T = 0;
  else
    T = 1;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "swap.b	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → swap lower 2 bytes → Rn" },
  code { "0110nnnnmmmm1000" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ((op1 < 16 FOR 16 > << 16) ∨ (op1 < 0 FOR 8 > << 8)) ∨ op1< 8 FOR 8 > ;
R n ← Register(op2);
)"},
  description
{R"(
Swaps the upper and lower parts of the contents of general register Rm and
stores the result in Rn.
The 8 bits from bit 15 to bit 8 of Rm are swapped with the 8 bits from bit 7 to
bit 0. The upper 16 bits of Rm are transferred directly to the upper 16 bits of
Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void SWAPB (int m, int n)
{
  unsigned long temp0, temp1;
  temp0 = R[m] & 0xFFFF0000;
  temp1 = (R[m] & 0x000000FF) << 8;
  R[n] = (R[m] & 0x0000FF00) >> 8;
  R[n] = R[n] | temp1 | temp0;
  PC += 2;
}
)"},

  example
{R"(
SWAP.B R0,R1 ;Before execution: R0 = H'12345678
;After execution: R1 = H'12347856
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "swap.w	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → swap upper/lower words → Rn" },
  code { "0110nnnnmmmm1001" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← (op1 < 0 FOR 16 > << 16) ∨ op1< 16 FOR 16 > ;
R n ← Register(op2);
)"},
  description
{R"(
Swaps the upper and lower parts of the contents of general register Rm and
stores the result in Rn.
The 16 bits from bit 31 to bit 16 of Rm are swapped with the 16 bits from bit
15 to bit 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void SWAPW (int m, int n)
{
  unsigned long temp;
  temp = (R[m] >> 16) & 0x0000FFFF;
  R[n] = R[m] << 16;
  R[n] |= temp;
  PC += 2;
}
)"},

  example
{R"(
SWAP.W R0,R1 ;Before execution: R0 = H'12345678
;After execution: R1 = H'5678123
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "xtrct	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm:Rn middle 32 bits → Rn" },
  code { "0010nnnnmmmm1101" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ZeroExtend 32(R n);
op2 ← op2 < 16 FOR 16 > ∨ (op1 < 0 FOR 16 > << 16);
R n ← Register(op2);
)"},
  description
{R"(
Extracts the middle 32 bits from the 64-bit contents of linked general registers
Rm and Rn, and stores the result in Rn.
<br /><img alt="Extract operation" src="xtrct.svg" height="110" />
)"},

  note
{R"(

)"},

  operation
{R"(
void XTRCT (int m, int n)
{
  unsigned long high = (R[m] << 16) & 0xFFFF0000;
  unsigned long low = (R[n] >> 16) & 0x0000FFFF;
  R[n] = high | low;
  PC += 2;
}
)"},

  example
{R"(
XTRCT R0,R1 ;Before execution: R0 = H'01234567, R1 = H'89ABCDEF
;After execution: R1 = H'456789AB
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Bit Manipulation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "band.b     #imm3,@disp12,Rn",
  SH2A,
  abstract { "(imm of (disp+Rn)) & T → T" },
  code { "0011nnnn0iii1001 0100dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },

  description
{R"(
ANDs a specified bit in memory at the address indicated by (disp + Rn) with the
T bit, and stores the result in the T bit. The bit number is specified by 3-bit
immediate data. With this instruction, data is read from memory as a byte unit.
)"},

// FIXME: <br /><img src="band.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BANDM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if ((T == 0) || (assignbit == 0))
    T = 0;
  else
    T = 1;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bandnot.b  #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "~(imm of (disp+Rn)) & T → T" },
  code { "0011nnnn0iii1001 1100dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },

  description
{R"(
ANDs the value obtained by inverting a specified bit of memory at the address
indicated by (disp + Rn) with the T bit, and stores the result in the T bit.
The bit number is specified by 3-bit immediate data. With this instruction, data
is read from memory as a byte unit.
)"},

// FIXME: <br /><img src="bandnot.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BANDNOTM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if ((T == 1) && (assignbit == 0))
    T = 1;
  else
    T = 0;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bclr.b     #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "0 → (imm of (disp+Rn))" },
  code { "0011nnnn0iii1001 0000dddddddddddd" },

  issue { SH2A, "3" },
  latency { SH2A, "2" },



  description
{R"(
Clears a specified bit of memory at the address indicated by (disp + Rn).
The bit number is specified by 3-bit immediate data. After data is read from
memory as a byte unit, clearing of the specified bit is executed and the
resulting data is then written to memory as a byte unit.
)"},

// FIXME: <br /><img src="bclr.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BCLRM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  temp &= (~(0x00000001 < <imm));
  Write_8 (R[n] + disp, temp);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bclr       #imm3,Rn",
  SH2A,
  abstract { "0 → imm of Rn" },
  code { "10000110nnnn0iii" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Clears a specified bit of the LSB 8 bits of a general register Rn.
The bit number is specified by 3-bit immediate data.
)"},

// FIXME: <br /><img src="bclr.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void CLR (int i, int n)
{
  long imm, temp;
  imm = (0x00000007 & (long)i);
  R[n] &= (~(0x00000001 << imm));
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bld.b      #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "(imm of (disp+Rn)) → T" },
  code { "0011nnnn0iii1001 0011dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },

  description
{R"(
Stores a specified bit of memory at the address indicated by (disp + Rn) in the
T bit. The bit number is specified by 3-bit immediate data. Data is read from
memory as a byte unit.
)"},

// FIXME: <br /><img src="bld.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BLDM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if (assignbit == 0)
    T = 0;
  else
    T = 1;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bld        #imm3,Rn",
  SH2A,
  abstract { "imm of Rn → T" },
  code { "10000111nnnn1iii" },
  flags { "Result → T" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Stores a specified bit of the LSB 8 bits of a general register Rn in the T bit.
The bit number is specified by 3-bit immediate data.
)"},

// FIXME: <br /><img src="bld.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BLD (int i, int n)
{
  long imm, assignbit;
  imm = (0x00000007 & (long)i);
  assignbit = (0x00000001 << imm) & R[n];

  if (assignbit == 0)
    T = 0;
  else
    T = 1;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bldnot.b   #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "~(imm of (disp+Rn)) → T" },
  code { "0011nnnn0iii1001 1011dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },

  description
{R"(
Inverts a specified bit of memory at the address indicated by (disp + Rn), and
stores the resulting value in the T bit. The bit number is specified by 3-bit
immediate data. Data is read from memory as a byte unit.
)"},

// FIXME: <br /><img src="bldnot.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BLDNOTM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if (assignbit == 0)
    T = 1;
  else
    T = 0;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bor.b      #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "(imm of (disp+Rn)) | T → T" },
  code { "0011nnnn0iii1001 0101dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },



  description
{R"(
ORs a specified bit in memory at the address indicated by (disp + Rn) with the
T bit, and stores the result in the T bit. The bit number is specified by 3-bit
immediate data. Data is read from memory as a byte unit.
)"},

// FIXME: <br /><img src="bor.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BORM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if ((T == 0) && (assignbit == 0))
    T = 0;
  else
    T = 1;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bornot.b   #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "~(imm of (disp+Rn)) | T → T" },
  code { "0011nnnn0iii1001 1101dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },

  description
{R"(
ORs the value obtained by inverting a specified bit of memory at the address
indicated by (disp + Rn) with the T bit, and stores the result in the T bit.
The bit number is specified by 3-bit immediate data. With this instruction,
data is read from memory as a byte unit.
)"},

// FIXME: <br /><img src="bornot.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BORNOTM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if ((T == 1) || (assignbit == 0))
    T = 1;
  else
    T = 0;

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bset.b     #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "1 → (imm of (disp+Rn))" },
  code { "0011nnnn0iii1001 0001dddddddddddd" },

  issue { SH2A, "3" },
  latency { SH2A, "2" },

  description
{R"(
Sets to 1 a specified bit of memory at the address indicated by (disp + Rn).
The bit number is specified by 3-bit immediate data. After data is read from
memory as a byte unit, the specified bit is set to 1, and the resulting data is
then written to memory as a byte unit.
)"},

// FIXME: <br /><img src="bset.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BSETM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  temp |= (0x00000001 << imm);
  Write_8 (R[n] + disp, temp);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bset       #imm3,Rn",
  SH2A,
  abstract { "1 → imm of Rn" },
  code { "10000110nnnn1iii" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Sets to 1 a specified bit of the LSB 8 bits of a general register Rn. The bit
number is specified by 3-bit immediate data.
)"},

// FIXME: <br /><img src="bset.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BSET (int i, int n)
{
  long imm, temp;
  imm = (0x00000007 & (long)i);
  R[n] |= (0x00000001 << imm);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bst.b      #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "T → (imm of (disp+Rn))" },
  code { "0011nnnn0iii1001 0010dddddddddddd" },

  issue { SH2A, "3" },
  latency { SH2A, "2" },

  description
{R"(
Transfers the contents of the T bit to a specified 1-bit location of memory at
the address indicated by (disp + Rn). The bit number is specified by 3-bit
immediate data. After data is read from memory as a byte unit, transfer from the
T bit to the specified bit is executed, and the resulting data is then written
to memory as a byte unit.
)"},

// FIXME: <br /><img src="bst.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BSTM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);

  if (T == 0)
    temp &= (~(0x00000001 << imm));
  else
    temp |= (0x00000001 << imm);

  Write_8 (R[n] + disp, temp);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bst        #imm3,Rn",
  SH2A,
  abstract { "T → imm of Rn" },
  code { "10000111nnnn0iii" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },

  description
{R"(
Transfers the contents of the T bit to a specified 1-bit location of the
LSB 8 bits of a general register Rn. The bit number is specified by 3-bit
immediate data.
)"},

// FIXME: <br /><img src="bst.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BST (int i, int n)
{
  long disp, imm;
  disp = (0x00000FFF & (long)d);
  imm = (0x00000007 & (long)i);

  if (T == 0)
    R[n] &= (~(0x00000001 << imm));
  else
    R[n] |= (0x00000001 << imm);

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bxor.b     #imm3,@(disp12,Rn)",
  SH2A,
  abstract { "(imm of (disp+Rn)) ^ T → T" },
  code { "0011nnnn0iii1001 0110dddddddddddd" },
  flags { "Result → T" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },



  description
{R"(
Exclusive-ORs a specified bit in memory at the address indicated by (disp + Rn)
with the T bit, and stores the result in the T bit. The bit number is specified
by 3-bit immediate data. With this instruction, data is read from memory as a
byte unit.
)"},

// FIXME: <br /><img src="bxor.b.svg" height="110" />

  note
{R"(

)"},

  operation
{R"(
void BXORM (int d, int i, int n)
{
  long disp = (0x00000FFF & (long)d);
  long imm = (0x00000007 & (long)i);
  long temp = Read_8 (R[n] + disp);
  long assignbit = (0x00000001 << imm) & temp;

  if (assignbit == 0)
  {
    if (T == 0)
      T = 0;
    else
      T = 1;
  }
  else
  {
    if (T == 0)
      T = 1;
    else
      T = 0;
  }

  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Arithmetic Operation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "add	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn + Rm → Rn" },
  code { "0011nnnnmmmm1100" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
op2 ← op2 + op1;
R n ← Register(op2)
)"},

  description
{R"(
Adds together the contents of general registers Rn and Rm and stores the
result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void ADD (int m, int n)
{
  R[n] += R[m];
  PC += 2;
}
)"},

  example
{R"(
ADD R0,R1 ;Before execution: R0 = H'7FFFFFFF, R1 = H'00000001
;After execution: R1 = H'80000000
)"},

  exceptions
{R"(

)"},

},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "add	#imm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn + (sign extension)imm" },
  code { "0111nnnniiiiiiii" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

brief
{R"(
imm ← SignExtend 8 (s);
op2 ← SignExtend 32 (Rn );
op2 ← op2 + imm;
R n ← Register(op2);
)"},

  description
{R"(
Adds together the contents of general register Rn and the immediate value
and stores the result in Rn.  The 8-bit immediate value is sign-extended to
32 bits, which allows it to be used for immediate subtraction or decrement
operations.
)"},

  note
{R"(

)"},

  operation
{R"(
void ADDI (int i, int n)
{
  if ((i & 0x80) == 0)
    R[n] += (0x000000FF & (long)i);
  else
    R[n] += (0xFFFFFF00 | (long)i);

  PC += 2;
}
)"},

  example
{R"(
ADD #H'01,R2 ;Before execution: R2 = H'00000000
;After execution: R2 = H'00000001
ADD #H'FE,R3 ;Before execution: R3 = H'00000001
;After execution: R3 = H'FFFFFFFF
)"},

  exceptions
{R"(

)"},

},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "addc	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn + Rm + T → Rn, carry → T" },
  code { "0011nnnnmmmm1110" },
  flags { "Carry → T" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
op2 ← (op2 + op1) + t;
t ← op2< 32 FOR 1 > ;
R n ← Register(op2);
T ← Bit(t);
)"},

  description
{R"(
Adds together the contents of general registers Rn and Rm and the T bit, and
stores the result in Rn.  A carry resulting from the operation is reflected in
the T bit.  This instruction can be used to implement additions exceeding 32
bits.

)"},

  note
{R"(

)"},

  operation
{R"(
void ADDC (int m, int n)
{
  unsigned long tmp0, tmp1;
  tmp1 = R[n] + R[m];
  tmp0 = R[n];
  R[n] = tmp1 + T;

  if (tmp0>tmp1)
    T = 1;
  else
    T = 0;

  if (tmp1 > R[n])
    T = 1;

  PC += 2;
}
)"},

  example
{R"(
CLRT ;R0:R1 (64 bits) + R2:R3 (64 bits) = R0:R1 (64 bits)
ADDC R3,R1 ;Before execution: T = 0, R1 = H'00000001, R3 = H'FFFFFFFF
;After execution: T = 1, R1 = H'00000000
ADDC R2,R0 ;Before execution: T = 1, R0 = H'00000000, R2 = H'00000000
;After execution: T = 0, R0 = H'00000001
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "addv	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn + Rm → Rn, overflow → T" },
  code { "0011nnnnmmmm1111" },
  flags { "Overflow → T" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
op2 ← op2 + op1;
t ← INT ((op2 < (- 231)) OR (op2 ≥ 2 31));
R n ← Register(op2);
T ← Bit(t);
)"},

  description
{R"(
Adds together the contents of general registers Rn and Rm and stores the result
in Rn.  If overflow occurs, the T bit is set.

)"},

  note
{R"(

)"},

  operation
{R"(
void ADDV (int m, int n)
{
  long dest, src, ans;

  if ((long)R[n] >= 0)
    dest = 0;
  else
    dest = 1;

  if ((long)R[m] >= 0)
    src = 0;
  else
    src = 1;

  src += dest;
  R[n] += R[m];

  if ((long)R[n] >= 0)
    ans = 0;
  else
    ans = 1;

  ans += dest;

  if (src == 0 || src == 2)
  {
    if (ans == 1)
      T = 1;
    else
      T = 0;
  }
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
ADDV R0,R1 ;Before execution: R0 = H'00000001, R1 = H'7FFFFFFE, T = 0
;After execution: R1 = H'7FFFFFFF, T = 0
ADDV R0,R1 ;Before execution: R0 = H'00000002, R1 = H'7FFFFFFE, T = 0
;After execution: R1 = H'80000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/eq	#imm,R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If R0 = (sign extension)imm: 1 → T\nElse: 0 → T" },
  code { "10001000iiiiiiii" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
imm ← SignExtend 8(s);
t ← INT (r0 = imm);
T ← Bit(t);
)"},

  description
{R"(
Compares general register R0 and the sign-extended 8-bit immediate data and sets
the T bit if the values are equal.  If they are not equal the T bit is cleared.
The contents of R0 are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPIM (int i)
{
  long imm;

  if ((i & 0x80) == 0)
    imm = (0x000000FF & (long i));
  else
    imm = (0xFFFFFF00 | (long i));

  if (R[0] == imm)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/eq	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn = Rm: 1 → T\nElse: 0 → T" },
  code { "0011nnnnmmmm0000" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
t ← INT (op2 = op1);
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if they are equal.
The contents of Rn and Rm are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPEQ (int m, int n)
{
  if (R[n] == R[m])
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/hs	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn &ge; Rm (unsigned): 1 → T\nElse: 0 → T" },
  code { "0011nnnnmmmm0010" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
t ← INT (op2 ≥ op1);
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if Rn is greater or
equal Rm.  The values for the comparison are interpreted as unsigned integer
values.  The contents of Rn and Rm are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPHI (int m, int n)
{
  if ((unsigned long)R[n] >= (unsigned long)R[m])
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/ge	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn &ge; Rm (signed): 1 → T\nElse: 0 → T" },
  code { "0011nnnnmmmm0011" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
t ← INT (op2 ≥ op1);
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if Rn is greater or
equal Rm.  The values for the comparison are interpreted as signed integer
values.  The contents of Rn and Rm are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPGE (int m, int n)
{
  if ((long)R[n] >= (long)R[m])
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/hi	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn > Rm (unsigned): 1 → T\nElse: 0 → T" },
  code { "0011nnnnmmmm0110" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
t ← INT (op2 > op1);
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if Rn is greater Rm.
The values for the comparison are interpreted as unsigned integer values.
The contents of Rn and Rm are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPHI (int m, int n)
{
  if ((unsigned long)R[n] > (unsigned long)R[m])
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/gt	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn > Rm (signed): 1 → T\nElse: 0 → T" },
  code { "0011nnnnmmmm0111" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
t ← INT (op2 > op1);
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if Rn is greater Rm.
The values for the comparison are interpreted as signed integer values.
The contents of Rn and Rm are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPGT (int m, int n)
{
  if ((long)R[n] > (long)R[m])
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/pl	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn > 0 (signed): 1 → T\nElse: 0 → T" },
  code { "0100nnnn00010101" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
t ← INT (op1 > 0);
T ← Bit(t);
)"},

  description
{R"(
Compares general register Rn and sets the T bit if Rn is greater 0.  The value
in Rn for the comparison is interpreted as signed integer.  The contents of Rn
are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPPL (int n)
{
  if ((long)R[n] > 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/pz	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn &ge; 0 (signed): 1 → T\nElse: 0 → T" },
  code { "0100nnnn00010001" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
t ← INT (op1 ≥ 0);
T ← Bit(t);
)"},

  description
{R"(
Compares general register Rn and sets the T bit if Rn is greater or equal 0.
The value in Rn for the comparison is interpreted as signed integer.  The
contents of Rn are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void CMPPZ (int n)
{
  if ((long)R[n] >= 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "cmp/str	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn and Rm have an equal byte: 1 → T\nElse: 0 → T" },
  code { "0010nnnnmmmm1100" },
  flags { "Result → T" },

  group { SH4, "MT", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
temp ← op1 ⊕ op2;
t ← INT (temp < 0 FOR 8 > = 0);
t ← (INT (temp < 8 FOR 8 > = 0)) ∨ t;
t ← (INT (temp < 16 FOR 8 > = 0)) ∨ t;
t ← (INT (temp < 24 FOR 8 > = 0)) ∨ t;
T ← Bit(t);
)"},

  description
{R"(
Compares general registers Rn and Rm, and sets the T bit if any of the 4 bytes
in Rn are equal to the corresponding byte in Rm. The contents of Rn and Rm are
not changed.
)"},

  note
{R"(
This instruction can be used to speed up some string operations such as
finding the string length of a zero terminated string or string matching.
)"},

  operation
{R"(
void CMPSTR (int m, int n)
{
  unsigned long temp;
  long HH, HL, LH, LL;
  temp = R[n] ^ R[m];
  HH = (temp & 0xFF000000) >> 24;
  HL = (temp & 0x00FF0000) >> 16;
  LH = (temp & 0x0000FF00) >> 8;
  LL = temp & 0x000000FF;
  HH = HH && HL && LH && LL;

  if (HH == 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
cmp/str  r2,r3    ! r2 = "ABCD", r3 = "XYCZ"
bt       target   ! T = 1, so branch is taken.
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clips.b	Rn",
  SH2A,
  abstract { "If Rn > 0x0000007F: 0x0000007F → Rn, 1 → CS\nIf Rn < 0xFFFFFF80: 0xFFFFFF80 → Rn, 1 → CS" },
  code { "0100nnnn10010001" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Determines saturation. Signed data is used with this instruction. The saturation
upper-limit value is stored in general register Rn if the contents of Rn exceed
the saturation upper-limit value, or the saturation lower-limit value is stored
in Rn if the contents of Rn are less than the saturation lower-limit value, and
the CS bit is set to 1.
The saturation upper-limit value is 0x0000007F (127).
The saturation lower-limit value is 0xFFFFFF80 (-128).
)"},

  note
{R"(
The CS bit value does not change if the contents of general register Rn do not
exceed the saturation upper-limit value or are not less than the saturation
lower-limit value.
)"},

  operation
{R"(
void CLIPSB (int n)
{
  if (R[n] > 0x0000007F)
  {
    R[n] = 0x0000007F;
    CS = 1;
  }
  else if (R[n] < 0xFFFFFF80)
  {
    R[n] = 0xFFFFFF80;
    CS = 1;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clips.w	Rn",
  SH2A,
  abstract { "If Rn > 0x00007FFF: 0x00007FFF → Rn, 1 → CS\nIf Rn < 0xFFFF8000: 0xFFFF8000 → Rn, 1 → CS" },
  code { "0100nnnn10010101" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Determines saturation. Signed data is used with this instruction. The saturation
upper-limit value is stored in general register Rn if the contents of Rn exceed
the saturation upper-limit value, or the saturation lower-limit value is stored
in Rn if the contents of Rn are less than the saturation lower-limit value, and
the CS bit is set to 1.
The saturation upper-limit value is 0x00007FFF (32767).
The saturation lower-limit value is 0xFFFF8000 (-32768).
)"},

  note
{R"(
The CS bit value does not change if the contents of general register Rn do not
exceed the saturation upper-limit value or are not less than the saturation
lower-limit value.
)"},

  operation
{R"(
void CLIPSW (int n)
{
  if (R[n] > 0x00007FFF)
  {
    R[n] = 0x00007FFF;
    CS = 1;
  }
  else if (R[n] < 0xFFFF8000)
  {
    R[n] = 0xFFFF8000;
    CS = 1;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clipu.b	Rn",
  SH2A,
  abstract { "If Rn > 0x000000FF: 0x000000FF → Rn, 1 → CS" },
  code { "0100nnnn10000001" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Determines saturation. Unsigned data is used with this instruction. If the
contents of general register Rn exceed the saturation value, the saturation
value is stored in Rn and the CS bit is set to 1.
The saturation value is 0x000000FF (255).
)"},

  note
{R"(
The CS bit value does not change if the contents of general register Rn do not
exceed the saturation upper-limit value.
)"},

  operation
{R"(
void CLIPUB (int n)
{
  if (R[n] > 0x000000FF)
  {
    R[n] = 0x000000FF;
    CS = 1;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clipu.w	Rn",
  SH2A,
  abstract { "If Rn > 0x0000FFFF: 0x0000FFFF → Rn, 1 → CS" },
  code { "0100nnnn10000101" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Determines saturation. Unsigned data is used with this instruction. If the
contents of general register Rn exceed the saturation value, the saturation
value is stored in Rn and the CS bit is set to 1.
The saturation value is 0x0000FFFF (65535).
)"},

  note
{R"(
The CS bit value does not change if the contents of general register Rn do not
exceed the saturation upper-limit value.
)"},

  operation
{R"(
void CLIPUW (int n)
{
  if (R[n] > 0x0000FFFF)
  {
    R[n] = 0x0000FFFF;
    CS = 1;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "div0s	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "MSB of Rn → Q, MSB of Rm → M, M ^ Q → T" },
  code { "0010nnnnmmmm0111" },
  flags { "Result → T" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
q ← op2< 31 FOR 1 > ;
m ← op1 < 31 FOR 1 > ;
t ← m ⊕ q;
Q ← Bit(q);
M ← Bit(m);
T ← Bit(t);
)"},

  description
{R"(
Performs initial settings for signed division. This instruction is followed by
a DIV1 instruction that executes 1-digit division, for example, and repeated
division steps are executed to find the quotient. See the description of the
DIV1 instruction for details.
)"},

  note
{R"(
This instruction can also be used to compare the signs of Rm and Rn.  If the
signs of Rm and Rn are equal, T will be set to 0.  If the signs of Rm and Rn
are not equal, T will be set to 1.
)"},

  operation
{R"(
void DIV0S (int m, int n)
{
  if ((R[n] & 0x80000000) == 0)
    Q = 0;
  else
    Q = 1;

  if ((R[m] & 0x80000000) == 0)
    M = 0;
  else
    M = 1;

  T = ! (M == Q);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "div0u",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 → M, 0 → Q, 0 → T" },
  code { "0000000000011001" },
  flags { "0 → T" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
q ← 0;
m ← 0;
t ← 0;
Q ← Bit(q);
M ← Bit(m);
T ← Bit(t);
)"},

  description
{R"(
Performs initial settings for unsigned division. This instruction is followed by
a DIV1 instruction that executes 1-digit division, for example, and repeated
division steps are executed to find the quotient. See the description of the
DIV1 instruction for details.
)"},

  note
{R"(

)"},

  operation
{R"(
void DIV0U (void)
{
  M = Q = T = 0;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "div1	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "1-step division (Rn / Rm)" },
  code { "0011nnnnmmmm0100" },
  flags { "Result → T" },

  group { SH4, "EX", SH4A, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
q ← ZeroExtend 1(Q);
m ← ZeroExtend 1(M);
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
oldq ← q;
q ← op2< 31 FOR 1 > ;
op2 ← ZeroExtend 32(op2 << 1) ∨ t;
IF (oldq = m)
op2 ← op2 - op1;
ELSE
op2 ← op2 + op1;
q ← (q ⊕ m) ⊕ op2< 32 FOR 1 > ;
t ← 1 - (q ⊕ m);
R n ← Register(op2);
Q ← Bit(q);
T ← Bit(t);
)"},

  description
{R"(
Performs 1-digit division (1-step division) of the 32-bit contents of general
register Rn (dividend) by the contents of Rm (divisor).
The quotient is obtained by repeated execution of this instruction alone or in
combination with other instructions. The specified registers and the M, Q, and
T bits must not be modified during these repeated executions.
<br /><br />
In 1-step division, the dividend is shifted 1 bit to the left, the divisor is
subtracted from this, and the quotient bit is reflected in the Q bit according
to whether the result is positive or negative.
<br /><br />
Detection of division by zero or overflow is not provided. Check for division by
zero and overflow division before executing the division. A remainder operation
is not provided. Find the remainder by finding the product of the divisor and
the obtained quotient, and subtracting this value from the dividend:
<br /><br />
<code>remainder = dividend - (divisor × quotient)</code>
<br /><br />
Initial settings should first be made with the DIV0S or DIV0U instruction. DIV1
is executed once for each bit of the divisor. If a quotient of more than 17
bits is required, place an ROTCL instruction before the DIV1 instruction. See
the examples for details of the division sequence.
)"},

  note
{R"(

)"},

  operation
{R"(
void DIV1 (int m, int n)
{
  unsigned long tmp0, tmp2;
  unsigned char old_q, tmp1;

  old_q = Q;
  Q = (0x80000000 & R[n]) != 0;
  tmp2 = R[m];
  R[n] <<= 1;
  R[n] |= (unsigned long)T;

  if (old_q == 0)
  {
    if (M == 0)
    {
      tmp0 = R[n];
      R[n] -= tmp2;
      tmp1 = R[n] > tmp0;

      if (Q == 0)
        Q = tmp1;
      else if (Q == 1)
        Q = tmp1 == 0;
    }

    else if (M == 1)
    {
      tmp0 = R[n];
      R[n] += tmp2;
      tmp1 = R[n] < tmp0;

      if (Q == 0)
        Q = tmp1 == 0;
      else if (Q == 1)
        Q = tmp1;
    }
  }

  else if (old_q == 1)
  {
    if (M == 0)
    {
      tmp0 = R[n];
      R[n] += tmp2;
      tmp1 = R[n] < tmp0;

      if (Q == 0)
        Q = tmp1;
      else if (Q == 1)
        Q = tmp1 == 0;
    }

    else if (M == 1)
    {
       tmp0 = R[n];
       R[n] -= tmp2;
       tmp1 = R[n] > tmp0;

       if (Q == 0)
         Q = tmp1 == 0;
       else if (Q == 1)
         Q = tmp1;
    }
  }

  T = (Q == M);
  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
! r1 (32 bits) / r0 (16 bits) = r1 (16 bits)  (unsigned)

shll16  r0        ! Set divisor in upper 16 bits, clear lower 16 bits to 0

tst     r0,r0     ! Check for division by zero
bt      zero_div

cmp/hs  r0,r1     ! Check for overflow
bt      over_div

div0u             ! Flag initialization

.rept 16
div1    r0,r1     ! Repeat 16 times
.endr

rotcl   r1
extu.w  r1,r1     ! r1 = quotient

- - - - - - - - - - - - - - - -

! r1:r2 (64 bits) / r0 (32 bits) = r2 (32 bits)  (unsigned)

tst     r0,r0     ! Check for division by zero
bt      zero_div

cmp/hs  r0,r1     ! Check for overflow
bt      over_div

div0u             ! Flag initialization

.rept 32
rotcl   r2        ! Repeat 32 times
div1    r0,r1
.endr

rotcl   r2        ! r2 = quotient

- - - - - - - - - - - - - - - -

! r1 (16 bits) / r0 (16 bits) = r1 (16 bits)  (signed)

shll16  r0        ! Set divisor in upper 16 bits, clear lower 16 bits to 0
exts.w  r1,r1     ! Dividend sign-extended to 32 bits
mov     #0,r2
mov     r1,r3
rotcl   r3
subc    r2,r1     ! If dividend is negative, subtract 1
div0s   r0,r1     ! Flag initialization

.rept 16
div1    r0,r1     ! Repeat 16 times
.endr

exts.w  r1,r1
rotcl   r1        ! r1 = quotient (one's complement notation)
addc    r2,r1     ! If MSB of quotient is 1, add 1 to convert to two's complement notation
exts.w  r1,r1     ! r1 = quotient (two's complement notation)

- - - - - - - - - - - - - - - -

! r2 (32 bits) / r0 (32 bits) = r2 (32 bits)  (signed)

mov     r2,r3
rotcl   r3
subc    r1,r1     ! Dividend sign-extended to 64 bits (r1:r2)
mov     #0,r3
subc    r3,r2     ! If dividend is negative, subtract 1 to convert to one's complement notation
div0s   r0,r1     ! Flag initialization

.rept 32
rotcl   r2        ! Repeat 32 times
div1    r0,r1
.endr

rotcl   r2        ! r2 = quotient (one's complement notation)
addc    r3,r2     ! If MSB of quotient is 1, add 1 to convert to two's complement notation
                  ! r2 = quotient (two's complement notation)

- - - - - - - - - - - - - - - -
! r4 (8 bits) / r5 (8 bits) = r0 (8 bits)  (unsigned)

extu.b  r4,r4     ! Optional, not needed if value is known to be zero extended.
extu.b  r5,r5     ! Optional, not needed if value is known to be zero extended.
shll8   r5
div0u

.rept 8
div1    r5,r4     ! Repeat 8 times
.endr

rotcl   r4
extu.b  r4,r0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "divs	R0,Rn",
  SH2A,
  abstract { "Signed, Rn / R0 → Rn\n32 / 32 → 32 bits" },
  code { "0100nnnn10010100" },

  issue { SH2A, "36" },
  latency { SH2A, "36" },



  description
{R"(
Executes division of the 32-bit contents of a general register Rn (dividend) by
the contents of R0 (divisor). This instruction executes signed division and
finds the quotient only. A remainder operation is not provided. To obtain the
remainder, find the product of the divisor and the obtained quotient, and
subtract this value from the dividend. The sign of the remainder will be the
same as that of the dividend.
)"},

  note
{R"(
An overflow exception will occur if the negative maximum value (0x00000000) is
divided by -1. If division by zero is performed a division by zero exception
will occur.
<br /><br />
If an interrupt is generated while this instruction is being executed, execution
will be halted. The return address will be the start address of this instruction,
and this instruction will be re-executed.  This avoids increased interrupt
latency.
)"},

  operation
{R"(
void DIVS (int n)
{
  R[n] = R[n] / R[0];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Overflow exception
Division by zero exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "divu	R0,Rn",
  SH2A,
  abstract { "Unsigned, Rn / R0 → Rn\n32 / 32 → 32 bits" },
  code { "0100nnnn10000100" },

  issue { SH2A, "36" },
  latency { SH2A, "36" },



  description
{R"(
Executes division of the 32-bit contents of a general register Rn (dividend) by
the contents of R0 (divisor). This instruction executes unsigned division and
finds the quotient only. A remainder operation is not provided. To obtain the
remainder, find the product of the divisor and the obtained quotient, and
subtract this value from the dividend.
)"},

  note
{R"(
A division by zero exception will occur if division by zero is performed.
<br /><br />
If an interrupt is generated while this instruction is being executed, execution
will be halted. The return address will be the start address of this instruction,
and this instruction will be re-executed.  This avoids increased interrupt
latency.
)"},

  operation
{R"(
void DIVU (int n)
{
  R[n]= (unsigned long)R[n] / (unsigned long)R[0];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Division by zero exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dmuls.l	Rm,Rn",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  abstract { "Signed, Rn × Rm → MACH:MACL\n32 × 32 → 64 bits" },
  code { "0011nnnnmmmm1101" },

  group { SH4, "CO", SH4A, "EX" },
  issue { SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH2, "2-4", SH3, "2-5", SH4A, "2", SH2A, "3", SH4, "4/4" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
mac ← op2 × op1;
macl ← mac;
mach ← mac >> 32;
MACL ← ZeroExtend 32 (macl);
MACH ← ZeroExtend 32 (mach);
)"},

  description
{R"(
Performs 32-bit multiplication of the contents of general register Rn by the
contents of Rm, and stores the 64-bit result in the MACH and MACL registers.
The multiplication is performed as a signed arithmetic operation.
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.
)"},

  operation
{R"(
void DMULS (int m, int n)
{
  unsigned long RnL, RnH, RmL, RmH, Res0, Res1, Res2;
  unsigned long temp0, temp1, temp2, temp3;
  long tempm, tempn, fnLmL;

  tempn = (long)R[n];
  tempm = (long)R[m];

  if (tempn < 0)
    tempn = 0 - tempn;

  if (tempm < 0)
    tempm = 0 - tempm;

  if ((long)(R[n] ^ R[m]) < 0)
    fnLmL = -1;
  else
    fnLmL = 0;

  temp1 = (unsigned long)tempn;
  temp2 = (unsigned long)tempm;

  RnL = temp1 & 0x0000FFFF;
  RnH = (temp1 >> 16) & 0x0000FFFF;

  RmL = temp2 & 0x0000FFFF;
  RmH = (temp2 >> 16) & 0x0000FFFF;

  temp0 = RmL * RnL;
  temp1 = RmH * RnL;
  temp2 = RmL * RnH;
  temp3 = RmH * RnH;

  Res2 = 0;
  Res1 = temp1 + temp2;
  if (Res1 < temp1)
    Res2 += 0x00010000;

  temp1 = (Res1 << 16) & 0xFFFF0000;
  Res0 = temp0 + temp1;
  if (Res0 < temp0)
    Res2++;

  Res2 = Res2 + ((Res1 >> 16) & 0x0000FFFF) + temp3;

  if (fnLmL < 0)
  {
    Res2 = ~Res2;
    if (Res0 == 0)
      Res2++;
    else
      Res0 = (~Res0) + 1;
  }

  MACH = Res2;
  MACL = Res0;
  PC += 2;
}
)"},

  example
{R"(
DMULS.L R0,R1 ;Before execution: R0 = H'FFFFFFFE, R1 = H'00005555
;After execution: MACH = H'FFFFFFFF, MACL = H'FFFF5556
STS MACH,R0 ;Operation result (top)
STS MACL,R0 ;Operation result (bottom)
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dmulu.l	Rm,Rn",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  abstract { "Unsigned, Rn × Rm → MACH:MACL\n32 × 32 → 64 bits" },
  code { "0011nnnnmmmm0101" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH2, "2-4", SH3, "2-5", SH4A, "2", SH2A, "2", SH4, "4/4" },

  brief
{R"(
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
mac ← op2 × op1;
macl ← mac;
mach ← mac >> 32;
MACL ← ZeroExtend 32 (macl);
MACH ← ZeroExtend 32 (mach);
)"},

  description
{R"(
Performs 32-bit multiplication of the contents of general register Rn by the
contents of Rm, and stores the 64-bit result in the MACH and MACL registers.
The multiplication is performed as an unsigned arithmetic operation.
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.
)"},

  operation
{R"(
void DMULU (int m, int n)
{
  unsigned long RnL, RnH, RmL, RmH, Res0, Res1, Res2;
  unsigned long temp0, temp1, temp2, temp3;

  RnL = R[n] & 0x0000FFFF;
  RnH = (R[n] >> 16) & 0x0000FFFF;

  RmL = R[m] & 0x0000FFFF;
  RmH = (R[m] >> 16) & 0x0000FFFF;

  temp0 = RmL * RnL;
  temp1 = RmH * RnL;
  temp2 = RmL * RnH;
  temp3 = RmH * RnH;

  Res2 = 0
  Res1 = temp1 + temp2;
  if (Res1 < temp1)
    Res2 += 0x00010000;

  temp1 = (Res1 << 16) & 0xFFFF0000;
  Res0 = temp0 + temp1;
  if (Res0 < temp0)
    Res2++;

  Res2 = Res2 + ((Res1 >> 16) & 0x0000FFFF) + temp3;

  MACH = Res2;
  MACL = Res0;
  PC += 2;
}
)"},

  example
{R"(
DMULU.L R0,R1 ;Before execution: R0 = H'FFFFFFFE, R1 = H'00005555
;After execution: MACH = H'FFFFFFFF, MACL = H'FFFF5556
STS MACH,R0 ;Operation result (top)
STS MACL,R0 ;Operation result (bottom
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dt	Rn",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  abstract { "Rn-1 → Rn\nIf Rn = 0: 1 → T\nElse: 0 → T" },
  code { "0100nnnn00010000" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
op1 ← op1 - 1;
t ← INT (op1 = 0);
R n ← Register(op1);
T ← Bit(t);
)"},

  description
{R"(
Decrements the contents of general register Rn by 1 and compares the result
with zero. If the result is zero, the T bit is set to 1.
If the result is nonzero, the T bit is cleared to 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void DT (int n)
{
  R[n]--;

  if (R[n] == 0)
    T = 1;
  else T = 0;

  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
    mov   #4,r4      ! Set loop count
loop:
    add   r0,r1
    dt    r5         ! Decrement r5 value and check for 0.
    bf    loop       ! if T = 0 branch to loop
                     ! (in this example, 4 loop iterations are executed)
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "exts.b	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm sign-extended from byte → Rn" },
  code { "0110nnnnmmmm1110" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 8(R m );
op2 ← op1;
R n ← Register(op2);
)"},

  description
{R"(
Sign-extends the contents of general register Rm and stores the result in Rn.
The value of Rm bit 7 is transferred to Rn bits 8 to 31.
)"},

  note
{R"(

)"},

  operation
{R"(
void EXTSB (int m, int n)
{
  R[n] = R[m];

  if ((R[m] & 0x00000080) == 0)
    R[n] & = 0x000000FF;
  else
    R[n] |= 0xFFFFFF00;

  PC += 2;
}
)"},

  example
{R"(
EXTS.B R0,R1 ;Before execution: R0 = H'00000080
;After execution: R1 = H'FFFFFF80
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "exts.w	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm sign-extended from word → Rn" },
  code { "0110nnnnmmmm1111" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 16 (Rm );
op2 ← op1;
R n ← Register(op2);
)"},

  description
{R"(
Sign-extends the contents of general register Rm and stores the result in Rn.
The value of Rm bit 15 is transferred to Rn bits 16 to 31.
)"},

  note
{R"(

)"},

  operation
{R"(
void EXTSW (int m, int n)
{
  R[n] = R[m];

  if ((R[m] & 0x00008000) == 0)
    R[n] & = 0x0000FFFF;
  else
    R[n] |= 0xFFFF0000;

  PC += 2;
}
)"},

  example
{R"(
EXTS.W R0,R1 ;Before execution: R0 = H'00008000
;After execution: R1 = H'FFFF8000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "extu.b	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm zero-extended from byte → Rn" },
  code { "0110nnnnmmmm1100" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 8(Rm );
op2 ← op1;
R n ← Register(op2);
)"},

  description
{R"(
Zero-extends the contents of general register Rm and stores the result in Rn.
0 is transferred to Rn bits 8 to 31.
)"},

  note
{R"(

)"},

  operation
{R"(
void EXTUB (int m, int n)
{
  R[n] = R[m];
  R[n] &= 0x000000FF;
  PC += 2;
}
)"},

  example
{R"(
EXTU.B R0,R1 ;Before execution: R0 = H'FFFFFF80
;After execution: R1 = H'00000080
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "extu.w	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm zero-extended from word → Rn" },
  code { "0110nnnnmmmm1101" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 16(R m);
op2 ← op1;
R n ← Register(op2)
)"},

  description
{R"(
Zero-extends the contents of general register Rm and stores the result in Rn.
0 is transferred to Rn bits 16 to 31.
)"},

  operation
{R"(
void EXTUW (int m, int n)
{
  R[n] = R[m];
  R[n] &= 0x0000FFFF;
  PC += 2;
}
)"},

  note
{R"(

)"},

  example
{R"(
EXTU.W R0,R1 ;Before execution: R0 = H'FFFF8000
;After execution: R1 = H'00008000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mac.l	@Rm+,@Rn+",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  abstract { "Signed, (Rn) × (Rm) + MAC → MAC\n32 × 32 + 64 → 64 bits" },
  code { "0000nnnnmmmm1111" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH2, "2", SH3, "2", SH4A, "2", SH2A, "4", SH4, "2" },
  latency { SH2, "2-4", SH3, "2-5", SH4A, "5", SH2A, "5", SH4, "2/4" },

  brief
{R"(
macl ← ZeroExtend 32 (MACL);
mach ← ZeroExtend 32(MACH);
s ← ZeroExtend 1 (S);
m_field ← ZeroExtend 4(m);
n_field ← ZeroExtend 4(n);
m_address ← SignExtend 32 (Rm );
n_address ← SignExtend 32 (Rn);
value2 ← SignExtend 32 (ReadMemory 32 (ZeroExtend 32 (n_address)));
n_address ← n_address + 4;
IF (n_field = m_field)
{
m_address ← m_address + 4;
n_address ← n_address + 4;
}
value1 ← SignExtend 32 (ReadMemory 32 (ZeroExtend 32 (m_address)));
m_address ← m_address + 4;
mul ← value2 × value1;
mac ← (mach << 32) + macl;
result ← mac + mul;
IF (s = 1)
IF (((result ⊕ mac) ∧ (result ⊕ mul)) < 63 FOR 1 > = 1)
IF (mac < 63 FOR 1 > = 0)
result ← 247 - 1;
ELSE
result ← - 247 ;
ELSE
result ← SignedSaturate 48 (result);
macl ← result;
mach ← result >> 32;
R m ← Register(m_address);
R n ← Register(n_address);
MACL ← ZeroExtend 32 (macl);
MACH ← ZeroExtend 32 (mach);
)"},
  description
{R"(
Performs signed multiplication of the 32-bit operands whose addresses are the
contents of general registers Rm and Rn, adds the 64-bit result to the MAC
register contents, and stores the result in the MAC register.
Operands Rm and Rn are each incremented by 4 each time they are read.
<br /><br />
When the S bit is cleared to 0, the 64-bit result is stored in the coupled MACH
and MACL registers.
<br /><br />
When bit S is set to 1, addition to the MAC register is a saturation operation
of 48 bits starting from the LSB. For the saturation operation, only the lower
48 bits of the MACL register are enabled and the result is limited to a range
of 0xFFFF800000000000 (minimum) and 0x00007FFFFFFFFFFF (maximum).
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.  In the case of consecutive executions of
MAC.W/MAC.L, the latency is decreased to 2 cycles.
)"},

  operation
{R"(
void MACL (int m, int n)
{
  unsigned long RnL, RnH, RmL, RmH, Res0, Res1, Res2;
  unsigned long temp0, temp1, temp2, temp3;
  long tempm, tempn, fnLmL;

  tempn = Read_32 (R[n]);
  R[n] += 4;
  tempm = Read_32 (R[m]);
  R[m] += 4;

  if ((long)(tempn ^ tempm) < 0)
    fnLmL = -1;
  else
    fnLmL = 0;

  if (tempn < 0)
    tempn = 0 - tempn;
  if (tempm < 0)
    tempm = 0 - tempm;

  temp1 = (unsigned long)tempn;
  temp2 = (unsigned long)tempm;

  RnL = temp1 & 0x0000FFFF;
  RnH = (temp1 >> 16) & 0x0000FFFF;
  RmL = temp2 & 0x0000FFFF;
  RmH = (temp2 >> 16) & 0x0000FFFF;
  temp0 = RmL * RnL;
  temp1 = RmH * RnL;
  temp2 = RmL * RnH;
  temp3 = RmH * RnH;

  Res2 = 0;

  Res1 = temp1 + temp2;
  if (Res1 < temp1)
    Res2 += 0x00010000;

  temp1 = (Res1 << 16) & 0xFFFF0000;

  Res0 = temp0 + temp1;
  if (Res0 < temp0)
    Res2++;

  Res2 = Res2 + ((Res1 >> 16) & 0x0000FFFF) + temp3;

  if(fnLmL < 0)
  {
    Res2 = ~Res2;
    if (Res0 == 0)
      Res2++;
    else
      Res0 = (~Res0) + 1;
  }

  if (S == 1)
  {
    Res0 = MACL + Res0;
    if (MACL > Res0)
      Res2++;

    Res2 += MACH & 0x0000FFFF;

    if (((long)Res2 < 0) && (Res2 < 0xFFFF8000))
    {
      Res2 = 0xFFFF8000;
      Res0 = 0x00000000;
    }

    if (((long)Res2 > 0) && (Res2 > 0x00007FFF))
    {
      Res2 = 0x00007FFF;
      Res0 = 0xFFFFFFFF;
    }

    MACH = (Res2 & 0x0000FFFF) | (MACH & 0xFFFF0000);
    MACL = Res0;
  }
  else
  {
    Res0 = MACL + Res0;
    if (MACL > Res0)
      Res2 ++;

    Res2 += MACH;
    MACH = Res2;
    MACL = Res0;
  }

  PC += 2;
}
)"},

  example
{R"(
MOVA TBLM,R0 ;Table address
MOV R0,R1 ;
MOVA TBLN,R0 ;Table address
CLRMAC ;MAC register initialization
MAC.L @R0+,@R1+ ;
MAC.L @R0+,@R1+ ;
STS MACL,R0 ;Store result into R0
...............
.align 2 ;
TBLM
.data.l H'1234ABCD ;
.data.l H'5678EF01 ;
TBLN
.data.l H'0123ABCD ;
.data.l H'4567DEF0 ;
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mac.w	@Rm+,@Rn+",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Signed, (Rn) × (Rm) + MAC → MAC\nSH1: 16 × 16 + 42 → 42 bits\nOther: 16 × 16 + 64 → 64 bits" },
  code { "0100nnnnmmmm1111" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "2", SH2A, "3", SH4, "2" },
  latency { SH1, "2-3", SH2, "2-3", SH3, "2-5", SH4A, "4", SH2A, "4", SH4, "2/4" },

  brief
{R"(
macl ← ZeroExtend 32 (MACL);
mach ← ZeroExtend 32(MACH);
s ← ZeroExtend 1 (S);
m_field ← ZeroExtend 4(m);
n_field ← ZeroExtend 4(n);
m_address ← SignExtend 32 (Rm );
n_address ← SignExtend 32 (Rn);
value2 ← SignExtend 16 (ReadMemory 16 (ZeroExtend 32 (n_address)));
n_address ← n_address + 2;
IF (n_field = m_field)
{
m_address ← m_address + 2;
n_address ← n_address + 2;
}
value1 ← SignExtend 16 (ReadMemory 16 (ZeroExtend 32 (m_address)));
m_address ← m_address + 2;
mul ← value2 × value1;
IF (s = 1)
{
macl ← SignExtend 32(macl) + mul;
temp ← SignedSaturate 32 (macl);
IF (macl = temp)
result ← (mach << 32) ∨ ZeroExtend 32 (macl);
ELSE
result ← (0x1 << 32) ∨ ZeroExtend 32 (temp);
}
ELSE
result ← ((mach << 32) + macl) + mul;
macl ← result;
mach ← result >> 32;
R m ← Register(m_address);
R n ← Register(n_address);
MACL ← ZeroExtend 32 (macl);
MACH ← ZeroExtend 32 (mach);
)"},
  description
{R"(
Performs signed multiplication of the 16-bit operands whose addresses are the
contents of general registers Rm and Rn, adds the 32-bit result to the MAC
register contents, and stores the result in the MAC register.
Operands Rm and Rn are each incremented by 2 each time they are read.
<br /><br />
If the S bit is 0, a 16 × 16 + 64 → 64-bit multiply-and-accumulate operation
is performed, and the 64-bit result is stored in the linked MACH and MACL
registers.
<br /><br />
If the S bit is 1, a 16 × 16 + 32 → 32-bit multiply-and-accumulate operation is
performed, and the addition to the MAC register contents is a saturation
operation. In a saturation operation, only the MACL register is valid, and the
result range is limited to 0x80000000 (minimum value) to 0x7FFFFFFF
(maximum value). If overflow occurs, the LSB of the MACH register is set to 1.
0x80000000 (minimum value) is stored in the MACL register if the result
overflows in the negative direction, and 0x7FFFFFFF (maximum value) is stored
if the result overflows in the positive direction
)"},

  note
{R"(
When the S bit is 0, the SH2 and SH-DSP CPU perform a 16 × 16 + 64 → 64 bit
multiply and accumulate operation and the SH1 CPU performs a 16 × 16 + 42 →
42 bit multiply and accumulate operation.
<br /><br />
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.  In the case of consecutive executions of
MAC.W/MAC.L, the latency is decreased to 2 cycles.
)"},

  operation
{R"(
void MACW (int m, int n)
{
  long tempm, tempn, dest, src, ans;
  unsigned long templ;

  tempn = Read_16 (R[n]);
  R[n] += 2;
  tempm = Read_16 (R[m]);
  R[m] += 2;

  templ = MACL;
  tempm = ((long)(short)tempn * (long)(short)tempm);

  if ((long)MACL >= 0)
    dest = 0;
  else
    dest = 1;

  if ((long)tempm >= 0)
  {
    src = 0;
    tempn = 0;
  }
  else
  {
    src = 1;
    tempn = 0xFFFFFFFF;
  }

  src += dest;
  MACL += tempm;

  if ((long)MACL >= 0)
    ans = 0;
  else
    ans = 1;

  ans += dest;

  if (S == 1)
  {
    if (ans == 1)
    {
      #if SH1
      if (src == 0 || src == 2)
        MACH |= 0x00000001;
      #endif

      if (src == 0)
        MACL = 0x7FFFFFFF;
      if (src == 2)
        MACL = 0x80000000;
    }
  }
  else
  {
    MACH += tempn;
    if (templ > MACL)
      MACH += 1;

    #if SH1
    if ((MACH & 0x00000200) == 0)
      MACH &= 0x000003FF;
    else
      MACH |= 0xFFFFFC00;
    #endif
  }

  PC += 2;
}
)"},

  example
{R"(
MOVA TBLM,R0 ;Table address
MOV R0,R1 ;
MOVA TBLN,R0 ;Table address
CLRMAC ;MAC register initialization
MAC.W @R0+,@R1+ ;
MAC.W @R0+,@R1+ ;
STS MACL,R0 ;Store result into R0
...............
.align 2 ;
TBLM
.data.w H'1234 ;
.data.w H'5678 ;
TBLN
.data.w H'0123 ;
.data.w H'4567 ;
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mul.l	Rm,Rn",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  abstract { "Rn × Rm → MACL\n32 × 32 → 32 bits" },
  code { "0000nnnnmmmm0111" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH2, "2-4", SH3, "2-4", SH4A, "2", SH2A, "3", SH4, "4/4" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
macl ← op1 × op2;
MACL ← ZeroExtend 32 (macl);
)"},
  description
{R"(
Performs 32-bit multiplication of the contents of general registers Rn and Rm,
and stores the lower 32 bits of the result in the MACL register. The contents
of MACH are not changed.
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.
)"},

  operation
{R"(
void MULL (int m, int n)
{
  MACL = R[n] * R[m];
  PC += 2;
}
)"},

  example
{R"(
MULL R0,R1 ;Before execution: R0 = H'FFFFFFFE, R1 = H'00005555
;After execution: MACL = H'FFFF5556
STS MACL,R0 ;Operation result
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mulr	R0,Rn",
  SH2A,
  abstract { "R0 × Rn → Rn\n32 × 32 → 32 bits" },
  code { "0100nnnn10000000" },

  issue { SH2A, "2" },
  latency { SH2A, "4" },



  description
{R"(
Performs 32-bit multiplication of the contents of general register R0 by Rn,
and stores the lower 32 bits of the result in general register Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void MULR (int n)
{
  R[n] = R[0] * R[n];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "muls.w	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Signed, Rn × Rm → MACL\n16 × 16 → 32 bits" },
  code { "0010nnnnmmmm1111" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1-3", SH2, "1-3", SH3, "1-3", SH4A, "1", SH2A, "2", SH4, "4/4" },

  brief
{R"(
op1 ← SignExtend 16 (SignExtend32 (Rm ));
op2 ← SignExtend 16 (SignExtend32 (Rn));
macl ← op1 × op2;
MACL ← ZeroExtend 32 (macl);
)"},
  description
{R"(
Performs 16-bit multiplication of the contents of general registers Rn and Rm,
and stores the 32-bit result in the MACL register. The multiplication is
performed as a signed arithmetic operation. The contents of MACH are not
changed.
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.
)"},

  operation
{R"(
void MULS (int m, int n)
{
  MACL = ((long)(short)R[n] * (long)(short)R[m]);
  PC += 2;
}
)"},

  example
{R"(
MULS R0,R1 ;Before execution: R0 = H'FFFFFFFE, R1 = H'00005555
;After execution: MACL = H'FFFF5556
STS MACL,R0 ;Operation result
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "mulu.w	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Unsigned, Rn × Rm → MACL\n16 × 16 → 32 bits" },
  code { "0010nnnnmmmm1110" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1-3", SH2, "1-3", SH3, "1-3", SH4A, "1", SH2A, "2", SH4, "4/4" },

  brief
{R"(
op1 ← ZeroExtend 16(SignExtend 32(R m));
op2 ← ZeroExtend 16(SignExtend 32(R n));
macl ← op1 × op2;
MACL ← ZeroExtend 32 (macl);
)"},
  description
{R"(
Performs 16-bit multiplication of the contents of general registers Rn and Rm,
and stores the 32-bit result in the MACL register. The multiplication is
performed as an unsigned arithmetic operation. The contents of MACH are not
changed.
)"},

  note
{R"(
On SH4, when MAC*/MUL* is followed by an STS.L MAC*,@-Rn instruction, the
latency of MAC*/MUL* is 5 cycles.
)"},

  operation
{R"(
void MULU (int m, int n)
{
  MACL = ((unsigned long)(unsigned short)R[n]* (unsigned long)(unsigned short)R[m];
  PC += 2;
}
)"},

  example
{R"(
MULU R0,R1 ;Before execution: R0 = H'00000002, R1 = H'FFFFAAAA
;After execution: MACL = H'00015554
STS MACL,R0 ;Operation result
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "neg	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 - Rm → Rn" },
  code { "0110nnnnmmmm1011" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← - op1;
R n ← Register(op2);
)"},
  description
{R"(
Finds the two's complement of the contents of general register Rm and stores
the result in Rn. That is, it subtracts Rm from 0 and stores the result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void NEG (int m, int n)
{
  R[n] = 0 - R[m];
  PC += 2;
}
)"},

  example
{R"(
NEG R0,R1 ;Before execution: R0 = H'00000001
;After execution: R1 = H'FFFFFFFF
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "negc	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 - Rm - T → Rn, borrow → T" },
  code { "0110nnnnmmmm1010" },
  flags { "Borrow → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(R m);
op2 ← (- op1) - t;
t ← op2< 32 FOR 1 > ;
R n ← Register(op2);
T ← Bit(t);
)"},
  description
{R"(
Subtracts the contents of general register Rm and the T bit from 0 and stores
the result in Rn. A borrow resulting from the operation is reflected in the
T bit. This instruction can be  used for sign inversion of a value exceeding
32 bits.
)"},

  note
{R"(
This instruction can also be used to efficiently store the reversed T bit value
in a general register, if the MOVRT instruction is not available.
)"},

  operation
{R"(
void NEGC (int m, int n)
{
  unsigned long temp;
  temp = 0 - R[m];
  R[n] = temp - T;

  if (0 < temp)
    T = 1;
  else
    T = 0;

  if (temp < R[n])
    T = 1;

  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT

! Sign inversion of r0:r1 (64 bits)

clrt
negc   r1,r1    ! Before execution: r1 = 0x00000001, T = 0
                ! After execution: r1 = 0xFFFFFFFF, T = 1
negc   r0,r0    ! Before execution: r0 = 0x00000000, T = 1
                ! After execution: r0 = 0xFFFFFFFF, T = 1

- - - - - - - - - - - - - - - -

! Store reversed T bit in r0

mov    #-1,r1
negc   r1,r0    ! r0 = 0 - (-1) - T
                ! r0 = 1 - T
                ! Notice that T bit will be modified by the negc operation.
                ! In this case, T will be always set to 1.
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sub	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn - Rm → Rn" },
  code { "0011nnnnmmmm1000" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
op2 ← op2 - op1;
R n ← Register(op2);
)"},
  description
{R"(
Subtracts the contents of general register Rm from the contents of general
register Rn and stores the result in Rn. For immediate data subtraction,
ADD #imm,Rn should be used.
)"},

  note
{R"(

)"},

  operation
{R"(
void SUB (int m, int n)
{
  R[n] -= R[m];
  PC += 2;
}
)"},

  example
{R"(
SUB R0,R1 ;Before execution: R0 = H'00000001, R1 = H'80000000
;After execution: R1 = H'7FFFFFFF
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "subc	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn - Rm - T → Rn, borrow → T" },
  code { "0011nnnnmmmm1010" },
  flags { "Borrow → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(SignExtend 32(R m));
op2 ← ZeroExtend 32(SignExtend 32(R n));
op2 ← (op2 - op1) - t;
t ← op2< 32 FOR 1 > ;
R n ← Register(op2);
T ← Bit(t);
)"},
  description
{R"(
Subtracts the contents of general register Rm and the T bit from the contents of
general register Rn, and stores the result in Rn. A borrow resulting from the
operation is reflected in the T bit. This instruction is used for subtractions
exceeding 32 bits.
)"},

  note
{R"(
This instruction can also be used to store the T bit to all the bits of a
general register.
)"},

  operation
{R"(
void SUBC (int m, int n)
{
  unsigned long tmp0, tmp1;
  tmp1 = R[n] - R[m];
  tmp0 = R[n];
  R[n] = tmp1 - T;

  if (tmp0 < tmp1)
    T = 1;
  else
    T = 0;

  if (tmp1 < R[n])
    T = 1;

  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT

! r0:r1(64 bits) - r2:r3(64 bits) = r0:r1(64 bits)

clrt
subc   r3,r1    ! Before execution: T = 0, r1 = 0x00000000, r3 = 0x00000001
                ! After execution: T = 1, r1 = 0xFFFFFFFF
subc   r2,r0    ! Before execution: T = 1, r0 = 0x00000000, r2 = 0x00000000
                ! After execution: T = 1, r0 = 0xFFFFFFFF

- - - - - - - - - - - - - - - -

! Store T bit to all bits of r0

subc   r0,r0    ! r0 = r0 - r0 - T
                ! r0 = 0 - T
                ! Notice that the T bit is modified by the subc operation.
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "subv	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn - Rm → Rn, underflow → T" },
  code { "0011nnnnmmmm1011" },
  flags { "Underflow → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
op2 ← op2 - op1;
t ← INT ((op2 < (- 231)) OR (op2 ≥ 2 31));
R n ← Register(op2);
T ← Bit(t);
)"},
  description
{R"(
Subtracts the contents of general register Rm from the contents of general
register Rn, and stores the result in Rn. If underflow occurs, the T bit is set.
)"},

  note
{R"(

)"},

  operation
{R"(
void SUBV (int m, int n)
{
  long dest, src, ans;

  if ((long)R[n] >= 0)
    dest = 0;
  else
    dest = 1;

  if ((long)R[m] >= 0)
    src = 0;
  else
    src = 1;

  src += dest;
  R[n] -= R[m];

  if ((long)R[n] >= 0)
    ans = 0;
  else
    ans = 1;

  ans += dest;

  if (src == 1)
  {
    if (ans == 1)
      T = 1;
    else
      T = 0;
  }
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
subv   r0,r1    ! Before execution: r0 = 0x00000002, r1 = 0x80000001
                ! After execution: r1 = 0x7FFFFFFF, T = 1

subv   r2,r3    ! Before execution: r2 = 0xFFFFFFFE, r3 = 0x7FFFFFFE
                ! After execution r3 = 0x80000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Logic Operation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "and	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn & Rm → Rn" },
  code { "0010nnnnmmmm1001" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ZeroExtend 32(R n);
op2 ← op2 ∧ op1;
R n ← Register(op2);
)"},

  description
{R"(
ANDs the contents of general registers Rn and Rm and stores the result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void AND (int m, int n)
{
  R[n] &= R[m];
  PC += 2;
}
)"},

  example
{R"(
AND R0,R1 ;Before execution: R0 = H'AAAAAAAA, R1 = H'55555555
;After execution: R1 = H'00000000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "and	#imm,R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 & (zero extend)imm → R0" },
  code { "11001001iiiiiiii" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
r0 ← ZeroExtend 32(R 0);
imm ← ZeroExtend 8(i);
r0 ← r0 ∧ imm;
R 0 ← Register(r0);
)"},

  description
{R"(
ANDs the contents of general register R0 and the zero-extended immediate value
and stores the result in R0.
)"},

  note
{R"(
Since the 8-bit immediate value is zero-extended, the upper 24 bits of R0 are
always cleared to zero.
)"},

  operation
{R"(
void ANDI (int i)
{
  R[0] &= (0x000000FF & (long)i);
  PC += 2;
}
)"},

  example
{R"(
AND #H'0F,R0 ;Before execution: R0 = H'FFFFFFFF
;After execution: R0 = H'0000000F
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "and.b	#imm,@(R0,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + GBR) & (zero extend)imm → (R0 + GBR)" },
  code { "11001101iiiiiiii" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "3", SH2A, "3", SH4, "4" },
  latency { SH1, "3", SH2, "3", SH3, "3", SH4A, "3", SH4, "4" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
gbr ← SignExtend 32 (GBR);
imm ← ZeroExtend 8(i);
address ← ZeroExtend 32(r0 + gbr);
value ← ZeroExtend 8 (ReadMemory 8(address));
value ← value ∧ imm;
WriteMemory 8(address, value);
)"},

  description
{R"(
ANDs the contents of the memory byte indicated by the indirect GBR address with
the immediate value and writes the result back to the memory byte.
)"},

  note
{R"(

)"},

  operation
{R"(
void ANDM (long i)
{
  long temp = Read_8 (GBR + R[0]);
  temp &= 0x000000FF & (long)i;
  Write_8 (GBR + R[0], temp);
  PC += 2;
}
)"},

  example
{R"(
AND.B #H'80,@(R0,GBR) ;Before execution: @(R0,GBR) = H'A5
;After execution: @(R0,GBR) = H'80
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
<br />
Exceptions are checked taking a data access by this instruction as a byte load
and a byte store.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "not	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "~Rm → Rn" },
  code { "0110nnnnmmmm0111" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ~ op1;
R n ← Register(op2);
)"},
  description
{R"(
Finds the one's complement of the contents of general register Rm and stores
the result in Rn. That is, it inverts the Rm bits and stores the result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void NOT (int m, int n)
{
  R[n] = ~R[m];
  PC += 2;
}
)"},

  example
{R"(
NOT R0,R1 ;Before execution: R0 = H'AAAAAAAA
;After execution: R1 = H'55555555
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "or	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn | Rm → Rn" },
  code { "0010nnnnmmmm1011" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ZeroExtend 32(R n);
op2 ← op2 ∨ op1;
R n ← Register(op2);
)"},
  description
{R"(
ORs the contents of general registers Rn and Rm and stores the result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void OR (int m, int n)
{
  R[n] |= R[m];
  PC += 2;
}
)"},

  example
{R"(
OR R0,R1 ;Before execution: R0 = H'AAAA5555, R1 = H'55550000
;After execution: R1 = H'FFFF5555
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "or	#imm,R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 | (zero extend)imm → R0" },
  code { "11001011iiiiiiii" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
r0 ← ZeroExtend 32(R 0);
imm ← ZeroExtend 8(i);
r0 ← r0 ∨ imm;
R 0 ← Register(r0);
)"},
  description
{R"(
ORs the contents of general register R0 and the zero-extended immediate value
and stores the result in R0.
)"},

  note
{R"(
Since the 8-bit immediate value is zero-extended, the upper 24 bits of R0 are
not modified.
)"},

  operation
{R"(
void ORI (int i)
{
  R[0] |= (0x000000FF & (long)i);
  PC += 2;
}
)"},

  example
{R"(
OR #H'F0,R0 ;Before execution: R0 = H'00000008
;After execution: R0 = H'000000F8
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "or.b	#imm,@(R0,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + GBR) | (zero extend)imm → (R0 + GBR)" },
  code { "11001111iiiiiiii" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "3", SH2A, "3", SH4, "4" },
  latency { SH1, "3", SH2, "3", SH3, "3", SH4A, "3", SH2A, "2", SH4, "4" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
gbr ← SignExtend 32 (GBR);
imm ← ZeroExtend 8(i);
address ← ZeroExtend 32(r0 + gbr);
value ← ZeroExtend 8 (ReadMemory 8(address));
value ← value ∨ imm;
WriteMemory 8(address, value);
)"},
  description
{R"(
ORs the contents of the memory byte indicated by the indirect GBR address with
the immediate value and writes the result back to the memory byte.
)"},

  note
{R"(

)"},

  operation
{R"(
void ORM (int i)
{
  long temp = Read_8 (GBR + R[0]);
  temp |= (0x000000FF & (long)i);
  Write_8 (GBR + R[0], temp);
  PC += 2;
}
)"},

  example
{R"(
OR.B #H'50,@(R0,GBR) ;Before execution: @(R0,GBR) = H'A5
;After execution: @(R0,GBR) = H'F5
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
<br />
Exceptions are checked taking a data access by this instruction as a byte load
and a byte store.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "tas.b	@Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If (Rn) = 0: 1 → T\nElse: 0 → T\n1 → MSB of (Rn)" },
  code { "0100nnnn00011011" },
  flags { "Result → T" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "4", SH2A, "3", SH4, "5" },
  latency { SH1, "4", SH2, "4", SH3, "3/4", SH4A, "4", SH2A, "3", SH4, "5" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1);
OCBP(address)
value ← ZeroExtend 8 (ReadMemory 8(address));
t ← INT (value = 0);
value ← value ∨ (1 << 7);
WriteMemory 8(address, value);
T ← Bit(t);
)"},
  description
{R"(
Reads byte data from the address specified by general register Rn, and sets the
T bit to 1 if the data is 0, or clears the T bit to 0 if the data is not 0.
Then, data bit 7 is set to 1, and the data is written to the address specified
by Rn. During this operation, the bus is not released.
<br /><br />

On SH4 and SH4A this instruction purges the cache block corresponding to the
memory area specified by the contents of general register Rn.
The purge operation is executed as follows.<br />
In a purge operation, data is accessed using the contents of general register Rn
as the effective address. If there is a cache hit and the corresponding cache
block is dirty (U bit = 1), the contents of that cache block are written back to
external memory, and the cache block is then invalidated (by clearing the V bit
to 0). If there is a cache hit and the corresponding cache block is clean (U bit
= 0), the cache block is simply invalidated (by clearing the V bit to 0). A
purge is not executed in the event of a cache miss, or if the accessed memory
location is non-cacheable.
)"},

  note
{R"(
The two TAS.B memory accesses are executed automatically. Another memory access
is not executed between the two TAS.B accesses.
<br /><br />
On SH3 the destination of the TAS instruction should be placed in a
non-cacheable space when the cache is enabled.
)"},

  operation
{R"(
void TAS (int n)
{
  int temp = Read_8 (R[n]); // Bus Lock

  if (temp == 0)
    T = 1;
  else
    T = 0;

  temp |= 0x00000080;
  Write_8 (R[n], temp);  // Bus unlock
  PC += 2;
}
)"},

  example
{R"(
_LOOP: TAS.B @R7 ;R7 = 1000
BF _LOOP ;Loops until data in address 1000 is 0
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
<br />
Exceptions are checked taking a data access by this instruction as a byte load
and a byte store.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "tst	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If Rn & Rm = 0: 1 → T\nElse: 0 → T" },
  code { "0010nnnnmmmm1000" },
  flags { "Result → T" },

  group { SH4A, "EX", SH4, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
t ← INT ((op1 ∧ op2) = 0);
T ← Bit(t);
)"},
  description
{R"(
ANDs the contents of general registers Rn and Rm, and sets the T bit if the
result is zero. If the result is nonzero, the T bit is cleared. The contents of
Rn are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void TST (int m, int n)
{
  if ((R[n] & R[m]) == 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
TST R0,R0 ;Before execution: R0 = H'00000000
;After execution: T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "tst	#imm,R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If R0 & (zero extend)imm = 0: 1 → T\nElse: 0 → T" },
  code { "11001000iiiiiiii" },
  flags { "Result → T" },

  group { SH4A, "EX", SH4, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
imm ← ZeroExtend 8(i);
t ← INT ((r0 ∧ imm) = 0);
T ← Bit(t);
)"},
  description
{R"(
ANDs the contents of general register R0 and the zero-extended immediate value
and sets the T bit if the result is zero. If the result is nonzero, the T bit
is cleared. The contents of Rn are not changed.
)"},

  note
{R"(
Since the 8-bit immediate value is zero-extended, this instruction can only be
used to test the lower 8 bits of R0.
)"},

  operation
{R"(
void TSTI (int i)
{
  long temp = R[0] & (0x000000FF & (long)i);

  if (temp == 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
TST #H'80,R0 ;Before execution: R0 = H'FFFFFF7F
;After execution: T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "tst.b	#imm,@(R0,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If (R0 + GBR) & (zero extend)imm = 0: 1 → T\nElse 0: → T" },
  code { "11001100iiiiiiii" },
  flags { "Result → T" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "3", SH2A, "3", SH4, "3" },
  latency { SH1, "3", SH2, "3", SH3, "3", SH4A, "3", SH2A, "3", SH4, "3" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
gbr ← SignExtend 32 (GBR);
imm ← ZeroExtend 8(i);
address ← ZeroExtend 32(r0 + gbr);
value ← ZeroExtend 8 (ReadMemory 8(address));
t ← ((value ∧ imm) = 0);
T ← Bit(t);
)"},
  description
{R"(
ANDs the contents of the memory byte indicated by the indirect GBR address with
the zero-extended immediate value and sets the T bit if the result is zero.
If the result is nonzero, the T bit is cleared.
The contents of the memory byte are not changed.
)"},

  note
{R"(

)"},

  operation
{R"(
void TSTM (int i)
{
  long temp = Read_8 (GBR + R[0]);
  temp &= (0x000000FF & (long)i);

  if (temp == 0)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
TST.B #H'A5,@(R0,GBR) ;Before execution: @(R0,GBR) = H'A5
;After execution: T = 0
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
<br />
Exceptions are checked taking a data access by this instruction as a byte load
and a byte store.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "xor	Rm,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn ^ Rm → Rn" },
  code { "0010nnnnmmmm1010" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R m);
op2 ← ZeroExtend 32(R n);
op2 ← op2 ⊕ op1;
R n ← Register(op2);
)"},
  description
{R"(
XORs the contents of general registers Rn and Rm and stores the result in Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void XOR (long m, long n)
{
  R[n] ^= R[m];
  PC += 2;
}
)"},

  example
{R"(
XOR R0,R1 ;Before execution: R0 = H'AAAAAAAA, R1 = H'55555555
;After execution: R1 = H'FFFFFFFF
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "xor	#imm,R0",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "R0 ^ (zero extend)imm → R0" },
  code { "11001010iiiiiiii" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
r0 ← ZeroExtend 32(R 0);
imm ← ZeroExtend 8(i);
r0 ← r0 ⊕ imm;
R 0 ← Register(r0)
)"},
  description
{R"(
XORs the contents of general register R0 and the zero-extended immediate value
and stores the result in R0.
)"},

  note
{R"(
Since the 8-bit immediate value is zero-extended, the upper 24 bits of R0 are
not modified.
)"},

  operation
{R"(
void XORI (int i)
{
  R[0] ^= (0x000000FF & (long)i);
  PC += 2;
}
)"},

  example
{R"(
XOR #H'F0,R0 ;Before execution: R0 = H'FFFFFFFF
;After execution: R0 = H'FFFFFF0F
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "xor.b	#imm,@(R0,GBR)",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(R0 + GBR) ^ (zero extend)imm → (R0 + GBR)" },
  code { "11001110iiiiiiii" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "3", SH2A, "3", SH4, "4" },
  latency { SH1, "3", SH2, "3", SH3, "3", SH4A, "3", SH2A, "2", SH4, "4" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
gbr ← SignExtend 32 (GBR);
imm ← ZeroExtend 8(i);
address ← ZeroExtend 32(r0 + gbr);
value ← ZeroExtend 8 (ReadMemory 8(address));
value ← value ⊕ imm;
WriteMemory 8(address, value);
)"},
  description
{R"(
XORs the contents of the memory byte indicated by the indirect GBR address with
the immediate value and writes the result back to the memory byte.
)"},

  note
{R"(

)"},

  operation
{R"(
void XORM (int i)
{
  int temp = Read_8 (GBR + R[0]);
  temp ^= (0x000000FF & (long)i);
  Write_8 (GBR + R[0], temp);
  PC += 2;
}
)"},

  example
{R"(
XOR.B #H'A5,@(R0,GBR) ;Before execution: @(R0,GBR) = H'A5
;After execution: @(R0,GBR) = H'00
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
<br />
Exceptions are checked taking a data access by this instruction as a byte load
and a byte store.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Shift Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rotcl	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T << Rn << T" },
  code { "0100nnnn00100100" },
  flags { "MSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(R n);
op1 ← (op1 << 1) ∨ t;
t ← op1< 32 FOR 1 > ;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Rotates the contents of general register Rn one bit to the left through the
T bit, and stores the result in Rn. The bit rotated out of the operand is
transferred to the T bit.
<br /><img alt="Rotate with Carry Left operation" src="rotcl.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void ROTCL (int n)
{
  long temp;

  if ((R[n] & 0x80000000) == 0)
    temp = 0;
  else
    temp = 1;

  R[n] <<= 1;

  if (T == 1)
    R[n] |= 0x00000001;
  else
    R[n] &= 0xFFFFFFFE;

  if (temp == 1)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
ROTCL R0 ;Before execution: R0 = H'80000000, T = 0
;After execution: R0 = H'00000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rotcr	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T >> Rn >> T" },
  code { "0100nnnn00100101" },
  flags { "LSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← ZeroExtend 1(T);
op1 ← ZeroExtend 32(R n);
oldt ← t;
t ← op1< 0 FOR 1 > ;
op1 ← (op1 >> 1) ∨ (oldt << 31);
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Rotates the contents of general register Rn one bit to the right through the
T bit, and stores the result in Rn. The bit rotated out of the operand is
transferred to the T bit.
<br /><img alt="Rotate with Carry Right" src="rotcr.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void ROTCR (int n)
{
  long temp;

  if ((R[n] & 0x00000001) == 0)
    temp = 0;
  else
    temp = 1;

  R[n] >>= 1;

  if (T == 1)
    R[n] |= 0x80000000;
  else
    R[n] &= 0x7FFFFFFF;

  if (temp == 1)
    T = 1;
  else
    T = 0;

  PC += 2;
}
)"},

  example
{R"(
ROTCR R0 ;Before execution: R0 = H'00000001, T = 1
;After execution: R0 = H'80000000, T = 1
)"},

  exceptions
{R"(

)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rotl	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T << Rn << MSB" },
  code { "0100nnnn00000100" },
  flags { "MSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
t ← op1< 31 FOR 1 > ;
op1 ← (op1 << 1) ∨ t;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Rotates the contents of general register Rn one bit to the left, and stores the
result in Rn. The bit rotated out of the operand is transferred to the T bit.
<br /><img alt="Rotate Left operation" src="rotl.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void ROTL (int n)
{
  if ((R[n] & 0x80000000) == 0)
    T = 0;
  else
    T = 1;

  R[n] <<= 1;

  if (T == 1)
    R[n] |= 0x00000001;
  else
    R[n] &= 0xFFFFFFFE;

  PC += 2;
}
)"},

  example
{R"(
ROTL R0 ;Before execution: R0 = H'80000000, T = 0
;After execution: R0 = H'00000001, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rotr	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "LSB >> Rn >> T" },
  code { "0100nnnn00000101" },
  flags { "LSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
t ← op1< 0 FOR 1 > ;
op1 ← (op1 >> 1) ∨ (t << 31);
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Rotates the contents of general register Rn one bit to the right, and stores the
result in Rn. The bit rotated out of the operand is transferred to the T bit.
<br /><img alt="Rotate Right operation" src="rotr.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void ROTR (int n)
{
  if ((R[n] & 0x00000001) == 0)
    T = 0;
  else
    T = 1;

  R[n] >>= 1;

  if (T == 1)
    R[n] |= 0x80000000;
  else
    R[n] &= 0x7FFFFFFF;

  PC += 2;
}
)"},

  example
{R"(
ROTR R0 ;Before execution: R0 = H'00000001, T = 0
;After execution: R0 = H'80000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shad	Rm,Rn",
  SH2A | SH3 | SH4 | SH4A,
  abstract { "If Rm >= 0: Rn << Rm → Rn\nIf Rm < 0: Rn >> |Rm| → [MSB → Rn]" },
  code { "0100nnnnmmmm1100" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← SignExtend 32 (Rn);
shift_amount ← ZeroExtend 5(op1);
IF (op1 ≥ 0)
op2 ← op2 << shift_amount;
ELSE IF (shift_amount ≠ 0)
op2 ← op2 >> (32 - shift_amount);
ELSE IF (op2 < 0)
op2 ← - 1;
ELSE
op2 ← 0;
R n ← Register(op2);
)"},
  description
{R"(
Arithmetically shifts the contents of general register Rn. General register Rm
specifies the shift direction and the number of bits to be shifted.
<br /><br />
Rn register contents are shifted to the left if the Rm register value is
positive, and to the right if negative. In a shift to the right, the MSB is
added at the upper end.
<br /><br />
The number of bits to be shifted is specified by the lower 5 bits (bits 4 to 0)
of the Rm register. If the value is negative (MSB = 1), the Rm register is
represented as a two's complement. The left shift range is 0 to 31, and the
right shift range, 1 to 32.
<br /><img src="shad.svg" height="220" />
)"},

  note
{R"(
On SH4, if there is a load of the shift amount immediately before an SHAD/SHLD
instruction, the latency of the load is increased by 1 cycle.
)"},

  operation
{R"(
void SHAD (int m, int n)
{
  int sgn = R[m] & 0x80000000;

  if (sgn == 0)
    R[n] <<= (R[m] & 0x1F);
  else if ((R[m] & 0x1F) == 0)
  {
    if ((R[n] & 0x80000000) == 0)
      R[n] = 0;
    else
      R[n] = 0xFFFFFFFF;
  }
  else
    R[n] = (long)R[n] >> ((~R[m] & 0x1F) + 1);

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shal	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T << Rn << 0" },
  code { "0100nnnn00100000" },
  flags { "MSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
t ← op1< 31 FOR 1 > ;
op1 ← op1 << 1;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Arithmetically shifts the contents of general register Rn one bit to the left
and stores the result in Rn. The bit shifted out of the operand is transferred
to the T bit.
<br /><img alt="Shift Arithmetic Left operation" src="shal.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHAL (int n)
{
  if ((R[n] & 0x80000000) == 0)
    T = 0;
  else
    T = 1;

  R[n] <<= 1;
  PC += 2;
}
)"},

  example
{R"(
SHAL R0 ;Before execution: R0 = H'80000001, T = 0
;After execution: R0 = H'00000002, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shar	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "MSB >> Rn >> T" },
  code { "0100nnnn00100001" },
  flags { "LSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
t ← op1< 0 FOR 1 > ;
op1 ← op1 >> 1;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Arithmetically shifts the contents of general register Rn one bit to the right
and stores the result in Rn. The bit shifted out of the operand is transferred
to the T bit.
<br /><img alt="Shift Arithmetic Right operation" src="shar.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHAR (int n)
{
  long temp;

  if ((R[n] & 0x00000001) == 0)
    T = 0;
  else
    T = 1;

  if ((R[n] & 0x80000000) == 0)
    temp = 0;
  else
    temp = 1;

  R[n] >>= 1;

  if (temp == 1)
    R[n] |= 0x80000000;
  else
    R[n] &= 0x7FFFFFFF;

  PC += 2;
}
)"},

  example
{R"(
SHAR R0 ;Before execution: R0 = H'80000001, T = 0
;After execution: R0 = H'C0000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shld	Rm,Rn",
  SH2A | SH3 | SH4 | SH4A,
  abstract { "If Rm >= 0: Rn << Rm → Rn\nIf Rm < 0: Rn >> |Rm| → [0 → Rn]" },
  code { "0100nnnnmmmm1101" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
op2 ← ZeroExtend 32(R n);
shift_amount ← ZeroExtend 5(op1);
IF (op1 ≥ 0)
op2 ← op2 << shift_amount;
ELSE IF (shift_amount ≠ 0)
op2 ← op2 >> (32 - shift_amount);
ELSE
op2 ← 0;
R n ← Register(op2);
)"},
  description
{R"(
Logically shifts the contents of general register Rn. General register Rm
specifies the shift direction and the number of bits to be shifted.
<br /><br />
Rn register contents are shifted to the left if the Rm register value is
positive, and to the right if negative. In a shift to the right, 0s are added
at the upper end.
<br /><br />
The number of bits to be shifted is specified by the lower 5 bits (bits 4 to 0)
of the Rm register. If the value is negative (MSB = 1), the Rm register is
represented as a two's complement. The left shift range is 0 to 31, and the
right shift range, 1 to 32.
<br /><img src="shld.svg" height="220" />
)"},

  note
{R"(
On SH4, if there is a load of the shift amount immediately before an SHAD/SHLD
instruction, the latency of the load is increased by 1 cycle.
)"},

  operation
{R"(
void SHLD (int m, int n)
{
  int sgn = R[m] & 0x80000000;

  if (sgn == 0)
    R[n] <<= (R[m] & 0x1F);
  else if ((R[m] & 0x1F) == 0)
    R[n] = 0;
  else
    R[n] = (unsigned)R[n] >> ((~R[m] & 0x1F) + 1);

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shll	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "T << Rn << 0" },
  code { "0100nnnn00000000" },
  flags { "MSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
t ← op1< 31 FOR 1 > ;
op1 ← op1 << 1;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Logically shifts the contents of general register Rn one bit to the left and
stores the result in Rn. The bit shifted out of the operand is transferred to
the T bit.
<br /><img alt="Shift Logical Left operation" src="shll.svg" height="100" />
)"},

  note
{R"(
Effectively, the operation performed is the same as the SHAL instruction.
)"},

  operation
{R"(
void SHLL (int n)
{
  if ((R[n] & 0x80000000) == 0)
    T = 0;
  else
    T = 1;

  R[n] <<= 1;
  PC += 2;
}
)"},

  example
{R"(
SHLL R0 ;Before execution: R0 = H'80000001, T = 0
;After execution: R0 = H'00000002, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shll2	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn << 2 → Rn" },
  code { "0100nnnn00001000" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 << 2;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 2 bits to the left and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Left 2 Bits operation" src="shll2.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLL2 (int n)
{
  R[n] <<= 2;
  PC += 2;
}
)"},

  example
{R"(
SHLL2 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'48D159E0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shll8	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn << 8 → Rn" },
  code { "0100nnnn00011000" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 << 8;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 8 bits to the left and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Left 8 Bits operation" src="shll8.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLL8 (int n)
{
  R[n] <<= 8;
  PC += 2;
}
)"},

  example
{R"(
SHLL8 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'34567800
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shll16	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn << 16 → Rn" },
  code { "0100nnnn00101000" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 << 16;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 16 bits to the left and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Left 16 Bits operation" src="shll16.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLL16 (int n)
{
  R[n] <<= 16;
  PC += 2;
}
)"},

  example
{R"(
SHLL16 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'56780000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shlr	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 >> Rn >> T" },
  code { "0100nnnn00000001" },
  flags { "LSB → T" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
t ← op1< 0 FOR 1 > ;
op1 ← op1 >> 1;
R n ← Register(op1);
T ← Bit(t);
)"},
  description
{R"(
Logically shifts the contents of general register Rn one bit to the right and
stores the result in Rn. The bit shifted out of the operand is transferred to
the T bit.
<br /><img alt="Shift Logical Right operation" src="shlr.svg" height="100" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLR (int n)
{
  if ((R[n] & 0x00000001) == 0)
    T = 0;
  else
    T = 1;

  R[n] >>= 1;
  R[n] &= 0x7FFFFFFF;
  PC += 2;
}
)"},

  example
{R"(
SHLR R0 ;Before execution: R0 = H'80000001, T = 0
;After execution: R0 = H'40000000, T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shlr2	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn >> 2 → [0 → Rn]" },
  code { "0100nnnn00001001" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 >> 2;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 2 bits to the right, and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Right 2 Bits operation" src="shlr2.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLR2 (int n)
{
  R[n] >>= 2;
  R[n] &= 0x3FFFFFFF;
  PC += 2;
}
)"},

  example
{R"(
SHLR2 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'048D159E
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shlr8	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn >> 8 → [0 → Rn]" },
  code { "0100nnnn00011001" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 >> 8;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 8 bits to the right, and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Right 8 Bits operation" src="shlr8.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLR8 (int n)
{
  R[n] >>= 8;
  R[n] &= 0x00FFFFFF;
  PC += 2;
}
)"},

  example
{R"(
SHLR8 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'00123456
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "shlr16	Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rn >> 16 → [0 → Rn]" },
  code { "0100nnnn00101001" },

  group { SH4A, "EX", SH4, "EX" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
op1 ← ZeroExtend 32(R n);
op1 ← op1 >> 16;
R n ← Register(op1);
)"},
  description
{R"(
Logically shifts the contents of general register Rn 16 bits to the right and
stores the result in Rn. The bits shifted out of the operand are discarded.
<br /><img alt="Shift Logical Right 16 Bits operation" src="shlr16.svg" height="120" />
)"},

  note
{R"(

)"},

  operation
{R"(
void SHLR16 (int n)
{
  R[n] >>= 16;
  R[n] &= 0x0000FFFF;
  PC += 2;
}
)"},

  example
{R"(
SHLR16 R0 ;Before execution: R0 = H'12345678
;After execution: R0 = H'00001234
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Branch Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bf	label",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If T = 0: disp × 2 + PC + 4 → PC\nElse: nop" },
  code { "10001011dddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "1/3", SH4, "1" },
  latency { SH1, "1/3", SH2, "1/3", SH3, "1/3", SH4A, "1", SH2A, "1/3", SH4, "1/2" },

  brief
{R"(
t ← ZeroExtend 1(T);
pc ← SignExtend 32 (PC);
newpc ← SignExtend 32 (PC’);
delayedpc ← SignExtend 32 (PC’’);
label ← SignExtend 8(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
IF (t = 0)
{
temp ← ZeroExtend32(pc + 4 + label);
newpc ← temp;
delayedpc ← temp + 2;
}
PC’ ← Register(newpc);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is a conditional branch instruction that references the T bit. The branch
is taken if T = 0, and not taken if T = 1. The branch destination is address
(PC + 4 + displacement × 2).  The PC source value is the BF instruction address.
As the 8-bit displacement is multiplied by two after sign-extension, the branch
destination can be located in the range from -256 to +254 bytes from the BF
instruction.
)"},

  note
{R"(
If the branch destination cannot be reached, the branch must be handled by using
BF in combination with a BRA or JMP instruction, for example.
<br /><br />
On some SH4 implementations a branch with a displacement value of zero does not
cause the pipeline I-stage to be stalled even if the branch is taken.  This can
be utilized for efficient conditional operations.
<br /><br />
On some SH2E implementations (SH7055) there is an FPU related hardware bug
which affects this instruction.  The recommended workaround is to use bt/s with
a nop in the delay slot.
See also documents "sh2eoc.pdf" and "win_update_a.pdf".
)"},

  operation
{R"(
void BF (int d)
{
  int disp;
  if ((d & 0x80) == 0)
    disp = (0x000000FF & d);
  else
    disp = (0xFFFFFF00 | d);

  if (T == 0)
    PC = PC + 4 + (disp << 1);
  else
    PC += 2;
}
)"},

  example
{R"(
CLRT ;T is always cleared to 0
BT TRGET_T ;Does not branch, because T = 0
BF TRGET_F ;Branches to TRGET_F, because T = 0
NOP ;
NOP ;← The PC location is used to calculate the branch destination
.......... ;  address of the BF instruction
TRGET_F: ;← Branch destination of the BF instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bf/s	label",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "If T = 0: disp × 2 + PC + 4 → PC\nElse: nop\n(Delayed branch)" },
  code { "10001111dddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "1/2", SH4, "1" },
  latency { SH2, "1/2", SH3, "1/2", SH4A, "1", SH2A, "1/2", SH4, "1/2" },

  brief
{R"(
t ← ZeroExtend 1(T);
pc ← SignExtend 32 (PC);
newpc ← SignExtend 32 (PC’);
delayedpc ← SignExtend 32 (PC’’);
label ← SignExtend 8(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
IF (t = 0)
{
temp ← ZeroExtend32(pc + 4 + label);
newpc ← temp;
delayedpc ← temp + 2;
}
PC’ ← Register(newpc);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is a delayed conditional branch instruction that references the T bit.
If T = 1, the next instruction is executed and the branch is not taken.
If T = 0, the branch is taken after execution of the next instruction.
<br /><br />
The branch destination is address (PC + 4 + displacement × 2). The PC source
value is the BF/S instruction address.  As the 8-bit displacement is multiplied
by two after sign-extension, the branch destination can be located in the range
from -256 to +254 bytes from the BF/S instruction.
)"},

  note
{R"(
As this is a delayed branch instruction, when the branch condition is satisfied,
the instruction following this instruction is executed before the branch
destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
<br /><br />
If this instruction is located in the delay slot immediately following a delayed
branch instruction, it is identified as a slot illegal instruction.
<br /><br />
If the branch destination cannot be reached, the branch must be handled by using
BF/S in combination with a BRA or JMP instruction, for example.
)"},

  operation
{R"(
void BFS (int d)
{
  int disp;
  unsigned int temp;
  temp = PC;
  if ((d & 0x80) == 0)
    disp = (0x000000FF & d);
  else
    disp = (0xFFFFFF00 | d);

  if (T == 0)
    PC = PC + 4 + (disp << 1);
  else
    PC += 4;

  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
CLRT ;T is always 0
BT/S TRGET_T ;Does not branch, because T = 0
NOP ;
BF/S TRGET_F ;Branches to TRGET_F, because T = 0
ADD R0,R1 ;Executed before branch .
NOP ;← The PC location is used to calculate the branch destination
.......... ;  address of the BF/S instruction
TRGET_F: ;← Branch destination of the BF/S instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bt	label",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "If T = 1: disp × 2 + PC + 4 → PC\nElse: nop" },
  code { "10001001dddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "1/3", SH4, "1" },
  latency { SH1, "1/3", SH2, "1/3", SH3, "1/3", SH4A, "1", SH2A, "1/3", SH4, "1/2" },

  brief
{R"(
t ← ZeroExtend 1(T);
pc ← SignExtend 32 (PC);
newpc ← SignExtend 32 (PC’);
delayedpc ← SignExtend 32 (PC’’);
label ← SignExtend 8(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
IF (t = 1)
{
temp ← ZeroExtend32(pc + 4 + label);
newpc ← temp;
delayedpc ← temp + 2;
}
PC’ ← Register(newpc);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is a conditional branch instruction that references the T bit. The branch
is taken if T = 1, and not taken if T = 0.  The branch destination is address
(PC + 4 + displacement × 2). The PC source value is the BT instruction address.
As the 8-bit displacement is multiplied by two after sign-extension, the branch
destination can be located in the range from -256 to +254 bytes from the BT
instruction.
)"},

  note
{R"(
If the branch destination cannot be reached, the branch must be handled by using
BT in combination with a BRA or JMP instruction, for example.
<br /><br />
On some SH4 implementations a branch with a displacement value of zero does not
cause the pipeline I-stage to be stalled even if the branch is taken.  This can
be utilized for efficient conditional operations.
<br /><br />
On some SH2E implementations (SH7055) there is an FPU related hardware bug
which affects this instruction.  The recommended workaround is to use bt/s with
a nop in the delay slot.
See also documents "sh2eoc.pdf" and "win_update_a.pdf".
)"},

  operation
{R"(
void BT (int d)
{
  int disp;
  if ((d & 0x80) == 0)
    disp = (0x000000FF & d);
  else
    disp = (0xFFFFFF00 | d);

  if (T == 1)
    PC = PC + 4 + (disp << 1);
  else
    PC += 2;
}
)"},

  example
{R"(
SETT ;T is always 1
BF TRGET_F ;Does not branch, because T = 1
BT TRGET_T ;Branches to TRGET_T, because T = 1
NOP ;
NOP ;← The PC location is used to calculate the branch destination
.......... ;  address of the BT instruction
TRGET_T: ;← Branch destination of the BT instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bt/s	label",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "If T = 1: disp × 2 + PC + 4 → PC\nElse: nop\n(Delayed branch)" },
  code { "10001101dddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "1/2", SH4, "1" },
  latency { SH2, "1/2", SH3, "1/2", SH4A, "1", SH2A, "1/2", SH4, "1/2" },

  brief
{R"(
t ← ZeroExtend 1(T);
pc ← SignExtend 32 (PC);
delayedpc ← SignExtend 32 (PC’’);
label ← SignExtend 8(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
IF (t = 1)
{
temp ← ZeroExtend32(pc + 4 + label);
delayedpc ← temp;
}
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is a conditional branch instruction that references the T bit. The branch
is taken if T = 1, and not taken if T = 0.  The PC source value is the BT/S
instruction address. As the 8-bit displacement is multiplied by two after
sign-extension, the branch destination can be located in the range from -256 to
+254 bytes from the BT/S instruction.
)"},

  note
{R"(
As this is a delayed branch instruction, when the branch condition is satisfied,
the instruction following this instruction is executed before the branch
destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
<br /><br />
If the branch destination cannot be reached, the branch must be handled by using
BT/S in combination with a BRA or JMP instruction, for example.
)"},

  operation
{R"(
void BTS (int d)
{
  int disp;
  unsigned temp;
  temp = PC;

  if ((d & 0x80) == 0)
    disp = (0x000000FF & d);
  else
    disp = (0xFFFFFF00 | d);

  if (T == 1)
    PC = PC + 4 + (disp << 1);
  else
    PC += 4;

  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
SETT ;T is always 1
BF/S TARGET_F ;Does not branch, because T = 1
NOP ;
BT/S TARGET_T ;Branches to TARGET, because T = 1
ADD R0,R1 ;Executes before branching.
NOP ;← The PC location is used to calculate the branch destination
.......... ;  address of the BT/S instruction
TARGET_T: ;← Branch destination of the BT/S instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bra	label",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "disp × 2 + PC + 4 → PC\n(Delayed branch)" },
  code { "1010dddddddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "2", SH4, "1" },
  latency { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
pc ← SignExtend 32 (PC);
label ← SignExtend 12(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
temp ← ZeroExtend32(pc + 4 + label);
delayedpc ← temp;
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is an unconditional branch instruction. The branch destination is address
(PC + 4 + displacement × 2). The PC source value is the BRA instruction address.
As the 12-bit displacement is multiplied by two after sign-extension, the branch
destination can be located in the range from -4096 to +4094 bytes from the BRA
instruction. If the branch destination cannot be reached, this branch can be
performed with a JMP instruction.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void BRA (int d)
{
  int disp;
  unsigned int temp;
  temp = PC;

  if ((d & 0x800) == 0)
    disp = (0x00000FFF & d);
  else
    disp = (0xFFFFF000 | d);

  PC = PC + 4 + (disp << 1);
  Delay_Slot(temp + 2);
}
)"},

  example
{R"(
BRA TRGET ;Branches to TRGET
ADD R0,R1 ;Executes ADD before branching
NOP ;← The PC location is used to calculate the branch destination
.......... ;address of the BRA instruction
TRGET: ;← Branch destination of the BRA instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "braf	Rm",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "Rm + PC + 4 → PC\n(Delayed branch)" },
  code { "0000mmmm00100011" },

  group { SH4A, "BR", SH4, "CO" },
  issue { SH2, "1", SH3, "1", SH4A, "4", SH2A, "2", SH4, "2" },
  latency { SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
pc ← SignExtend 32 (PC);
op1 ← SignExtend 32 (Rn);
IF (IsDelaySlot())
THROW ILLSLOT;
target ← ZeroExtend32(pc + 4 + op1);
delayedpc ← target ∧ (~ 0x1);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
This is an unconditional branch instruction. The branch destination is address
(PC + 4 + Rm).
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void BRAF (int m)
{
  unsigned int temp;
  temp = PC;
  PC = PC + 4 + R[m];
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
MOV.L #(TARGET-BSRF_PC),R0 ;Sets displacement.
BRA TRGET ;Branches to TARGET
ADD R0,R1 ;Executes ADD before branching
BRAF_PC: ;← The PC location is used to calculate the
;branch destination address of the BRAF
;instruction
NOP
....................
TARGET: ;← Branch destination of the BRAF instruction
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bsr	label",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "PC + 4 → PR, disp × 2 + PC + 4 → PC\n(Delayed branch)" },
  code { "1011dddddddddddd" },

  group { SH4A, "BR", SH4, "BR" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1-3", SH2A, "2", SH4, "1" },
  latency { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
pc ← SignExtend 32 (PC);
label ← SignExtend 12(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
delayedpr ← pc + 4;
temp ← ZeroExtend32(pc + 4 + label);
delayedpc ← temp;
PR’’ ← Register(delayedpr);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
Branches to address (PC + 4 + displacement × 2), and stores address (PC + 4) in
PR. The PC source value is the BSR instruction address.
As the 12-bit displacement is multiplied by two after sign-extension, the branch
destination can be located in the range from -4096 to +4094 bytes from the BSR
instruction. If the branch destination cannot be reached, this branch can be
performed with a JSR instruction.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void BSR (int d)
{
  int disp;
  unsigned int temp;
  temp = PC;

  if ((d & 0x800) == 0)
    disp = (0x00000FFF & d);
  else
    disp = (0xFFFFF000 | d);

  PR = PC + 4;
  PC = PC + 4 + (disp << 1);
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
BSR TRGET ;Branches to TRGET
MOV R3,R4 ;Executes the MOV instruction before branching
ADD R0,R1 ;← The PC location is used to calculate the branch destination
;address of the BSR instruction (return address for when the
;subroutine procedure is completed (PR data))
.......
.......
TRGET: ;← Procedure entrance
MOV R2,R3 ;
RTS ;Returns to the above ADD instruction
MOV #1,R0 ;Executes MOV before branching
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "bsrf	Rm",
  SH2 | SH2A | SH3 | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "PC + 4 → PR, Rm + PC + 4 → PC\n(Delayed branch)" },
  code { "0000mmmm00000011" },

  group { SH4A, "BR", SH4, "CO" },
  issue { SH2, "1", SH3, "1", SH4A, "4", SH2A, "2", SH4, "2" },
  latency { SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
pc ← SignExtend 32 (PC);
label ← SignExtend 12(s) << 1;
IF (IsDelaySlot())
THROW ILLSLOT;
delayedpr ← pc + 4;
temp ← ZeroExtend32(pc + 4 + label);
delayedpc ← temp;
PR’’ ← Register(delayedpr);
PC’’ ← Register(delayedpc);
)"},

  description
{R"(
Branches to address (PC + 4 + Rm), and stores address (PC + 4) in PR. The PC
source value is the BSRF instruction address. The branch destination address is
the result of adding the 32-bit contents of general register Rm to PC + 4.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void BSRF (int m)
{
  unsigned int temp;
  temp = PC;
  PR = PC + 4;
  PC = PC + 4 + R[m];
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
MOV.L #(TARGET-BSRF_PC),R0 ;Sets displacement.
BRSF R0 ;Branches to TARGET
MOV R3,R4 ;Executes the MOV instruction before
branching
BSRF_PC: ;← The PC location is used to calculate the
;  branch destination with BSRF.
ADD R0,R1
.....
.....
TARGET: ;←Procedure entrance
MOV R2,R3 ;
RTS ;Returns to the above ADD instruction
MOV #1,R0 ;Executes MOV before branching
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "jmp	@Rm",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "Rm → PC\n(Delayed branch)" },
  code { "0100mmmm00101011" },

  group { SH4A, "BR", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "4", SH2A, "2", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
IF (IsDelaySlot())
THROW ILLSLOT;
target ← op1;
delayedpc ← target ∧ (~ 0x1);
PC’’ ← Register(delayedpc);
)"},
  description
{R"(
Unconditionally makes a delayed branch to the address specified by Rm.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void JMP (int m)
{
  unsigned int temp;
  temp = PC;
  PC = R[m];
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
MOV.L JMP_TABLE,R0 ;Address of R0 = TRGET
JMP @R0 ;Branches to TRGET
MOV R0,R1 ;Executes MOV before branching
.align 4
JMP_TABLE:
.data.l TRGET ;Jump table
.................
TRGET: ADD #1,R1 ;← Branch destination
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "jsr	@Rm",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "PC + 4 → PR, Rm → PC\n(Delayed branch)" },
  code { "0100mmmm00001011" },

  group { SH4A, "BR", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "4", SH2A, "2", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
pc ← SignExtend 32 (PC);
op1 ← SignExtend 32 (Rn);
IF (IsDelaySlot())
THROW ILLSLOT;
delayedpr ← pc + 4;
target ← op1;
delayedpc ← target ∧ (~ 0x1);
PR’’ ← Register(delayedpr);
PC’’ ← Register(delayedpc);
)"},
  description
{R"(
Makes a delayed branch to the subroutine procedure at the specified address
after execution of the following instruction. Return address (PC + 4) is saved
in PR, and a branch is made to the address indicated by general register Rm.
JSR is used in combination with RTS for subroutine procedure calls.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a slot
illegal instruction.
)"},

  operation
{R"(
void JSR (int m)
{
  unsigned int temp;
  temp = PC;
  PR = PC + 4;
  PC = R[m];
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
MOV.L JSR_TABLE,R0 ;Address of R0 = TRGET
JSR @R0 ;Branches to TRGET
XOR R1,R1 ;Executes XOR before branching
ADD R0,R1 ;← Return address for when the subroutine
;  procedure is completed (PR data)
...........
.align 4
JSR_TABLE:
.data.l TRGET ;Jump table
TRGET: NOP ;← Procedure entrance
MOV R2,R3 ;
RTS ;Returns to the above ADD instruction
MOV #70,R1 ;Executes MOV before RTS
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "jsr/n	@Rm",
  SH2A,
  abstract { "PC + 2 → PR, Rm → PC" },
  code { "0100mmmm01001011" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },



  description
{R"(
Branches to a subroutine procedure at the designated address. The contents of
PC are stored in PR and execution branches to the address indicated by the
contents of general register Rm as 32-bit data. The stored contents of PC
indicate the starting address of the second instruction after the present
instruction. This instruction is used with RTS as a subroutine procedure call.
)"},

  note
{R"(
This is not a delayed branch instruction.
)"},

  operation
{R"(
void JSRN (int m)
{
  unsigned long temp;
  temp = PC;
  PR = PC + 2;
  PC = R[m];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "jsr/n	@@(disp8,TBR)",
  SH2A,
  abstract { "PC + 2 → PR, (disp × 4 + TBR) → PC" },
  code { "10000011dddddddd" },

  issue { SH2A, "5" },
  latency { SH2A, "5" },



  description
{R"(
Branches to a subroutine procedure at the designated address. The contents of PC
are stored in PR and execution branches to the address indicated by the address
read from memory address (disp × 4 + TBR). The stored contents of PC indicate
the starting address of the second instruction after the present instruction.
This instruction is used with RTS as a subroutine procedure call.
)"},

  note
{R"(
This is not a delayed branch instruction.
)"},

  operation
{R"(
void JSRNM (int d)
{
  long disp = (0x000000FF & d);
  PR = PC + 2;
  PC = Read_32 (TBR + (disp << 2));
}
)"},

  example
{R"(
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rts",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH_ALL, "Delayed Branch" } } },
  abstract { "PR → PC\nDelayed branch" },
  code { "0000000000001011" },

  group { SH4A, "BR", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1-4", SH2A, "2", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "2", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
pr ← SignExtend 32 (PR);
IF (IsDelaySlot())
THROW ILLSLOT;
target ← pr;
delayedpc ← target ∧ (~ 0x1);
PC’’ ← Register(delayedpc);
)"},
  description
{R"(
Returns from a subroutine procedure by restoring the PC from PR. Processing
continues from the address indicated by the restored PC value. This instruction
can be used to return from a subroutine procedure called by a BSR or JSR
instruction to the source of the call.
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following this
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction.
<br /><br />
If the following instruction is a branch instruction, it is identified as a
slot illegal instruction.
<br /><br />
The instruction that restores PR must be executed before the RTS instruction.
This restore instruction cannot be in the RTS delay slot.
)"},

  operation
{R"(
void RTS (void)
{
  unsigned int temp;
  temp = PC;
  PC = PR;
  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
MOV.L TABLE,R3 ;R3 = Address of TRGET
JSR @R3 ;Branches to TRGET
NOP ;Executes NOP before branching
ADD R0,R1 ;← Return address for when the subroutine procedure is
completed (PR data)
.............
TABLE: .data.l TRGET ;Jump table
.............
TRGET: MOV R1,R0 ;← Procedure entrance
RTS ;PR data → PC
MOV #12,R0 ;Executes MOV before branching
)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rts/n",
  SH2A,
  abstract { "PR → PC" },
  code { "0000000001101011" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },



  description
{R"(
Performs a return from a subroutine procedure. That is, the PC is restored from
PR, and processing is resumed from the address indicated by the PC. This
instruction enables a return to be made from a subroutine procedure called by a
BSR or JSR instruction to the origin of the call.
)"},

  note
{R"(
This is not a delayed branch instruction.
)"},

  operation
{R"(
void RTSN (void)
{
  PC = PR;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rtv/n	Rm",
  SH2A,
  abstract { "Rm → R0, PR → PC" },
  code { "0000mmmm01111011" },

  issue { SH2A, "3" },
  latency { SH2A, "3" },



  description
{R"(
Performs a return from a subroutine procedure after a transfer from specified
general register Rm to R0. That is, after the Rm value is stored in R0, the PC
is restored from PR, and processing is resumed from the address indicated by the
PC. This instruction enables a return to be made from a subroutine procedure
called by a BSR or JSR instruction to the origin of the call.
)"},

  note
{R"(
This is not a delayed branch instruction.
)"},

  operation
{R"(
void RTVN (int m)
{
  R[0] = R[m];
  PC = PR;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "System Control Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clrmac",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 → MACH, 0 → MACL" },
  code { "0000000000101000" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
macl ← 0;
mach ← 0;
MACL ← ZeroExtend 32 (macl);
MACH ← ZeroExtend 32 (mach);
)"},

  description
{R"(
Clears the MACH and MACL registers.
)"},

  note
{R"(

)"},

  operation
{R"(
void CLRMAC (void)
{
  MACH = 0;
  MACL = 0;
  PC += 2;
}
)"},

  example
{R"(
CLRMAC ;Clears and initializes the MAC register
MAC.W @R0+,@R1+ ;Multiply and accumulate operation
MAC.W @R0+,@R1+ ;
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clrs",
  SH3 | SH4 | SH4A,
  abstract { "0 → S" },
  code { "0000000001001000" },
  flags { "0 → S" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
s ← 0;
S ← Bit(s);
)"},

  description
{R"(
Clears the S bit to 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void CLRS (void)
{
  S = 0;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "clrt",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "0 → T" },
  code { "0000000000001000" },
  flags { "0 → T" },

  group { SH4A, "EX", SH4, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },

  brief
{R"(
t ← 0;
T ← Bit(t)
)"},

  description
{R"(
Clears the T bit.
)"},

  note
{R"(

)"},

  operation
{R"(
void CLRT (void)
{
  T = 0;
  PC += 2;
}
)"},

  example
{R"(
CLRT ;Before execution: T = 1
;After execution: T = 0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "icbi	@Rn",
  SH4A,
  abstract { "Invalidate instruction cache block indicated by logical address" },
  code { "0000nnnn11100011" },

  group { SH4A, "CO" },
  issue { SH4A, "16" },
  latency { SH4A, "13" },



  description
{R"(
Accesses the instruction cache at the effective address indicated by the
contents of Rn. When the cache is hit, the corresponding cache block is
invalidated (the V bit is cleared to 0). At this time, write-back is not
performed. No operation is performed in the case of a cache miss or access to
a non-cache area.
)"},

  note
{R"(
When a program is overwriting RAM to modify its own execution, the corresponding
block of the instruction cache should be invalidated by the ICBI instruction.
This prevents execution of the program from the instruction cache, where the
non-overwritten instructions are stored.
)"},

  operation
{R"(
void ICBI (int n)
{
  invalidate_instruction_cache_block (R[n]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Instruction TLB multiple-hit exception
Instruction TLB miss exception
Instruction TLB protection violation exception
Instruction address error
Slot illegal instruction exception
Exceptions may occur when invalidation is not performed.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldbank	@Rm,R0",
  SH2A,
  abstract { "(Specified register bank entry) → R0" },
  code { "0100mmmm11100101" },

  issue { SH2A, "6" },
  latency { SH2A, "5" },



  description
{R"(
The register bank entry indicated by the contents of general register Rm is
transferred to general register R0. The register bank number and register stored
in the bank are specified by general register Rm.
<br /><img src="ldbank.svg" height="400" />
)"},

  note
{R"(
The architecture supports a maximum of 512 banks. However, the number of banks
differs depending on the product.
)"},

  operation
{R"(
void LDBANK (int m)
{
  R[0] = Read_Bank_32 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,SR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "Rm → SR" },
  code { "0100mmmm00001110" },
  flags { "LSB → T" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "7", SH2A, "3", SH4, "4" },
  latency { SH1, "1", SH2, "1", SH3, "5", SH4A, "4", SH2A, "2", SH4, "4" },

  name { "Load to Control Register" },
  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
sr ← op1;
SR ← Register(sr);
)"},
  description
{R"(
Stores the source operand in the control register SR.
)"},

  note
{R"(
This instruction is only usable in privileged mode. Issuing this instruction in
user mode will cause an illegal instruction exception.
)"},

  operation
{R"(
void LDCSR (int m)
{
  #if SH1 || SH2 || SH2 || SH3
  SR = R[m] & 0x0FFF0FFF;

  #elif SH2A
  SR = R[m] & 0x000063F3;

  #elif SH4 || SH4A
  SR = R[m] & 0x700083F3;

  #endif

  PC += 2;
}
)"},

  example
{R"(
LDC R0,SR ;Before execution: R0 = H'FFFFFFFF, SR = H'00000000
;After execution: SR = H'0FFF0FFF
)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,SR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "(Rm) → SR, Rm+4 → Rm" },
  code { "0100mmmm00000111" },
  flags { "LSB → T" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "2", SH4A, "9", SH2A, "5", SH4, "4" },
  latency { SH1, "3", SH2, "3", SH3, "7", SH4A, "4", SH2A, "4", SH4, "4/4" },

  name { "Load to Control Register" },
  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
sr ← SignExtend 32(ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
SR ← Register(sr);
)"},
  description
{R"(
Stores the source operand in the control register SR.
)"},

  note
{R"(
This instruction is only usable in privileged mode. Issuing this instruction in
user mode will cause an illegal instruction exception.
)"},

  operation
{R"(
void LDCMSR (int m)
{
  #if SH1 || SH2 || SH2 || SH3
  SR = Read_32 (R[m]) & 0x0FFF0FFF;

  #elif SH2A
  SR = Read_32 (R[m]) & 0x000063F3;

  #elif SH4 || SH4A
  SR = Read_32 (R[m]) & 0x700083F3;

  #endif

  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,TBR",
  SH2A,
  abstract { "Rm → TBR" },
  code { "0100mmmm01001010" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Stores a source operand in control register TBR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCTBR (int m)
{
  TBR = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,GBR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → GBR" },
  code { "0100mmmm00011110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },
  latency { SH1, "1", SH2, "1", SH3, "1/3", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
gbr ← op1;
GBR ← Register(gbr)
)"},
  description
{R"(
Stores a source operand in control register GBR.
)"},

  note
{R"(
This instruction can also be issued in user mode.
)"},

  operation
{R"(
void LDCGBR (int m)
{
  GBR = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,GBR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → GBR, Rm+4 → Rm" },
  code { "0100mmmm00010111" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },
  latency { SH1, "3", SH2, "3", SH3, "1/5", SH4A, "1", SH2A, "2", SH4, "3/3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
gbr ← SignExtend 32 (ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
GBR ← Register(gbr);
)"},
  description
{R"(
Stores a source operand in control register GBR.
)"},

  note
{R"(
This instruction can also be issued in user mode.
)"},

  operation
{R"(
void LDCMGBR (int m)
{
  GBR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(
LDC.L @R15+,GBR ;Before execution: R15 = H'10000000
;After execution: R15 = H'10000004, GBR = @H'10000000
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,VBR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "Rm → VBR" },
  code { "0100mmmm00101110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1/3", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
vbr← op1;
VBR ← Register(vbr);
)"},
  description
{R"(
Stores a source operand in control register VBR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCVBR (int m)
{
  VBR = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,VBR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "(Rm) → VBR, Rm+4 → Rm" },
  code { "0100mmmm00100111" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "3", SH2, "3", SH3, "1/5", SH4A, "1", SH2A, "2", SH4, "1/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
vbr ← SignExtend 32 (ReadMemory 32(address));
op1 ← op1 + 4;
R m ← Register(op1);
VBR ← Register(vbr);
)"},
  description
{R"(
Stores a source operand in control register VBR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCMVBR (int m)
{
  VBR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,MOD",
  SH_DSP,
  abstract { "Rm → MOD" },
  code { "0100mmmm01011110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/3" },



  description
{R"(
Stores a source operand in control register MOD.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDCMOD (int m)
{
  MOD = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,MOD",
  SH_DSP,
  abstract { "(Rm) → MOD, Rm+4 → Rm" },
  code { "0100mmmm01010111" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores a source operand in control register MOD.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 3 cycles.
)"},

  operation
{R"(
void LDCMMOD (int m)
{
  MOD = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,RE",
  SH_DSP,
  abstract { "Rm → RE" },
  code { "0100mmmm01111110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/3" },



  description
{R"(
Stores a source operand in control register RE.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDCRE (int m)
{
  RE = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,RE",
  SH_DSP,
  abstract { "(Rm) → RE, Rm+4 → Rm" },
  code { "0100mmmm01110111" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores a source operand in control register RE.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 3 cycles.
)"},

  operation
{R"(
void LDCMRE (int m)
{
  RE = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,RS",
  SH_DSP,
  abstract { "Rm → RS" },
  code { "0100mmmm01101110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/3" },



  description
{R"(
Stores a source operand in control register RS.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDCRS (int m)
{
  RS = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,RS",
  SH_DSP,
  abstract { "(Rm) → RS, Rm+4 → Rm" },
  code { "0100mmmm01100111" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores a source operand in control register RS.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 3 cycles.
)"},

  operation
{R"(
void LDCMRS (int m)
{
  RS = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,SGR",
  SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm → SGR" },
  code { "0100mmmm00111010" },

  group { SH4A, "CO" },
  issue { SH4A, "4" },
  latency { SH4A, "4" },



  description
{R"(
Stores a source operand in control register SGR.
)"},

  note
{R"(
Not sure whether it is also available on SH4.
It is not marked as new instruction for SH4A but is also not listed in SH4
manuals.
)"},

  operation
{R"(
void LDCSGR (int m)
{
  SGR = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,SGR",
  SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "(Rm) → SGR, Rm+4 → Rm" },
  code { "0100mmmm00110110" },

  group { SH4A, "CO" },
  issue { SH4A, "4" },
  latency { SH4A, "4" },



  description
{R"(
Stores a source operand in control register SGR.
)"},

  note
{R"(
Not sure whether it is also available on SH4.
It is not marked as new instruction for SH4A but is also not listed in SH4
manuals.
)"},

  operation
{R"(
void LDCMSGR (int m)
{
  SGR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,SSR",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm → SSR" },
  code { "0100mmmm00111110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1/3", SH4A, "1", SH4, "3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
ssr ← op1;
SSR ← Register(ssr);
)"},
  description
{R"(
Stores a source operand in control register SSR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCSSR (int m)
{
  SSR = R[m],
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,SSR",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "(Rm) → SSR, Rm+4 → Rm" },
  code { "0100mmmm00110111" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1/5", SH4A, "1", SH4, "1/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
ssr ← SignExtend 32(ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
SSR ← Register(ssr);
)"},
  description
{R"(
Stores a source operand in control register SSR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCMSSR (int m)
{
  SSR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,SPC",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm → SPC" },
  code { "0100mmmm01001110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "3" },
  latency { SH3, "1/3", SH4A, "1", SH4, "1" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
spc ← op1;
SPC ← Register(spc);
)"},
  description
{R"(
Stores a source operand in control register SPC.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCSPC (int m)
{
  SPC = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,SPC",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "(Rm) → SPC, Rm+4 → Rm" },
  code { "0100mmmm01000111" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1/5", SH4A, "1", SH4, "1/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
spc ← SignExtend 32 (ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
SPC ← Register(spc);
)"},
  description
{R"(
Stores a source operand in control register SPC.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCMSPC (int m)
{
  SPC = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,DBR",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm → DBR" },
  code { "0100mmmm11111010" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH4A, "4", SH4, "1" },
  latency { SH4A, "4", SH4, "3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
dbr← op1;
DBR ← Register(dbr)
)"},
  description
{R"(
Stores a source operand in control register DBR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCDBR (int m)
{
  DBR = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,DBR",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "(Rm) → DBR, Rm+4 → Rm" },
  code { "0100mmmm11110110" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH4A, "4", SH4, "1" },
  latency { SH4A, "4", SH4, "1/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
dbr ← SignExtend 32 (ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
DBR ← Register(dbr);
)"},
  description
{R"(
Stores a source operand in control register DBR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCMDBR (int m)
{
  DBR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc	Rm,Rn_BANK",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm → Rn_BANK (n = 0-7)" },
  code { "0100mmmm1nnn1110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1/3", SH4A, "1", SH4, "3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
rn_bank← op1;
Rn_BANK ← Register(rn_bank);
)"},
  description
{R"(
Stores a source operand in banked general register.
Rn_BANK0 is accessed when the RB bit in the SR register is 1, and Rn_BANK1 is
accessed when this bit is 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCRn_BANK (int m)
{
  Rn_BANK = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldc.l	@Rm+,Rn_BANK",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "(Rm) → Rn_BANK, Rm+4 → Rm" },
  code { "0100mmmm1nnn0111" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1/5", SH4A, "1", SH4, "1/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
rn_bank ← SignExtend 32 (ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
Rn_BANK ← Register(rn_bank);
)"},
  description
{R"(
Stores a source operand in banked general register.
Rn_BANK0 is accessed when the RB bit in the SR register is 1, and Rn_BANK1 is
accessed when this bit is 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDCMRn_BANK (int m)
{
  Rn_BANK = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldre	@(disp,PC)",
  SH_DSP,
  abstract { "disp × 2 + PC → RE" },
  code { "10001110dddddddd" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "3" },



  description
{R"(
Stores the effective address of the source operand in the repeat end register
RE. The effective address is an address specified by PC + displacement. The PC
is the address four bytes after this instruction. The 8-bit displacement is
sign-extended and doubled. Consequently, the relative interval from the branch
destination is -256 to +254 bytes.
)"},

  note
{R"(
The effective address value designated for the RE reregister is different from
the actual repeat end address. Refer to RS and RE Design Rules, for more
information.
<br /><br />
When this instruction is arranged immediately after the delayed
branch instruction, PC becomes the "first address +2" of the branch destination.
<br /><br />
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDRE (int d)
{
  long disp;

  if ((d & 0x80) == 0)
    disp = (0x000000FF & (long)d);
  else
    disp = (0xFFFFFF00 | (long)d);

  RE = PC + (disp << 1);
  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
    ldrs   start     ! Set repeat start address to RS
    ldre   end       ! Set repeat end address to RE
    setrc  #32       ! Repeat 32 times from <instruction A> to <instruction B>
    ...
start:
    <instruction A>
    ...
    ...
    ...
end:
    <instruction B>
    ...
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldrs	@(disp,PC)",
  SH_DSP,
  abstract { "disp × 2 + PC → RS" },
  code { "10001100dddddddd" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "3" },



  description
{R"(
Stores the effective address of the source operand in the repeat start register
RS. The effective address is an address specified by PC + displacement. The PC
is the address four bytes after this instruction. The 8-bit displacement is
sign-extended and doubled. Consequently, the relative interval from the branch
destination is -256 to +254 bytes.
)"},

  note
{R"(
When the instructions of the repeat (loop) program are below 3, the effective
address value designated for the RS register is different from the actual repeat
start address. Refer to "RS and RE setting rule", for more information. If this
<br /><br />
instruction is arranged immediately after the delayed branch instruction, the PC
becomes "the first address +2" of the branch destination.
<br /><br />
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDRS (int d)
{
  long disp;

  if ((d & 0x80) == 0)
    disp = (0x000000FF & (long)d);
  else
    disp = (0xFFFFFF00 | (long)d);

  RS = PC + (disp << 1);
  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
    ldrs   start     ! Set repeat start address to RS
    ldre   end       ! Set repeat end address to RE
    setrc  #32       ! Repeat 32 times from <instruction A> to <instruction B>
    ...
start:
    <instruction A>
    ...
    ...
    ...
end:
    <instruction B>
    ...
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,MACH",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → MACH" },
  code { "0100mmmm00001010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
mach ← op1;
MACH ← ZeroExtend 32 (mach);
)"},
  description
{R"(
Stores the source operand into the system register MACH.
)"},

  note
{R"(
On SH1, only the lower 10 bits are stored in MACH.
<br /><br />
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.   When an LDS to MAC* is followed by
MAC.W/MAC.L, the latency of the LDS to MAC* is 1 cycle.
)"},

  operation
{R"(
void LDSMACH (int m)
{
  MACH = R[m];

  #if SH1
  if ((MACH & 0x00000200) == 0)
    MACH &= 0x000003FF;
  else
    MACH |= 0xFFFFFC00;
  #endif

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,MACH",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → MACH, Rm+4 → Rm" },
  code { "0100mmmm00000110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "1/3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
mach ← SignExtend 32 (ReadMemory 32 (address));
op1 ← op1 + 4;
R m ← Register(op1);
MACH ← ZeroExtend 32 (mach);
)"},
  description
{R"(
Stores the source operand into the system register MACH.
)"},

  note
{R"(
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.   When an LDS to MAC* is followed by
MAC.W/MAC.L, the latency of the LDS to MAC* is 1 cycle.
)"},

  operation
{R"(
void LDSMMACH (int m)
{
  MACH = Read_32 (R[m]);

  #if SH1
  if ((MACH & 0x00000200) == 0)
    MACH &= 0x000003FF;
  else
    MACH |= 0xFFFFFC00;
  #endif

  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(
LDS.L @R15+,MACL ;Before execution: R15 = H'10000000
                 ;After execution:  R15 = H'10000004, MACL = @H'10000000
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,MACL",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → MACL" },
  code { "0100mmmm00011010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
macl ← op1;
MACL ← ZeroExtend 32 (macl);
)"},
  description
{R"(
Stores the source operand into the system register MACL.
)"},

  note
{R"(
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.   When an LDS to MAC* is followed by
MAC.W/MAC.L, the latency of the LDS to MAC* is 1 cycle.
)"},

  operation
{R"(
void LDSMACL (int m)
{
  MACL = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,MACL",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → MACL, Rm+4 → Rm" },
  code { "0100mmmm00010110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "1/3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
macl ← SignExtend 32(ReadMemory 32(address));
op1 ← op1 + 4;
R m ← Register(op1);
MACL ← ZeroExtend 32 (macl)
)"},
  description
{R"(
Stores the source operand into the system register MACL.
)"},

  note
{R"(
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.   When an LDS to MAC* is followed by
MAC.W/MAC.L, the latency of the LDS to MAC* is 1 cycle.
)"},

  operation
{R"(
void LDSMMACL (int m)
{
  MACL = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(
LDS.L @R15+,MACL ;Before execution: R15 = H'10000000
;After execution: R15 = H'10000004, MACL = @H'10000000
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,PR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "Rm → PR" },
  code { "0100mmmm00101010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
newpr ← op1;
delayedpr ← newpr;
PR’ ← Register(newpr);
PR’’ ← Register(delayedpr);
)"},
  description
{R"(
Stores the source operand into the system register PR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSPR (int m)
{
  PR = R[m];
  PC += 2;
}
)"},

  example
{R"(
LDS R0,PR ;Before execution: R0 = H'12345678, PR = H'00000000
;After execution: PR = H'12345678
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,PR",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "(Rm) → PR, Rm+4 → Rm" },
  code { "0100mmmm00100110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2/3" },

  brief
{R"(
op1 ← SignExtend 32 (Rm );
address ← ZeroExtend 32(op1);
newpr ← SignExtend 32 (ReadMemory 32(address));
delayedpr ← newpr;
op1 ← op1 + 4;
R m ← Register(op1);
PR’ ← Register(newpr);
PR’’ ← Register(delayedpr);
)"},
  description
{R"(
Stores the source operand into the system register PR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMPR (int m)
{
  PR = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,DSR",
  SH_DSP,
  abstract { "Rm → DSR" },
  code { "0100mmmm01101010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },

  name { "Load to FPU System register" },
  description
{R"(
Stores the source operand into the DSP register DSR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSDSR (int m)
{
  DSR = R[m] & 0x0000000F;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

insn { "lds	Rm,A0",
  SH_DSP,
  abstract { "Rm → A0" },
  code { "0100mmmm01111010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register A0.
)"},

note
{R"(

)"},

operation
{R"(
void LDSA0 (long m)
{
 A0 = R[m];

 if ((A0 & 0x80000000) == 0)
   A0G = 0x00;
 else
   A0G = 0xFF;

 PC += 2;
}
)"},

example
{R"(

)"},

exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,DSR",
  SH_DSP,
  abstract { "(Rm) → DSR, Rm+4 → Rm" },
  code { "0100mmmm01100110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },

  name { "Load to FPU System register" },
  description
{R"(
Stores the source operand into the DSP register DSR.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void LDSMDSR (int m)
{
  DSR = Read_32 (R[m]) & 0x0000000F;
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,A0",
  SH_DSP,
  abstract { "(Rm) → A0, Rm+4 → Rm" },
  code { "0100mmmm01110110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register A0.  The MSB of the data is
copied into A0G.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMA0 (int m)
{
  A0 = Read_32 (R[m]);

  if ((A0 & 0x80000000) == 0)
    A0G = 0x00;
  else
    A0G = 0xFF;

  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,X0",
  SH_DSP,
  abstract { "Rm → X0" },
  code { "0100mmmm10001010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register X0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSX0 (int m)
{
  X0 = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,X0",
  SH_DSP,
  abstract { "(Rm) → X0, Rm+4 → Rm" },
  code { "0100nnnn10000110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores the source operand into the DSP register X0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMX0 (int m)
{
  X0 = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,X1",
  SH_DSP,
  abstract { "Rm → X1" },
  code { "0100mmmm10011010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register X1.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSX1 (int m)
{
  X1 = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,X1",
  SH_DSP,
  abstract { "(Rm) → X1, Rm+4 → Rm" },
  code { "0100nnnn10010110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores the source operand into the DSP register X1.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMX1 (int m)
{
  X1 = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,Y0",
  SH_DSP,
  abstract { "Rm → Y0" },
  code { "0100mmmm10101010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register Y0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSY0 (int m)
{
  Y0 = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,Y0",
  SH_DSP,
  abstract { "(Rm) → Y0, Rm+4 → Rm" },
  code { "0100nnnn10100110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/5" },



  description
{R"(
Stores the source operand into the DSP register Y0.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMY0 (int m)
{
  Y0 = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,Y1",
  SH_DSP,
  abstract { "Rm → Y1" },
  code { "0100mmmm10111010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register Y1.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSY1 (int m)
{
  Y1 = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,Y1",
  SH_DSP,
  abstract { "(Rm) → Y1, Rm+4 → Rm" },
  code { "0100nnnn10110110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the source operand into the DSP register Y1.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMY1 (int m)
{
  Y1 = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ldtlb",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "PTEH/PTEL → TLB" },
  code { "0000000000111000" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1", SH4A, "1", SH4, "1" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
UTLB[MMUCR.URC].ASID ← PTEH.ASID
UTLB[MMUCR.URC].VPN ← PTEH.VPN
UTLB[MMUCR.URC].PPN ← PTEH.PPN
UTLB[MMUCR.URC].SZ ← PTEL.SZ1<<1 + PTEL.SZ0
UTLB[MMUCR.URC].SH ← PTEL.SH
UTLB[MMUCR.URC].PR ← PTEL.PR
UTLB[MMUCR.URC].WT ← PTEL.WT
UTLB[MMUCR.URC].C ← PTEL.C
UTLB[MMUCR.URC].D ← PTEL.D
UTLB[MMUCR.URC].V ← PTEL.V
)"},
  description
{R"(
Loads the contents of the PTEH/PTEL registers into the TLB (translation
lookaside buffer) specified by MMUCR.URC (random counter field in the MMC
control register).
<br /><br />
LDTLB is a privileged instruction, and can only be used in privileged mode. Use of this
instruction in user mode will cause an illegal instruction exception.
)"},

  note
{R"(
As this instruction loads the contents of the PTEH/PTEL registers into a TLB, it
should be used either with the MMU disabled, or in the P1 or P2 virtual space
with the MMU enabled (see the MMU section of the applicable hardware manual for
details).
<br /><br />
After this instruction is issued, there must be at least one instruction between
the LDTLB instruction and issuance of an instruction relating to address to the
P0, U0, and P3 areas (i.e. BRAF, BSRF, JMP, JSR, RTS, or RTE).
<br /><br />
If the instruction is issued in an exception handler, it should be at least two
instructions prior to an RTE instruction that terminates the handler.
)"},

  operation
{R"(
void LDTLB (void)
{
  #if SH3
  TLB_tag = PTEH;
  TLB_data = PTEL;

  #elif SH4
  TLB[MMUCR.URC].ASID = PTEH & 0x000000FF;
  TLB[MMUCR.URC].VPN = (PTEH & 0xFFFFFC00) >> 10;
  TLB[MMUCR.URC].PPN = (PTEH & 0x1FFFFC00) >> 10;
  TLB[MMUCR.URC].SZ = (PTEL & 0x00000080) >> 6 | (PTEL & 0x00000010) >> 4;
  TLB[MMUCR.URC].SH = (PTEH & 0x00000002) >> 1;
  TLB[MMUCR.URC].PR = (PTEH & 0x00000060) >> 5;
  TLB[MMUCR.URC].WT = (PTEH & 0x00000001);
  TLB[MMUCR.URC].C = (PTEH & 0x00000008) >> 3;
  TLB[MMUCR.URC].D = (PTEH & 0x00000004) >> 2;
  TLB[MMUCR.URC].V = (PTEH & 0x00000100) >> 8;

  #endif

  PC += 2;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movca.l	R0,@Rn",
  SH4 | SH4A,
  abstract { "R0 → (Rn) (without fetching cache block)" },
  code { "0000nnnn11000011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "3-7" },

  brief
{R"(
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rn);
IF (AddressUnavailable(op1))
THROW WADDERR, op1;
IF (MMU() AND DataAccessMiss(op1))
THROW WTLBMISS, op1;
IF (MMU() AND WriteProhibited(op1))
THROW WRITEPROT, op1;
IF (MMU() AND NOT DirtyBit(op1))
THROW FIRSTWRITE, op1
ALLOCO(op1);
address ← ZeroExtend 32(op1);
WriteMemory 32(op1, r0);
)"},
  description
{R"(
Stores the contents of general register R0 in the memory location indicated by
effective address Rn. This instruction differs from other store instructions as
follows.
<br /><br />
If write-back is selected for the accessed memory, and a cache miss occurs, the
cache block will be allocated but an R0 data write will be performed to that
cache block without performing a block read. Other cache block contents are
undefined.
)"},

  note
{R"(

)"},

  operation
{R"(
void MOVCAL (int n)
{
  if (is_write_back_memory (R[n]) && look_up_in_operand_cache (R[n]) == MISS)
    allocate_operand_cache_block (R[n]);

  Write_32 (R[n], R[0]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "nop",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "No operation" },
  code { "0000000000001001" },

  group { SH4A, "MT", SH4, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "0" },



  description
{R"(

)"},

  note
{R"(
Increments the program counter (PC), advancing the processing flow to execution
of the next instruction.
)"},

  operation
{R"(
void NOP (void)
{
  PC += 2;
}
)"},

  example
{R"(
NOP ;Executes in one cycle
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ocbi	@Rn",
  SH4 | SH4A,
  abstract { "Invalidate operand cache block" },
  code { "0000nnnn10010011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1-2" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
IF (AddressUnavailable(op1))
THROW WADDERR, op1;
IF (MMU() AND DataAccessMiss(op1))
THROW WTLBMISS, op1;
IF (MMU() AND WriteProhibited(op1))
THROW WRITEPROT, op1;
IF (MMU() AND NOT DirtyBit(op1))
THROW FIRSTWRITE, op1
OCBI(op1);
)"},
  description
{R"(
Accesses data using the contents indicated by effective address Rn. In the case
of a hit in the cache, the corresponding cache block is invalidated (the V bit
is cleared to 0). If there is unwritten information (U bit = 1), write-back is
not performed even if write-back mode is selected. No operation is performed in
the case of a cache miss or an access to a non-cache area.
)"},

  note
{R"(

)"},

  operation
{R"(
void OCBI (int n)
{
  invalidate_operand_cache_block (R[n]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
Note that the above exceptions are generated even if OCBI does not operate.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ocbp	@Rn",
  SH4 | SH4A,
  abstract { "Write back and invalidate operand cache block" },
  code { "0000nnnn10100011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1-5" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
IF (AddressUnavailable(op1))
THROW RADDERR, op1;
IF (MMU() AND DataAccessMiss(op1))
THROW RTLBMISS, op1;
IF (MMU() AND (ReadProhibited(op1) AND WriteProhibited(op1)))
THROW READPROT, op1;
OCBP(op1);
)"},
  description
{R"(
Accesses data using the contents indicated by effective address Rn. If the cache
is hit and there is unwritten information (U bit = 1), the corresponding cache
block is written back to external memory and that block is invalidated (the V
bit is cleared to 0). If there is no unwritten information (U bit = 0), the
block is simply invalidated. No operation is performed in the case of a cache
miss or an access to a non-cache area.
)"},

  note
{R"(

)"},

  operation
{R"(
void OCBP (int n)
{
  if (is_dirty_block (R[n]))
    write_back (R[n])

  invalidate_operand_cache_block (R[n]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Note that the above exceptions are generated even if OCBP does not operate.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ocbwb	@Rn",
  SH4 | SH4A,
  abstract { "Write back operand cache block" },
  code { "0000nnnn10110011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1-5" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
IF (AddressUnavailable(op1))
THROW RADDERR, op1;
IF (MMU() AND DataAccessMiss(op1))
THROW RTLBMISS, op1;
IF (MMU() AND (ReadProhibited(op1) AND WriteProhibited(op1)))
THROW READPROT, op1;
OCBWB(op1);
)"},
  description
{R"(
Accesses data using the contents indicated by effective address Rn. If the cache
is hit and there is unwritten information (U bit = 1), the corresponding cache
block is written back to external memory and that block is cleaned (the U bit
is cleared to 0). In other cases (i.e. in the case of a cache miss or an access
to a non-cache area, or if the block is already clean), no operation is
performed.
)"},

  note
{R"(

)"},

  operation
{R"(
void OCBWB (int n)
{
  if (is_dirty_block (R[n]))
    write_back (R[n]);

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Note that the above exceptions are generated even if OCBWB does not operate.
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pref	@Rn",
  SH2A | SH3 | SH4 | SH4A,
  abstract { "(Rn) → operand cache" },
  code { "0000nnnn10000011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH3, "1/2", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
op1 ← SignExtend 32 (Rn);
IF (AddressUnavailable(op1))
THROW RADDERR, op1
IF (NOT (MMU() AND DataAccessMiss(op1)))
IF (NOT (MMU() AND ReadProhibited(op1)))
PREF(op1);
)"},
  description
{R"(
SH4 and SH4A
<br />
Reads a 32-byte data block starting at a 32-byte boundary into the operand
cache. The lower 5 bits of the address specified by Rn are masked to zero.
<br />
This instruction is also used to trigger a Store Queue write-back operation if
the specified address points to the Store Queue area.  For more information
refer to Store Queues in the manual.
<br /><br />

SH3 and SH2A
<br />
Reads a 16-byte data block into the cache.  The address specified by Rn should
be on 32-bit boundary.  No address related error is detected in this
instruction. In case of an error, the instruction operates as NOP.
)"},

  note
{R"(
On products with no cache, this instruction is handled as a NOP instruction.
)"},

  operation
{R"(
void PREF (int n)
{
  prefetch_operand_cache_block (R[n]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
)"},

},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "prefi	@Rn",
  SH4A,
  abstract { "Reads 32-byte instruction block into instruction cache" },
  code { "0000nnnn11010011" },

  group { SH4A, "CO" },
  issue { SH4A, "13" },
  latency { SH4A, "10" },



  description
{R"(
Reads a 32-byte block of data starting at a 32-byte boundary within the
instruction cache. The lower 5 bits of the address specified by Rn are masked
by zeroes.
<br /><br />
This instruction does not generate data address error and MMU exceptions. In the
event of an error, the PREFI instruction is treated as an NOP (no operation)
instruction.
<br /><br />
When the address to be prefetched is missing from UTLB or is protected, the
PREFI instruction is treated as an NOP instruction and a TLB exception does not
occur.
)"},

  note
{R"(
This instruction can be used before the SLEEP command is issued to prefetch
instructions for execution on return from the SLEEP state.
)"},

  operation
{R"(
void PREFI (int n)
{
  prefetch_instruction_cache_block (R[n]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "resbank",
  SH2A,
  abstract { "Bank → R0 to R14, GBR, MACH, MACL, PR" },
  code { "0000000001011011" },

  issue { SH2A, "9/19" },
  latency { SH2A, "8/20" },



  description
{R"(
Restores the last register saved to a register bank.
)"},

  note
{R"(
The issue cycle count is 19 when a bank overflow has occured and the registers
are restored from the stack.
)"},

  operation
{R"(
void RESBANK (void)
{
  int m;  // Number of register bank to which a save was last performed.

  if (BO == 0)
  {
    PR = Register_Bank[m].PR_BANK;
    GBR = Register_Bank[m].GBR_BANK;
    MACL = Register_Bank[m].MACL_BANK;
    MACH = Register_Bank[m].MACH_BANK;

    for (int i = 0; i <= 14; i++)
      R[i] = Register_Bank[m].R_BANK[i];
  }
  else
  {
    for (int i = 0; i <= 14; i++)
    {
      R[i] = Read_32 (R[15]);
      R[15] += 4;
    }

    PR = Read_32 (R[15]);
    R[15] += 4;
    GBR = Read_32 (R[15]);
    R[15] += 4;
    MACH = Read_32 (R[15]);
    R[15] += 4;
    MACL = Read_32 (R[15]);
    R[15] += 4;
  }

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "rte",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" }, { SH_ALL, "Delayed Branch" } } },
  abstract { "Delayed branch\nSH1*,SH2*: stack area → PC/SR\nSH3*,SH4*: SSR/SPC → SR/PC" },
  code { "0000000000101011" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "5", SH2A, "6", SH4, "5" },
  latency { SH1, "4", SH2, "4", SH3, "4", SH4A, "4", SH2A, "5", SH4, "5" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
ssr ← SignExtend 32(SSR);
pc ← SignExtend32(PC)
IF (IsDelaySlot())
THROW ILLSLOT;
target ← pc;
delayedpc ← target ∧ (~ 0x1);
PC’’ ← Register(delayedpc);
)"},
  description
{R"(
Returns from an exception or interrupt handling routine by restoring the PC and
SR values.  Program execution continues from the address specified
by the restored PC value.
<br /><br />
On SH3 and SH4 the PC and SR values are restored from SPC and SSR.  The SR value
accessed by the instruction in the RTE delay slot is the value restored from SSR
by the RTE instruction. The SR and MD values defined prior to RTE execution are
used to fetch the instruction in the RTE delay slot.
<br /><br />
On SH1, SH2 and SH2A the PC and SR values are from the stack (R15).
)"},

  note
{R"(
As this is a delayed branch instruction, the instruction following the RTE
instruction is executed before the branch destination instruction.
<br /><br />
Interrupts are not accepted between this instruction and the following
instruction. An exception must not be generated by the instruction in this
instruction's delay slot. If the following instruction is a branch instruction,
it is identified as a slot illegal instruction.
<br /><br />
If this instruction is located in the delay slot immediately following a delayed
branch instruction, it is identified as a slot illegal instruction.
<br /><br />
On SH3 and SH4 the SR value accessed by the instruction in the RTE delay slot is
the value restored from SSR by the RTE instruction. The SR and MD values defined
prior to RTE execution are used to fetch the instruction in the RTE delay slot.
)"},

  operation
{R"(
void RTE (void)
{
  unsigned long temp = PC;

  #if SH1 || SH2 || SH2A
  PC = Read_32 (R[15]);
  R[15] += 4;
  SR = Read_32 (R[15]) & 0x000063F3;
  R[15] += 4;

  #elif SH3 || SH4 || SH4A
  SR = SSR;
  PC = SPC;

  #endif

  Delay_Slot (temp + 2);
}
)"},

  example
{R"(
RTE ;Returns to the original routine
ADD #8,R14 ;Executes ADD before branching
)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "setrc	Rn",
  SH_DSP,
  abstract { "Rn[11:0] → RC (SR[27:16])" },
  code { "0100mmmm00010100" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "3" },



  description
{R"(
Sets the repeat count to the SR register's RC counter. The bottom 12 bits of the
general register Rn are used as the repeat count.
Set repeat control flags to RF1, RF0 bits of the SR register. Use of the SETRC
instruction is subject to any limitations. Refer to the DSP Repeat (Loop)
Control section of the manual for more information.
<br /><img alt="Set Repeat Count to RC operation" src="setrc.svg" height="140" />
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void SETRC (int m)
{
  long temp = (R[m] & 0x00000FFF) << 16;
  SR &= 0x00000FF3;
  SR |= temp;
  RF1 = Repeat_Control_Flag1;
  RF0 = Repeat_Control_Flag0;
  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
    ldrs   start     ! Set repeat start address to RS
    ldre   end       ! Set repeat end address to RE
    setrc  r14       ! Repeat n times from <instruction A> to <instruction B>
    ...
start:
    <instruction A>
    ...
    ...
    ...
end:
    <instruction B>
    ...
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "setrc	#imm",
  SH_DSP,
  abstract { "imm → RC (SR[23:16]), 0 → SR[27:24]" },
  code { "10000010iiiiiiii" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "3" },



  description
{R"(
Sets the repeat count to the SR register's RC counter. The 8-bit immediate value
is zero-extended and used as the repeat count.
Set repeat control flags to RF1, RF0 bits of the SR register. Use of the SETRC
instruction is subject to any limitations. Refer to the DSP Repeat (Loop)
Control section of the manual for more information.
<br /><img alt="Set Repeat Count to RC operation" src="setrci.svg" height="140" />
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 1 cycle.
)"},

  operation
{R"(
void SETRCI (int i)
{
  long temp = ((long)i & 0x000000FF) << 16;
  SR &= 0x00000FFF;
  SR |= temp;
  RF1 = Repeat_Control_Flag1;
  RF0 = Repeat_Control_Flag0;
  PC += 2;
}
)"},

  example
{R"(#NOREFORMAT
    ldrs   start     ! Set repeat start address to RS
    ldre   end       ! Set repeat end address to RE
    setrc  #32       ! Repeat 32 times from <instruction A> to <instruction B>
    ...
start:
    <instruction A>
    ...
    ...
    ...
end:
    <instruction B>
    ...
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sets",
  SH3 | SH4 | SH4A,
  abstract { "1 → S" },
  code { "0000000001011000" },
  flags { "1 → S" },

  group { SH4A, "EX", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "1" },
  latency { SH3, "1", SH4A, "1", SH4, "1" },

  brief
{R"(
s ← 1;
S ← Bit(s);
)"},
  description
{R"(
Sets the S bit to 1.
)"},

  note
{R"(

)"},

  operation
{R"(
void SETS (void)
{
  S = 1;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sett",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract { "1 → T" },
  code { "0000000000011000" },
  flags { "1 → T" },

  group { SH4A, "EX", SH4, "MT" },
  issue { SH1 | SH2 | SH3 | SH2A | SH4 | SH4A, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  brief
{R"(
t ← 1;
T ← Bit(t);
)"},
  description
{R"(
Sets the T bit to 1.
)"},

  note
{R"(

)"},

  operation
{R"(
void SETT (void)
{
  T = 1;
  PC += 2;
}
)"},

  example
{R"(
SETT ;Before execution: T = 0
;After execution: T = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sleep",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Sleep or standby" },
  code { "0000000000011011" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "2", SH4A, "ud", SH2A, "5", SH4, "4" },
  latency { SH1, "3", SH2, "3", SH3, "4", SH4A, "ud", SH2A, "0", SH4, "4" },



  description
{R"(
Places the CPU in the power-down state.
<br /><br />
In power-down mode, the CPU retains its internal state, but immediately stops
executing instructions and waits for an interrupt request. When it receives an
interrupt request, the CPU exits the power-down state.
<br /><br />
SLEEP is a privileged instruction, and can only be used in privileged mode. Use
of this instruction in user mode will cause an illegal instruction exception.
)"},

  note
{R"(
SLEEP performance depends on the standby control register (STBCR). See
Power-Down Modes in the target product's hardware manual, for details.
<br /><br />
The number of cycles given is for the transition to sleep mode. "ud" means
the number of cycles is undefined.
<br /><br />
Some SH4 implementations have a hardware bug which restricts the instructions
that should follow this instruction for safe operation.  There are two
recommended workarounds:
Put 8 NOP instructions following the SLEEP instruction.
Put 5 "OR R0,R0" instructions following the SLEEP instruction
<br />
For more information see the document "tnsh7456ae.pdf".
)"},

  operation
{R"(
void SLEEP (void)
{
  Sleep_standby();
}
)"},

  example
{R"(
SLEEP ;Enters power-down mode
)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stbank	R0,@Rn",
  SH2A,
  abstract { "R0 → (specified register bank entry)" },
  code { "0100nnnn11100001" },

  issue { SH2A, "7" },
  latency { SH2A, "6" },



  description
{R"(
R0 is transferred to the register bank entry indicated by the contents of
general register Rn. The register bank number and register stored in the bank
are specified by general register Rn.
<br /><img src="stbank.svg" height="400" />
)"},

  note
{R"(
The architecture supports a maximum of 512 banks. However, the number of banks
differs depending on the product.
)"},

  operation
{R"(
void STBANK (int n)
{
  Write_Bank_32 (R[n], R[0])
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	SR,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "SR → Rn" },
  code { "0000nnnn00000010" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
sr ← SignExtend 32(SR);
op1 ← sr
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SR in the destination.
)"},

  note
{R"(
This instruction is only usable in privileged mode. Issuing this instruction in
user mode will cause an illegal instruction exception.
)"},

  operation
{R"(
void STCSR (int n)
{
  R[n] = SR;
  PC += 2;
}
)"},

  example
{R"(
STC SR,R0 ;Before execution: R0 = H'FFFFFFFF, SR = H'00000000
;After execution: R0 = H'00000000
)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	SR,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, SR → (Rn)" },
  code { "0100nnnn00000011" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "1/2", SH4A, "1", SH2A, "2", SH4, "2/2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
sr ← SignExtend 32(SR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, sr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SR in the destination.
)"},

  note
{R"(
This instruction is only usable in privileged mode. Issuing this instruction in
user mode will cause an illegal instruction exception.
)"},

  operation
{R"(
void STCMSR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], SR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	TBR,Rn",
  SH2A,
  environments { { { SH2A, "Interrupt Disabled" } } },
  abstract { "TBR → Rn" },
  code { "0000nnnn01001010" },

  issue { SH2A, "1" },
  latency { SH2A, "1" },



  description
{R"(
Stores control register TBR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCTBR (int n)
{
  R[n] = TBR;
  PC += 2;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	GBR,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "GBR → Rn" },
  code { "0000nnnn00010010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },

  brief
{R"(
gbr ← SignExtend 32 (GBR);
op1 ← gbr;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register GBR in the destination.
)"},

  note
{R"(
This instruction can also be issued in user mode.
)"},

  operation
{R"(
STCGBR (int n)
{
  R[n] = GBR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	GBR,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, GBR → (Rn)" },
  code { "0100nnnn00010011" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "1/2", SH4A, "1", SH2A, "1", SH4, "2/2" },

  brief
{R"(
gbr ← SignExtend 32 (GBR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, gbr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register GBR in the destination.
)"},

  note
{R"(
This instruction can also be issued in user mode.
)"},

  operation
{R"(
void STCMGBR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], GBR);
  PC += 2;
}
)"},

  example
{R"(
STC.L GBR,@-R15 ;Before execution: R15 = H'10000004
;After execution: R15 = H'10000000, @R15 = GBR
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	VBR,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "VBR → Rn" },
  code { "0000nnnn00100010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
vbr ← SignExtend 32 (VBR);
op1 ← vbr
R n ← Register(op1);
)"},
  description
{R"(
Stores control register VBR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCVBR (int n)
{
  R[n] = VBR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	VBR,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, VBR → (Rn)" },
  code { "0100nnnn00100011" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "2", SH2, "2", SH3, "1/2", SH4A, "1", SH2A, "1", SH4, "2/2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
vbr ← SignExtend 32 (VBR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, vbr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register VBR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMVBR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], VBR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	MOD,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "MOD → Rn" },
  code { "0000nnnn01010010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores control register MOD in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMOD (int n)
{
  R[n] = MOD;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	MOD,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, MOD → (Rn)" },
  code { "0100nnnn01010011" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/2" },



  description
{R"(
Stores control register MOD in the destination.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 2 cycles.
)"},

  operation
{R"(
void STCMMOD (int n)
{
  R[n] -= 4;
  Write_32 (R[n], MOD);
  PC += 2;
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	RE,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "RE → Rn" },
  code { "0000nnnn01110010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores control register RE in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCRE (int n)
{
  R[n] = RE;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	RE,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, RE → (Rn)" },
  code { "0100nnnn01110011" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/2" },



  description
{R"(
Stores control register RE in the destination.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 2 cycles.
)"},

  operation
{R"(
void STCMRE (int n)
{
  R[n] -= 4;
  Write_32 (R[n], RE);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	RS,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "RS → Rn" },
  code { "0000nnnn01100010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores control register RS in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCRS (int n)
{
  R[n] = RS;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	RS,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, RS → (Rn)" },
  code { "0100nnnn01100011" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1/2" },



  description
{R"(
Stores control register RS in the destination.
)"},

  note
{R"(
On the SH-DSP the latency of this instruction is 2 cycles.
)"},

  operation
{R"(
void STCMRS (int n)
{
  R[n] -= 4;
  Write_32 (R[n], RS);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	SGR,Rn",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "SGR → Rn" },
  code { "0000nnnn00111010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH4A, "1", SH4, "3" },
  latency { SH4A, "1", SH4, "3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
sgr ← SignExtend 32(SGR);
op1 ← sgr
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SGR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCSGR (int n)
{
  R[n] = SGR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	SGR,@-Rn",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, SGR → (Rn)" },
  code { "0100nnnn00110010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH4A, "1", SH4, "3" },
  latency { SH4A, "1", SH4, "3/3" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
sgr ← SignExtend 32(SGR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, sgr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SGR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMSGR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], SGR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	SSR,Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "SSR → Rn" },
  code { "0000nnnn00110010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "2" },
  latency { SH3, "1", SH4A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
ssr ← SignExtend 32(SSR);
op1 ← ssr
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SSR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCSSR (int n)
{
  R[n] = SSR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	SSR,@-Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, SSR → (Rn)" },
  code { "0100nnnn00110011" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "2" },
  latency { SH3, "1/2", SH4A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
ssr ← SignExtend 32(SSR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, ssr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SSR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMSSR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], SSR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	SPC,Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "SPC → Rn" },
  code { "0000nnnn01000010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "2" },
  latency { SH3, "1", SH4A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
spc ← SignExtend 32 (SPC);
op1 ← spc
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SPC in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCSPC (int n)
{
  R[n] = SPC;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	SPC,@-Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, SPC → (Rn)" },
  code { "0100nnnn01000011" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH4A, "1", SH4, "2" },
  latency { SH4A, "1", SH4, "2/2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
spc ← SignExtend 32 (SPC);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, spc);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register SPC in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMSPC (int n)
{
  R[n] -= 4;
  Write_32 (R[n], SPC);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	DBR,Rn",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "DBR → Rn" },
  code { "0000nnnn11111010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH4A, "1", SH4, "2" },
  latency { SH4A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
dbr ← SignExtend 32 (DBR);
op1 ← dbr
R n ← Register(op1);
)"},
  description
{R"(
Stores control register DBR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCDBR (int n)
{
  R[n] = DBR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	DBR,@-Rn",
  SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, DBR → (Rn)" },
  code { "0100nnnn11110010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH4A, "1", SH4, "2" },
  latency { SH4A, "1", SH4, "2/2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
dbr ← SignExtend 32 (DBR);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, dbr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores control register DBR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMDBR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], DBR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc	Rm_BANK,Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rm_BANK → Rn (m = 0-7)" },
  code { "0000nnnn1mmm0010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "1", SH4A, "1", SH4, "2" },
  latency { SH3, "1", SH4A, "1", SH4, "2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm _BANK);
op2 ← op1;
Rn ← Register(op2);
)"},
  description
{R"(
Stores a banked general register in the destination. Rn_BANK0 is accessed when
the RB bit in the SR register is 1, and Rn_BANK1 is accessed when this bit is 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCRm_BANK (int n)
{
  R[n] = Rm_BANK;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "stc.l	Rm_BANK,@-Rn",
  SH3 | SH4 | SH4A,
  environments { { { SH4A, "Privileged" } } },
  abstract { "Rn-4 → Rn, Rm_BANK → (Rn) (m = 0-7)" },
  code { "0100nnnn1mmm0011" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH3, "2", SH4A, "1", SH4, "2" },
  latency { SH3, "2", SH4A, "1", SH4, "2/2" },

  brief
{R"(
md ← ZeroExtend1(MD);
IF (md = 0)
THROW RESINST;
op1 ← SignExtend 32 (Rm_BANK);
op2 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op2 - 4);
WriteMemory 32(address, op1);
op2 ← address;
R n ← Register(op2);
)"},
  description
{R"(
Stores a banked general register in the destination. Rn_BANK0 is accessed when
the RB bit in the SR register is 1, and Rn_BANK1 is accessed when this bit is 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void STCMRm_BANK (int n)
{
  R[n] -= 4;
  Write_32 (R[n], Rm_BANK);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
General illegal instruction exception
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	MACH,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "MACH → Rn" },
  code { "0000nnnn00001010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
mach ← SignExtend 32 (MACH);
op1 ← mach;
R n ← Register(op1)
)"},
  description
{R"(
Stores system register MACH in the destination.
)"},

  note
{R"(
On SH1, the value of bit 9 is transferred to and stored in the higher 22 bits
(bits 31 to 10) of the destination.
<br /><br />
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.
)"},

  operation
{R"(
void STSMACH (int n)
{
  R[n] = MACH;

  #if SH1
  if ((R[n] & 0x00000200) == 0)
    R[n] &= 0x000003FF;
  else
    R[n] |= 0xFFFFFC00;

  #endif

  PC += 2;
}
)"},

  example
{R"(
STS MACH,R0 ;Before execution: R0 = H'FFFFFFFF, MACH = H'00000000
;After execution: R0 = H'00000000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	MACH,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, MACH → (Rn)" },
  code { "0100nnnn00000010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },

  brief
{R"(
mach ← SignExtend 32 (MACH);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, mach);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores system register MACH in the destination.
)"},

  note
{R"(
On SH1, the value of bit 9 is transferred to and stored in the higher 22 bits
(bits 31 to 10) of the destination.
<br /><br />
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.
)"},

  operation
{R"(
void STSMMACH (int n)
{
  R[n] -= 4;

  #if SH1
  if ((MACH & 0x00000200) == 0)
    Write_32 (R[n], MACH & 0x000003FF);
  else
    Write_32 (R[n], MACH | 0xFFFFFC00)

  #else
  Write_32 (R[n], MACH);

  #endif

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	MACL,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "MACL → Rn" },
  code { "0000nnnn00011010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "2", SH4, "3" },

  brief
{R"(
macl ← SignExtend 32(MACL);
op1 ← macl;
R n ← Register(op1);
)"},
  description
{R"(
Stores system register MACL in the destination.
)"},

  note
{R"(
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.
)"},

  operation
{R"(
void STSMACL (int n)
{
  R[n] = MACL;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	MACL,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, MACL → (Rn)" },
  code { "0100nnnn00010010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },

  brief
{R"(
macl ← SignExtend 32(MACL);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, macl);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores system register MACL in the destination.
)"},

  note
{R"(
On SH4, when an LDS to MAC* is followed by an STS.L MAC*,@-Rn instruction, the
latency of the LDS to MAC* is 4 cycles.
)"},

  operation
{R"(
void STSMMACL (int n)
{
  R[n] -= 4;
  Write_32 (R[n], MACL);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	PR,Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "PR → Rn" },
  code { "0000nnnn00101010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },

  brief
{R"(
pr ← SignExtend 32 (PR’);
op1 ← pr;
R n ← Register(op1);
)"},
  description
{R"(
Stores system register PR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSPR (int n)
{
  R[n] = PR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	PR,@-Rn",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  environments { { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, PR → (Rn)" },
  code { "0100nnnn00100010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2" },
  latency { SH1, "1", SH2, "1", SH3, "1", SH4A, "1", SH2A, "1", SH4, "2/2" },

  brief
{R"(
pr ← SignExtend 32 (PR’);
op1 ← SignExtend 32 (Rn);
address ← ZeroExtend 32(op1 - 4);
WriteMemory 32(address, pr);
op1 ← address;
R n ← Register(op1);
)"},
  description
{R"(
Stores system register PR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMPR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], PR);
  PC += 2;
}
)"},

  example
{R"(
STS.L PR,@–R15 ;Before execution: R15 = H'10000004
;After execution: R15 = H'10000000, @R15 = PR
)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Initial page write exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	DSR,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "DSR → Rn" },
  code { "0000nnnn01101010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },

  name { "Store from FPU System Register" },
  description
{R"(
Stores DSP register DSR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSDSR (int n)
{
  R[n] = DSR;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	DSR,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, DSR → (Rn)" },
  code { "0100nnnn01100010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },

  name { "Store from FPU System Register" },
  description
{R"(
Stores DSP register DSR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMDSR (int n)
{
  R[n] -= 4;
  Write_32 (R[n], DSR);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	A0,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "A0 → Rn" },
  code { "0000nnnn01111010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register A0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSA0 (int n)
{
  R[n] = A0;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	A0,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, A0 → (Rn)" },
  code { "0100nnnn01110010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register A0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMA0 (int n)
{
  R[n] -= 4;
  Write_32 (R[n], A0);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	X0,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "X0 → Rn" },
  code { "0000nnnn10001010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register X0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSX0 (int n)
{
  R[n] = X0;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	X0,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, X0 → (Rn)" },
  code { "0100nnnn10000010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register X0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMX0 (int n)
{
  R[n] -= 4;
  Write_32 (R[n], X0);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	X1,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "X1 → Rn" },
  code { "0000nnnn10011010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register X1 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSX1 (int n)
{
  R[n] = X1;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	X1,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, X1 → (Rn)" },
  code { "0100nnnn10010010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register X1 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMX1 (int n)
{
  R[n] -= 4;
  Write_32 (R[n], X1);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	Y0,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Y0 → Rn" },
  code { "0000nnnn10101010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register Y0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSY0 (int n)
{
  R[n] = Y0;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	Y0,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, Y0 → (Rn)" },
  code { "0100nnnn10100010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register Y0 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMY0 (int n)
{
  R[n] -= 4;
  Write_32 (R[n], Y0);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	Y1,Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Y1 → Rn" },
  code { "0000nnnn10111010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register Y1 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSY1 (int n)
{
  R[n] = Y1;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	Y1,@-Rn",
  SH_DSP,
  environments { { { SH_DSP, "Interrupt Disabled" } } },
  abstract { "Rn-4 → Rn, Y1 → (Rn)" },
  code { "0100nnnn10110010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores DSP register Y1 in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMY1 (int n)
{
  R[n] -= 4;
  Write_32 (R[n], Y1);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "synco",
  SH4A,
  abstract { "Prevents the next instruction from being issued until instructions issued before this instruction has been completed." },
  code { "0000000010101011" },

  group { SH4A, "CO" },
  issue { SH4A, "ud" },
  latency { SH4A, "ud" },



  description
{R"(
This instruction is used to synchronize data operations. When this instruction
is executed, the subsequent bus accesses are not executed until the execution of
all preceding bus accesses has been completed.
)"},

  note
{R"(
The SYNCO instruction can not guarantee the ordering of receipt timing which is
notified by the memory-mapped peripheral resources through the method except bus
when the register is changed by bus accesses. Refer to the description of each
registers to guarantee this ordering.
<br /><br />
Common example usages are:
Ordering access to memory areas which are shared with other memory users
Flushing all write buffers
Stopping memory-access operations from merging and becoming ineffective
Waiting for the completion of cache-control instructions
)"},

  operation
{R"(
void SYNCO (void)
{
  synchronize_data_operaiton ();
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "trapa	#imm",
  SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
  abstract {R"(SH1*,SH2*: PC/SR → stack area, (imm*4 + VBR) → PC
SH3*,SH4*: PC/SR → SPC/SSR, imm*4 → TRA, 0x160 → EXPEVT, VBR + 0x0100 → PC)"},
  code { "11000011iiiiiiii" },

  group { SH4A, "CO", SH4, "CO" },
  issue { SH1, "2", SH2, "2", SH3, "2", SH4A, "14", SH2A, "5", SH4, "7" },
  latency { SH1, "8", SH2, "8", SH3, "8", SH4A, "13", SH2A, "6", SH4, "7" },

  brief
{R"(
imm ← ZeroExtend 8(i);
IF (IsDelaySlot())
THROW ILLSLOT;
THROW TRAP, imm;
)"},
  description
{R"(
Starts trap exception handling.

SH1, SH2 and SH2A:
<br />
The PC and SR values are stored on the stack, and the program branches to an
address specified by the vector. The vector is a memory address obtained by
zero-extending the 8-bit immediate data and then quadrupling it. The PC is the
start address of the next instruction. TRAPA and RTE are both used together for
system calls.
<br /><br />

SH3, SH4 and SH4A:
<br />
The values of (PC + 2), SR, and R15 are saved to SPC, SSR and SGR, and 8-bit
immediate data is stored in the TRA register (bits 9 to 2). The processor mode
is switched to privileged mode (the MD bit in SR is set to 1), and the BL bit
and RB bit in SR are set to 1.  As a result, exception and interrupt requests
are masked (not accepted), and the BANK1 registers (R0_BANK1 to R7_BANK1) are
selected. Exception code 0x160 is written to the EXPEVT register (bits 11 to 0).
The program branches to address (VBR + 0x00000100), indicated by the sum of the
VBR register contents and offset 0x00000100.
)"},

  note
{R"(
Some SH4 implementations have a hardware bug which restricts the instructions
that should follow this instruction for safe operation.  There are two
recommended workarounds:
Put 8 NOP instructions following the TRAPA instruction.
Put 5 "OR R0,R0" instructions following the TRAPA instruction
<br />
For more information see the document "tnsh7456ae.pdf".
<br /><br />
Some SH2E implementations (SH7055) have an FPU related hardware bug which
affects this instruction.  The recommended workaround is to align the addresses
of trapa handlers to 4 bytes and not to place any FPU or FPU related
instructions at addresses 4n + 2 in the handler.
)"},

  operation
{R"(
void TRAPA (int i)
{
  int imm = (0x000000FF & i);

  #if SH1 || SH2 || SH2A
  R[15] -= 4;
  Write_32 (R[15], SR);
  R[15] -= 4;
  Write_32 (R[15], PC + 2);
  PC = Read_32 (VBR + (imm << 2));

  #elif SH3 || SH4 || SH4A
  TRA = imm << 2;
  SSR = SR;
  SPC = PC + 2;
  SGR = R15;
  SR.MD = 1;
  SR.BL = 1;
  SR.RB = 1;
  EXPEVT = 0x00000160;
  PC = VBR + 0x00000100;

  #endif
}
)"},

  example
{R"(
VBR+H'80 .data.l 10000000 ;
..........
TRAPA #H'20 ;Branches to an address specified by data in address VBR + H'80
TST #0,R0 ;← Return address from the trap routine (stacked PC value)
..........
100000000 XOR R0,R0 ;← Trap routine entrance
100000002 RTE ;Returns to the TST instruction
100000004 NOP ;Executes NOP before RTE
)"},

  exceptions
{R"(
Unconditional trap
Slot illegal instruction exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "32 Bit Floating-Point Data Transfer Instructions (FPSCR.SZ = 0)",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRm → FRn" },
  code { "1111nnnnmmmm1100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2 ← op1;
FR n ← FloatRegister 32(op2);
)"},
  description
{R"(
Transfers FRm contents to FRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV (int m, int n)
{
  FR[n] = FR[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	@Rm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(Rm) → FRn" },
  code { "1111nnnnmmmm1000" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0/2", SH4, "2" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemory 32 (address);
FR 2n ← FloatRegister 32 (op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to FRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_LOAD (int m, int n)
{
  FR[n] = Read_32 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	FRm,@Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRm → (Rn)" },
  code { "1111nnnnmmmm1010" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2);
WriteMemory 32(address, op1);
)"},

  description
{R"(
Transfers FRm contents to memory at address indicated by Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_STORE (int m, int n)
{
  Write_32 (R[n], FR[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	@Rm+,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(Rm) → FRn, Rm+4 → Rm" },
  code { "1111nnnnmmmm1001" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1/2", SH4, "1/2" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemory 32 (address);
op1 ← op1 + 4;
R m ← Register(op1);
FR n ← FloatRegister 32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to FRn, and adds 4 to
Rm.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_RESTORE (int m, int n)
{
  FR[n] = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	FRm,@-Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "Rn-4 → Rn, FRm → (Rn)" },
  code { "1111nnnnmmmm1011" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1/0", SH4, "1/1" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2 - 4);
WriteMemory 32(address, op1);
op2 ← address;
R n ← Register(op2);
)"},
  description
{R"(
Subtracts 4 from Rn, and transfers FRm contents to memory at address indicated
by resulting Rn value.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_SAVE (int m, int n)
{
  Write_32 (R[n] - 4, FR[m]);
  R[n] -= 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	@(R0,Rm),FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(R0 + Rm) → FRn" },
  code { "1111nnnnmmmm0110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0/2", SH4, "2" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op1);
op2 ← ReadMemory 32 (address);
FR n ← FloatRegister 32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by (R0 + Rm) to FRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_LOAD (int m, int n)
{
  FR[n] = Read_32 (R[0] + R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	FRm,@(R0,Rn)",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRm → (R0 + Rn)" },
  code { "1111nnnnmmmm0111" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "1" },

  restriction { "Available only when SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← FloatValue 32 (FRm );
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op2);
WriteMemory 32(address, op1)
)"},
  description
{R"(
Transfers FRm contents to memory at address indicated by (R0 + Rn).
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_STORE (int m, int n)
{
  Write_32 (R[0] + R[n], FR[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	@(disp12,Rm),FRn",
  SH2A,
  abstract { "(disp × 4 + Rm) → FRn" },
  code { "0011nnnnmmmm0001 0111dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "0/2" },



  description
{R"(
Transfers memory contents at the address indicated by (disp + Rn) to FRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_DISP12_LOAD (int m, int n, int d)
{
  long disp = (0x00000FFF & (long)d);
  FR[n] = Read_32 (R[m] + (disp << 2));
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.s	FRm,@(disp12,Rn)",
  SH2A,
  abstract { "FRm → (disp × 4 + Rn)" },
  code { "0011nnnnmmmm0001 0011dddddddddddd" },

  issue { SH2A, "1" },
  latency { SH2A, "0" },



  description
{R"(
Transfers FRm contents to memory at the address indicated by (disp + Rn).
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_DISP12_STORE (int m, int n, int d)
{
  long disp = (0x00000FFF & (long)d);
  Write_32 (R[n] + (disp << 2), FR[m]);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

});

insn_blocks.push_back
(insns { "64 Bit Floating-Point Data Transfer Instructions (FPSCR.SZ = 1)",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRm → DRn" },
  code { "1111nnn0mmm01100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "1", SH4, "0" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValuePair32(FP 2m);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2 ← op1;
FP2n ← FloatRegisterPair32(op2);
)"},

  description
{R"(
Transfers DRm contents to DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_DR (int m, int n)
{
  DR[n >> 1] = DR[m >> 1];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	DRm,XDn",
  SH4 | SH4A,
  abstract { "DRm → XDn" },
  code { "1111nnn1mmm01100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "0" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValuePair32(DR2m);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2 ← op1;
XD2n ← FloatRegisterPair 32(op2);
)"},
  description
{R"(
Transfers DRm contents to XDn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_DRXD (int m, int n)
{
  XD[n >> 1] = DR[m >> 1];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	XDm,DRn",
  SH4 | SH4A,
  abstract { "XDm → DRn" },
  code { "1111nnn0mmm11100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "0" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValuePair32(DR2m);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2 ← op1;
XD2n ← FloatRegisterPair 32(op2);
)"},
  description
{R"(
Transfers XDm contents to DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_XDDR (int m, int n)
{
  DR[n >> 1] = XD[m >> 1];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	XDm,XDn",
  SH4 | SH4A,
  abstract { "XDm → XDn" },
  code { "1111nnn1mmm11100" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "0" },

  name { "Floating-point Move Extension" },
  description
{R"(
Transfers XDm contents to XDn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_XDXD (int m, int n)
{
  XD[n >> 1] = XD[m >> 1];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@Rm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "(Rm) → DRn" },
  code { "1111nnn0mmmm1000" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/4", SH4, "2" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemoryPair 32 (address);
FP2n ← FloatRegisterPair32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_LOAD_DR (int m, int n)
{
  DR[n >> 1] = Read_64 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@Rm,XDn",
  SH4 | SH4A,
  abstract { "(Rm) → XDn" },
  code { "1111nnn1mmmm1000" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "2" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemoryPair 32 (address);
XD 2n ← FloatRegisterPair 32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to XDn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_LOAD_XD (int m, int n)
{
  XD[n >> 1] = Read_64 (R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	DRm,@Rn",
  SH4 | SH4A | SH2A,
  abstract { "DRm → (Rn)" },
  code { "1111nnnnmmm01010" },
  mnemonic { "FMOV.D?" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "0", SH4, "1" },


  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValuePair32(FP 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2);
WriteMemoryPair 32 (address, op1);
)"},
  description
{R"(
Transfers DRm contents to memory at address indicated by Rn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_STORE_DR (int m, int n)
{
  Write_64 (R[n], DR[m >> 1]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	XDm,@Rn",
  SH4 | SH4A,
  abstract { "XDm → (Rn)" },
  code { "1111nnnnmmm11010" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValuePair32(XD 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2);
WriteMemoryPair 32 (address, op1);
)"},
  description
{R"(
Transfers contents of memory at address indicated by (R0 + Rm) to XDn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_STORE_XD (int m, int n)
{
  Write_64 (R[n], XD[m >> 1]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@Rm+,DRn",
  SH4 | SH4A | SH2A,
  abstract { "(Rm) → DRn, Rm + 8 → Rm" },
  code { "1111nnn0mmmm1001" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "1/4", SH4, "1/2" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemoryPair 32 (address);
op1 ← op1 + 8;
R m ← Register(op1);
FP2n ← FloatRegisterPair32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to DRn, and adds 8 to
Rm.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_RESTORE_DR (int m, int n)
{
  DR[n >> 1] = Read_64 (R[m]);
  R[m] += 8;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@Rm+,XDn",
  SH4 | SH4A,
  abstract { "(Rm) → XDn, Rm+8 → Rm" },
  code { "1111nnn1mmmm1001" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1/2" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
op2 ← ReadMemoryPair 32 (address);
op1 ← op1 + 8;
R m ← Register(op1);
XD2n ← FloatRegisterPair 32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by Rm to XDn, and adds 8 to
Rm.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_RESTORE_XD (int m, int n)
{
  XD[n >> 1] = Read_64 (R[m]);
  R[m] += 8;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	DRm,@-Rn",
  SH4 | SH4A | SH2A,
  abstract { "Rn-8 → Rn, DRm → (Rn)" },
  code { "1111nnnnmmm01011" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/1", SH4, "1/1" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValuePair32(FP 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2 - 8);
WriteMemoryPair 32 (address, op1);
op2 ← address;
R n ← Register(op2);
)"},

  description
{R"(
Subtracts 8 from Rn, and transfers DRm contents to memory at address indicated
by resulting Rn value.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_SAVE_DR (int m, int n)
{
  Write_64 (R[n] - 8, DR[m >> 1]);
  R[n] -= 8;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	XDm,@-Rn",
  SH4 | SH4A,
  abstract { "Rn-8 → Rn, (Rn) → XDm" },
  code { "1111nnnnmmm11011" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1/1" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValuePair32(XD 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op2 - 8);
WriteMemoryPair 32 (address, op1);
op2 ← address;
R n ← Register(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Subtracts 8 from Rn, and transfers XDm contents to memory at address indicated
by resulting Rn value.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_SAVE_XD (int m, int n)
{
  Write_64 (R[n] - 8, XD[m >> 1]);
  R[n] -= 8;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@(R0,Rm),DRn",
  SH4 | SH4A | SH2A,
  abstract { "(R0 + Rm) → DRn" },
  code { "1111nnn0mmmm0110" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/4", SH4, "2" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op1);
op2 ← ReadMemoryPair 32 (address);
FP2n ← FloatRegisterPair32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by (R0 + Rm) to DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_LOAD_DR (int m, int n)
{
  DR[n >> 1] = Read_64 (R[0] + R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	@(R0,Rm),XDn",
  SH4 | SH4A,
  abstract { "(R0 + Rm) → XDn" },
  code { "1111nnn1mmmm0110" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "2" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op1);
op2 ← ReadMemoryPair 32 (address);
XD 2n ← FloatRegisterPair 32(op2);
)"},
  description
{R"(
Transfers contents of memory at address indicated by (R0 + Rm) to XDn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_LOAD_XD (int m, int n)
{
  XD[n >> 1] = Read_64 (R[0] + R[m]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	DRm,@(R0,Rn)",
  SH4 | SH4A | SH2A,
  abstract { "DRm → (R0 + Rn)" },
  code { "1111nnnnmmm00111" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "2", SH4, "1" },
  latency { SH4A, "1", SH2A, "0", SH4, "1" },

  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← FloatValuePair32(FP 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op2);
WriteMemoryPair 32 (address, op1);
)"},
  description
{R"(
Transfers DRm contents to memory at address indicated by (R0 + Rn).
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_STORE_DR (int m, int n)
{
  Write_64 (R[0] + R[n], DR[m >> 1]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov	XDm,@(R0,Rn)",
  SH4 | SH4A,
  abstract { "XDm → (R0 + Rn)" },
  code { "1111nnnnmmm10111" },
  mnemonic { "FMOV.D" }, // ??

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1" },

  name { "Floating-point Move Extension" },
  restriction { "Available only when PR=0 and SZ=1" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
r0 ← SignExtend 32 (R0);
op1 ← FloatValuePair32(XD 2m);
op2 ← SignExtend 32 (Rn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(r0 + op2);
WriteMemoryPair 32 (address, op1);
)"},
  description
{R"(
Transfers XDm contents to memory at address indicated by (R0 + Rn).
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_STORE_XD (int m, int n)
{
  Write_64 (R[0] + R[n], XD[m >> 1]);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.d	@(disp12,Rm),DRn",
  SH2A,
  abstract { "(disp × 8 + Rm) → DRn" },
  code { "0011nnn0mmmm0001 0111dddddddddddd" },

  issue { SH2A, "2" },
  latency { SH2A, "0/4" },



  description
{R"(
Transfers memory contents at the address indicated by (disp + Rn) to DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_DISP12_LOAD_DR (int m, int n, int d)
{
  long disp = (0x00000FFF & (long)d);
  DR[n >> 1] = Read_64 (R[m] + (disp << 3));
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmov.d	DRm,@(disp12,Rn)",
  SH2A,
  abstract { "DRm → (disp × 8 + Rn)" },
  code { "0011nnnnmmm00001 0011dddddddddddd" },

  issue { SH2A, "2" },
  latency { SH2A, "0" },



  description
{R"(
Transfers DRm contents to memory at the address indicated by (disp + Rn).
)"},

  note
{R"(

)"},

  operation
{R"(
void FMOV_INDEX_DISP12_STORE_DR (int m, int n, int d)
{
  long disp = (0x00000FFF & (long)d);
  Write_64 (R[n] + (disp << 3), DR[m >> 1]);
  PC += 4;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},


});

insn_blocks.push_back
(insns { "Floating-Point Single-Precision Instructions (FPSCR.PR = 0)",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fldi0	FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "0x00000000 → FRn" },
  code { "1111nnnn10001101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← 0x00000000;
FR n ← FloatRegister 32(op1);
)"},

  description
{R"(
When FPSCR.PR = 0, this instruction loads floating-point 0.0 (0x00000000) into
FRn.<br />
If FPSCR.PR = 1, the instruction is handled as an illegal instruction.
)"},

  note
{R"(

)"},

  operation
{R"(
void FLDI0 (int n)
{
  FR[n] = 0x00000000;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fldi1	FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "0x3F800000 → FRn" },
  code { "1111nnnn10011101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← 0x3F800000;
FR n ← FloatRegister 32(op1);
)"},

  description
{R"(
When FPSCR.PR = 0, this instruction loads floating-point 1.0 (0x3F800000) into
FRn.<br />
If FPCSR.PR = 1, the instruction is handled as an illegal instruction.
)"},

  note
{R"(

)"},

  operation
{R"(
void FLDI1 (int n)
{
  FR[n] = 0x3F800000;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "flds	FRm,FPUL",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRm → FPUL" },
  code { "1111mmmm00011101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValue 32 (FRm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fpul ← op1;
FPUL ← ZeroExtend 32(fpul);
)"},

  description
{R"(
Transfers the contents of floating-point register FRm into system register FPUL.
)"},

  note
{R"(

)"},

  operation
{R"(
void FLDS (int m)
{
  FPUL = FR[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsts	FPUL,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FPUL → FRn" },
  code { "1111nnnn00001101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  brief
{R"(
sr ← ZeroExtend 32 (SR);
fpul ← SignExtend 32(FPUL);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← fpul;
FR n ← FloatRegister 32(op1);
)"},
  description
{R"(
Transfers the contents of system register FPUL to floating-point register FRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FSTS (int n)
{
  FR[n] = FPUL;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fabs	FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn & 0x7FFFFFFF → FRn" },
  code { "1111nnnn01011101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← FABS_S(op1);
FR n ← FloatRegister 32(op1);
)"},

  description
{R"(
Clears the most significant bit of the contents of floating-point register FRn
to 0, and stores the result in FRn.
)"},

  note
{R"(
The cause and flag fields in FPSCR are not updated.
<br /><br />
A double-precision floating-point register DRn consists of a single-precision
floating-point register pair FRn:FRn+1, where FRn is the high part and FRn+1 is
the low part.  This instruction operates only on the high part and thus the
operation performed for double and single precision setting is the same.  It is
not necessary to adjust the FPSRC.PR setting before this instruction.
)"},

  operation
{R"(
void FABS (int n)
{
  FR[n] = FR[n] & 0x7FFFFFFFF;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fneg	FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn ^ 0x80000000 → FRn" },
  code { "1111nnnn01001101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← FNEG_S(op1);
FR n ← FloatRegister 32(op1);
)"},
  description
{R"(
Inverts the most significant bit (sign bit) of the contents of floating-point
register FRn, and stores the result in FRn.
)"},

  note
{R"(
The cause and flag fields in FPSCR are not updated.
<br /><br />
A double-precision floating-point register DRn consists of a single-precision
floating-point register pair FRn:FRn+1, where FRn is the high part and FRn+1 is
the low part.  This instruction operates only on the high part and thus the
operation performed for double and single precision setting is the same.  It is
not necessary to adjust the FPSRC.PR setting before this instruction.
)"},

  operation
{R"(
void FNEG (int n)
{
  FR[n] = -FR[n];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fadd	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn + FRm → FRn" },
  code { "1111nnnnmmmm0000" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FADD_S(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},

  description
{R"(
Arithmetically adds the two single-precision floating-point numbers in FRn and
FRm, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and FRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fadd.svg" height="300" />
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FADD (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case NORM:
        normal_faddsub (m, n, ADD);
        break;
      case PZERO:
      case NZERO:
        register_copy (m, n);
        break;
      default:
        break;
      }
      break;

    case PZERO:
      switch (data_type_of (n))
      {
      case NZERO:
        zero (n, 0);
        break;
      default:
        break;
      }
      break;

    case NZERO:
      break;

    case PINF:
      switch (data_type_of (n))
      {
      case NINF:
        invalid (n);
        break;
      default:
        inf (n, 0);
        break;
      }
      break;

     case NINF:
       switch (data_type_of (n))
       {
       case PINF:
         invalid (n);
         break;
       default:
         inf (n, 1);
         break;
       }
       break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
FRn and FRm have the same sign and the exponent of at least one value is 0xFE


Underflow
<br />
Generation of underflow-exception traps
<br />
FRn and FRm have different signs and neither has an exponent greater than 0x18


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsub	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn - FRm → FRn" },
  code { "1111nnnnmmmm0001" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FSUB_S(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Arithmetically subtracts the single-precision floating-point number in FRm from
the single-precision floating-point number in FRn, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction.  When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and FRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fsub.svg" height="300" />
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FSUB (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case NORM:
        normal_faddsub (m, n, SUB);
        break;
      case PZERO:
      case NZERO:
        register_copy (m, n);
        FR[n] = -FR[n];
        break;
      default:
        break;
      }
      break;

    case PZERO:
      break;

    case NZERO:
      switch (data_type_of (n))
      {
      case NZERO:
        zero (n, 0);
        break;
      default:
        break;
      }
      break;

    case PINF:
      switch (data_type_of (n))
      {
      case PINF:
        invalid (n);
        break;
      default:
        inf (n, 1);
        break;
      }
      break;

    case NINF:
      switch (data_type_of (n))
      {
      case NINF:
        invalid (n);
        break;
      default:
        inf (n, 0);
        break;
      }
      break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
FRn and FRm have the same sign and the exponent of at least one value is 0xFE


Underflow
<br />
Generation of underflow-exception traps
<br />
FRn and FRm have different signs and neither has an exponent greater than 0x18


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmul	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn × FRm → FRn" },
  code { "1111nnnnmmmm0010" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FMUL_S(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Arithmetically multiplies the two single-precision floating-point numbers in
FRn and FRm, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and FRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fmul.svg" height="300" />
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FMUL (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      case PINF:
      case NINF:
        inf (n, sign_of (m) ^ sign_of (n));
        break;
      default:
        normal_fmul (m, n);
        break;
      }
      break;

    case PZERO:
    case NZERO:
      switch (data_type_of (n))
      {
      case PINF:
      case NINF:
        invalid (n);
        break;
      default:
        zero (n,sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case PINF:
    case NINF:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      default:
        inf (n, sign_of (m) ^ sign_of (n));
        break
      }
      break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
(exponent of FRn) + (exponent of FRm) - 0x7F is not less than 0xFE


Underflow
<br />
Generation of underflow-exception traps
<br />
When both FRn and FRm are normalized numbers:
(exponent of FRn) + (exponent of FRm) - 0x7F is not more than 0x00
<br />
When at least FRn or FRm is not a normalized number:
(exponent of FRn) + (exponent of FRm) - 0x7F is not more than 0x18


Inexact

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmac	FR0,FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FR0 × FRm + FRn → FRn" },
  code { "1111nnnnmmmm1110" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
fr0 ← FloatValue 32 (FR0);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FMAC_S(fr0, op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},

  description
{R"(
Arithmetically multiplies the two single-precision floating-point numbers in
FR0 and FRm, arithmetically adds the contents of FRn, and stores the result in
FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction.  When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and FRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fmac.svg" height="942" />
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
<br /><br />
This instruction rounds only the final result and does not round the
intermediate result of the multiplication.  Thus, for IEEE 754 compliant code,
this instruction cannot be used as a replacement for individual FADD and FMUL
instructions.
)"},

  operation
{R"(
void FMAC (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (FPSCR_PR == 1)
    undefined_operation ();

  else if (data_type_of (0) == sNaN
           || data_type_of (m) == sNaN
           || data_type_of (n) == sNaN)
    invalid (n);

  else if (data_type_of (0) == qNaN || data_type_of (m) == qNaN)
    qnan (n);

  else if (data_type_of (0) == DENORM || data_type_of (m) == DENORM)
    set_E ();

  else
    switch (data_type_of (0))
    {
    case NORM:
      switch (data_type_of (m))
      {
      case PZERO:
      case NZERO:
        switch (data_type_of (n))
        {
        case DENORM:
          set_E ();
          break;
        case qNaN:
          qnan (n);
          break;
        case PZERO:
        case NZERO:
          zero (n, sign_of (0) ^ sign_of (m) ^ sign_of (n));
          break;
        default:
          break;
        }

      case PINF:
      case NINF:
        switch (data_type_of (n))
        {
        case DENORM:
          set_E ();
          break;
        case qNaN:
          qnan (n);
          break;
        case PINF:
        case NINF:
          if (sign_of (0) ^ sign_of (m) ^ sign_of (n))
            invalid (n);
          else
            inf (n, sign_of (0) ^ sign_of (m));
          break;
        default:
          inf (n, sign_of (0) ^ sign_of (m));
          break;
        }

      case NORM:
        switch (data_type_of (n))
        {
        case DENORM:
          set_E ();
          break;
        case qNaN:
          qnan (n);
          break;
        case PINF:
        case NINF:
          inf (n, sign_of (n));
          break;
        case PZERO:
        case NZERO:
        case NORM:
          normal_fmac (m, n);
          break;
        }
        break;

      case PZERO:
      case NZERO:
        switch (data_type_of (m))
        {
        case PINF:
        case NINF:
          invalid (n);
          break;
        case PZERO:
        case NZERO:
        case NORM:
          switch (data_type_of (n))
          {
          case DENORM:
            set_E ();
            break;
          case qNaN:
            qnan (n);
            break;
          case PZERO:
          case NZERO:
            zero (n, sign_of (0) ^ sign_of (m) ^ sign_of (n));
            break;
          default:
            break;
          }
          break;
        }
        break;

      case PINF:
      case NINF:
        switch (data_type_of (m))
        {
        case PZERO:
        case NZERO:
          invalid (n);
          break;
        default:
          switch (data_type_of (n))
          {
          case DENORM:
            set_E ();
            break;
          case qNaN:
            qnan(n);
            break;
          default:
            inf (n, sign_of (0) ^ sign_of (m) ^ sign_of (n));
            break
          }
          break;
        }
        break;
      }
    }
}

void normal_fmac (int m, int n)
{
  union
  {
    int double x;
    int l[4];
  } dstx, tmpx;

  float dstf, srcf;

  if (data_type_of (n) == PZERO || data_type_of (n) == NZERO)
    srcf = 0.0; // flush denormalized value
  else
    srcf = FR[n];

  tmpx.x = FR[0]; // convert single to int double
  tmpx.x *= FR[m]; //exact product
  dstx.x = tmpx.x + srcf;

  if ((dstx.x == srcf && tmpx.x != 0.0)
      || (dstx.x == tmpx.x && srcf != 0.0))
  {
    set_I ();
    if (sign_of (0) ^ sign_of (m) ^ sign_of (n))
    {
      dstx.l[3] -= 1; // correct result
      if (dstx.l[3] == 0xFFFFFFFF)
        dstx.l[2] -= 1;
      if (dstx.l[2] == 0xFFFFFFFF)
        dstx.l[1] -= 1;
      if (dstx.l[1] == 0xFFFFFFFF)
        dstx.l[0] -= 1;
    }
    else
      dstx.l[3] |= 1
  }

  if ((dstx.l[1] & 0x01FFFFFF) || dstx.l[2] || dstx.l[3])
    set_I();

  if(FPSCR_RM == 1)
  {
    dstx.l[1] &= 0xFE000000; // round toward zero
    dstx.l[2] = 0x00000000;
    dstx.l[3] = 0x00000000;
  }

  dstf = dstx.x;
  check_single_exception (&FR[n], dstf);
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
At least one of the following results is not less than 0xFD:
<br />
(exponent of FR0) + (exponent of FRm)
<br />
(exponent of FRn)


Underflow
<br />
Generation of underflow-exception traps
<br />
At least one of the following results is not more than 0x2E:
<br />
(exponent of FR0) + (exponent of FRm)
<br />
(exponent of FRn)


Inexact

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fdiv	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FRn / FRm → FRn" },
  code { "1111nnnnmmmm0011" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "13", SH3E, "13", SH4A, "14", SH2A, "12", SH4, "12/13" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FDIV_S(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuEnableZ(fps) AND FpuCauseZ(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op2);
FPSCR ← ZeroExtend 32 (fps);
)"},


  description
{R"(
Arithmetically divides the single-precision floating-point number in FRn by the
single-precision floating-point number in FRm, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and FRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fdiv.svg" height="300" />
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FDIV (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PINF:
      case NINF:
        inf (n, sign_of (m) ^ sign_of (n));
        break;
      case PZERO:
      case NZERO:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      case DENORM:
        set_E ();
        break;
      default:
        normal_fdiv_single (m, n);
        break;
      }
      break;

    case PZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      case PINF:
      case NINF:
        break;
      default:
        dz (n, sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case NZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      case PINF:
        inf (n, 1);
        break;
      case NINF:
        inf (n, 0);
        break;
      default:
        dz (FR[n], sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case DENORM:
      set_E ();
      break;

    case PINF:
    case NINF:
      switch (data_type_of (n))
      {
      case DENORM:
        set_E ();
        break;
      case PINF:
      case NINF:
        invalid (n);
        break;
      default:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      }
      break;
    }
}

void normal_fdiv_single (int m, int n)
{
  union
  {
    float f;
    int l;
  } dstf, tmpf;

  union
  {
    double d;
    int l[2];
  } tmpd;

  tmpf.f = FR[n];   // save destination value
  dstf.f /= FR[m];  // round toward nearest or even
  tmpd.d = dstf.f;  // convert single to double
  tmpd.d *= FR[m];

  if (tmpf.f != tmpd.d)
    set_I ();
  if (tmpf.f < tmpd.d && FPSCR_RM == 1)
    dstf.l -= 1; // round toward zero

  check_single_exception (&FR[n], dstf.f);
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation
Division by zero
Overflow
<br />
Generation of overflow-exception traps
<br />
(exponent of FRn) - (exponent of FRm) + 0x7F is not less than 0xFF


Underflow
<br />
Generation of underflow-exception traps
<br />
(exponent of FRn) - (exponent of FRm) + 0x7F is not more than 0x01


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsqrt	FRn",
  SH3E, SH4 | SH4A | SH2A,
  abstract { "sqrt (FRn) → FRn" },
  code { "1111nnnn01101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH3E, "13", SH4A, "30", SH2A, "11", SH4, "11/12" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1, fps ← FSQRT_S(op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF (FpuEnableI(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op1);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Finds the arithmetical square root of the single-precision floating-point number
in FRn, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When an exception occurs, correct
exception information is reflected in FPSCR.cause and FPSCR.flag and FRn is not
updated. Appropriate processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fsqrt.svg" height="128" />
)"},

  note
{R"(
SH3E supports only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FSQRT (int n)
{
  PC += 2;
  clear_cause ();

  switch (data_type_of (n))
  {
  case NORM:
    if (sign_of (n) == 0)
      normal_fsqrt_single (n);
    else
      invalid (n);
    break;

  case DENORM:
    if (sign_of (n) == 0)
      set_E ();
    else
      invalid (n);
    break;

  case PZERO:
  case NZERO:
  case PINF:
    break;

  case NINF:
    invalid (n);
    break;

  case qNAN:
    qnan (n);
    break;

  case sNAN:
    invalid (n);
    break;
  }
}

void normal_fsqrt_single (int n)
{
  union
  {
    float f;
    int l;
  } dstf, tmpf;

  union
  {
    double d;
    int l[2];
  } tmpd;

  tmpf.f = FR[n];         // save destination value
  dstf.f = sqrt (FR[n]);  // round toward nearest or even
  tmpd.d = dstf.f;        // convert single to double
  tmpd.d *= dstf.f;

  if (tmpf.f != tmpd.d)
    set_I ();
  if (tmpf.f < tmpd.d && FPSCR_RM == 1)
    dstf.l -= 1;  // round toward zero
  if (FPSCR & ENABLE_I)
    fpu_exception_trap ();
  else
    FR[n] = dstf.f;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcmp/eq	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "If FRn = FRm: 1 → T\nElse: 0 → T" },
  code { "1111nnnnmmmm0100" },
  flags { "Result → T" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "2/4" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
t, fps ← FCMPEQ_D(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
T ← Bit(t);
)"},


  description
{R"(
Arithmetically compares the two single-precision floating-point numbers in FRn
and FRm, and stores 1 in the T bit if they are equal, or 0 otherwise.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcmpeq.svg" height="300" />
)"},

  note
{R"(
Special Cases:
When FPSCR.DN is 1, a positive denormalized number is treated as +0 and a
negative denormalized number as -0. This flush-to-zero treatment is applied before
exception detection and special case handling.
)"},

  operation
{R"(
void FCMP_EQ (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (fcmp_chk_single (m, n) == INVALID)
    fcmp_invalid ();
  else if (fcmp_chk_single (m, n) == EQ)
    T = 1;
  else
    T = 0;
}

int fcmp_chk_single (int m, int n)
{
  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    return INVALID;
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    return UO;
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PINF:
        return GT;
      case NINF:
        return LT;
      default:
        break;
      }
      break;

    case PZERO:
    case NZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        return EQ;
      default:
        break;
      }
      break;

    case PINF:
      switch (data_type_of (n))
      {
        case PINF:
          return EQ;
        default:
          return LT;
      }

    case NINF:
      switch (data_type_of (n))
      {
      case NINF:
        return EQ;
      default:
        return GT;
      }
    }

  if (FR[n] == FR[m])
    return EQ;
  else if (FR[n] > FR[m])
    return GT;
  else
    return LT;
}

void fcmp_invalid (void)
{
  set_V ();

  if ((FPSCR & ENABLE_V) == 0)
    T = 0;
  else
    fpu_exception_trap ();
}

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcmp/gt	FRm,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "If FRn > FRm: 1 → T\nElse: 0 → T" },
  code { "1111nnnnmmmm0101" },
  flags { "Result → T" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "2/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
op2 ← FloatValue 32 (FRn);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
t, fps ← FCMPGT_S(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
T ← Bit(t);
)"},


  description
{R"(
Arithmetically compares the two single-precision floating-point numbers in FRn
and FRm, and stores 1 in the T bit if FRn > FRm, or 0 otherwise.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcmpgt.svg" height="300" />
)"},

  note
{R"(
For IEEE 754 conform less-than-or-equal comparison it is not sufficient to swap
the operands.  The FCMP/EQ must be used as well.
)"},

  operation
{R"(
void FCMP_GT (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (fcmp_chk_single (m, n) == INVALID || fcmp_chk_single (m, n) == UO)
    fcmp_invalid ();
  else if (fcmp_chk_single (m, n) == GT)
    T = 1;
  else
    T = 0;
}

int fcmp_chk_single (int m, int n)
{
  // see description of FCMP/EQ instruction.
}

void fcmp_invalid (void)
{
  // see description of FCMP/EQ instruction.
}

)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "float	FPUL,FRn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(float)FPUL → FRn" },
  code { "1111nnnn00101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
fpul ← SignExtend 32(FPUL);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1, fps ← FLOAT_LS(fpul, fps);
IF (FpuEnableI(fps))
THROW FPUEXC, fps;
FR n ← FloatRegister 32(op1);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Taking the contents of FPUL as a 32-bit integer, converts this integer to a
single-precision floating-point number and stores the result in FRn.
<br /><br />
When FPSCR.enable.I = 1 an FPU exception trap is generated regardless of whether
or not an exception has occurred. When an exception occurs, correct exception
information is reflected in FPSCR.cause and FPSCR.flag, and FRn is not updated.
Appropriate processing should therefore be performed by software.
)"},

  note
{R"(
SH2E and SH3E support only invalid operation (V) and division by zero
(Z) exception flags.
)"},

  operation
{R"(
void FLOAT_single (int n)
{
  union
  {
    double d;
    int l[2];
  } tmp;

  PC += 2;
  clear_cause ();

  FR[n] = FPUL; // convert from integer to float
  tmp.d = FPUL;
  if (tmp.l[1] & 0x1FFFFFFF)
    inexact();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ftrc	FRm,FPUL",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(long)FRm → FPUL" },
  code { "1111mmmm00111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 32 (FRm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fpul, fps ← FTRC_SL(op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
FPUL ← ZeroExtend 32(fpul);
)"},
  description
{R"(
Converts the single-precision floating-point number in FRm to a 32-bit integer,
and stores the result in FPUL.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="ftrc.svg" height="128" />
)"},

  note
{R"(
The rounding mode is always truncation.

The original SH4 has a pipeline exception.  If the FTRC instruction is followed
by an STS FPUL, Rn instruction, the latency of the FTRC instruction is reduced
to 1 cycle.
)"},

  operation
{R"(
#define NEG_INT_SINGLE_RANGE 0xCF000000 & 0x7FFFFFFF // -1.000000 * 2^31
#define POS_INT_SINGLE_RANGE 0x4EFFFFFF              // 1.FFFFFE * 2^30

void FTRC_single (int m)
{
  PC += 2;
  clear_cause ();

  switch (ftrc_single_type_of (m))
  {
    case NORM:
      FPUL = FR[m];  // Convert float to integer
      break;
    case PINF:
      ftrc_invalid (0, &FPUL);
      break;
    case NINF:
      ftrc_invalid (1, &FPUL);
      break;
  }
}

int ftrc_single_type_of (int m)
{
  if (sign_of (m) == 0)
  {
    if (FR_HEX[m] > 0x7F800000)
      return NINF;  // NaN
    else if (FR_HEX[m] > POS_INT_SINGLE_RANGE)
      return PINF;  // out of range, +INF
    else
      return NORM;  // +0, +NORM
  }
  else
  {
    if ((FR_HEX[m] & 0x7FFFFFFF) > NEG_INT_SINGLE_RANGE)
      return NINF;  // out of range, +INF, NaN
    else
      return NORM;  // -0, -NORM
  }
}

void ftrc_invalid (int sign, int* result)
{
  set_V ();

  if ((FPSCR & ENABLE_V) == 0)
  {
    if (sign == 0)
      *result = 0x7FFFFFFF;
    else
      *result = 0x80000000;
  }
  else
    fpu_exception_trap ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fipr	FVm,FVn",
  SH4 | SH4A,
  abstract { "inner_product (FVm, FVn) → FR[n+3]" },
  code { "1111nnmm11101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "4/5" },



  description
{R"(
Calculates the inner products of the 4-dimensional single-precision
floating-point vector indicated by FVn and FVm, and stores the results in
FR[n + 3].
<br /><br />
The FIPR instruction is intended for speed rather than accuracy, and therefore
the results will differ from those obtained by using a combination of FADD and
FMUL instructions. The FIPR execution sequence is as follows:
<ol type="1">
 Multiplies all terms.  The results are 28 bits long.
 Aligns these results, rounding them to fit within 30 bits.
 Adds the aligned values.
 Performs normalization and rounding.
</ol>

Special processing is performed in the following cases:
<ol type="1">
If an input value is an sNaN, an invalid exception is generated.

If the input values to be multiplied include a combination of 0 and
infinity, an invalid exception is generated.

In cases other than the above, if the input values include a qNaN, the
result will be a qNaN.

In cases other than the above, if the input values include infinity:
  <ol type="a">
  If multiplication results in two or more infinities and the signs are
  different, an invalid exception will be generated.
  Otherwise, correct infinities will be stored.
  </ol>

If the input values do not include an sNaN, qNaN, or infinity, processing
is performed in the normal way.
</ol>
When FPSCR.enable.U/I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag, and FR[n+3] is not updated. Appropriate
processing should therefore be performed by software.
)"},

  note
{R"(
FV0 = { FR0, FR1, FR2, FR3 }<br />
FV4 = { FR4, FR5, FR6, FR7 }<br />
FV8 = { FR8, FR9, FR10, FR11 }<br />
FV12 = { FR12, FR13, FR14, FR15 }<br />
)"},

  operation
{R"(
void FIPR (int m, int n)
{
  if (FPSCR_PR == 0)
  {
    PC += 2;
    clear_cause ();
    fipr (m,n);
  }
  else
    undefined_operation ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
Overflow
<br />
Generation of overflow exception traps
<br />
At least one of the following results is not less than 0xFC
<br />
(exponent of FRn) + (exponent of FRm)<br />
(exponent of FR(n + 1)) + (exponent of FR(m + 1))<br />
(exponent of FR(n + 2)) + (exponent of FR(m + 2))<br />
(exponent of FR(n + 3)) + (exponent of FR(m + 3))<br />

Underflow
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ftrv	XMTRX,FVn",
  SH4 | SH4A,
  abstract { "transform_vector (XMTRX, FVn) → FVn" },
  code { "1111nn0111111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "4", SH4, "5/8" },



  description
{R"(
Takes the contents of floating-point registers XF0 to XF15 indicated by XMTRX
as a 4-row &times 4-column matrix, takes the contents of floating-point
registers FR[n] to FR[n + 3] indicated by FVn as a 4-dimensional vector,
multiplies the array by the vector, and stores the results in FV[n].

<br /><img src="ftrv.svg" height="128" /><br />

The FTRV instruction is intended for speed rather than accuracy, and therefore
the results will differ from those obtained by using a combination of FADD and
FMUL instructions. The FTRV execution sequence is as follows:
<ol type="1">
Multiplies all terms. The results are 28 bits long.
Aligns these results, rounding them to fit within 30 bits.
Adds the aligned values.
Performs normalization and rounding.
</ol>

Special processing is performed in the following cases:
<ol type="1">
If an input value is an sNaN, an invalid exception is generated.

If the input values to be multiplied include a combination of 0 and
infinity, an invalid operation exception is generated.

In cases other than the above, if the input values include a qNaN, the
result will be a qNaN.

In cases other than the above, if the input values include infinity:
  <ol type="a">
  If multiplication results in two or more infinities and the signs are
  different, an invalid exception will be generated.

  Otherwise, correct infinities will be stored.
  </ol>


If the input values do not include an sNaN, qNaN, or infinity, processing
is performed in the normal way.

</ol>

When FPSCR.enable.V/O/U/I is set, an FPU exception trap is generated regardless
of whether or not an exception has occurred. When an exception occurs, correct
exception information is reflected in FPSCR.cause and FPSCR.flag, and FVn is not
updated. Appropriate processing should therefore be performed by software.
)"},

  note
{R"(
A 4-dimensional matrix &times matrix transformation can be realized by four FTRV
instructions, where every FTRV calculates a column of the result matrix.  The
resulting matrix can be set to the XMTRX registers by toggling the FPSCR.FR bit
to switch register banks without copying them.
)"},

  operation
{R"(
void FTRV (int n)
{
  if (FPSCR_PR != 0)
    undefined_operation ();
  else
  {
    float saved_vec[4];
    float result_vec[4];
    int saved_fpscr;
    int dst;

    PC += 2;
    clear_cause ();

    saved_fpscr = FPSCR;
    FPSCR &= ~ENABLE_VOUI;  // mask VOUI enable
    dst = 12 - n;           // select other vector than FVn

    for (int i = 0; i < 4; i++)
      saved_vec[i] = FR[dst+i];

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
        FR[dst+j] = XF[i+4j];

      fipr (n, dst);
      saved_fpscr |= FPSCR & (CAUSE | FLAG);
      result_vec[i] = FR[dst+3];
    }

    for (int i = 0; i < 4; i++)
      FR[dst+i] = saved_vec[i];

    FPSCR = saved_fpscr;

    if (FPSCR & ENABLE_VOUI)
      fpu_exception_trap();
    else
      for (int i = 0; i < 4; i++)
        FR[n+i] = result_vec[i];
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
Overflow
Underflow
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsrra	FRn",
  SH4A,
  abstract { "1.0 / sqrt (FRn) → FRn" },
  code { "1111nnnn01111101" },

  group { SH4A, "FE" },
  issue { SH4A, "1" },
  latency { SH4A, "1" },



  description
{R"(
Takes the approximate inverse of the arithmetic square root (absolute error is
within &plusmn;2<sup>-21</sup>) of the single-precision floating-point in FRn
and writes the result to FRn. Since the this instruction operates by
approximation, an imprecision exception is required when the input is a
normalized value. In other cases, the instruction does not require an
imprecision exception.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated. When an
exception occurs, correct exception information is reflected in FPSCR.cause and
FPSCR.flag, and FRn is not updated. Appropriate processing should therefore be
performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fsrra.svg" height="128" />
)"},

  note
{R"(
This instruction is also supported by the SH4 variant SH7091.  Other SH4
variants such as SH7751, SH7760, SH7761 might also support it.
)"},

  operation
{R"(
void FSRRA (int n)
{
  if (FPSCR_PR != 0)
    undefined_operation ();
  else
  {
    PC += 2;
    clear_cause();

    switch (data_type_of (n))
    {
    case NORM:
      if (sign_of (n) == 0)
      {
        set_I ();
        FR[n] = 1 / sqrt (FR[n]);
      }
      else
        invalid (n);
      break;

    case DENORM:
      if (sign_of (n) == 0)
        fpu_error ();
      else
        invalid (n);
      break;

    case PZERO:
    case NZERO:
      dz (n, sign_of (n));
      break;

    case PINF:
      FR[n] = 0;
      break;

    case NINF:
      invalid (n);
      break;

    case qNAN:
      qnan (n);
      break;

    case sNAN:
      invalid (n);
      break;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU error
Invalid operation
Division by zero
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsca	FPUL,DRn",
  SH4A,
  abstract { "sin (FPUL) → FRn\ncos (FPUL) → FR[n+1]" },
  code { "1111nnn011111101" },

  group { SH4A, "FE" },
  issue { SH4A, "1" },
  latency { SH4A, "3" },



  description
{R"(
Calculates the sine and cosine approximations of FPUL (absolute error is
within &plusmn;2<sup>-21</sup>) as single-precision floating point values, and
places the values of the sine and cosine in FRn and FR[n + 1], respectively.
Since this instruction is an approximate operation instruction, an imprecision
exception is always required (even if the input is a 0, the result is
imprecise).
<br /><br />
The input angle is specified as a signed fraction in twos complement.  The
result of sin and cos is a single-precision floating-point number.
<br />
0x7FFFFFFF to 0x00000001:
360&times;2<sup>15</sup>&minus;360/2<sup>16</sup> to 360/2<sup>16</sup> degrees
<br />
0x00000000: 0 degree
<br />
0xFFFFFFFF to 0x80000000:
&minus;360/2<sup>16</sup> to &minus;360&times2<sup>15</sup> degrees
)"},

  note
{R"(
This instruction is also supported by the SH4 variant SH7091.  Other SH4
variants such as SH7751, SH7760, SH7761 might also support it.
)"},

  operation
{R"(
void FSCA (int n)
{
  if (FPSCR_PR != 0)
    undefined_operation ();
  else
  {
    float angle;
    long offset = 0x00010000;
    long fraction = 0x0000FFFF;

    set_I ();
    fraction &= FPUL;  // extract sub-rotation (fraction) part
    angle = fraction;  // convert to float
    angle = 2 * M_PI * angle / offset;  // convert to radian
    FR[n] = sin (angle);
    FR[n+1] = cos (angle);
    PC += 2;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Floating-Point Double-Precision Instructions (FPSCR.PR = 1)",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fabs	DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn & 0x7FFFFFFFFFFFFFFF → DRn" },
  code { "1111nnn001011101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← FABS_D(op1);
DR 2n ← FloatRegister 64 (op1);
)"},

  description
{R"(
Clears the most significant bit of the contents of floating-point register DRn
to 0, and stores the result in DRn.
)"},

  note
{R"(
The cause and flag fields in FPSCR are not updated.
<br /><br />
A double-precision floating-point register DRn consists of a single-precision
floating-point register pair FRn:FRn+1, where FRn is the high part and FRn+1 is
the low part.  This instruction operates only on the high part and thus the
operation performed for double and single precision setting is the same.  It is
not necessary to adjust the FPSRC.PR setting before this instruction.
)"},

  operation
{R"(
void FABS (int n)
{
  FR[n] = FR[n] & 0x7FFFFFFFF;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fneg	DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn ^ 0x8000000000000000 → DRn" },
  code { "1111nnn001001101" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0", SH4, "0" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1 ← FNEG_D(op1);
DR 2n ← FloatRegister 64 (op1);
)"},
  description
{R"(
Inverts the most significant bit (sign bit) of the contents of floating-point
register DRn, and stores the result in DRn.
)"},

  note
{R"(
The cause and flag fields in FPSCR are not updated.
<br /><br />
A double-precision floating-point register DRn consists of a single-precision
floating-point register pair FRn:FRn+1, where FRn is the high part and FRn+1 is
the low part.  This instruction operates only on the high part and thus the
operation performed for double and single precision setting is the same.  It is
not necessary to adjust the FPSRC.PR setting before this instruction.
)"},

  operation
{R"(
void FNEG (int n)
{
  FR[n] = -FR[n];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fadd	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn + DRm → DRn" },
  code { "1111nnn0mmm00000" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/8", SH4, "7/9" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FADD_D(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op2);
FPSCR ← ZeroExtend 32 (fps);
)"},

  description
{R"(
Arithmetically adds the two double-precision floating-point numbers in DRn and
DRm, and stores the result in DRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and DRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fadd.svg" height="300" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FADD (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case NORM:
        normal_faddsub (m, n, ADD);
        break;
      case PZERO:
      case NZERO:
        register_copy (m, n);
        break;
      default:
        break;
      }
      break;

    case PZERO:
      switch (data_type_of (n))
      {
      case NZERO:
        zero (n, 0);
        break;
      default:
        break;
      }
      break;

    case NZERO:
      break;

    case PINF:
      switch (data_type_of (n))
      {
      case NINF:
        invalid (n);
        break;
      default:
        inf (n, 0);
        break;
      }
      break;

     case NINF:
       switch (data_type_of (n))
       {
       case PINF:
         invalid (n);
         break;
       default:
         inf (n, 1);
         break;
       }
       break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
DRn and DRm have the same sign and the exponent of at least one value is 0x7FE


Underflow
<br />
Generation of underflow-exception traps
<br />
DRn and DRm have different signs and neither has an exponent greater than 0x035


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsub	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn - DRm → DRn" },
  code { "1111nnn0mmm00001" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/8", SH4, "7/9" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FSUB_D(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op2);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Arithmetically subtracts the double-precision floating-point number in DRm from
the double-precision floating-point number in DRn, and stores the result in DRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction.  When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and DRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fsub.svg" height="300" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FSUB (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case NORM:
        normal_faddsub (m, n, SUB);
        break;
      case PZERO:
      case NZERO:
        register_copy (m, n);
        FR[n] = -FR[n];
        break;
      default:
        break;
      }
      break;

    case PZERO:
      break;
      case NZERO:
        switch (data_type_of (n))
        {
        case NZERO:
          zero (n, 0);
          break;
        default:
          break;
        }
        break;

    case PINF:
      switch (data_type_of (n))
      {
      case PINF:
        invalid (n);
        break;
      default:
        inf (n, 1);
        break;
      }
      break;

    case NINF:
      switch (data_type_of (n))
      {
      case NINF:
        invalid (n);
        break;
      default:
        inf (n, 0);
        break;
      }
      break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
DRn and DRm have the same sign and the exponent of at least one value is 0x7FE


Underflow
<br />
Generation of underflow-exception traps
<br />
DRn and DRm have different signs and neither has an exponent greater than 0x035


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fmul	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn × DRm → DRn" },
  code { "1111nnn0mmm00010" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "3", SH2A, "0/8", SH4, "7/9" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FMUL_D(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op2);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Arithmetically multiplies the two double-precision floating-point numbers in
DRn and DRm, and stores the result in FRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and DRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fmuld.svg" height="300" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FMUL (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else if (data_type_of (m) == DENORM || data_type_of (n) == DENORM)
    set_E ();
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      case PINF:
      case NINF:
        inf (n, sign_of (m) ^ sign_of (n));
        break;
      default:
        normal_fmul (m, n);
        break;
      }
      break;

    case PZERO:
    case NZERO:
      switch (data_type_of (n))
      {
      case PINF:
      case NINF:
        invalid (n);
        break;
      default:
        zero (n,sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case PINF:
    case NINF:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      default:
        inf (n, sign_of (m) ^ sign_of (n));
        break
      }
      break;
    }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation

Overflow
<br />
Generation of overflow-exception traps
<br />
(exponent of DRn) + (exponent of DRm) - 0x3FF is not less than 0x7FE


Underflow
<br />
Generation of underflow-exception traps
<br />
(exponent of DRn) + (exponent of DRm) - 0x3FF is not more than 0x000


Inexact

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fdiv	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "DRn / DRm → DRn" },
  code { "1111nnn0mmm00011" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "14", SH2A, "0/24", SH4, "24/26" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op2, fps ← FDIV_D(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuEnableZ(fps) AND FpuCauseZ(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op2);
FPSCR ← ZeroExtend 32 (fps);
)"},


  description
{R"(
Arithmetically divides the double-precision floating-point number in DRn by the
double-precision floating-point number in DRm, and stores the result in DRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag and DRn is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fdiv.svg" height="300" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FDIV (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    invalid (n);
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    qnan (n);
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PINF:
      case NINF:
        inf (n, sign_of (m) ^ sign_of (n));
        break;
      case PZERO:
      case NZERO:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      case DENORM:
        set_E ();
        break;
      default:
        normal_fdiv_double (m, n);
        break;
      }
      break;

    case PZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      case PINF:
      case NINF:
        break;
      default:
        dz (n, sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case NZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        invalid (n);
        break;
      case PINF:
        inf (n, 1);
        break;
      case NINF:
        inf (n, 0);
        break;
      default:
        dz (FR[n], sign_of (m) ^ sign_of (n));
        break;
      }
      break;

    case DENORM:
      set_E ();
      break;

    case PINF:
    case NINF:
      switch (data_type_of (n))
      {
      case DENORM:
        set_E ();
        break;
      case PINF:
      case NINF:
        invalid (n);
        break;
      default:
        zero (n, sign_of (m) ^ sign_of (n));
        break;
      }
      break;
    }
}

void normal_fdiv_double (int m, int n)
{
  union
  {
    double d;
    int l[2];
  } dstd, tmpd;

  union
  {
    int double x;
    int l[4];
  } tmpx;

  tmpd.d = DR[n >> 1];   // save destination value
  dstd.d /= DR[m >> 1];  // round toward nearest or even
  tmpx.x = dstd.d;       // convert double to int double
  tmpx.x *= DR[m >> 1];

  if (tmpd.d != tmpx.x)
    set_I ();
  if (tmpd.d < tmpx.x && FPSCR_RM == 1)
  {
    dstd.l[1] -= 1;  // round toward zero
    if (dstd.l[1] == 0xFFFFFFFF)
      dstd.l[0] -= 1;
  }

  check_double_exception (&DR[n >> 1], dstd.d);
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation
Division by zero
Overflow
<br />
Generation of overflow-exception traps
<br />
(exponent of DRn) - (exponent of DRm) + 0x3FF is not less than 0x7FF


Underflow
<br />
Generation of underflow-exception traps
<br />
(exponent of DRn) - (exponent of DRm) + 0x3FF is not more than 0x001


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fsqrt	DRn",
  SH4 | SH4A | SH2A,
  abstract { "sqrt (DRn) → DRn" },
  code { "1111nnn001101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "30", SH2A, "0/24", SH4, "23/25" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1, fps ← FSQRT_D(op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF (FpuEnableI(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op1);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Finds the arithmetical square root of the double-precision floating-point number
in DRn, and stores the result in DRn.
<br /><br />
When FPSCR.enable.I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When an exception occurs, correct
exception information is reflected in FPSCR.cause and FPSCR.flag and DRn is not
updated. Appropriate processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fsqrt.svg" height="128" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FSQRT (int n)
{
  PC += 2;
  clear_cause ();

  switch (data_type_of (n))
  {
  case NORM:
    if (sign_of (n) == 0)
      normal_fsqrt_double (n);
    else
      invalid (n);
    break;

  case DENORM:
    if (sign_of (n) == 0)
      set_E ();
    else
      invalid (n);
    break;

  case PZERO:
  case NZERO:
  case PINF:
    break;

  case NINF:
    invalid (n);
    break;

  case qNAN:
    qnan (n);
    break;

  case sNAN:
    invalid (n);
    break;
  }
}

void normal_fsqrt_double (int n)
{
  union
  {
    double d;
    int l[2];
  } dstd, tmpd;

  union
  {
    int double x;
    int l[4];
  } tmpx;

  tmpd.d = DR[n >> 1];         // save destination value
  dstd.d = sqrt (DR[n >> 1]);  // round toward nearest or even
  tmpx.x = dstd.d;             // convert double to int double
  tmpx.x *= dstd.d;

  if (tmpd.d != tmpx.x)
    set_I ();
  if (tmpd.d < tmpx.x && FPSCR_RM == 1)
  {
    dstd.l[1] -= 1;  // round toward zero
    if (dstd.l[1] == 0xFFFFFFFF)
      dstd.l[0] -= 1;
  }
  if (FPSCR & ENABLE_I)
    fpu_exception_trap();
  else
    DR[n >> 1] = dstd.d;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU Error
Invalid Operation
Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcmp/eq	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "If DRn = DRm: 1 → T\nElse: 0 → T" },
  code { "1111nnn0mmm00100" },
  flags { "Result → T" },

  group { SH4A, "FE", SH4, "CO" },
  issue { SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH4A, "1", SH2A, "3", SH4, "3/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
t, fps ← FCMPEQ_D(op1, op2, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
T ← Bit(t);
)"},

  description
{R"(
Arithmetically compares the two double-precision floating-point numbers in DRn
and DRm, and stores 1 in the T bit if they are equal, or 0 otherwise.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcmpeq.svg" height="300" />
)"},

  note
{R"(
)"},

  operation
{R"(
void FCMP_EQ (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (fcmp_chk_double (m, n) == INVALID)
    fcmp_invalid ();
  else if (fcmp_chk_double (m, n) == EQ)
    T = 1;
  else
    T = 0;
}

int fcmp_chk_double (int m, int n)
{
  if (data_type_of (m) == sNaN || data_type_of (n) == sNaN)
    return INVALID;
  else if (data_type_of (m) == qNaN || data_type_of (n) == qNaN)
    return UO;
  else
    switch (data_type_of (m))
    {
    case NORM:
      switch (data_type_of (n))
      {
      case PINF:
        return GT;
      case NINF:
        return LT;
      default:
        break;
      }
      break;

    case PZERO:
    case NZERO:
      switch (data_type_of (n))
      {
      case PZERO:
      case NZERO:
        return EQ;
      default:
        break;
      }
      break;

    case PINF:
      switch (data_type_of (n))
      {
        case PINF:
          return EQ;
        default:
          return LT;
      }

    case NINF:
      switch (data_type_of (n))
      {
      case NINF:
        return EQ;
      default:
        return GT;
      }
    }

  if (DR[n >> 1] == DR[m >> 1])
    return EQ;
  else if (DR[n >> 1] > DR[m >> 1])
    return GT;
  else
    return LT;
}

void fcmp_invalid (void)
{
  set_V ();

  if ((FPSCR & ENABLE_V) == 0)
    T = 0;
  else
    fpu_exception_trap ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcmp/gt	DRm,DRn",
  SH4 | SH4A | SH2A,
  abstract { "If DRn > DRm: 1 → T\nElse: 0 → T" },
  code { "1111nnn0mmm00101" },
  flags { "Result → T" },

  group { SH4A, "FE", SH4, "CO" },
  issue { SH4A, "1", SH2A, "2", SH4, "2" },
  latency { SH4A, "1", SH2A, "3", SH4, "3/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
op2 ← FloatValue 64 (DR2n );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
t, fps ← FCMPGT_D(op2, op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
T ← Bit(t);
)"},


  description
{R"(
Arithmetically compares the two double-precision floating-point numbers in DRn
and DRm, and stores 1 in the T bit if DRn > DRm, or 0 otherwise.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcmpgt.svg" height="300" />
)"},

  note
{R"(
For IEEE 754 conform less-than-or-equal comparison it is not sufficient to swap
the operands.  The FCMP/EQ must be used as well.
)"},

  operation
{R"(
void FCMP_GT (int m, int n)
{
  PC += 2;
  clear_cause ();

  if (fcmp_chk_double (m, n) == INVALID || fcmp_chk_double (m, n) == UO)
    fcmp_invalid ();
  else if (fcmp_chk_double (m, n) == GT)
    T = 1;
  else
    T = 0;
}

int fcmp_chk_double (int m, int n)
{
  // see description of FCMP/EQ instruction.
}

void fcmp_invalid (void)
{
  // see description of FCMP/EQ instruction.
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "float	FPUL,DRn",
  SH4 | SH4A | SH2A,
  abstract { "(double)FPUL → DRn" },
  code { "1111nnn000101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/4", SH4, "3/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
fpul ← SignExtend 32(FPUL);
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1, fps ← FLOAT_LD(fpul, fps);
DR 2n ← FloatRegister 64 (op1);
)"},

  description
{R"(
Taking the contents of FPUL as a 32-bit integer, converts this integer to a
double-precision floating-point number and stores the result in DRn.
)"},

  note
{R"(

)"},

  operation
{R"(
void FLOAT_double (int n)
{
  union
  {
    double d;
    int l[2];
  } tmp;

  PC += 2;
  clear_cause ();

  DR[n >> 1] = FPUL; // convert from integer to double
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "ftrc	DRm,FPUL",
  SH4 | SH4A | SH2A,
  abstract { "(long)DRm → FPUL" },
  code { "1111mmm000111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "0/4", SH4, "4/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fpul, fps ← FTRC_DL(op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
FPUL ← ZeroExtend 32(fpul);
FPSCR ← ZeroExtend 32 (fps);
)"},
  description
{R"(
Converts the double-precision floating-point number in DRm to a 32-bit integer,
and stores the result in FPUL.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="ftrc.svg" height="128" />
)"},

  note
{R"(
The rounding mode is always truncation.
)"},

  operation
{R"(
#define NEG_INT_DOUBLE_RANGE 0xC1E0000000200000 & 0x7FFFFFFFFFFFFFFF
#define POS_INT_DOUBLE_RANGE 0x41E0000000000000

void FTRC_double (int m)
{
  PC += 2;
  clear_cause ();

  switch (ftrc_double_type_of (m))
  {
    case NORM:
      FPUL = DR[m >> 1];  // Convert double to integer
      break;
    case PINF:
      ftrc_invalid (0, &FPUL);
      break;
    case NINF:
      ftrc_invalid (1, &FPUL);
      break;
  }
}

int ftrc_double_type_of (int m)
{
  if (sign_of (m) == 0)
  {
    if (FR_HEX[m] > 0x7FF00000
        || (FR_HEX[m] == 0x7FF00000 && FR_HEX[m+1] != 0x00000000))
      return NINF;  // NaN
    else if (DR_HEX[m >> 1] >= POS_INT_DOUBLE_RANGE)
      return PINF;  // out of range, +INF
    else
      return NORM;  // +0, +NORM
  }
  else
  {
    if ((DR_HEX[m >> 1] & 0x7FFFFFFFFFFFFFFF) >= NEG_INT_DOUBLE_RANGE)
      return NINF;  // out of range, +INF, NaN
    else
      return NORM;  // -0, -NORM
  }
}

void ftrc_invalid (int sign, int* result)
{
  set_V ();

  if ((FPSCR & ENABLE_V) == 0)
  {
    if (sign == 0)
      *result = 0x7FFFFFFF;
    else
      *result = 0x80000000;
  }
  else
    fpu_exception_trap ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Invalid operation
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcnvds	DRm,FPUL",
  SH4 | SH4A | SH2A,
  abstract { "double_to_float (DRm) → FPUL" },
  code { "1111mmm010111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "4", SH4, "4/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
op1 ← FloatValue 64 (DR2m);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fpul, fps ← FCNV_DS(op1, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
IF ((FpuEnableI(fps) OR FpuEnableO(fps)) OR FpuEnableU(fps))
THROW FPUEXC, fps;
FPSCR ← ZeroExtend 32 (fps);
FPUL ← ZeroExtend 32(fpul);
)"},


  description
{R"(
Converts the double-precision floating-point number in DRm to a single-precision
floating-point number, and stores the result in FPUL.
<br /><br />
When FPSCR.enable. I is set, an FPU exception trap is generated regardless of
whether or not an exception has occurred. When FPSCR.enable.O/U is set, FPU
exception traps are generated on actual generation by the FPU exception source
and on the satisfaction of certain special conditions that apply to this the
instruction. When an exception occurs, correct exception information is
reflected in FPSCR.cause and FPSCR.flag, and FPUL is not updated. Appropriate
processing should therefore be performed by software.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcnvds.svg" height="128" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FCNVDS (int m)
{
  if (FPSCR_PR != 1)
    undefined_operation ();
  else
  {
    PC += 2;
    clear_cause ();

    switch (data_type_of (m))
    {
    case NORM:
    case PZERO:
    case NZERO:
      normal_fcnvds (m, &FPUL);
      break;

    case DENORM:
      set_E ();

    case PINF:
      FPUL = 0x7F800000;
      break;

    case NINF:
      FPUL = 0xFF800000;
      break;

    case qNaN:
      FPUL = 0x7FBFFFFF;
      break;

    case sNaN:
      set_V ();
      if ((FPSCR & ENABLE_V) == 0)
        FPUL = 0x7FBFFFFF;
      else
        fpu_exception_trap ();
      break;
    }
  }
}

void normal_fcnvds (int m, float* result)
{
  int sign;
  float abs;

  union
  {
    float f;
    int l;
  } dstf, tmpf;

  union
  {
    double d;
    int l[2];
  } dstd;

  dstd.d = DR [m >> 1];

  if (dstd.l[1] & 0x1FFFFFFF))
    set_I ();

  if (FPSCR_RM == 1)
    dstd.l[1] &= 0xE0000000;  // round toward zero

  dstf.f = dstd.d;
  check_single_exception (result, dstf.f);
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU error
Invalid operation
Overflow
<br />
Generation of overflow exception traps
<br />
The exponent of DRn is not less than 0x47E


Underflow
<br />
Generation of underflow exception traps
<br />
The exponent of DRn is not more than 0x380


Inexact
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fcnvsd	FPUL,DRn",
  SH4 | SH4A | SH2A,
  abstract { "float_to_double (FPUL) → DRn" },
  code { "1111nnn010101101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "4", SH4, "3/5" },

  restriction { "Available only when PR=1 and SZ=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fps ← ZeroExtend 32 (FPSCR);
fpul ← SignExtend 32(FPUL);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
op1, fps ← FCNV_SD(fpul, fps);
IF (FpuEnableV(fps) AND FpuCauseV(fps))
THROW FPUEXC, fps;
IF (FpuCauseE(fps))
THROW FPUEXC, fps;
DR 2n ← FloatRegister 64 (op1);
FPSCR ← ZeroExtend 32 (fps);
)"},


  description
{R"(
Converts the single-precision floating-point number in FPUL to a
double-precision floating-point number, and stores the result in DRn.
<br /><br /><b><i>Operation result special cases</i></b>
<br /><img src="fcnvsd.svg" height="128" />
)"},

  note
{R"(

)"},

  operation
{R"(
void FCNVSD (int n)
{
  if (FPSCR_PR != 1)
    undefined_operation ();
  else
  {
    switch (fpul_type ())
    {
    case PZERO:
    case NZERO:
    case PINF:
    case NINF:
    case NORM:
      DR[n >> 1] = FPUL;  // convert float to double
      break;

    case DENORM:
      set_E ();
      break;

    case qNaN:
      qnan (n);
      break;

    case sNaN:
      invalid (n);
      break;
    }
  }
}

int fpul_type ()
{
  int abs = FPUL & 0x7FFFFFFF;
  if (abs < 0x00800000)
  {
    if (FPSCR_DN == 1 || abs == 0x00000000)
    {
      if (sign_of (FPUL) == 0)
        return PZERO;
      else
        return NZERO;
    }
    else
      return DENORM;
  }
  else if (abs < 0x7F800000)
    return NORM;
  else if (abs == 0x7F800000)
  {
    if (sign_of (FPUL) == 0)
      return PINF;
    else
      return NINF;
  }
  else if (abs < 0x7FC00000)
    return qNaN;
  else
    return sNaN;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
FPU error
Invalid operation
)"},
},


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "Floating-Point Control Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,FPSCR",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "Rm → FPSCR" },
  code { "0100mmmm01101010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "4" },

  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fps, pr, sz, fr ← UnpackFPSCR(op1);
FPSCR ← ZeroExtend 32 (fps);
SR.PR ← Bit(pr);
SR.SZ ← Bit(sz);
SR.FR ← Bit(fr);
)"},
  description
{R"(
Loads the source operand into FPU system register FPSCR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSFPSCR (int m)
{
  #if SH2E || SH3E
  FPSCR = R[m] & 0x00018C60;

  #elif SH4 || SH4A || SH2A
  FPSCR = R[m] & 0x003FFFFF;

  #endif

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	FPSCR,Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FPSCR → Rn" },
  code { "0000nnnn01101010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "3" },



  description
{R"(
Stores FPU system register FPSCR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSFPSCR (int n)
{
  #if SH2E || SH3E
  R[n] = FPSCR;

  #elif SH4 || SH4A || SH2A
  R[n] = FPSCR & 0x003FFFFF;

  #endif

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,FPSCR",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(Rm) → FPSCR, Rm+4 → Rm" },
  code { "0100mmmm01100110" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "3", SH4, "3" },

  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
value ← ReadMemory 32(address);
fps, pr, sz, fr ← UnpackFPSCR(value);
op1 ← op1 + 4;
R m ← Register(op1);
FPSCR ← ZeroExtend 32 (fps);
SR.PR ← Bit(pr);
SR.SZ ← Bit(sz);
SR.FR ← Bit(fr);
)"},
  description
{R"(
Loads the source operand into FPU system register FPSCR.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMFPSCR (int m)
{
  #if SH2E || SH3E
  FPSCR = Read_32 (R[m]) & 0x00018C60;

  #elif SH4 || SH4A || SH2A
  FPSCR = Read_32 (R[m]) & 0x003FFFFF;

  #endif

  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	FPSCR,@-Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "Rn-4 → Rn, FPSCR → (Rn)" },
  code { "0100nnnn01100010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1/1" },



  description
{R"(
Stores FPU system register FPSCR in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMFPSCR (int n)
{
  R[n] -= 4;

  #if SH2E || SH3E
  Write_32 (R[n], FPSCR);

  #elif SH4 || SH4A || SH2A
  Write_32 (R[n], FPSCR & 0x003FFFFF);

  #endif

  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds	Rm,FPUL",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "Rm → FPUL" },
  code { "0100mmmm01011010" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },

  name { "Load to FPU System register" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fpul ← op1;
FPUL ← ZeroExtend 32(fpul);
)"},
  description
{R"(
Loads the source operand into FPU system register FPUL.
)"},

  note
{R"(

)"},

  operation
{R"(
void LDSFPUL (int m)
{
  FPUL = R[m];
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts	FPUL,Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "FPUL → Rn" },
  code { "0000nnnn01011010" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "3" },

  name { "Store from FPU System Register" },
  description
{R"(
Stores FPU system register FPUL in the destination.

)"},

  note
{R"(
The original SH4 has a pipeline exception.  If the FTRC instruction is followed
by an STS FPUL, Rn instruction, the latency of the FTRC instruction is reduced
to 1 cycle.
)"},

  operation
{R"(
void STSFPUL (int n)
{
  R[n] = FPUL;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "lds.l	@Rm+,FPUL",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "(Rm) → FPUL, Rm+4 → Rm" },
  code { "0100mmmm01010110" },

  group { SH4A, "LS", SH4, "LS" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "1/2" },

  name { "Load to FPU System register" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
op1 ← SignExtend 32 (Rm );
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
address ← ZeroExtend 32(op1);
fpul ← ReadMemory 32 (address);
op1 ← op1 + 4;
R m ← Register(op1);
FPUL ← ZeroExtend 32(fpul);
)"},
  description
{R"(

)"},

  note
{R"(

)"},

  operation
{R"(
void LDSMFPUL (int m)
{
  FPUL = Read_32 (R[m]);
  R[m] += 4;
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "sts.l	FPUL,@-Rn",
  SH2E | SH3E | SH4 | SH4A | SH2A,
  abstract { "Rn-4 → Rn, FPUL → (Rn)" },
  code { "0100nnnn01010010" },

  group { SH4A, "LS", SH4, "CO" },
  issue { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH2E, "1", SH3E, "1", SH4A, "1", SH2A, "2", SH4, "1/1" },

  name { "Store from FPU System Register" },
  description
{R"(
Stores FPU system register FPUL in the destination.
)"},

  note
{R"(

)"},

  operation
{R"(
void STSMFPUL (int n)
{
  R[n] -= 4;
  Write_32 (R[n], FPUL);
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(
Data TLB multiple-hit exception
Data TLB miss exception
Data TLB protection violation exception
Data address error
Initial page write exception
)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "frchg",
  SH4 | SH4A,
  abstract { "If FPSCR.PR = 0: ~FPSCR.FR → FPSCR.FR\nElse: Undefined Operation" },
  code { "1111101111111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH4, "1" },
  latency { SH4A, "1", SH4, "1/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
fr ← ZeroExtend 1 (SR.FR);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
fr ← fr ⊕ 1;
SR.FR ← Bit(fr);
)"},
  description
{R"(
Inverts the FR bit in floating-point register FPSCR. When the FR bit in FPSCR is
changed, FR0 to FR15 in FPR0_BANK0 to FPR15_BANK0 and FPR0_BANK1 to FPR15_BANK1
become XR0 to XR15, and XR0 to XR15 become FR0 to FR15. When FPSCR.FR = 0,
FPR0_BANK0 to FPR15_BANK0 correspond to FR0 to FR15, and FPR0_BANK1 to
FPR15_BANK1 correspond to XR0 to XR15. When FPSCR.FR = 1, FPR0_BANK1 to
FPR15_BANK1 correspond to FR0 to FR15, and FPR0_BANK0 to FPR15_BANK0 correspond
to XR0 to XR15.
)"},

  note
{R"(

)"},

  operation
{R"(
void FRCHG (void)
{
  if (FPSCR_PR == 0)
  {
    FPSCR ^= 0x00200000;  // toggle bit 21
    PC += 2;
  }
  else
    undefined_operation ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fschg",
  SH2A | SH4 | SH4A,
  abstract { "If FPSCR.PR = 0: ~FPSCR.SZ → FPSCR.SZ\nElse: Undefined Operation" },
  code { "1111001111111101" },

  group { SH4A, "FE", SH4, "FE" },
  issue { SH4A, "1", SH2A, "1", SH4, "1" },
  latency { SH4A, "1", SH2A, "1", SH4, "1/4" },

  restriction { "Available only when PR=0" },
  brief
{R"(
sr ← ZeroExtend 32 (SR);
sz ← ZeroExtend1 (SR.SZ);
IF (FpuIsDisabled(sr) AND IsDelaySlot())
THROW SLOTFPUDIS;
IF (FpuIsDisabled(sr))
THROW FPUDIS;
sz ← sz ⊕ 1;
SR.SZ ← Bit(sz);
)"},
  description
{R"(
Inverts the SZ bit of the floating-point status register FPSCR. Changing the
value of the SZ bit in FPSCR switches the amount of data for transfer by the
FMOV instruction between one single-precision data and a pair of
single-precision data. When FPSCR.SZ = 0, an FMOV instruction transfers a
single-precision number. When FPSCR.SZ = 1, the FMOV instruction transfers a
pair of single-precision numbers.
)"},

  note
{R"(

)"},

  operation
{R"(
void FSCHG (void)
{
  if (FPSCR_PR == 0)
  {
    FPSCR ^= 0x00100000;  // toggle bit 20
    PC += 2;
  }
  else
    undefined_operation ();
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "fpchg",
  SH4A,
  abstract { "~FPSCR.PR → FPSCR.PR" },
  code { "1111011111111101" },

  group { SH4A, "FE" },
  issue { SH4A, "1" },
  latency { SH4A, "1" },



  description
{R"(
Inverts the PR bit of the floating-point status register FPSCR. The value of
this bit selects single-precision or double-precision operation.
)"},

  note
{R"(

)"},

  operation
{R"(
void FPCHG (void)
{
  FPSCR ^= 0x00080000;  // toggle bit 19
  PC += 2;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP Data Transfer Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "nopx",
  SH_DSP,
  abstract { "No operation" },
  code { "1111000*0*0*00**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
No access operation for X memory.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	@Ax,Dx",
  SH_DSP,
  abstract { "(Ax) → MSW of Dx, 0 → LSW of Dx" },
  code { "111100A*D*0*01**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for X memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(
MOVX.W  @R4,X0   ! Before execution: R4 = 0x08010000, @R4 = 0x5555, X0 = 0x12345678
                 ! After execution:  R4 = 0x08010000, X0 = 0x55550000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	@Ax+,Dx",
  SH_DSP,
  abstract { "(Ax) → MSW of Dx, 0 → LSW of Dx, Ax+2 → Ax" },
  code { "111100A*D*0*10**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for X memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(
MOVX.W  @R4+,X0  ! Before execution: R4 = 0x08010000, @R4 = 0x5555, X0 = 0x12345678
                 ! After execution:  R4 = 0x08010002, X0 = 0x55550000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	@Ax+Ix,Dx",
  SH_DSP,
  abstract { "(Ax) → MSW of Dx, 0 → LSW of Dx, Ax+Ix → Ax" },
  code { "111100A*D*0*11**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for X memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	Da,@Ax",
  SH_DSP,
  abstract { "MSW of Da → (Ax)" },
  code { "111100A*D*1*01**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for X memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	Da,@Ax+",
  SH_DSP,
  abstract { "MSW of Da → (Ax), Ax+2 → Ax" },
  code { "111100A*D*1*10**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for X memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movx.w	Da,@Ax+Ix",
  SH_DSP,
  abstract { "MSW of Da → (Ax), Ax+Ix → Ax" },
  code { "111100A*D*1*11**" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for X memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVY instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "nopy",
  SH_DSP,
  abstract { "No Operation" },
  code { "111100*0*0*0**00" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
No access operation for Y memory.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	@Ay,Dy",
  SH_DSP,
  abstract { "(Ay) → MSW of Dy, 0 → LSW of Dy" },
  code { "111100*A*D*0**01" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for Y memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	@Ay+,Dy",
  SH_DSP,
  abstract { "(Ay) → MSW of Dy, 0 → LSW of Dy, Ay+2 → Ay" },
  code { "111100*A*D*0**10" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for Y memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	@Ay+Iy,Dy",
  SH_DSP,
  abstract { "(Ay) → MSW of Dy, 0 → LSW of Dy, Ay+Iy → Ay" },
  code { "111100*A*D*0**11" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the memory source operand data to the destination register operand.
The transferred data can only be word length for Y memory. The word data is
loaded to the top word of the register and the bottom word is cleared with
zeros.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
<br />
LSW = Low-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	Da,@Ay",
  SH_DSP,
  abstract { "MSW of Da → (Ay)" },
  code { "111100*A*D*1**01" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for Y memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	Da,@Ay+",
  SH_DSP,
  abstract { "MSW of Da → (Ay), Ay+2 → Ay" },
  code { "111100*A*D*1**10" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for Y memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movy.w	Da,@Ay+Iy",
  SH_DSP,
  abstract { "MSW of Da → (Ay), Ay+Iy → Ay" },
  code { "111100*A*D*1**11" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the register source operand data to the destination memory operand.
The transferred data can only be word length for Y memory. The source word data
is the top word of the register.
)"},

  note
{R"(
"*" of the instruction code is MOVX instruction designation area.
<br />
MSW = High-order word of operand.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	@-As,Ds",
  SH_DSP,
  abstract { "As-2 → As, (As) → MSW of Ds, 0 → LSW of Ds" },
  code { "111101AADDDD0000" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the word data is loaded to the top word of the register and the bottom
word is cleared with zeros.  When the destination operand is a register with
guard bits, the sign is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	@As,Ds",
  SH_DSP,
  abstract { "(As) → MSW of Ds, 0 → LSW of Ds" },
  code { "111101AADDDD0100" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the word data is loaded to the top word of the register and the bottom
word is cleared with zeros.  When the destination operand is a register with
guard bits, the sign is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	@As+,Ds",
  SH_DSP,
  abstract { "(As) → MSW of Ds, 0 → LSW of Ds, As+2 → As" },
  code { "111101AADDDD1000" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the word data is loaded to the top word of the register and the bottom
word is cleared with zeros.  When the destination operand is a register with
guard bits, the sign is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	@As+Ix,Ds",
  SH_DSP,
  abstract { "(As) → MSW of Ds, 0 → LSW of DS, As+Ix → As" },
  code { "111101AADDDD1100" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the word data is loaded to the top word of the register and the bottom
word is cleared with zeros.  When the destination operand is a register with
guard bits, the sign is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	Ds,@-As",
  SH_DSP,
  abstract { "As-2 → As, MSW of Ds → (As)" },
  code { "111101AADDDD0001" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the top word of the register is stored as the word data.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	Ds,@As",
  SH_DSP,
  abstract { "MSW of Ds → (As)" },
  code { "111101AADDDD0101" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the top word of the register is stored as the word data.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	Ds,@As+",
  SH_DSP,
  abstract { "MSW of Ds → (As), As+2 → As" },
  code { "111101AADDDD1001" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the top word of the register is stored as the word data.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.w	Ds,@As+Is",
  SH_DSP,
  abstract { "MSW of DS → (As), As+Is → As" },
  code { "111101AADDDD1101" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
word, the top word of the register is stored as the word data.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	@-As,Ds",
  SH_DSP,
  abstract { "As-4 → As, (As) → Ds" },
  code { "111101AADDDD0010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.  When the destination operand is a register with guard bits, the sign
is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	@As,Ds",
  SH_DSP,
  abstract { "(As) → Ds" },
  code { "111101AADDDD0110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.  When the destination operand is a register with guard bits, the sign
is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	@As+,Ds",
  SH_DSP,
  abstract { "(As) → Ds, As+4 → As" },
  code { "111101AADDDD1010" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.  When the destination operand is a register with guard bits, the sign
is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	@As+Is,Ds",
  SH_DSP,
  abstract { "(As) → Ds, As+Is → As" },
  code { "111101AADDDD1110" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.  When the destination operand is a register with guard bits, the sign
is extended and stored in the guard bits.
)"},

  note
{R"(

)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	Ds,@-As",
  SH_DSP,
  abstract { "As-4 → As, Ds → (As)" },
  code { "111101AADDDD0011" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	Ds,@As",
  SH_DSP,
  abstract { "Ds → (As)" },
  code { "111101AADDDD0111" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	Ds,@As+",
  SH_DSP,
  abstract { "Ds → (As), As+4 → As" },
  code { "111101AADDDD1011" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "movs.l	Ds,@As+Is",
  SH_DSP,
  abstract { "Ds → (As), As+Is → As" },
  code { "111101AADDDD1111" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Transfers the source operand data to the destination. The transferred data is a
longword.
)"},

  note
{R"(
When one of the guard bit registers A0G and A1G is the source operand it is
sign extended and stored as a word.
)"},

  operation
{R"(

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP ALU Arithmetic Operation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pabs		Sx,Dz",
  SH_DSP,
  abstract { "If Sx >= 0: Sx → Dz\nIf Sx < 0: 0 - Sx → Dz" },
  code { "111110********** 10001000xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Finds absolute values. When the Sx operand is positive, the contents of the
operand are transferred to the Dz operand. If the value is negative, the value
of the Sx operand is subtracted from 0 and stored in the Dz operand.
<br /><br />
The DC bit of the DSR register are updated according to the specifications of
the CS bits. The N, Z, V, and GT bits of the DSR register are updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pabs_sx (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC2 = X0;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = X1;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC2 = A0;
    DSP_ALU_SRC2G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC2 = A1;
    DSP_ALU_SRC2G = A1G;
    break;
  }

  if (DSP_ALU_SRC2G_BIT7 == 0)
  {
    // positive value
    DSP_ALU_DST = 0x0 + DSP_ALU_SRC2;
    carry_bit = 0;
    DSP_ALU_DSTG_LSB8 = 0x0 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  }
  else
  {
    // negative value
    DSP_ALU_DST = 0x0 - DSP_ALU_SRC2;
    borrow_bit = 1;
    DSP_ALU_DSTG_LSB8 = 0x0 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  }

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"

  if (DSP_ALU_SRC2G_BIT7 == 0)
  {
    #include "fixed_pt_plus_dc_bit.c"
  }
  else
  {
    overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
    #include "fixed_pt_minus_dc_bit.c"
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pabs		Sy,Dz",
  SH_DSP,
  abstract { "If Sy >= 0: Sy → Dz\nIf Sy < 0: 0 - Sy → Dz" },
  code { "111110********** 1010100000yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Finds absolute values. When the Sy operand is positive, the contents of the
operand are transferred to the Dz operand. If the value is negative, the value
of the Sy operand is subtracted from 0 and stored in the Dz operand.
<br /><br />
The DC bit of the DSR register are updated according to the specifications of
the CS bits. The N, Z, V, and GT bits of the DSR register are updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pabs_sy (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  if (DSP_ALU_SRC2G_BIT7 == 0)
  {
    // positive value
    DSP_ALU_DST = 0x0 + DSP_ALU_SRC2;
    carry_bit = 0;
    DSP_ALU_DSTG_LSB8 = 0x0 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  }
  else
  {
    // negative value
    DSP_ALU_DST = 0x0 - DSP_ALU_SRC2;
    borrow_bit = 1;
    DSP_ALU_DSTG_LSB8 = 0x0 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  }

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"

  if (DSP_ALU_SRC2G_BIT7 == 0)
  {
    #include "fixed_pt_plus_dc_bit.c"
  }
  else
  {
    overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
    #include "fixed_pt_minus_dc_bit.c"
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "padd		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx + Sy → Dz" },
  code { "111110********** 10110001xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Adds the contents of the Sx and Sy operands and stores the result in the Dz
operand.  The DC bit of the DSR register is updated according to the
specifications for the CS bits. The N, Z, V, and GT bits of the DSR register
are also updated.
)"},

  note
{R"(
The DC bit is updated depending on the state of the CS [2:0] bit immediately
before the operation.
)"},

  operation
{R"(
void padd (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
  break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1: DSP_ALU_SRC2 = Y1;
    break;

  case 0x2: DSP_ALU_SRC2 = M0;
    break;

  case 0x3: DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;

  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);

  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
  #include "fixed_pt_overflow_protection.c"

  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(
PADD   X0,Y0,A0   NOPX   NOPY    ! Before execution: X0 = 0x22222222, Y0 = 0x33333333, A0 = 0x123456789A
                                 ! After execution:  X0 = 0x22222222, Y0 = 0x33333333, A0 = 0x0055555555
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct padd	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx + Sy → Dz\nElse: nop" },
  code { "111110********** 10110010xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds the contents of the Sx and Sy operands and stores the result
in the Dz operand.  The instruction is executed of the DC bit is set to 1.
Otherwise no operation is performed.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void padd_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
  break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1: DSP_ALU_SRC2 = Y1;
    break;

  case 0x2: DSP_ALU_SRC2 = M0;
    break;

  case 0x3: DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;

  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);

  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG [ex2_dz_no] = DSP_ALU_DST;
    if(ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7) A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no==1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf padd	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx + Sy → Dz\nElse: nop" },
  code { "111110********** 10110011xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds the contents of the Sx and Sy operands and stores the result
in the Dz operand.  The instruction is executed of the DC bit is set to 0.
Otherwise no operation is performed.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void padd_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
  break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1: DSP_ALU_SRC2 = Y1;
    break;

  case 0x2: DSP_ALU_SRC2 = M0;
    break;

  case 0x3: DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;

  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);

  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);
  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG [ex2_dz_no] = DSP_ALU_DST;
    if(ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7) A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no==1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "padd		Sx,Sy,Du\npmuls		Se,Sf,Dg",
  SH_DSP,
  abstract { "Sx + Sy → Du\nMSW of Se × MSW of Sf → Dg" },
  code { "111110********** 0111eeffxxyygguu" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Adds the contents of the Sx and Sy operands and stores the result in the Du
operand. The contents of the top word of the Se and Sf operands are multiplied
as signed and the result stored in the Dg operand. These two processes are
executed simultaneously in parallel.
<br /><br />
The DC bit of the DSR register is updated according to the results of the ALU
operation and the specifications for the CS bits. The N, Z, V, and GT bits of
the DSR register are also updated according to the results of the ALU operation.
)"},

  note
{R"(
Since the PMULS is fixed decimal point multiplication, the operation result is
different from that of MULS even though the source data is the same.
)"},

  operation
{R"(
void padd_pmuls (void)
{
  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  switch (EX2_DU)
  {
  case 0x0:
    X0 = DSP_ALU_DST;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;

  case 0x1:
    Y0 = DSP_ALU_DST;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;

  case 0x2:
    A0 = DSP_ALU_DST;
    A0G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A0G = A0G | MASKFFFFFF00;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;

  case 0x3:
    A1 = DSP_ALU_DST;
    A1G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A1G = A1G | MASKFFFFFF00;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;
  }

  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(
PADD  A0,M0,A0  PMULS X0,YO,MO  NOPX  NOPY
                ! Before execution:  X0 = 0x00020000, Y0 = 0x00030000, M0 = 0x22222222, A0 = 0x0055555555
                ! After execution: X0 = 0x00020000, Y0 = 0x00030000, M0 = 0x0000000C, A0 = 0x0077777777
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "paddc		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx + Sy + DC → Dz" },
  code { "111110********** 10110000xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Adds the contents of the Sx and Sy operands to the DC bit and stores the result
in the Dz operand. The DC bit of the DSR register is updated as the carry flag.
The N, Z, V, and GT bits of the DSR register are also updated.

)"},

  note
{R"(
The DC bit is updated as the carry flag after execution of the PADDC instruction
regardless of the CS bits.
<br /><br />
CS[2:0] = ***: Always operate as Carry or Borrow mode, regardless of the status
of the DC bit.
)"},

  operation
{R"(
void paddc (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2 + DSPDCBIT;

  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);

  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;

  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_dc_always_carry.c"
}
)"},

  example
{R"(
PADDC X0,Y0,M0  NOPX  NOPY   ! Before execution: X0 = 0xB3333333, Y0 = 0x55555555 M0 = 0x12345678, DC = 0
                             ! After execution: X0 = 0xB3333333, Y0 = 0x55555555 M0 = 0x08888888, DC = 1


PADDC X0,Y0,M0  NOPX  NOPY   ! Before execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0x12345678, DC = 1
                             ! After execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0x88888889, DC = 0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pclr		Dz",
  SH_DSP,
  abstract { "0x00000000 → Dz" },
  code { "111110********** 100011010000zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Clears the Dz operand.  The DC bit of the DSR register is updated according to
the specifications for the CS bits. The Z bit of the DSR register is set to 1.
The N, V, and GT bits are cleared to 0.
)"},

  note
{R"(

)"},

  operation
{R"(
void pclr (void)
{
  DSP_REG[ex2_dz_no] = 0x0;

  if (ex2_dz_no == 0)
    A0G = 0x0;
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  carry_bit = 0;
  negative_bit = 0;
  zero_bit = 1;
  overflow_bit = 0;

  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(
PCLR  A0  NOPX  NOPY   ! Before execution: A0 = 0xFF87654321
                       ! After execution: A0 = 0x0000000000
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pclr	Dz",
  SH_DSP,
  abstract { "If DC = 1: 0x00000000 → Dz\nElse: nop" },
  code { "111110********** 100011100000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally clears the Dz operand.  The instruction is executed when the DC
bit is set to 1.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pclr_dct (void)
{
  if (DC == 1)
    DSP_REG[ex2_dz_no] = 0x0;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pclr	Dz",
  SH_DSP,
  abstract { "If DC = 0: 0x00000000 → Dz\nElse: nop" },
  code { "111110********** 100011110000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally clears the Dz operand.  The instruction is executed when the DC
bit is set to 0.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pclr_dcf (void)
{
  if (DC == 0)
    DSP_REG[ex2_dz_no] = 0x0;
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pcmp		Sx,Sy",
  SH_DSP,
  abstract { "Sx - Sy" },
  code { "111110********** 10000100xxyy0000" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts the contents of the Sy operand from the Sx operand. The DC bit of the
DSR register is updated according to the specifications for the CS bits.
The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcmp (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;

  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);

  borrow_bit = ! carry_bit;

  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;

  negative_bit = DSP_ALU_DSTG_BIT7;
  zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_minus_dc_bit.c"

}
)"},

  example
{R"(
PCMP  X0,Y0  NOPX  NOPY  ! Before execution: X0 = 0x22222222, Y0 = 0x33333333
                         ! After execution: X0 = 0x22222222, Y0 = 0x33333333
                         !                  N = 1, Z = 0, V = 0, GT = 0
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pcopy		Sx,Dz",
  SH_DSP,
  abstract { "Sx → Dz" },
  code { "111110********** 11011001xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the Sx operand in the Dz operand.  The DC bit of the DSR register
is updated according to the specifications for the CS bits. The N, Z, V, and GT
bits are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sx (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_SRC2 = 0;
  DSP_ALU_SRC2G = 0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_plus_dc_bit.c"
}

)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pcopy		Sy,Dz",
  SH_DSP,
  abstract { "Sy → Dz" },
  code { "111110********** 1111100100yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the Sy operand in the Dz operand.  The DC bit of the DSR register
is updated according to the specifications for the CS bits. The N, Z, V, and GT
bits are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sy (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(
PCOPY  X0,A0  NOPX  NOPY  ! Before execution: X0 = 0x55555555, A0 = 0xFFFFFFFF
                          ! After execution: X0 = 0x55555555, A0 = 0x0055555555
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pcopy	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx → Dz\nElse: nop" },
  code { "111110********** 11011010xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Sx operand in the Dz operand.  The instruction
is executed if the DC bit is set to 1.  The DC, N, Z, V, and GT bits are not
updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sx_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_SRC2 = 0;
  DSP_ALU_SRC2G = 0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pcopy	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sy → Dz\nElse: nop" },
  code { "111110********** 1111101000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Sy operand in the Dz operand.  The instruction
is executed if the DC bit is set to 1.  The DC, N, Z, V, and GT bits are not
updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sy_dct (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pcopy	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx → Dz\nElse: nop" },
  code { "111110********** 11011011xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Sx operand in the Dz operand.  The instruction
is executed if the DC bit is set to 0.  The DC, N, Z, V, and GT bits are not
updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sx_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_SRC2 = 0;
  DSP_ALU_SRC2G = 0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pcopy	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sy → Dz\nElse: nop" },
  code { "111110********** 1111101100yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Sy operand in the Dz operand.  The instruction
is executed if the DC bit is set to 0.  The DC, N, Z, V, and GT bits are not
updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pcopy_sy_dcf (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 + DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pneg		Sx,Dz",
  SH_DSP,
  abstract { "0 - Sx → Dz" },
  code { "111110********** 11001001xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Reverses the sign. Subtracts the Sx operand from 0 and stores the result in
the Dz operand.  The DC bit of the DSR register is updated according to the
specifications for the CS bits. The N, Z, V, and GT bits of the DSR register
are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sx (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC2 = X0;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = X1;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC2 = A0;
    DSP_ALU_SRC2G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC2 = A1;
    DSP_ALU_SRC2G = A1G;
    break;
  }

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_minus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pneg		Sy,Dz",
  SH_DSP,
  abstract { "0 - Sy → Dz" },
  code { "111110********** 1110100100yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Reverses the sign. Subtracts the Sy operand from 0 and stores the result in
the Dz operand.  The DC bit of the DSR register is updated according to the
specifications for the CS bits. The N, Z, V, and GT bits of the DSR register
are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sy (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_minus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pneg	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 1: 0 - Sx → Dz\nElse: nop" },
  code { "111110********** 11001010xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally reverses the sign.  The instruction is executed if the DC bit is
set to 1.  Subtracts the Sx operand from 0 and stores the result in the Dz
operand.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sx_dct (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC2 = X0;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = X1;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC2 = A0;
    DSP_ALU_SRC2G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC2 = A1;
    DSP_ALU_SRC2G = A1G;
    break;
  }

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pneg	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: 0 - Sy → Dz\nElse: nop" },
  code { "111110********** 1110101000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally reverses the sign.  The instruction is executed if the DC bit is
set to 1.  Subtracts the Sy operand from 0 and stores the result in the Dz
operand.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sy_dct (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pneg	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 0: 0 - Sx → Dz\nElse: nop" },
  code { "111110********** 11001011xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally reverses the sign.  The instruction is executed if the DC bit is
set to 0.  Subtracts the Sx operand from 0 and stores the result in the Dz
operand.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sx_dcf (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC2 = X0;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = X1;
    if (DSP_ALU_SRC2_MSB)
      DSP_ALU_SRC2G = 0xFF;
    else
      DSP_ALU_SRC2G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC2 = A0;
    DSP_ALU_SRC2G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC2 = A1;
    DSP_ALU_SRC2G = A1G;
    break;
  }

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pneg	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: 0 - Sy → Dz\nElse: nop" },
  code { "111110********** 1110101100yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally reverses the sign.  The instruction is executed if the DC bit is
set to 0.  Subtracts the Sy operand from 0 and stores the result in the Dz
operand.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pneg_sy_dcf (void)
{
  DSP_ALU_SRC1 = 0;
  DSP_ALU_SRC1G = 0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psub		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx - Sy → Dz" },
  code { "111110********** 10100001xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts the contents of the Sy operand from the Sx operand and stores the
result in the Dz operand.  The DC bit of the DSR register is updated according
to the specifications for the CS bits. The N, Z, V, and GT bits of the DSR
register are updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psub (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_minus_dc_bit.c"
}
)"},

  example
{R"(
PSUB  X0,Y0,A0  NOPX  NOPY  ! Before execution: X0 = 0x55555555, Y0 = 0x33333333, A0 = 0x123456789A
                            ! After execution: X0 = 0x55555555, Y0 = 0x33333333, A0 = 0x0022222222
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct psub	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx - Sy → Dz\nElse: nop" },
  code { "111110********** 10100010xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts the contents of the Sy operand from the Sx operand and
stores the result in the Dz operand.  The instruction is executed if the DC bit
is set to 1.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psub_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf psub 	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx - Sy → Dz\nElse: nop" },
  code { "111110********** 10100011xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts the contents of the Sy operand from the Sx operand and
stores the result in the Dz operand.  The instruction is executed if the DC bit
is set to 0.  The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psub_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psub		Sx,Sy,Du\npmuls		Se,Sf,Dg",
  SH_DSP,
  abstract { "Sx - Sy → Du\nMSW of Se × MSW of Sf → Dg" },
  code { "111110********** 0110eeffxxyygguu" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts the contents of the Sy operand from the Sx operand and stores the
result in the Du operand. The contents of the top word of the Se and Sf operands
are multiplied as signed and the result stored in the Dg operand. These two
processes are executed simultaneously in parallel.
<br /><br />
The DC bit of the DSR register is updated according to the results of the ALU
operation and the specifications for the CS bits. The N, Z, V, and GT bits of
the DSR register are also updated according to the results of the ALU operation.
)"},

  note
{R"(

)"},

  operation
{R"(
void psub_pmuls (void)
{
  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  switch (EX2_DU)
  {
  case 0x0:
    X0 = DSP_ALU_DST;
    negative_bit = DSP_ALU_DST_MSB;
    zero_bit = (DSP_ALU_DST == 0);
    break;

  case 0x1:
    Y0  = DSP_ALU_DST;
    negative_bit = DSP_ALU_DST_MSB;
    zero_bit = (DSP_ALU_DST == 0);
    break;

  case 0x2:
    A0 = DSP_ALU_DST;
    A0G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A0G = A0G | MASKFFFFFF00;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;

  case 0x3:
    A1 = DSP_ALU_DST;
    A1G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A1G = A1G | MASKFFFFFF00;
    negative_bit = DSP_ALU_DSTG_BIT7;
    zero_bit = (DSP_ALU_DST == 0) & (DSP_ALU_DSTG_LSB8 == 0);
    break;
  }

  #include "fixed_pt_minus_dc_bit.c"
}
)"},

  example
{R"(
PSUB  A0,M0,A0  PMULS X0,Y0,M0  NOPX  NOPY
            ! Before execution: X0 = 0x00020000, Y0 = 0xFFFE0000, M0 = 0x33333333, A0 = 0x0022222222
            ! After execution: X0 = 0x00020000, Y0 = 0xFFFE0000, M0 = 0xFFFFFFF8, A0 = 0x55555555
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psubc		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx - Sy - DC → Dz" },
  code { "111110********** 10100000xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts the contents of the Sy operand and the DC bit from the Sx operand and
stores the result in the Dz operand. The DC bit of the DSR register is updated
as the borrow flag. The N, Z, V, and GT bits of the DSR register are also
updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psubc (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  DSP_ALU_DST = DSP_ALU_SRC1 - DSP_ALU_SRC2 - DSPDCBIT;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = MINUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_dc_always_borrow.c"
}
)"},

  example
{R"(
PSUBC X0,Y0,M0  NOPX  NOPY  ! Before execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0x0012345678, DC = 0
                            ! After execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0xFFDDDDDDDE, DC = 1

PSUBC X0,Y0,M0  NOPX  NOPY  ! Before execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0x0012345678, DC = 1
                            ! After execution: X0 = 0x33333333, Y0 = 0x55555555 M0 = 0xFFDDDDDDDD, DC = 1
)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pdec		Sx,Dz",
  SH_DSP,
  abstract { "MSW of Sx - 1 → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 10001001xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts 1 from the top word of the Sx operand, stores the result in the upper
word of the Dz operand, and clears the bottom word of the Dz operand with zeros.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(
The bottom word of the destination register is ignored when the DC bit is
updated.
)"},

  operation
{R"(
void pdec_sx (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"
  #include "integer_unconditional_update.c"
  #include "integer_minus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pdec		Sy,Dz",
  SH_DSP,
  abstract { "MSW of Sy - 1 → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 1010100100yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Subtracts 1 from the top word of the Sy operand, stores the result in the upper
word of the Dz operand, and clears the bottom word of the Dz operand with zeros.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(
The bottom word of the destination register is ignored when the DC bit is
updated.
)"},

  operation
{R"(
void pdec_sy (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"
  #include "integer_unconditional_update.c"
  #include "integer_minus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pdec	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 1: MSW of Sx - 1 → MSW of DZ, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10001010xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts 1 from the top word of the Sx operand, stores the result
in the upper word of the Dz operand, and clears the bottom word of the Dz
operand with zeros.  The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdec_sx_dct (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pdec	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: MSW of Sy - 1 → MSW of DZ, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1010101000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts 1 from the top word of the Sy operand, stores the result
in the upper word of the Dz operand, and clears the bottom word of the Dz
operand with zeros.  The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdec_sy_dct (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pdec	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 0: MSW of Sx - 1 → MSW of DZ, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10001011xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts 1 from the top word of the Sx operand, stores the result
in the upper word of the Dz operand, and clears the bottom word of the Dz
operand with zeros.  The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdec_sx_dcf (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pdec	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: MSW of Sy - 1 → MSW of DZ, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1010101100yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally subtracts 1 from the top word of the Sy operand, stores the result
in the upper word of the Dz operand, and clears the bottom word of the Dz
operand with zeros.  The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdec_sy_dcf (void)
{
  DSP_ALU_SRC2 = 0x1;
  DSP_ALU_SRC2G = 0x0;

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW - 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | ! DSP_ALU_SRC2_MSB) && ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & ! DSP_ALU_SRC2_MSB);
  borrow_bit = ! carry_bit;
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 - DSP_ALU_SRC2G_LSB8 - borrow_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pinc		Sx,Dz",
  SH_DSP,
  abstract { "MSW of Sy + 1 → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 10011001xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Adds 1 to the top word of the Sx operand, stores the result in the upper word
of the Dz operand, and clears the bottom word of the Dz operand with zeros.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sx (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"
  #include "integer_unconditional_update.c"
  #include "integer_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pinc		Sy,Dz",
  SH_DSP,
  abstract { "MSW of Sy + 1 → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 1011100100yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Adds 1 to the top word of the Sy operand, stores the result in the upper word
of the Dz operand, and clears the bottom word of the Dz operand with zeros.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sy (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"
  #include "integer_unconditional_update.c"
  #include "integer_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pinc	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 1: MSW of Sx + 1 → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10011010xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds 1 to the top word of the Sx operand, stores the result in the
upper word of the Dz operand, and clears the bottom word of the Dz operand with
zeros. The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sx_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pinc	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: MSW of Sy + 1 → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1011101000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds 1 to the top word of the Sy operand, stores the result in the
upper word of the Dz operand, and clears the bottom word of the Dz operand with
zeros. The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sy_dct (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pinc	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 0: MSW of Sx + 1 → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10011011xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds 1 to the top word of the Sx operand, stores the result in the
upper word of the Dz operand, and clears the bottom word of the Dz operand with
zeros. The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sx_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pinc	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: MSW of Sy + 1 → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1011101100yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally adds 1 to the top word of the Sy operand, stores the result in the
upper word of the Dz operand, and clears the bottom word of the Dz operand with
zeros. The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pinc_sy_dcf (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW + 1;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "integer_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pdmsb		Sx,Dz",
  SH_DSP,
  abstract { "Sx data MSB position → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 10011101xx00zzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Finds the first position to change in the lineup of Sx operand bits and stores
the bit position in the Dz operand.  The DC bit of the DSR register is updated
according to the specifications for the CS bits. The N, Z, V, and GT bits of the
DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sx (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;
  overflow_bit = 0;

  #include "integer_unconditional_update.c"
  #include "integer_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pdmsb		Sy,Dz",
  SH_DSP,
  abstract { "Sy data MSB position → MSW of Dz, clear LSW of Dz" },
  code { "111110********** 1011110100yyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Finds the first position to change in the lineup of Sy operand bits and stores
the bit position in the Dz operand.  The DC bit of the DSR register is updated
according to the specifications for the CS bits. The N, Z, V, and GT bits of the
DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sy (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;
  overflow_bit = 0;

  #include "integer_unconditional_update.c"
  #include "integer_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pdmsb	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx data MSB position → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10011110xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally finds the first position to change in the lineup of Sx operand
bits and stores the bit position in the Dz operand. The instruction is executed
if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sx_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pdmsb	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sy data MSB position → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1011111000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally finds the first position to change in the lineup of Sy operand
bits and stores the bit position in the Dz operand. The instruction is executed
if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sy_dct (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pdmsb	Sx,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx data MSB position → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10011111xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally finds the first position to change in the lineup of Sx operand
bits and stores the bit position in the Dz operand. The instruction is executed
if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sx_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pdmsb	Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sy data MSB position → MSW of Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 1011111100yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally finds the first position to change in the lineup of Sy operand
bits and stores the bit position in the Dz operand. The instruction is executed
if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pdmsb_sy_dcf (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  short int i;
  unsigned char msb, src1g;
  unsigned long src1 = DSP_ALU_SRC1;
  msb = DSP_ALU_SRC1G_BIT7;
  src1g = (DSP_ALU_SRC1G_LSB8 << 1);
  for (i = 38; ((msb == (src1g >> 7)) && (i >= 32)); i--)
    src1g <<= 1;

  if (i == 31)
    for(i; ((msb == (src1 >> 31)) && (i >= 0)); i--)
      src1 <<= 1;

  DSP_ALU_DST = 0x0;
  DSP_ALU_DST_HW = (short int)(30 - i);
  if (DSP_ALU_DST_MSB)
    DSP_ALU_DSTG_LSB8 = 0xFF;
  else
    DSP_ALU_DSTG_LSB8 = 0x0;

  carry_bit = 0;

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "prnd		Sx,Dz",
  SH_DSP,
  abstract { "Sx + 0x00008000 → Dz, clear LSW of Dz" },
  code { "111110********** 10011000xx00zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Does rounding. Adds the immediate data 0x00008000 to the contents of the Sx
operand, stores the result in the upper word of the Dz operand, and clears the
bottom word of Dz with zeros.
<br /><br />
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void prnd_sx (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  DSP_ALU_DST = (DSP_ALU_SRC1 + DSP_ALU_SRC2) & MASKFFFF0000;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "prnd		Sy,Dz",
  SH_DSP,
  abstract { "Sy + 0x00008000 → Dz, clear LSW of Dz" },
  code { "111110********** 1011100000yyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Does rounding. Adds the immediate data 0x00008000 to the contents of the Sy
operand, stores the result in the upper word of the Dz operand, and clears the
bottom word of Dz with zeros.
<br /><br />
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void prnd_sy (void)
{
  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC1 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = M1;
    break;
  }

  if (DSP_ALU_SRC1_MSB)
    DSP_ALU_SRC1G = 0xFF;
  else
    DSP_ALU_SRC1G = 0x0;

  DSP_ALU_DST = (DSP_ALU_SRC1 + DSP_ALU_SRC2) & MASKFFFF0000;
  carry_bit = ((DSP_ALU_SRC1_MSB | DSP_ALU_SRC2_MSB) & ! DSP_ALU_DST_MSB)
              | (DSP_ALU_SRC1_MSB & DSP_ALU_SRC2_MSB);
  DSP_ALU_DSTG_LSB8 = DSP_ALU_SRC1G_LSB8 + DSP_ALU_SRC2G_LSB8 + carry_bit;
  overflow_bit = PLUS_OP_G_OV || ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "fixed_pt_plus_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP ALU Logical Operation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pand		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx & Sy → Dz, clear LSW of Dz" },
  code { "111110********** 10010101xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Does an AND of the upper word of the Sx operand and the upper word of the Sy
operand, stores the result in the upper word of the Dz operand, and clears the
bottom word of the Dz operand with zeros. When Dz is a register that has guard
bits, the guard bits are also zeroed. The DC bit of the DSR register is updated
according to the specifications for the CS bits. The N, Z, V, and GT bits of
the DSR register are also updated.
)"},

  note
{R"(
The bottom word of the destination register and the guard bits are ignored when
the DC bit is updated.
)"},

  operation
{R"(
void pand (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW & DSP_ALU_SRC2_HW;

  DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
  DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
  if (ex2_dz_no == 0)
    A0G = 0x0;  // clear Guard bits
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  carry_bit = 0x0;
  negative_bit = DSP_ALU_DST_MSB;
  zero_bit = (DSP_ALU_DST_HW == 0);
  overflow_bit = 0x0;

  #include "logical_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pand	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx & Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10010110xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally does an AND of the upper word of the Sx operand and the upper word
of the Sy operand, stores the result in the upper word of the Dz operand, and
clears the bottom word of the Dz operand with zeros. When Dz is a register that
has guard bits, the guard bits are also zeroed.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pand_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW & DSP_ALU_SRC2_HW;

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no==1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pand	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx & Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10010111xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally does an AND of the upper word of the Sx operand and the upper word
of the Sy operand, stores the result in the upper word of the Dz operand, and
clears the bottom word of the Dz operand with zeros. When Dz is a register that
has guard bits, the guard bits are also zeroed.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pand_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW & DSP_ALU_SRC2_HW;

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no==1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "por		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx | Sy → Dz, clear LSW of Dz" },
  code { "111110********** 10110101xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Takes the OR of the top word of the Sx operand and the top word of the Sy
operand, stores the result in the top word of the Dz operand, and clears the
bottom word of Dz with zeros. When Dz is a register that has guard bits, the
guard bits are also zeroed.   The DC bit of the DSR register is updated
according to the specifications for the CS bits. The N, Z, V, and GT bits of
the DSR register are also updated.
)"},

  note
{R"(
The bottom word of the destination register and the guard bits are ignored when
the DC bit is updated.
)"},

  operation
{R"(
void por (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW | DSP_ALU_SRC2_HW;

  DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
  DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
  if (ex2_dz_no == 0)
    A0G = 0x0;  // clear Guard bits
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  carry_bit = 0x0;
  negative_bit = DSP_ALU_DST_MSB;
  zero_bit = (DSP_ALU_DST_HW == 0);
  overflow_bit = 0x0;

  #include "logical_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct por		Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx | Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10110110xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally takes the OR of the top word of the Sx operand and the top word
of the Sy operand, stores the result in the top word of the Dz operand, and
clears the bottom word of Dz with zeros. When Dz is a register that has guard
bits, the guard bits are also zeroed.  The instruction is executed if the DC bit
is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void por_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW | DSP_ALU_SRC2_HW;

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // /*  */
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf por		Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx | Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10110111xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally takes the OR of the top word of the Sx operand and the top word
of the Sy operand, stores the result in the top word of the Dz operand, and
clears the bottom word of Dz with zeros. When Dz is a register that has guard
bits, the guard bits are also zeroed.  The instruction is executed if the DC bit
is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void por_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW | DSP_ALU_SRC2_HW;

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // /*  */
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pxor		Sx,Sy,Dz",
  SH_DSP,
  abstract { "Sx ^ Sy → Dz, clear LSW of Dz" },
  code { "111110********** 10100101xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Takes the exclusive OR of the top word of the Sx operand and the top word of the
Sy operand, stores the result in the top word of the Dz operand, and clears the
bottom word of Dz with zeros. When Dz is a register that has guard bits, the
guard bits are also zeroed.  The DC bit of the DSR register is updated according
to the specifications for the CS bits. The N, Z, V, and GT bits of the DSR
register are also updated.
)"},

  note
{R"(
The bottom word of the destination register and the guard bits are ignored when
the DC bit is updated.
)"},

  operation
{R"(
void pxor (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW ^ DSP_ALU_SRC2_HW;

  DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
  DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
  if (ex2_dz_no == 0)
    A0G = 0x0;  // clear Guard bits
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  carry_bit = 0x0;
  negative_bit = DSP_ALU_DST_MSB;
  zero_bit = (DSP_ALU_DST_HW == 0);
  overflow_bit = 0x0;

  #include "logical_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pxor	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1: Sx ^ Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10100110xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally takes the exclusive OR of the top word of the Sx operand and the
top word of the Sy operand, stores the result in the top word of the Dz operand,
and clears the bottom word of Dz with zeros. When Dz is a register that has
guard bits, the guard bits are also zeroed.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pxor_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW ^ DSP_ALU_SRC2_HW;

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pxor	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0: Sx ^ Sy → Dz, clear LSW of Dz\nElse: nop" },
  code { "111110********** 10100111xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally takes the exclusive OR of the top word of the Sx operand and the
top word of the Sy operand, stores the result in the top word of the Dz operand,
and clears the bottom word of Dz with zeros. When Dz is a register that has
guard bits, the guard bits are also zeroed.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pxor_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1;
    break;
  }

  DSP_ALU_DST_HW = DSP_ALU_SRC1_HW ^ DSP_ALU_SRC2_HW;

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP Fixed Decimal Point Multiplication Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pmuls	Se,Sf,Dg",
  SH_DSP,
  abstract { "MSW of Se × MSW of Sf → Dg" },
  code { "111110********** 0100eeff0000gg00" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
The contents of the top word of the Se and Sf operands are multiplied as signed
and the result stored in the Dg operand. The DC, N, Z, V, and GT bits of the
DSR register are not updated.
)"},

  note
{R"(
Since PMULS is fixed decimal point multiplication, the operation result is
different from that of MULS even though the source data is the same.
)"},

  operation
{R"(
void pmuls (void)
{
  switch (ee)  // Se Operand selection bit (ee)
  {
  case 0x0:
    DSP_M_SRC1 = X0;
    break;

  case 0x1:
    DSP_M_SRC1 = X1;
    break;

  case 0x2:
    DSP_M_SRC1 = Y0;
    break;

  case 0x3:
    DSP_M_SRC1 = A1;
    break;
  }

  switch (ff)  // Sf Operand selection bit (ff)
  {
  case 0x0:
    DSP_M_SRC2 = Y0;
    break;

  case 0x1:
    DSP_M_SRC2 = Y1;
    break;

  case 0x2:
    DSP_M_SRC2 = X0;
    break;

  case 0x3:
    DSP_M_SRC2 = A1;
    break;
  }

  if ((SBIT == 1) && (DSP_M_SRC1 == 0x8000) && (DSP_M_SRC2 == 0x8000))
    DSP_M_DST = 0x7FFFFFFF;  // overflow protection
  else
    DSP_M_DST= ((long)(short)DSP_M_SRC1 * (long)(short)DSP_M_SRC2) << 1;

  if (DSP_M_DST_MSB)
    DSP_M_DSTG_LSB8 = 0xFF;
  else
    DSP_M_DSTG_LSB8 = 0x0;

  switch (gg)  // Dg Operand selection bit (gg)
  {
  case 0x0:
    M0 = DSP_M_DST;
    break;

  case 0x1:
    M1 = DSP_M_DST;
    break;

  case 0x2:
    A0 = DSP_M_DST;
    if (DSP_M_DSTG_LSB8 == 0x0)
      A0G=0x0;
    else
      A0G = 0xFFFFFFFF;
    break;

  case 0x3:
    A1 = DSP_M_DST;
    if (DSP_M_DSTG_LSB8 == 0x0)
      A1G = 0x0;
    else
      A1G = 0xFFFFFFFF;
    break;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP Shift Operation Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psha		Sx,Sy,Dz",
  SH_DSP,
  abstract { "If Sy >= 0: Sx << Sy → Dz\nIf Sy < 0: Sx >> Sy → Dz" },
  code { "111110********** 10010001xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Arithmetically shifts the contents of the Sx or Dz operand and stores the result
in the Dz operand. The amount of the shift is specified by the Sy operand.
When the shift amount is positive, it shifts left. When the shift amount is
negative, it shifts right.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psha (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK007F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK007F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK007F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1 & MASK007F0000;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  if ((DSP_ALU_SRC2_HW & MASK0040) == 0)
  {
    // Left Shift 0 <= cnt <= 32
    char cnt = DSP_ALU_SRC2_HW & MASK003F;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST = DSP_ALU_SRC1 << cnt;
    DSP_ALU_DSTG = ((DSP_ALU_SRC1G << cnt)
                   | (DSP_ALU_SRC1 >> (32 - cnt))) & MASK000000FF;
    carry_bit = ((DSP_ALU_DSTG & MASK00000001) == 0x1);
  }
  else
  {
    // Right Shift 0 < cnt <= 32
    char cnt = (~DSP_ALU_SRC2_HW & MASK003F) + 1;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! shift -%2X exceed range.\n", cnt);
      exit ();
    }

    if ((cnt > 8) && DSP_ALU_SRC1G_BIT7)
    {
      // MSB copy
      DSP_ALU_DST = (DSP_ALU_SRC1 >> 8) | (DSP_ALU_SRC1G << (32 - 8));
      DSP_ALU_DST = (long)DSP_ALU_DST >> (cnt - 8);
    }
    else
      DSP_ALU_DST = (DSP_ALU_SRC1 >> cnt) | (DSP_ALU_SRC1G << (32 - cnt));

    DSP_ALU_DSTG_LSB8 = (char)DSP_ALU_SRC1G_LSB8 >> cnt--;
    carry_bit = ((DSP_ALU_SRC1 >> cnt) & MASK00000001) == 0x1;
  }

  overflow_bit = ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "shift_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct psha	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1 & Sy >= 0: Sx << Sy → Dz\nIf DC = 1 & Sy < 0: Sx >> Sy → Dz\nIf DC = 0: nop" },
  code { "111110********** 10010010xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally arithmetically shifts the contents of the Sx operand and
stores the result in the Dz operand. The amount of the shift is specified by
the Sy operand.  When the shift amount is positive, it shifts left. When the
shift amount is negative, it shifts right.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psha_dct (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK007F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK007F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK007F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1 & MASK007F0000;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  if ((DSP_ALU_SRC2_HW & MASK0040) == 0)
  {
    // Left Shift 0 <= cnt <= 32
    char cnt = DSP_ALU_SRC2_HW & MASK003F;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST = DSP_ALU_SRC1 << cnt;
    DSP_ALU_DSTG = ((DSP_ALU_SRC1G << cnt)
                   | (DSP_ALU_SRC1 >> (32 - cnt))) & MASK000000FF;
    carry_bit = ((DSP_ALU_DSTG & MASK00000001) == 0x1);
  }
  else
  {
    // Right Shift 0 < cnt <= 32
    char cnt = (~DSP_ALU_SRC2_HW & MASK003F) + 1;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! shift -%2X exceed range.\n", cnt);
      exit ();
    }

    if ((cnt > 8) && DSP_ALU_SRC1G_BIT7)
    {
      // MSB copy
      DSP_ALU_DST = (DSP_ALU_SRC1 >> 8) | (DSP_ALU_SRC1G << (32 - 8));
      DSP_ALU_DST = (long)DSP_ALU_DST >> (cnt - 8);
    }
    else
      DSP_ALU_DST = (DSP_ALU_SRC1 >> cnt) | (DSP_ALU_SRC1G << (32 - cnt));

    DSP_ALU_DSTG_LSB8 = (char)DSP_ALU_SRC1G_LSB8 >> cnt--;
    carry_bit = ((DSP_ALU_SRC1 >> cnt) & MASK00000001) == 0x1;
  }

  overflow_bit = ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf psha	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0 & Sy >= 0: Sx << Sy → Dz\nIf DC = 0 & Sy < 0: Sx >> Sy → Dz\nIf DC = 1: nop" },
  code { "111110********** 10010011xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally arithmetically shifts the contents of the Sx operand and
stores the result in the Dz operand. The amount of the shift is specified by
the Sy operand.  When the shift amount is positive, it shifts left. When the
shift amount is negative, it shifts right.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psha_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    DSP_ALU_SRC1G = A1G;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK007F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK007F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK007F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2 = M1 & MASK007F0000;
    break;
  }

  if (DSP_ALU_SRC2_MSB)
    DSP_ALU_SRC2G = 0xFF;
  else
    DSP_ALU_SRC2G = 0x0;

  if ((DSP_ALU_SRC2_HW & MASK0040) == 0)
  {
    // Left Shift 0 <= cnt <= 32
    char cnt = DSP_ALU_SRC2_HW & MASK003F;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST = DSP_ALU_SRC1 << cnt;
    DSP_ALU_DSTG = ((DSP_ALU_SRC1G << cnt)
                   | (DSP_ALU_SRC1 >> (32 - cnt))) & MASK000000FF;
    carry_bit = ((DSP_ALU_DSTG & MASK00000001) == 0x1);
  }
  else
  {
    // Right Shift 0 < cnt <= 32
    char cnt = (~DSP_ALU_SRC2_HW & MASK003F) + 1;
    if (cnt > 32)
    {
      printf ("\nPSHA Sz,Sy,Dz Error! shift -%2X exceed range.\n", cnt);
      exit ();
    }

    if ((cnt > 8) && DSP_ALU_SRC1G_BIT7)
    {
      // MSB copy
      DSP_ALU_DST = (DSP_ALU_SRC1 >> 8) | (DSP_ALU_SRC1G << (32 - 8));
      DSP_ALU_DST = (long)DSP_ALU_DST >> (cnt - 8);
    }
    else
      DSP_ALU_DST = (DSP_ALU_SRC1 >> cnt) | (DSP_ALU_SRC1G << (32 - cnt));

    DSP_ALU_DSTG_LSB8 = (char)DSP_ALU_SRC1G_LSB8 >> cnt--;
    carry_bit = ((DSP_ALU_SRC1 >> cnt) & MASK00000001) == 0x1;
  }

  overflow_bit = ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"

  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = DSP_ALU_DST;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G = A0G | MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G = A1G | MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psha		#imm,Dz",
  SH_DSP,
  abstract { "If imm >= 0: Dz << imm → Dz\nIf imm < 0: Dz >> imm → Dz" },
  code { "111110********** 00000iiiiiiizzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Arithmetically shifts the contents of the Dz operand and stores the result in
the Dz operand. The amount of the shift is specified by the immediate value.
When the shift amount is positive, it shifts left. When the shift amount is
negative, it shifts right.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void psha_imm (void)
{
  unsigned short tmp_imm;
  DSP_ALU_SRC1 = DSP_REG[ex2_dz_no];

  switch (ex2_dz_no)
  {
    case 0x0:
      DSP_ALU_SRC1G = A0G;
      break;

    case 0x1:
      DSP_ALU_SRC1G = A1G;
      break;

    default:
      if (DSP_ALU_SRC1_MSB)
        DSP_ALU_SRC1G = 0xFF;
      else
        DSP_ALU_SRC1G = 0x0;
  }

  tmp_imm = ((EX2_LW >> 4) & MASK0000007F); // bit[10:4]

  if ((tmp_imm & MASK0040) == 0)
  {
    // Left Shift 0 <= cnt <= 32
    char cnt = tmp_imm & MASK003F;
    if (cnt > 32)
    {
      printf ("\nPSHA Dz,#Imm,Dz Error! #Imm=%7X exceed range.\n", tmp_imm);
      exit ();
    }

    DSP_ALU_DST = DSP_ALU_SRC1 << cnt;
    DSP_ALU_DSTG = ((DSP_ALU_SRC1G << cnt)
                   | (DSP_ALU_SRC1 >> (32 - cnt))) & MASK000000FF;
    carry_bit = (DSP_ALU_DSTG & MASK00000001) == 0x1;
  }
  else
  {
    // Right Shift 0 < cnt <= 32
    char cnt = (~tmp_imm & MASK003F) + 1;
    if (cnt > 32)
    {
      printf ("\nPSHA Dz,#Imm,Dz Error! #Imm=%7X exceed range.\n", tmp_imm);
      exit ();
    }

    if ((cnt > 8) && DSP_ALU_SRC1G_BIT7)
    {
      // MSB copy
      DSP_ALU_DST = (DSP_ALU_SRC1 >> 8) | (DSP_ALU_SRC1G << (32 - 8));
      DSP_ALU_DST = (long)DSP_ALU_DST >> (cnt - 8);
    }
    else
      DSP_ALU_DST = (DSP_ALU_SRC1 >> cnt) | (DSP_ALU_SRC1G << (32 - cnt));

    DSP_ALU_DSTG_LSB8 = (char)DSP_ALU_SRC1G_LSB8 >> cnt--;
    carry_bit = ((DSP_ALU_SRC1 >> cnt) & MASK00000001) == 0x1;
  }

  overflow_bit = ! (POS_NOT_OV || NEG_NOT_OV);

  #include "fixed_pt_overflow_protection.c"
  #include "fixed_pt_unconditional_update.c"
  #include "shift_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pshl		Sx,Sy,Dz",
  SH_DSP,
  abstract { "If Sy >= 0: Sx << Sy → Dz, clear LSW of Dz\nIf Sy < 0: Sx >> Sy → Dz, clear LSW of Dz" },
  code { "111110********** 10000001xxyyzzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Logically shifts the top word contents of the Sx operand, stores the result in
the top word of the Dz operand, and clears the bottom word of the Dz operand
with zeros. When Dz is a register that has guard bits, the guard bits are also
zeroed. The amount of the shift is specified by the Sy operand. When the shift
amount is positive, it shifts left. When the shift amount is negative, it
shifts right.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pshl (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK003F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK003F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK003F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2  = M1 & MASK003F0000;
    break;
  }

  if ((DSP_ALU_SRC2_HW & MASK0020) == 0)
  {
    // Left Shift 0 <= cnt <= 16
    char cnt = DSP_ALU_SRC2_HW & MASK001F;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }
    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW << cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW << cnt) & MASK8000) == 0x8000;
  }
  else
  {
    // Right Shift 0 < cnt <= 16
    char cnt = (~DSP_ALU_SRC2_HW & MASK000F) + 1;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift -%2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW >> cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW >> cnt) & MASK0001) == 0x1;
  }

  DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
  DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
  if (ex2_dz_no == 0)
    A0G = 0x0;  // clear Guard bits
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  negative_bit = DSP_ALU_DST_MSB;
  zero_bit = DSP_ALU_DST_HW == 0;
  overflow_bit = 0x0;

  #include "shift_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct pshl	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 1 & Sy >= 0: Sx << Sy → Dz, clear LSW of Dz\nIf DC = 1 & Sy < 0: Sx >> Sy → Dz, clear LSW of Dz\nIf DC = 0: nop" },
  code { "111110********** 10000010xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally logically shifts the top word contents of the Sx operand, stores
the result in the top word of the Dz operand, and clears the bottom word of the
Dz operand with zeros. When Dz is a register that has guard bits, the guard bits
are also zeroed. The amount of the shift is specified by the Sy operand. When
the shift amount is positive, it shifts left. When the shift amount is negative,
it shifts right.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits are not updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pshl_dct
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK003F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK003F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK003F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2  = M1 & MASK003F0000;
    break;
  }

  if ((DSP_ALU_SRC2_HW & MASK0020) == 0)
  {
    // Left Shift 0 <= cnt <= 16
    char cnt = DSP_ALU_SRC2_HW & MASK001F;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }
    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW << cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW << cnt) & MASK8000) == 0x8000;
  }
  else
  {
    // Right Shift 0 < cnt <= 16
    char cnt = (~DSP_ALU_SRC2_HW & MASK000F) + 1;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift -%2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW >> cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW >> cnt) & MASK0001) == 0x1;
  }

  if (DC == 1)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf pshl	Sx,Sy,Dz",
  SH_DSP,
  abstract { "If DC = 0 & Sy >= 0: Sx << Sy → Dz, clear LSW of Dz\nIf DC = 0 & Sy < 0: Sx >> Sy → Dz, clear LSW of Dz\nIf DC = 1: nop" },
  code { "111110********** 10000011xxyyzzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally logically shifts the top word contents of the Sx operand, stores
the result in the top word of the Dz operand, and clears the bottom word of the
Dz operand with zeros. When Dz is a register that has guard bits, the guard bits
are also zeroed. The amount of the shift is specified by the Sy operand. When
the shift amount is positive, it shifts left. When the shift amount is negative,
it shifts right.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits are not updated.

)"},

  note
{R"(

)"},

  operation
{R"(
void pshl_dcf (void)
{
  switch (EX2_SX)
  {
  case 0x0:
    DSP_ALU_SRC1 = X0;
    break;

  case 0x1:
    DSP_ALU_SRC1 = X1;
    break;

  case 0x2:
    DSP_ALU_SRC1 = A0;
    break;

  case 0x3:
    DSP_ALU_SRC1 = A1;
    break;
  }

  switch (EX2_SY)
  {
  case 0x0:
    DSP_ALU_SRC2 = Y0 & MASK003F0000;
    break;

  case 0x1:
    DSP_ALU_SRC2 = Y1 & MASK003F0000;
    break;

  case 0x2:
    DSP_ALU_SRC2 = M0 & MASK003F0000;
    break;

  case 0x3:
    DSP_ALU_SRC2  = M1 & MASK003F0000;
    break;
  }

  if ((DSP_ALU_SRC2_HW & MASK0020) == 0)
  {
    // Left Shift 0 <= cnt <= 16
    char cnt = DSP_ALU_SRC2_HW & MASK001F;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift %2X exceed range.\n", cnt);
      exit ();
    }
    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW << cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW << cnt) & MASK8000) == 0x8000;
  }
  else
  {
    // Right Shift 0 < cnt <= 16
    char cnt = (~DSP_ALU_SRC2_HW & MASK000F) + 1;
    if (cnt > 16)
    {
      printf ("\nPSHL Sx,Sy,Dz Error! Shift -%2X exceed range.\n", cnt);
      exit ();
    }

    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW >> cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW >> cnt) & MASK0001) == 0x1;
  }

  if (DC == 0)
  {
    DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
    DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
    if (ex2_dz_no == 0)
      A0G = 0x0;  // clear Guard bits
    else if (ex2_dz_no == 1)
      A1G = 0x0;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "pshl		#imm,Dz",
  SH_DSP,
  abstract { "If imm >= 0: Dz << imm → Dz, clear LSW of Dz\nIf imm < 0: Dz >> imm, clear LSW of Dz" },
  code { "111110********** 00010iiiiiiizzzz" },
  flags { "Update → DC" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Logically shifts the top word contents of the Dz operand, stores the result in
the top word of the Dz operand, and clears the bottom word of the Dz operand
with zeros. When Dz is a register that has guard bits, the guard bits
are also zeroed. The amount of the shift is specified by the immediate value.
When the shift amount is positive, it shifts left. When the shift amount is
negative, it shifts right.
The DC bit of the DSR register is updated according to the specifications for
the CS bits. The N, Z, V, and GT bits of the DSR register are also updated.
)"},

  note
{R"(

)"},

  operation
{R"(
void pshl_imm (void)
{
  unsigned short tmp_imm;
  DSP_ALU_SRC1 = DSP_REG[ex2_dz_no];
  switch (ex2_dz_no)
  {
  case 0x0:
    DSP_ALU_SRC1G = A0G;
    break;

  case 0x1:
    DSP_ALU_SRC1G = A1G;
    break;

  default:
    if (DSP_ALU_SRC1_MSB)
      DSP_ALU_SRC1G = 0xFF;
    else
      DSP_ALU_SRC1G = 0x0;
  }

  tmp_imm = ((EX2_LW >> 4) & MASK0000003F); // bit[9:4]
  if ((tmp_imm & MASK0020) == 0)
  {
    // Left Shift 0 <= cnt < 16
    char cnt = tmp_imm & MASK001F;
    if (cnt > 16)
    {
      printf ("\nPSHL Dz,#Imm,Dz Error! #Imm=%6X exceed range.\n", tmp_imm);
      exit ();
    }
    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW << cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW << cnt) & MASK8000) == 0x8000;
  }
  else
  {
    // Right Shift 0 < cnt <= 16
    char cnt = (~tmp_imm & MASK001F) + 1;
    if (cnt > 16)
    {
      printf ("\nPSHL Dz,#Imm,Dz Error! #Imm=%6X exceed range.\n", tmp_imm);
      exit ();
    }
    DSP_ALU_DST_HW = DSP_ALU_SRC1_HW >> cnt--;
    carry_bit = ((DSP_ALU_SRC1_HW >> cnt) & MASK0001) == 0x1;
  }

  DSP_REG_WD[ex2_dz_no*2] = DSP_ALU_DST_HW;
  DSP_REG_WD[ex2_dz_no*2+1] = 0x0;  // clear LSW
  if (ex2_dz_no == 0)
    A0G = 0x0;  // clear Guard bits
  else if (ex2_dz_no == 1)
    A1G = 0x0;

  negative_bit = DSP_ALU_DST_MSB;
  zero_bit = DSP_ALU_DST_HW == 0;
  overflow_bit = 0x0;

  #include "shift_dc_bit.c"
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});

insn_blocks.push_back
(insns { "DSP System Control Instructions",

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "plds		Dz,MACH",
  SH_DSP,
  abstract { "Dz → MACH" },
  code { "111110********** 111011010000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the Dz operand in the MACH register.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_mach (void)
{
  MACH = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "plds		Dz,MACL",
  SH_DSP,
  abstract { "Dz → MACL" },
  code { "111110********** 111111010000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the Dz operand in the MACL register.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_macl (void)
{
  MACL = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct plds	Dz,MACH",
  SH_DSP,
  abstract { "If DC = 1: Dz → MACH\nElse: nop" },
  code { "111110********** 111011100000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Dz operand in the MACH register.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_mach_dct (void)
{
  if (DC == 1)
    MACH = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct plds	Dz,MACL",
  SH_DSP,
  abstract { "If DC = 1: Dz → MACL\nElse: nop" },
  code { "111110********** 111111100000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Dz operand in the MACL register.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_macl_dct (void)
{
  if (DC == 1)
    MACL = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf plds	Dz,MACH",
  SH_DSP,
  abstract { "If DC = 0: Dz → MACH\nElse: nop" },
  code { "111110********** 111011110000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Dz operand in the MACH register.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_mach_dcf (void)
{
  if (DC == 0)
    MACH = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf plds	Dz,MACL",
  SH_DSP,
  abstract { "If DC = 0: Dz → MACL\nElse: nop" },
  code { "111110********** 111111110000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the Dz operand in the MACL register.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX, and MOVY can be designated in parallel, their execution may
take two cycles.
)"},

  operation
{R"(
void plds_macl_dcf (void)
{
  if (DC == 0)
    MACL = DSP_REG[ex2_dz_no];
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psts		MACH,Dz",
  SH_DSP,
  abstract { "MACH → Dz" },
  code { "111110********** 110011010000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the contents of the MACH register in the Dz operand.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_mach (void)
{
  DSP_REG[ex2_dz_no] = MACH;
  if (ex2_dz_no == 0)
  {
    A0G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A0G |= MASKFFFFFF00;
  }
  else if (ex2_dz_no == 1)
  {
    A1G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A1G |= MASKFFFFFF00;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "psts		MACL,Dz",
  SH_DSP,
  abstract { "MACL → Dz" },
  code { "111110********** 110111010000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Stores the contents of the MACL register in the Dz operand.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_macl (void)
{
  DSP_REG[ex2_dz_no] = MACL;
  if (ex2_dz_no == 0)
  {
    A0G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A0G |= MASKFFFFFF00;
  }
  else if (ex2_dz_no == 1)
  {
    A1G = DSP_ALU_DSTG & MASK000000FF;
    if (DSP_ALU_DSTG_BIT7)
      A1G |= MASKFFFFFF00;
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct psts	MACH,Dz",
  SH_DSP,
  abstract { "If DC = 1: MACH → Dz\nElse: nop" },
  code { "111110********** 110011100000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the contents of the MACH register in the Dz operand.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_mach_dct (void)
{
  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = MACH;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G |= MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G |= MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dct psts	MACL,Dz",
  SH_DSP,
  abstract { "If DC = 1: MACL → Dz\nElse: nop" },
  code { "111110********** 110111100000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the contents of the MACL register in the Dz operand.
The instruction is executed if the DC bit is set to 1.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_macl_dct (void)
{
  if (DC == 1)
  {
    DSP_REG[ex2_dz_no] = MACL;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G |= MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G |= MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf psts	MACH,Dz",
  SH_DSP,
  abstract { "If DC = 0: MACH → Dz\nElse: nop" },
  code { "111110********** 110011110000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the contents of the MACH register in the Dz operand.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_mach_dcf (void)
{
  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = MACH;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G |= MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G |= MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
insn { "dcf psts	MACL,Dz",
  SH_DSP,
  abstract { "If DC = 0: MACL → Dz\nElse: nop" },
  code { "111110********** 110111110000zzzz" },

  issue { SH_DSP, "1" },
  latency { SH_DSP, "1" },



  description
{R"(
Conditionally stores the contents of the MACL register in the Dz operand.
The instruction is executed if the DC bit is set to 0.
The DC, N, Z, V, and GT bits of the DSR register are not updated.
)"},

  note
{R"(
Though PSTS, MOVX and MOVY can be designated in parallel, their execution may
take 2 cycles.
)"},

  operation
{R"(
void psts_macl_dcf (void)
{
  if (DC == 0)
  {
    DSP_REG[ex2_dz_no] = MACL;
    if (ex2_dz_no == 0)
    {
      A0G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A0G |= MASKFFFFFF00;
    }
    else if (ex2_dz_no == 1)
    {
      A1G = DSP_ALU_DSTG & MASK000000FF;
      if (DSP_ALU_DSTG_BIT7)
        A1G |= MASKFFFFFF00;
    }
  }
}
)"},

  example
{R"(

)"},

  exceptions
{R"(

)"},
},

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
});


  for(insns& block : insn_blocks)
  {
    for(insn& instruction : block)
    {
      auto& fmt = instruction.data<format>();
      auto& n = instruction.data<name>();
      auto& e = instruction.data<environments>();
      for(const instruction_info_t& info : name_data)
      {
        std::smatch match;
        if(std::regex_search(fmt, match, std::regex(instruction.data<mnemonic>(), std::regex_constants::extended)) &&
           (n.empty() || n == info.name) &&
           e == environments { info.environments })
        {
          instruction.data<citations>() = { info.citations };
        }

      }

    }
  }

  return insn_blocks;
}
