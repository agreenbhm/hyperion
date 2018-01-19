/* DASDBLKS.H   (C) Copyright Roger Bowler, 1999-2012                */
/*              DASD control block structures                        */
/*                                                                   */
/*   Released under "The Q Public License Version 1"                 */
/*   (http://www.hercules-390.org/herclic.html) as modifications to  */
/*   Hercules.                                                       */

/*-------------------------------------------------------------------*/
/* This header file contains definitions of OS Data Management       */
/* control block structures for use by the Hercules DASD utilities.  */
/* It also contains function prototypes for the DASD utilities.      */
/*-------------------------------------------------------------------*/


#include "hercules.h"

#ifndef _DASDUTIL_C_
#ifndef _HDASD_DLL_
#define DUT_DLL_IMPORT DLL_IMPORT
#else   /* _HDASD_DLL_ */
#define DUT_DLL_IMPORT extern
#endif  /* _HDASD_DLL_ */
#else
#define DUT_DLL_IMPORT DLL_EXPORT
#endif
//  Forward references...

typedef  struct  FORMAT1_DSCB   FORMAT1_DSCB;   // DSCB1: Dataset descriptor
typedef  struct  FORMAT3_DSCB   FORMAT3_DSCB;   // DSCB3: Additional extents
typedef  struct  FORMAT4_DSCB   FORMAT4_DSCB;   // DSCB4: VTOC descriptor
typedef  struct  FORMAT5_DSCB   FORMAT5_DSCB;   // DSCB5: Free space map

typedef  struct  F5AVEXT        F5AVEXT;        // Available extent in DSCB5
typedef  struct  DSXTENT        DSXTENT;        // Dataset extent descriptor

typedef  struct  PDSDIR         PDSDIR;         // PDS directory entry
typedef  struct  CIFBLK         CIFBLK;         // CKD image file descriptor

typedef  struct  COPYR1         COPYR1;         // IEBCOPY header record 1
typedef  struct  COPYR2         COPYR2;         // IEBCOPY header record 2
typedef  struct  DATABLK        DATABLK;        // IEBCOPY unload data rec

#define  MAX_TRACKS   32767

/*-------------------------------------------------------------------*/
/* Definition of DSCB records in VTOC                                */
/*-------------------------------------------------------------------*/
struct DSXTENT {                        /* Dataset extent descriptor */
        BYTE    xttype;                 /* Extent type               */
        BYTE    xtseqn;                 /* Extent sequence number    */
        HWORD   xtbcyl;                 /* Extent begin cylinder     */
        HWORD   xtbtrk;                 /* Extent begin track        */
        HWORD   xtecyl;                 /* Extent end cylinder       */
        HWORD   xtetrk;                 /* Extent end track          */
};

/* Bit definitions for extent type */
#define XTTYPE_UNUSED           0x00    /* Unused extent descriptor  */
#define XTTYPE_DATA             0x01    /* Data extent               */
#define XTTYPE_OVERFLOW         0x02    /* Overflow extent           */
#define XTTYPE_INDEX            0x04    /* Index extent              */
#define XTTYPE_USERLBL          0x40    /* User label extent         */
#define XTTYPE_SHARCYL          0x80    /* Shared cylinders          */
#define XTTYPE_CYLBOUND         0x81    /* Extent on cyl boundary    */

struct FORMAT1_DSCB {                   /* DSCB1: Dataset descriptor */
        BYTE    ds1dsnam[44];           /* Key (44 byte dataset name)*/
        BYTE    ds1fmtid;               /* Format identifier (0xF1)  */
        BYTE    ds1dssn[6];             /* Volume serial number      */
        HWORD   ds1volsq;               /* Volume sequence number    */
        BYTE    ds1credt[3];            /* Dataset creation date...
                                           ...byte 0: Binary year-1900
                                           ...bytes 1-2: Binary day  */
        BYTE    ds1expdt[3];            /* Dataset expiry date       */
        BYTE    ds1noepv;               /* Number of extents         */
        BYTE    ds1bodbd;               /* #bytes used in last dirblk*/
        BYTE    resv1;                  /* Reserved                  */
        BYTE    ds1syscd[13];           /* System code (IBMOSVS2)    */
        BYTE    resv2[7];               /* Reserved                  */
        BYTE    ds1dsorg[2];            /* Dataset organization      */
        BYTE    ds1recfm;               /* Record format             */
        BYTE    ds1optcd;               /* Option codes              */
        HWORD   ds1blkl;                /* Block length              */
        HWORD   ds1lrecl;               /* Logical record length     */
        BYTE    ds1keyl;                /* Key length                */
        HWORD   ds1rkp;                 /* Relative key position     */
        BYTE    ds1dsind;               /* Dataset indicators        */
        FWORD   ds1scalo;               /* Secondary allocation...
                                           ...byte 0: Allocation units
                                           ...bytes 1-3: Quantity    */
        BYTE    ds1lstar[3];            /* Last used TTR             */
        HWORD   ds1trbal;               /* Bytes unused on last trk  */
        BYTE    resv3[2];               /* Reserved                  */
        DSXTENT ds1ext1;                /* First extent descriptor   */
        DSXTENT ds1ext2;                /* Second extent descriptor  */
        DSXTENT ds1ext3;                /* Third extent descriptor   */
        BYTE    ds1ptrds[5];            /* CCHHR of F2 or F3 DSCB    */
};

