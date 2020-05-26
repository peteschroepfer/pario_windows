/*++
Copyright (c) 2015 Dynamic Engineering All rights reserved.
   150 DuBois, Suite C
   Santa Cruz, CA 95060
   831-457-8891
   engineering@dyneng.com
   http://www.dyneng.com

Module Name:
   PmcParIoPublic.h

Abstract:
   This module contains the common declarations shared by driver
   and user applications.

Environment:
   user and kernel
--*/

// {F256BB51-0B89-4d17-AFBD-683413789D47}
DEFINE_GUID(GUID_PMC_PAR_IO_INTERFACE,
    0xf256bb51, 0xb89, 0x4d17, 0xaf, 0xbd, 0x68, 0x34, 0x13, 0x78, 0x9d, 0x47);

#define PMC_PAR_IO_DEVICE_TYPE   0x8081
#define PMC_PAR_IO_MAKE_IOCTL(t,c)\
        (ULONG)CTL_CODE((t), 0x800+(c), METHOD_BUFFERED, FILE_ANY_ACCESS)

// Returns the current driver version and instance number
// Input:  None
// Output: Current Version and instance number(PMC_PAR_IO_DRIVER_DEVICE_INFO structure)
#define IOCTL_PMC_PAR_IO_GET_INFO\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 0)

 // Writes value(s) to the output data registers
 // Input:  Output data register values(PMC_PAR_IO_DATA structure)
 // Output: None
#define IOCTL_PMC_PAR_IO_SET_OUT_DATA\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 1)

 // Reads the values from the output data registers
 // Input:  None
 // Output: Output data register values(PMC_PAR_IO_DATA structure)
#define IOCTL_PMC_PAR_IO_GET_OUT_DATA\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 2)

 // Reads the values from the input data bus
 // Input:  None
 // Output: Input data bus values(PMC_PAR_IO_DATA structure)
#define IOCTL_PMC_PAR_IO_READ_IN_DATA\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 3)

 // Writes a value to the clock configuration register
 // Input:  Clock configuration register value(UCHAR)
 // Output: None
#define IOCTL_PMC_PAR_IO_SET_CLOCK_CONFIG\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 4)

 // Reads the value in the clock configuration register
 // Input:  None
 // Output: Clock configuration register value(UCHAR)
#define IOCTL_PMC_PAR_IO_GET_CLOCK_CONFIG\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 5)

 // Writes a value to the interrupt configuration register
 // Input:  Interrupt configuration register value(UCHAR)
 // Output: None
#define IOCTL_PMC_PAR_IO_SET_INT_CONFIG\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 6)

 // Reads the value in the interrupt configuration register
 // Input:  None
 // Output: Interrupt configuration register value(UCHAR)
#define IOCTL_PMC_PAR_IO_GET_INT_CONFIG\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 7)

 // Reads the value of the interrupt control & status register in the Plx 9052
 // Input:  None
 // Output: Interrupt control/status register value(ULONG)
#define IOCTL_PMC_PAR_IO_GET_INT_STATUS\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 8)

 // Registers an Event object to be signalled when an interrupt occurs
 // Input:  Handle to Event object
 // Output: None
#define IOCTL_PMC_PAR_IO_REGISTER_EVENT\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 9)

 // Enables the master interrupt
 // Input:  None
 // Output: None
#define IOCTL_PMC_PAR_IO_ENABLE_INTERRUPT\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 10)

 // Disables the master interrupt
 // Input:  None
 // Output: None
#define IOCTL_PMC_PAR_IO_DISABLE_INTERRUPT\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 11)

 // Cause an interrupt
 // Input:  None
 // Output: None
#define IOCTL_PMC_PAR_IO_FORCE_INTERRUPT\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 12)

 // Returns the interrupt status read in the last ISR
 // Input:  None
 // Output: Interrupt control/status register value(ULONG)
#define IOCTL_PMC_PAR_IO_GET_ISR_STATUS\
            PMC_PAR_IO_MAKE_IOCTL(PMC_PAR_IO_DEVICE_TYPE, 13)

 // PMC_PAR_IO control registers bit defines
#define CLOCK_SELECT_EXT            0x00000001  // Input data register clock select(1=ext, 0=int)
#define CLKEN_SELECT_EXT            0x00000002  // Input data register clock enable select(1=ext, 0=int)
#define CLKEN_INTRN_ON              0x00000004  // Internal clock enable set(1=enabled, 0=disabled)
#define CLOCK_INTRN_DIV_MASK        0x00000070  // Divisor mask for internal clock(f=33MHz/(2**(n+2)))
#define CLOCK_INTRN_DIV_SHIFT       4           // Shift for internal clock divisor select
#define CLOCK_CONFIG_MASK          (CLOCK_SELECT_EXT | CLKEN_SELECT_EXT | CLKEN_INTRN_ON | CLOCK_INTRN_DIV_MASK)

#define INT0_ACT_HIGH               0x00000001  // Interrupt 0 polarity(1=active high, 0=active low)
#define INT0_EDGE_TRIG              0x00000002  // Interrupt 0 type(1=edge, 0=level)
#define INT0_ENABLED                0x00000004  // Interrupt 0 enable(1=enabled, 0=disabled)
#define INT1_ACT_HIGH               0x00000010  // Interrupt 1 polarity(1=active high, 0=active low)
#define INT1_EDGE_TRIG              0x00000020  // Interrupt 1 type(1=edge, 0=level)
#define INT1_ENABLED                0x00000040  // Interrupt 1 enable(1=enabled, 0=disabled)
#define INT_CONFIG_MASK            (INT0_ACT_HIGH | INT0_EDGE_TRIG | INT0_ENABLED | \
                                    INT1_ACT_HIGH | INT1_EDGE_TRIG | INT1_ENABLED)

 // Retry delay
