#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define INIT_EIP_ADDRESS 0
#define OPECODE_NUM 10  // the number of opecode
#define EXIT_MSG(msg) printf("%s\n",msg)
#define MEMORY_SIZE_BYTE 100




typedef struct{
    uint32_t *eax;
    uint32_t *ecx;
    uint32_t *edx;
    uint32_t *ebx;
    uint32_t *esp;
    uint32_t *ebp;
    uint32_t *esi;
    uint32_t *edi;
    uint32_t *eip;
    uint32_t *eflags;
    uint8_t *memory;
} EMULATOR;


EMULATOR initialize_EMULATOR(EMULATOR *emu){
    // EMULATOR *emu;

    emu = (EMULATOR*)malloc(sizeof(EMULATOR));

    emu->eax = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->ecx = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->edx = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->ebx = (uint32_t*)calloc(1,sizeof(uint32_t));

    
    emu->ebp = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->esi = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->edi = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->eip = (uint32_t*)calloc(1,sizeof(uint32_t));
    emu->eflags = (uint32_t*)calloc(1,sizeof(uint32_t));

    emu->memory = (uint8_t*)calloc(MEMORY_SIZE_BYTE,sizeof(uint8_t));

    emu->esp = (uint32_t*)emu->memory + MEMORY_SIZE_BYTE;

    printf("emulator struct initialized.\n");
    printf(\
    "eax: (addr,value)=(%x,%x)\n\
    eip: (addr,value)=(%x,%x)\n\
    memory addr: %x, memory[0]: %x, memory[1]: %x \n",\
     (unsigned int)emu->eax, *emu->eax,\
     (unsigned int)emu->eip, *emu->eip,\
     (unsigned int)emu->memory, *(emu->memory + 1),*(emu->memory + 2));

// printf(\
//     "eax: (addr,value)=(%x,%x)\n\
//     memory addr: %x, memory[0]: %x, memory[1]: %x \n",\
//      (unsigned int)emu->eax, *emu->eax,\
//      (unsigned int)emu->memory, *(emu->memory + 1),*(emu->memory + 2));

    return *emu;

}

typedef struct {
    char *mnemonic;
    void (*func)(EMULATOR *emu);
} OPECODE;




static void not_defined_halt(EMULATOR *emu);
static void add_Mimm_R(EMULATOR *emu);
// static void or_Mimm_R(EMULATOR *emu);


void initialize_OPECODE(OPECODE ope[OPECODE_NUM])
{
    for (int i=0; i<OPECODE_NUM; i++){
        ope[i].mnemonic = "NOT_DEFINED";
        ope[i].func = not_defined_halt;
    }

    ope[0x01].mnemonic = "add [M+imm], R";
    ope[0x01].func = add_Mimm_R;

    ope[0x09].mnemonic = "or [M+imm], R";
    // ope[0x09].func = or_Mimm_R;
}

typedef struct {
    uint8_t modrm;
    uint8_t mod;
    uint8_t raw_r;
    uint8_t raw_m;
    uint32_t *R;
    uint32_t *M;
} MODRM;


//proto type declaration 
MODRM read_modrm(EMULATOR *emu);
static uint32_t modrm_M_imm_to_addr(MODRM m, EMULATOR *emu);
static uint8_t readmem_next_uint8(EMULATOR *emu);
static uint32_t readmem_next_uint32(EMULATOR *emu);
static uint32_t readmem_uint32(uint32_t addr,EMULATOR *emu);
// void update_flag(uint32_t src, uint32_t );
void writemem_uint32(uint32_t src_addr, uint32_t write_value, EMULATOR *emu);


static void add_Mimm_R(EMULATOR *emu){
    MODRM m = read_modrm(emu);

    if (m.mod < 3){
        const uint32_t mem = modrm_M_imm_to_addr(m,emu);
        const uint32_t src = readmem_uint32(mem, emu); // Get x (add x y) 
        // update_flag(src, *m.R, emu);
        writemem_uint32(mem, src+*m.R, emu);
    }

}

MODRM read_modrm(EMULATOR *emu)
{
    MODRM m;

    m.modrm = *(emu->memory + *(emu->eip));
    m.mod   = (m.mod & 0b11000000)>>6;
    m.raw_r = (m.mod & 0b00111000)>>3;
    m.raw_m = m.mod & 0b00000111;

    switch (m.raw_r)
    {
    case 0b00000000: m.R = emu->eax;  break;
    case 0b00000001: m.R = emu->ecx;  break;
    case 0b00000010: m.R = emu->edx;  break;
    case 0b00000011: m.R = emu->ebx;  break;
    case 0b00000100: m.R = emu->esp;  break;
    case 0b00000101: m.R = emu->ebp;  break;
    case 0b00000110: m.R = emu->esi;  break;
    case 0b00000111: m.R = emu->esi;  break;
    default:
        break;
    }

    switch (m.raw_m)
    {
    case 0b00000000: m.M = emu->eax;  break;
    case 0b00000001: m.M = emu->ecx;  break;
    case 0b00000010: m.M = emu->edx;  break;
    case 0b00000011: m.M = emu->ebx;  break;
    case 0b00000100: m.M = emu->esp;  break;
    case 0b00000101: m.M = emu->ebp;  break;
    case 0b00000110: m.M = emu->esi;  break;
    case 0b00000111: m.M = emu->esi;  break;
    default:
        break;
    }

    return m;
}

static void not_defined_halt(EMULATOR *emu){
    printf("Not defined for this opecode \n");
};

static uint32_t modrm_M_imm_to_addr(const MODRM m, EMULATOR *emu){

    uint32_t mem;
    if (m.mod == 0) mem = *m.M;
    else if (m.mod == 1) mem = *m.M + (uint8_t)readmem_next_uint8(emu); // inside resistor data + 8 bit imm data(program pointor below) 
    else if (m.mod == 2) mem = *m.M +         readmem_next_uint32(emu);
    else EXIT_MSG("ModRM Error. ModRM's M must be address. Therefor mod should not be 3\n");
    return mem;
}




static uint8_t readmem_next_uint8(EMULATOR *emu){
    uint8_t* opecode_immcode;
//    *(emu->eip) ++; // program pointer + 1
//    opecode_immcode = emu->memory + *(emu->eip);

// printf("eip is %lx \n",emu->eip);
printf("eip2 is %x \n",(unsigned int)emu->eip);



   return *opecode_immcode;
}

static uint32_t readmem_next_uint32(EMULATOR *emu){
    uint32_t* imm32;
    imm32 = (uint32_t*)(emu->memory+*(emu->eip)+1);
    *(emu->eip)=*(emu->eip)+4;
    return *imm32 ;
}

static uint32_t readmem_uint32(uint32_t addr,EMULATOR *emu){
    uint32_t memory_32data;
    memory_32data = *(emu->memory + addr);
    // memory_32data = *(addr);
    return memory_32data;
}

void writemem_uint32(uint32_t src_addr, uint32_t write_value, EMULATOR *emu){
    uint32_t *target_addr = (uint32_t*)(emu->memory + src_addr);
    *target_addr = write_value;
    return ;
}

typedef struct {
    uint32_t carry;
    uint32_t overflow;
    uint32_t sign;
    uint32_t zero;
} FLAG;