/* Bit definitions for ds1dsind */
#define DS1DSIND_LASTVOL        0x80    /* Last volume of dataset    */
#define DS1DSIND_RACFIND        0x40    /* RACF indicated            */
#define DS1DSIND_BLKSIZ8        0x20    /* Blocksize multiple of 8   */
#define DS1DSIND_PASSWD         0x10    /* Password protected        */
#define DS1DSIND_WRTPROT        0x04    /* Write protected           */
#define DS1DSIND_UPDATED        0x02    /* Updated since last backup */
#define DS1DSIND_SECCKPT        0x01    /* Secure checkpoint dataset */

/* Bit definitions for ds1optcd */
#define DS1OPTCD_ICFDSET        0x80    /* Dataset in ICF catalog    */
#define DS1OPTCD_ICFCTLG        0x40    /* ICF catalog               */

/* Bit definitions for ds1scalo byte 0 */
#define DS1SCALO_UNITS          0xC0    /* Allocation units...       */
#define DS1SCALO_UNITS_ABSTR    0x00    /* ...absolute tracks        */
#define DS1SCALO_UNITS_BLK      0x40    /* ...blocks                 */
#define DS1SCALO_UNITS_TRK      0x80    /* ...tracks                 */
#define DS1SCALO_UNITS_CYL      0xC0    /* ...cylinders              */
#define DS1SCALO_CONTIG         0x08    /* Contiguous space          */
#define DS1SCALO_MXIG           0x04    /* Maximum contiguous extent */
#define DS1SCALO_ALX            0x02    /* Up to 5 largest extents   */
#define DS1SCALO_ROUND          0x01    /* Round to cylinders        */

struct FORMAT3_DSCB {                   /* DSCB3: Additional extents */
        BYTE    ds3keyid[4];            /* Key (4 bytes of 0x03)     */
        DSXTENT ds3extnt[4];            /* Four extent descriptors   */
        BYTE    ds3fmtid;               /* Format identifier (0xF3)  */
        DSXTENT ds3adext[9];            /* Nine extent descriptors   */
        BYTE    ds3ptrds[5];            /* CCHHR of next F3 DSCB     */
};

struct FORMAT4_DSCB {                   /* DSCB4: VTOC descriptor    */
        BYTE    ds4keyid[44];           /* Key (44 bytes of 0x04)    */
        BYTE    ds4fmtid;               /* Format identifier (0xF4)  */
        BYTE    ds4hpchr[5];            /* CCHHR of highest F1 DSCB  */
        HWORD   ds4dsrec;               /* Number of format 0 DSCBs  */
        BYTE    ds4hcchh[4];            /* CCHH of next avail alt trk*/
        HWORD   ds4noatk;               /* Number of avail alt tracks*/
        BYTE    ds4vtoci;               /* VTOC indicators           */
        BYTE    ds4noext;               /* Number of extents in VTOC */
        BYTE    resv1[2];               /* Reserved                  */
        FWORD   ds4devsz;               /* Device size (CCHH)        */
        HWORD   ds4devtk;               /* Device track length       */
        BYTE    ds4devi;                /* Non-last keyed blk overhd */
        BYTE    ds4devl;                /* Last keyed block overhead */
        BYTE    ds4devk;                /* Non-keyed block difference*/
        BYTE    ds4devfg;               /* Device flags              */
        HWORD   ds4devtl;               /* Device tolerance          */
        BYTE    ds4devdt;               /* Number of DSCBs per track */
        BYTE    ds4devdb;               /* Number of dirblks/track   */
        DBLWRD  ds4amtim;               /* VSAM timestamp            */
        BYTE    ds4vsind;               /* VSAM indicators           */
        HWORD   ds4vscra;               /* CRA track location        */
        DBLWRD  ds4r2tim;               /* VSAM vol/cat timestamp    */
        BYTE    resv2[5];               /* Reserved                  */
        BYTE    ds4f6ptr[5];            /* CCHHR of first F6 DSCB    */
        DSXTENT ds4vtoce;               /* VTOC extent descriptor    */
        BYTE    resv3[25];              /* Reserved                  */
};

