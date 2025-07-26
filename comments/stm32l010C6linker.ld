
ENTRY(_reset)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 32K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
    EEPROM (rwx) : ORIGIN = 0x08080000, LENGTH = 256
    /*  System memory 0x1FF0 0000, 4K */
    /* Factory Options 0x1FF8 0020, 96 */
    /* User Option bytes 0x1FF8 0000, 32 */

    /* Beginning of FLASH - interrupt vectors */

    /*
    NVM
    each 8 bits has an address
    32 bit memory cells words (little endian)
    erase - page-wise
    unlock - sector-wise

    FLASH:
    page = 32 word
    sector = 32 pages

    EEPROM:
    page = word
    sector = whole ?


    */
}

SECTIONS
{
    /* 
    Merges .text and .rodata sections into .mytext section
    I guess the order of sections in SECTIONS and the order of things in each section 
    corresponds to the order in memory,
    there is no space between sections and things in the sections 
    and sections start at the beginning
    
    > vma AT> lma
    What are the absolute addresses in the linker script?
    You take the sections from the object files and put them in the corresponding regions
    */
    .my_text:
    {

        /*  Take all .text sections from the input .o files and merge them */
        *(.text)
        *(.rodata)
    } > FLASH

    .my_data:
    {
        *(.data)
    } > SRAM AT > FLASH
}

/*

Location counter

Linker symbols
Added to the symbol table by the linker

abc = .
tracks vma, not lma

efg = 0x20

*/