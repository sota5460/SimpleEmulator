#include "emu.h"

// EMULATOR emu;

void main(int argc, char *argv[]){

    printf("Emulator Start \n \n");

    // OPECODE ope[OPECODE_NUM];
    EMULATOR emu;
    emu = initialize_EMULATOR(&emu);

     printf("eip is %x at %d\n", emu.eip, __LINE__);

// READ BIN FILE to EMULATOR MEMORY

    FILE *bin;  

    // bin =fopen(argv[1], "rb");
    bin =fopen("./hoge", "rb");
    if(bin == NULL){
        printf("fopen failed");
    }; 

    // offset 0 from end of the file
    fseek(bin, 0, SEEK_END); // arg1: file discriptor, arg2:offset, arg3: offset start point

    const int bin_size = ftell(bin); // fseek() moved file position to the end and get file_size by ftell() to get file position.
    // const int bin_size = 4;

    printf("binary size is %d byte\n", bin_size);

    printf("eip is %x at %d\n", emu.eip, __LINE__);

    rewind(bin); //initizlize file position. 
    fseek(bin, 0, SEEK_SET);

    fread(emu.memory + INIT_EIP_ADDRESS, 1, bin_size, bin); //arg1: buff pointer, arg2: read byte size for each, arg3: read size, arg4: file 
    fclose(bin);

    printf("bin file read to memory done.\n");
    printf("memory[0]:%x\nmemory[1]:%x\nmemory[2]:%x\nmemory[3]:%x\n...\n",*emu.memory,*(emu.memory+1),*(emu.memory+2),*(emu.memory+3));
    // printf("memory[0]:%x\n",*emu.memory);

    printf("eip is %x at %d\n", emu.eip, __LINE__);


// EXECUTE EACH LINE OF MEMORY
    printf("binary read starts\n");

    for(int i = 0; i < 10; i++){
        const int opecode = readmem_next_uint8(&emu);
        // printf("eip is %x ,opecode [%d] = %x \n",*emu.eip, i, opecode);
        printf("now in for loop \n");
        // ope[opecode].func(&emu);
        // if(opecode==0xF4)break;
    }

}