/* Bit definitions for ds4vtoci */
#define DS4VTOCI_DOS            0x80    /* Format 5 DSCBs not valid  */
#define DS4VTOCI_DOSSTCK        0x10    /* DOS stacked pack          */
#define DS4VTOCI_DOSCNVT        0x08    /* DOS converted pack        */
#define DS4VTOCI_DIRF           0x40    /* VTOC contains errors      */
#define DS4VTOCI_DIRFCVT        0x20    /* DIRF reclaimed            */

/* Bit definitions for ds4devfg */
#define DS4DEVFG_TOL            0x01    /* Tolerance factor applies to
                                           all but last block of trk */

struct F5AVEXT {                       /* Available extent in DSCB5 */
        HWORD   btrk;                   /* Extent begin track address*/
        HWORD   ncyl;                   /* Number of full cylinders  */
        BYTE    ntrk;                   /* Number of odd tracks      */
};

struct FORMAT5_DSCB {                   /* DSCB5: Free space map     */
        BYTE    ds5keyid[4];            /* Key (4 bytes of 0x05)     */
        F5AVEXT ds5avext[8];            /* First 8 available extents */
        BYTE    ds5fmtid;               /* Format identifier (0xF5)  */
        F5AVEXT ds5mavet[18];           /* 18 more available extents */
        BYTE    ds5ptrds[5];            /* CCHHR of next F5 DSCB     */
};

/*-------------------------------------------------------------------*/
/* Definitions of DSORG and RECFM fields                             */
/*-------------------------------------------------------------------*/
/* Bit settings for dataset organization byte 0 */
#define DSORG_IS                0x80    /* Indexed sequential        */
#define DSORG_PS                0x40    /* Physically sequential     */
#define DSORG_DA                0x20    /* Direct access             */
#define DSORG_PO                0x02    /* Partitioned organization  */
#define DSORG_U                 0x01    /* Unmovable                 */

/* Bit settings for dataset organization byte 1 */
#define DSORG_AM                0x08    /* VSAM dataset              */

/* Bit settings for record format */
#define RECFM_FORMAT            0xC0    /* Bits 0-1=Record format    */
#define RECFM_FORMAT_V          0x40    /* ...variable length        */
#define RECFM_FORMAT_F          0x80    /* ...fixed length           */
#define RECFM_FORMAT_U          0xC0    /* ...undefined length       */
#define RECFM_TRKOFLOW          0x20    /* Bit 2=Track overflow      */
#define RECFM_BLOCKED           0x10    /* Bit 3=Blocked             */
#define RECFM_SPANNED           0x08    /* Bit 4=Spanned or standard */
#define RECFM_CTLCHAR           0x06    /* Bits 5-6=Carriage control */
#define RECFM_CTLCHAR_A         0x04    /* ...ANSI carriage control  */
#define RECFM_CTLCHAR_M         0x02    /* ...Machine carriage ctl.  */

/*-------------------------------------------------------------------*/
/* Definition of PDS directory entry                                 */
/*-------------------------------------------------------------------*/
struct PDSDIR {                         /* PDS directory entry       */
        BYTE    pds2name[8];            /* Member name               */
        BYTE    pds2ttrp[3];            /* TTR of first block        */
        BYTE    pds2indc;               /* Indicator byte            */
        BYTE    pds2usrd[62];           /* User data (0-31 halfwords)*/
};

/* Bit definitions for PDS directory indicator byte */
#define PDS2INDC_ALIAS          0x80    /* Bit 0: Name is an alias   */
#define PDS2INDC_NTTR           0x60    /* Bits 1-2: User TTR count  */
#define PDS2INDC_NTTR_SHIFT     5       /* Shift count for NTTR      */
#define PDS2INDC_LUSR           0x1F    /* Bits 3-7: User halfwords  */

