#pragma once
/*****************************************************************************
 * File Name: RegPmcParIo.h
 * Description:  This file defines all the PmcParIo Control/Status Registers.
 *****************************************************************************/

 /*****************************************************************************
  * PCI Device/Vendor Ids
  *****************************************************************************/

#define PMC_PAR_IO_PCI_VENDOR_ID       0x10B5
#define PMC_PAR_IO_PCI_DEVICE_ID       0x9050


//PLX REGISTER OFFSETS FROM BAR0
#define PLX_LOC_ADDR_SPACE_0    0 //0x000
#define PLX_LOC_ADDR_SPACE_1    1 //0x004
#define PLX_LOC_ADDR_SPACE_2    2 //0x008
#define PLX_LOC_ADDR_SPACE_3    3 //0x00C
#define PLX_LOC_EXP_ROM         4 //0x010
#define PLX_LOC_ADDR_SPACE_BA_0 5 //0x014
#define PLX_LOC_ADDR_SPACE_BA_1 6 //0x018
#define PLX_LOC_ADDR_SPACE_BA_2 7 //0x01C
#define PLX_LOC_ADDR_SPACE_BA_3 8 //0x020
#define PLX_LOC_EXP_ROM_BA      9 //0x024
#define PLX_LOC_ADD_SPACE_BRD_0 10 //0x028
#define PLX_LOC_ADD_SPACE_BRD_1 11 //0x02C
#define PLX_LOC_ADD_SPACE_BRD_2 12 //0x030
#define PLX_LOC_ADD_SPACE_BRD_3 13 //0x034
#define PLX_LOC_EXP_ROM_RD      14 //0x038
#define PLX_CHIP_SEL_BASE_0     15 //0x03C
#define PLX_CHIP_SEL_BASE_1     16 //0x040
#define PLX_CHIP_SEL_BASE_2     17 //0x044
#define PLX_CHIP_SEL_BASE_3     18 //0x048
#define PLX_INT_CONT_STAT       19 //0x04C
#define PLX_CONTROL             20 //0x050

//PARIO REG OFFSETS FROM BAR2
#define PAR_IO_DATA_OUT_0   0 //0x000
#define PAR_IO_DATA_OUT_1   1 //0x004
#define PAR_IO_DATA_IN_0    4 //0x008
#define PAR_IO_DATA_IN_1    5 //0x00C
#define PAR_CLK_CNTRL       8 //0x020
#define PAR_INT_CONFIG_CNTL 9 //0x024

  /*****************************************************************************
   * Define the base control interface (CLK_CNTRL)
   *****************************************************************************/
typedef struct _CLK_CNTRL {
    unsigned int ClkSel : 1;  // bit 0
    unsigned int ClkEnSel : 1;  // bit 1
    unsigned int IntClkEn : 1;  // bit 2
    unsigned int spare1 : 1;  // bit 3
    unsigned int ClkDivisor : 3;  // bit 4
    unsigned int spare2 : 25; // bit 7-31
} CLK_CNTRL;

/*****************************************************************************
 * Define the base control interface (INT_REG)
 *****************************************************************************/
typedef struct _INT_REG {
    unsigned int PolarityHi0 : 1;  // bit 0
    unsigned int EdgeTrig0 : 1;  // bit 1
    unsigned int IntEn0 : 1;  // bit 2
    unsigned int spare1 : 1;  // bit 3
    unsigned int PolarityHi1 : 1;  // bit 4
    unsigned int EdgeTrig1 : 1;  // bit 5
    unsigned int IntEn1 : 1;  // bit 6
    unsigned int spare2 : 25; // bit 7-31
} INT_REG;

/*****************************************************************************
 * Define the base control interface (INT_PLX)
 *****************************************************************************/
typedef struct _INT_PLX {
    unsigned int LocIntEn1 : 1;  // bit 0
    unsigned int LocIntPol1 : 1;  // bit 1
    unsigned int LocIntStat1 : 1;  // bit 2
    unsigned int LocIntEn2 : 1;  // bit 3
    unsigned int LocIntPol2 : 1;  // bit 4
    unsigned int LocIntStat2 : 1;  // bit 5
    unsigned int PciIntEn : 1;  // bit 6
    unsigned int ForceInt : 1;  // bit 7
    unsigned int LocIntSelEn1 : 1;  // bit 8
    unsigned int LocIntSelEn2 : 1;  // bit 9
    unsigned int ClearBit1 : 1;  // bit 10
    unsigned int ClearBit2 : 1;  // bit 11
    unsigned int InterfaceModeEn : 1;  // bit 12
    unsigned int spare2 : 19; // bit 13-31
} INT_PLX;


/*****************************************************************************
 * Register union defines
 *****************************************************************************/
typedef union _CLK_CNTRL_REG {
    CLK_CNTRL   bits;
    ULONG       val32;
} CLK_CNTRL_REG;

typedef union _INT_REG_REG {
    INT_REG     bits;
    ULONG       val32;
} INT_REG_REG;

typedef union _INT_PLX_REG {
    INT_PLX     bits;
    ULONG       val32;
} INT_PLX_REG;

#pragma warning(default:4214)


/*****************************************************************************
 * PMC_PAR_IO_REGS structure
 *****************************************************************************/
typedef struct _PMC_PAR_IO_REGS {         // lngwrd num  Address offset
    unsigned int   ParIoDataOut0;  //     0        0x000
    unsigned int   ParIoDataOut1;  //     1        0x004
    unsigned int   pad1[2];  //     2-3      0x008 - 0x00C
    unsigned int   ParIoDataIn0;  //     4        0x010
    unsigned int   ParIoDataIn1;  //     5        0x014
    unsigned int   pad2[2];  //     6-7      0x018 - 0x01C
    CLK_CNTRL      ClkCntl;  //     8        0x020
    INT_REG        IntConfigCntl;  //     9        0x024
} PMC_PAR_IO_REGS, * PPMC_PAR_IO_REGS;

/*****************************************************************************
 * PCI_9052_REGS structure
 *****************************************************************************/
typedef struct _PCI_9052_REGS {        // lngwrd num  Address offset
    unsigned int   LocAddSpace0;  //     0        0x000
    unsigned int   LocAddSpace1;  //     1        0x004
    unsigned int   LocAddSpace2;  //     2        0x008
    unsigned int   LocAddSpace3;  //     3        0x00C
    unsigned int   LocExpROM;  //     4        0x010
    unsigned int   LocAddSpaceBA0;  //     5        0x014
    unsigned int   LocAddSpaceBA1;  //     6        0x018
    unsigned int   LocAddSpaceBA2;  //     7        0x01C
    unsigned int   LocAddSpaceBA3;  //     8        0x020
    unsigned int   LocExpROMBA;  //     9        0x024
    unsigned int   LocAddSpaceBRD0;  //     10       0x028
    unsigned int   LocAddSpaceBRD1;  //     11       0x02C
    unsigned int   LocAddSpaceBRD2;  //     12       0x030
    unsigned int   LocAddSpaceBRD3;  //     13       0x034
    unsigned int   LocExpROMBRD;  //     14       0x038
    unsigned int   ChipSelBase0;  //     15       0x03C
    unsigned int   ChipSelBase1;  //     16       0x040
    unsigned int   ChipSelBase2;  //     17       0x044
    unsigned int   ChipSelBase3;  //     18       0x048
    INT_PLX        IntContStat;    //    19       0x4C
    unsigned int   Control;  //     20       0x050
} PCI_9052_REGS, * PPCI_9052_REGS;