#define PLX_RETRY_DLY_CNT        4  // Multiply by 8 -> 32 local clocks

 // PLX register bit defines
#define PLX_LOC_ADDR_EN             0x00000001  // Enable decoding of PCI adrresses for local access

#define PLX_LOC_RDY_EN              0x00000002  // Ready input enabled
#define PLX_LOC_PF_CNTEN            0x00000020  // Pre-fetch count enabled
#define PLX_LOC_MODE_32             0x00800000  // 32-bit local bus width
#define PLX_LOC_RA_WT_MSK           0x000007C0  // Read wait-states
#define PLX_LOC_RD_WT_MSK           0x00001800  // Burst read delay
#define PLX_LOC_XA_WT_MSK           0x00006000  // Delay between slave accesses
#define PLX_LOC_WA_WT_MSK           0x000F8000  // Write wait-states
#define PLX_LOC_WD_WT_MSK           0x00300000  // Burst write delay
#define PLX_LOC_RA_WT_SHFT          6
#define PLX_LOC_RD_WT_SHFT          11
#define PLX_LOC_XA_WT_SHFT          13
#define PLX_LOC_WA_WT_SHFT          15
#define PLX_LOC_WD_WT_SHFT          20

#define PLX_CS_ENABLE               0x00000001  // Chip select enabled
#define PLX_CS_SIZE_256             0x00000200  // Size of local bus region

#define PLX_ICS_LOC_INT1EN          0x00000001  // Local interrupt 1 enabled
#define PLX_ICS_LOC_INT1HI          0x00000002  // Local interrupt 1 active high
#define PLX_ICS_LOC_INT1ACTV        0x00000004  // Local interrupt 1 is active
#define PLX_ICS_LOC_INT2EN          0x00000008  // Local interrupt 2 enabled
#define PLX_ICS_LOC_INT2HI          0x00000010  // Local interrupt 2 active high
#define PLX_ICS_LOC_INT2ACTV        0x00000020  // Local interrupt 2 is active
#define PLX_ICS_PCI_INTEN           0x00000040  // PCI interrupt enabled
#define PLX_ICS_FORCE_INT           0x00000080  // Causes an interrupt to occur
#define PLX_ICS_INT_STATUS         (PLX_ICS_LOC_INT1ACTV | PLX_ICS_LOC_INT2ACTV | PLX_ICS_FORCE_INT)

#define PLX_CNTRL_USER0_DATA        0x00000004  // User 0 output data when configured as output
#define PLX_CNTRL_USER1_DATA        0x00000020  // User 1 output data when configured as output
#define PLX_CNTRL_USER2_DATA        0x00000100  // User 2 output data when configured as output
#define PLX_CNTRL_USER3_DATA        0x00000800  // User 3 output data when configured as output
#define PLX_CNTRL_RTRYDL_MASK       0x00780000  // Mask of PCI target retry delay count (8 clocks * value)
#define PLX_CNTRL_RTRYDL_SHFT       19          // Shift for PCI target retry delay count

 // Structures for IOCTLs
 //  Driver version and instance information
typedef struct _PMC_PAR_IO_DRIVER_DEVICE_INFO
{
    ULONG    DriverVersion;
    ULONG    InstanceNumber;
} PMC_PAR_IO_DRIVER_DEVICE_INFO, * PPMC_PAR_IO_DRIVER_DEVICE_INFO;

// Output and Input data values
typedef struct _PMC_PAR_IO_DATA
{
    ULONG    LoWord;     // Bits  0..31
    ULONG    HiWord;     // Bits 32..63
} PMC_PAR_IO_DATA, * PPMC_PAR_IO_DATA;

// Clock configuration
typedef struct _PMC_PAR_IO_CLOCK_CONFIG
{
    UCHAR    FreqSelect;       // f=33MHz:0=f/4,1=f/8,2=f/16,3=f/32,4=f/64,5=f/128,6=f/256,7=f/512
    BOOLEAN  ExtClockSelect;   // Input data register clock select(TRUE=ext, FALSE=int)
    BOOLEAN  ExtEnableSelect;  // Input data register clock enable select(TRUE=ext, FALSE=int)
    BOOLEAN  IntEnableOn;      // Internal clock enable set(TRUE=enabled, FALSE=disabled)
} PMC_PAR_IO_CLOCK_CONFIG, * PPMC_PAR_IO_CLOCK_CONFIG;

// Interrupt configuration
typedef struct _PMC_PAR_IO_INT_CONFIG
{
    BOOLEAN  Int0Enabled;      // Interrupt 0 enable(TRUE=enabled, FALSE=disabled)
    BOOLEAN  Int0Edge_Level;   // Interrupt 0 type(TRUE=edge, FALSE=level)
    BOOLEAN  Int0PolHi_Lo;     // Interrupt 0 polarity(TRUE=active high, FALSE=active low)
    BOOLEAN  Int1Enabled;      // Interrupt 1 enable(TRUE=enabled, FALSE=disabled)
    BOOLEAN  Int1Edge_Level;   // Interrupt 1 type(TRUE=edge, FALSE=level)
    BOOLEAN  Int1PolHi_Lo;     // Interrupt 1 polarity(TRUE=active high, FALSE=active low)
} PMC_PAR_IO_INT_CONFIG, * PPMC_PAR_IO_INT_CONFIG;