/*-------------------------------------------------------------------*/
/* Text unit keys for transmit/receive                               */
/*-------------------------------------------------------------------*/
#define INMDDNAM        0x0001          /* DDNAME for the file       */
#define INMDSNAM        0x0002          /* Name of the file          */
#define INMMEMBR        0x0003          /* Member name list          */
#define INMSECND        0x000B          /* Secondary space quantity  */
#define INMDIR          0x000C          /* Directory space quantity  */
#define INMEXPDT        0x0022          /* Expiration date           */
#define INMTERM         0x0028          /* Data transmitted as msg   */
#define INMBLKSZ        0x0030          /* Block size                */
#define INMDSORG        0x003C          /* File organization         */
#define INMLRECL        0x0042          /* Logical record length     */
#define INMRECFM        0x0049          /* Record format             */
#define INMTNODE        0x1001          /* Target node name/number   */
#define INMTUID         0x1002          /* Target user ID            */
#define INMFNODE        0x1011          /* Origin node name/number   */
#define INMFUID         0x1012          /* Origin user ID            */
#define INMLREF         0x1020          /* Date last referenced      */
#define INMLCHG         0x1021          /* Date last changed         */
#define INMCREAT        0x1022          /* Creation date             */
#define INMFVERS        0x1023          /* Origin vers# of data fmt  */
#define INMFTIME        0x1024          /* Origin timestamp          */
#define INMTTIME        0x1025          /* Destination timestamp     */
#define INMFACK         0x1026          /* Originator request notify */
#define INMERRCD        0x1027          /* RECEIVE command error code*/
#define INMUTILN        0x1028          /* Name of utility program   */
#define INMUSERP        0x1029          /* User parameter string     */
#define INMRECCT        0x102A          /* Transmitted record count  */
#define INMSIZE         0x102C          /* File size in bytes        */
#define INMFFM          0x102D          /* Filemode number           */
#define INMNUMF         0x102F          /* #of files transmitted     */
#define INMTYPE         0x8012          /* Dataset type              */

/*-------------------------------------------------------------------*/
/* Definitions of IEBCOPY header records                             */
/*-------------------------------------------------------------------*/
struct COPYR1 {                         /* IEBCOPY header record 1   */
        BYTE    uldfmt;                 /* Unload format             */
        BYTE    hdrid[3];               /* Header identifier         */
        HWORD   ds1dsorg;               /* Dataset organization      */
        HWORD   ds1blkl;                /* Block size                */
        HWORD   ds1lrecl;               /* Logical record length     */
        BYTE    ds1recfm;               /* Record format             */
        BYTE    ds1keyl;                /* Key length                */
        BYTE    ds1optcd;               /* Option codes              */
        BYTE    ds1smsfg;               /* SMS indicators            */
        HWORD   uldblksz;               /* Block size of container   */
                                        /* Start of DEVTYPE fields   */
        FWORD   ucbtype;                /* Original device type      */
        FWORD   maxblksz;               /* Maximum block size        */
        HWORD   cyls;                   /* Number of cylinders       */
        HWORD   heads;                  /* Number of tracks/cylinder */
        HWORD   tracklen;               /* Track length              */
        HWORD   overhead;               /* Block overhead            */
        BYTE    keyovhead;              /* Keyed block overhead      */
        BYTE    devflags;               /* Flags                     */
        HWORD   tolerance;              /* Tolerance factor          */
                                        /* End of DEVTYPE fields     */
        HWORD   hdrcount;               /* Number of header records
                                           (if zero, then 2 headers) */
        BYTE    resv1;                  /* Reserved                  */
        BYTE    ds1refd[3];             /* Last reference date       */
        BYTE    ds1scext[3];            /* Secondary space extension */
        BYTE    ds1scalo[4];            /* Secondary allocation      */
        BYTE    ds1lstar[3];            /* Last track used TTR       */
        HWORD   ds1trbal;               /* Last track balance        */
        HWORD   resv2;                  /* Reserved                  */
};

/* Bit settings for unload format byte */
#define COPYR1_ULD_FORMAT       0xC0    /* Bits 0-1=unload format... */
#define COPYR1_ULD_FORMAT_OLD   0x00    /* ...old format             */
#define COPYR1_ULD_FORMAT_PDSE  0x40    /* ...PDSE format            */
#define COPYR1_ULD_FORMAT_ERROR 0x80    /* ...error during unload    */
#define COPYR1_ULD_FORMAT_XFER  0xC0    /* ...transfer format        */
#define COPYR1_ULD_PROGRAM      0x10    /* Bit 3=Contains programs   */
#define COPYR1_ULD_PDSE         0x01    /* Bit 7=Contains PDSE       */

/* Bit settings for header identifier */
#define COPYR1_HDRID    "\xCA\x6D\x0F"  /* Constant value for hdrid  */

struct COPYR2 {                         /* IEBCOPY header record 2   */
        BYTE    debbasic[16];           /* Last 16 bytes of basic
                                           section of original DEB   */
        BYTE    debxtent[16][16];       /* First 16 extent descriptors
                                           from original DEB         */
        FWORD   resv;                   /* Reserved                  */
};

/*-------------------------------------------------------------------*/
/* Definition of data record block in IEBCOPY unload file            */
/*-------------------------------------------------------------------*/
struct DATABLK {                        /* IEBCOPY unload data rec   */
        FWORD   header;                 /* Reserved                  */
        HWORD   cyl;                    /* Cylinder number           */
        HWORD   head;                   /* Head number               */
        BYTE    rec;                    /* Record number             */
        BYTE    klen;                   /* Key length                */
        HWORD   dlen;                   /* Data length               */
#define MAX_DATALEN      32767
        BYTE    kdarea[MAX_DATALEN];    /* Key and data area         */
};

/*-------------------------------------------------------------------*/
/* Internal structures used by DASD utility functions                */
/*-------------------------------------------------------------------*/
struct CIFBLK {                         /* CKD image file descriptor */
        char   *fname;                  /* -> CKD image file name    */
        int     fd;                     /* CKD image file descriptor */
        u_int   trksz;                  /* CKD image track size      */
        BYTE   *trkbuf;                 /* -> Track buffer           */
        u_int   curcyl;                 /* Cylinder number of track
                                           currently in track buffer */
        u_int   curhead;                /* Head number of track
                                           currently in track buffer */
        int     trkmodif;               /* 1=Track has been modified */
        u_int   heads;                  /* Tracks per cylinder       */
        DEVBLK  devblk;                 /* Device Block              */
};

/*-------------------------------------------------------------------*/
/* Function prototypes                                               */
/*-------------------------------------------------------------------*/

/* Functions in module dasdutil.c */
DUT_DLL_IMPORT void string_to_upper (char *source);
DUT_DLL_IMPORT void string_to_lower (char *source);
DUT_DLL_IMPORT void convert_to_ebcdic (BYTE *dest, int len, char *source);
DUT_DLL_IMPORT int  make_asciiz (char *dest, int destlen, BYTE *src, int srclen);
DUT_DLL_IMPORT void data_dump (void *addr, int len);
DUT_DLL_IMPORT int  read_track (CIFBLK *cif, U32 cyl, U8 head);
int  rewrite_track (CIFBLK *cif);
DUT_DLL_IMPORT int  read_block (CIFBLK *cif, U32 cyl, U8 head, U8 rec,
        BYTE **keyptr, U8 *keylen, BYTE **dataptr, U16 *datalen);
DUT_DLL_IMPORT int  search_key_equal (CIFBLK *cif, BYTE *key, U8 keylen, u_int noext,
        DSXTENT extent[], U32 *cyl, U8 *head, U8 *rec);
DUT_DLL_IMPORT int  convert_tt (u_int tt, u_int noext, DSXTENT extent[], U8 heads,
        U32 *cyl, U8 *head);
#define IMAGE_OPEN_NORMAL   0x00000000
#define IMAGE_OPEN_DASDCOPY 0x00000001
#define IMAGE_OPEN_QUIET    0x00000002  /* (no msgs) */
DUT_DLL_IMPORT CIFBLK* open_ckd_image (char *fname, char *sfname, int omode,
        int option);
DUT_DLL_IMPORT CIFBLK* open_fba_image (char *fname, char *sfname, int omode,
        int option);
DUT_DLL_IMPORT int  close_ckd_image (CIFBLK *cif);
#define close_image_file(cif) close_ckd_image((cif))
DUT_DLL_IMPORT int  build_extent_array (CIFBLK *cif, char *dsnama, DSXTENT extent[],
        int *noext);
DUT_DLL_IMPORT int  capacity_calc (CIFBLK *cif, int used, int keylen, int datalen,
        int *newused, int *trkbaln, int *physlen, int *kbconst,
        int *lbconst, int *nkconst, BYTE*devflag, int *tolfact,
        int *maxdlen, int *numrecs, int *numhead, int *numcyls);
DUT_DLL_IMPORT int create_ckd (char *fname, U16 devtype, U32 heads, U32 maxdlen,
        U32 volcyls, char *volser, BYTE comp, int lfs, int dasdcopy,
        int nullfmt, int rawflag, int flagECmode, int flagMachinecheck);
DUT_DLL_IMPORT int create_fba (char *fname, U16 devtype, U32 sectsz, U32 sectors,
        char *volser, BYTE comp, int lfs, int dasdcopy, int rawflag);
int create_compressed_fba (char *fname, U16 devtype, U32 sectsz,
        U32 sectors, char *volser, BYTE comp, int lfs, int dasdcopy,
        int rawflag);
int get_verbose_util(void);
DUT_DLL_IMPORT void set_verbose_util(int v);

DUT_DLL_IMPORT int valid_dsname( const char *pszdsname );
#define DEFAULT_FBA_TYPE 0x3370
