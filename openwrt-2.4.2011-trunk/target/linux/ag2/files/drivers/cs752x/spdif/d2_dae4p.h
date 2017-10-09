
#define NUM_REGS_MAX 361

typedef struct reg_data{
   unsigned int Reg_Addr;
   unsigned int Reg_Val;

} reg_data;


#if 1
reg_data const d2_reg[NUM_REGS_MAX] = {
{ 0x000000, 0x800000},
{ 0x000001, 0xE00000},
{ 0x000002, 0xE00000},
{ 0x000003, 0xE00000},
{ 0x000004, 0xE00000},
{ 0x000005, 0xE00000},
{ 0x000006, 0x800000},
{ 0x000007, 0x000000},
{ 0x000008, 0x000000},
{ 0x000009, 0x800000},
{ 0x00000A, 0x834ED4},
{ 0x00000B, 0x000000},
{ 0x00000C, 0x9DC83B},
{ 0x00000D, 0x000000},
{ 0x00000E, 0x834ED4},
{ 0x00000F, 0x000000},
{ 0x000010, 0x9DC83B},
{ 0x000011, 0x000000},
{ 0x000012, 0x0000E4},
{ 0x000013, 0x24011D},
{ 0x000014, 0x7C0000},
{ 0x000015, 0x7EB852},
{ 0x000016, 0x04322A},
{ 0x000017, 0x006D0B},
{ 0x000018, 0x020000},
{ 0x000019, 0x24011D},
{ 0x00001A, 0x7C0000},
{ 0x00001B, 0x7EB852},
{ 0x00001C, 0x04322A},
{ 0x00001D, 0x006D0B},
{ 0x00001E, 0x020000},
{ 0x00001F, 0x022222},
{ 0x000020, 0x0ECE1F},
{ 0x000021, 0x000000},
{ 0x000022, 0x022222},
{ 0x000023, 0x0ECE1F},
{ 0x000024, 0x000000},
{ 0x000025, 0x022222},
{ 0x000026, 0x0ECE1F},
{ 0x000027, 0x000000},
{ 0x000028, 0x022222},
{ 0x000029, 0x0ECE1F},
{ 0x00002A, 0x000000},
{ 0x00002B, 0x022222},
{ 0x00002C, 0x0ECE1F},
{ 0x00002D, 0x000000},
{ 0x00002E, 0x022222},
{ 0x00002F, 0x0ECE1F},
{ 0x000030, 0x000000},
{ 0x000031, 0x022222},
{ 0x000032, 0x0ECE1F},
{ 0x000033, 0x000000},
{ 0x000034, 0x022222},
{ 0x000035, 0x0ECE1F},
{ 0x000036, 0x000000},
{ 0x000037, 0x022222},
{ 0x000038, 0x0ECE1F},
{ 0x000039, 0x000000},
{ 0x00003A, 0x022222},
{ 0x00003B, 0x0ECE1F},
{ 0x00003C, 0x000000},
{ 0x00003D, 0x055555},
{ 0x00003E, 0x0ECE1F},
{ 0x00003F, 0x000000},
{ 0x000040, 0x055555},
{ 0x000041, 0x0ECE1F},
{ 0x000042, 0x000000},
{ 0x000043, 0x055555},
{ 0x000044, 0x0ECE1F},
{ 0x000045, 0x000000},
{ 0x000046, 0x055555},
{ 0x000047, 0x0ECE1F},
{ 0x000048, 0x000000},
{ 0x000049, 0x055555},
{ 0x00004A, 0x0ECE1F},
{ 0x00004B, 0x000000},
{ 0x00004C, 0x055555},
{ 0x00004D, 0x0ECE1F},
{ 0x00004E, 0x000000},
{ 0x00004F, 0x055555},
{ 0x000050, 0x0ECE1F},
{ 0x000051, 0x000000},
{ 0x000052, 0x055555},
{ 0x000053, 0x0ECE1F},
{ 0x000054, 0x000000},
{ 0x000055, 0x055555},
{ 0x000056, 0x0ECE1F},
{ 0x000057, 0x000000},
{ 0x000058, 0x055555},
{ 0x000059, 0x0ECE1F},
{ 0x00005A, 0x000000},
{ 0x00005B, 0x055555},
{ 0x00005C, 0x0ECE1F},
{ 0x00005D, 0x000000},
{ 0x00005E, 0x055555},
{ 0x00005F, 0x0ECE1F},
{ 0x000060, 0x000000},
{ 0x000061, 0x022222},
{ 0x000062, 0x0ECE1F},
{ 0x000063, 0x000000},
{ 0x000064, 0x022222},
{ 0x000065, 0x0ECE1F},
{ 0x000066, 0x000000},
{ 0x000067, 0x022222},
{ 0x000068, 0x0ECE1F},
{ 0x000069, 0x000000},
{ 0x00006A, 0x022222},
{ 0x00006B, 0x0ECE1F},
{ 0x00006C, 0x000000},
{ 0x00006D, 0x022222},
{ 0x00006E, 0x0ECE1F},
{ 0x00006F, 0x000000},
{ 0x000070, 0x022222},
{ 0x000071, 0x0ECE1F},
{ 0x000072, 0x000000},
{ 0x000073, 0x022222},
{ 0x000074, 0x0ECE1F},
{ 0x000075, 0x000000},
{ 0x000076, 0x022222},
{ 0x000077, 0x0ECE1F},
{ 0x000078, 0x000000},
{ 0x000079, 0x022222},
{ 0x00007A, 0x0ECE1F},
{ 0x00007B, 0x000000},
{ 0x00007C, 0x022222},
{ 0x00007D, 0x0ECE1F},
{ 0x00007E, 0x000000},
{ 0x00007F, 0x800000},
{ 0x000080, 0x7FFFFF},
{ 0x000081, 0x7641AF},
{ 0x000082, 0x800000},
{ 0x000083, 0x7FFFFF},
{ 0x000084, 0x30FBC5},
{ 0x000085, 0x800000},
{ 0x000086, 0x7FFFFF},
{ 0x000087, 0x7641AF},
{ 0x000088, 0x800000},
{ 0x000089, 0x7FFFFF},
{ 0x00008A, 0x30FBC5},
{ 0x00008B, 0x800000},
{ 0x00008C, 0x7FFFFF},
{ 0x00008D, 0x7641AF},
{ 0x00008E, 0x800000},
{ 0x00008F, 0x7FFFFF},
{ 0x000090, 0x30FBC5},
{ 0x000091, 0x800000},
{ 0x000092, 0x7FFFFF},
{ 0x000093, 0x7641AF},
{ 0x000094, 0x800000},
{ 0x000095, 0x7FFFFF},
{ 0x000096, 0x30FBC5},
{ 0x000097, 0x800000},
{ 0x000098, 0x7FFFFF},
{ 0x000099, 0x7641AF},
{ 0x00009A, 0x800000},
{ 0x00009B, 0x7FFFFF},
{ 0x00009C, 0x30FBC5},
{ 0x00009D, 0x800000},
{ 0x00009E, 0x7FFFFF},
{ 0x00009F, 0x7641AF},
{ 0x0000A0, 0x800000},
{ 0x0000A1, 0x7FFFFF},
{ 0x0000A2, 0x30FBC5},
{ 0x0000A3, 0x800000},
{ 0x0000A4, 0x7FFFFF},
{ 0x0000A5, 0x7641AF},
{ 0x0000A6, 0x800000},
{ 0x0000A7, 0x7FFFFF},
{ 0x0000A8, 0x30FBC5},
{ 0x0000A9, 0x800000},
{ 0x0000AA, 0x7FFFFF},
{ 0x0000AB, 0x7641AF},
{ 0x0000AC, 0x800000},
{ 0x0000AD, 0x7FFFFF},
{ 0x0000AE, 0x30FBC5},
{ 0x0000AF, 0x400000},
{ 0x0000B0, 0x400000},
{ 0x0000B1, 0x800000},
{ 0x0000B2, 0x7FFFFF},
{ 0x0000B3, 0x7641AF},
{ 0x0000B4, 0x800000},
{ 0x0000B5, 0x7FFFFF},
{ 0x0000B6, 0x30FBC5},
{ 0x0000B7, 0x800000},
{ 0x0000B8, 0x7FFFFF},
{ 0x0000B9, 0x7641AF},
{ 0x0000BA, 0x800000},
{ 0x0000BB, 0x7FFFFF},
{ 0x0000BC, 0x30FBC5},
{ 0x0000BD, 0x24011D},
{ 0x0000BE, 0x7C0000},
{ 0x0000BF, 0x7EB852},
{ 0x0000C0, 0x04322A},
{ 0x0000C1, 0x006D0B},
{ 0x0000C2, 0x020000},
{ 0x0000C3, 0x24011D},
{ 0x0000C4, 0x7C0000},
{ 0x0000C5, 0x7EB852},
{ 0x0000C6, 0x04322A},
{ 0x0000C7, 0x006D0B},
{ 0x0000C8, 0x020000},
{ 0x0000C9, 0x24011D},
{ 0x0000CA, 0x7C0000},
{ 0x0000CB, 0x7EB852},
{ 0x0000CC, 0x04322A},
{ 0x0000CD, 0x006D0B},
{ 0x0000CE, 0x020000},
{ 0x0000CF, 0x24011D},
{ 0x0000D0, 0x7C0000},
{ 0x0000D1, 0x7EB852},
{ 0x0000D2, 0x04322A},
{ 0x0000D3, 0x006D0B},
{ 0x0000D4, 0x020000},
{ 0x0000D5, 0x24011D},
{ 0x0000D6, 0x7C0000},
{ 0x0000D7, 0x7EB852},
{ 0x0000D8, 0x04322A},
{ 0x0000D9, 0x006D0B},
{ 0x0000DA, 0x020000},
{ 0x0000DB, 0x000000},
{ 0x0000DC, 0x81AA26},
{ 0x0000DD, 0xEF2603},
{ 0x0000DE, 0x000000},
{ 0x0000DF, 0x81AA26},
{ 0x0000E0, 0xEF2603},
{ 0x0000E1, 0x000000},
{ 0x0000E2, 0x81AA26},
{ 0x0000E3, 0xEF2603},
{ 0x0000E4, 0x000000},
{ 0x0000E5, 0x81AA26},
{ 0x0000E6, 0xEF2603},
{ 0x0000E7, 0x000000},
{ 0x0000E8, 0x81AA26},
{ 0x0000E9, 0xEF2603},
#ifdef CONFIG_SOUND_DAC_SSP
{ 0x020001, 0xC0000E},
#else
{ 0x020001, 0xC0000F},
#endif
{ 0x0000EB, 0x000003},
{ 0x0000EC, 0xC0E09C},
{ 0x0000ED, 0x03E793},
{ 0x0000EE, 0x80AB20},
{ 0x0000EF, 0xF1FEB3},
{ 0x0000F0, 0xE00000},
{ 0x0000F1, 0x800000},
{ 0x0000F2, 0x7FFFFF},
{ 0x0000F3, 0x000002},
{ 0x0000F4, 0x11999A},
{ 0x0000F5, 0x5A85F9},
{ 0x0000F6, 0x000002},
{ 0x0000F7, 0x11999A},
{ 0x0000F8, 0x5A85F9},
{ 0x0000F9, 0x000006},
{ 0x0000FA, 0x000006},
{ 0x0000FB, 0x13BBBC},
{ 0x0000FC, 0x13BBBC},
{ 0x0000FD, 0x008889},
{ 0x0000FE, 0x008889},
{ 0x0000FF, 0x00369D},
{ 0x000100, 0x00369D},
{ 0x000101, 0x00CCCD},
{ 0x000102, 0x00CCCD},
{ 0x000103, 0x006D3A},
{ 0x000104, 0x006D3A},
{ 0x000105, 0x800000},
{ 0x000106, 0x800000},
{ 0x000107, 0x800000},
{ 0x000108, 0x800000},
{ 0x000109, 0x00CCCD},
{ 0x00010A, 0x001B4F},
{ 0x00010B, 0x00001F},
{ 0x00010C, 0x40348E},
{ 0x00010D, 0x08408D},
{ 0x00010E, 0x08408D},
{ 0x00010F, 0x140000},
{ 0x000110, 0x000000},
{ 0x000111, 0x11999A},
{ 0x000112, 0x000000},
{ 0x000113, 0xB33333},
{ 0x000114, 0x000000},
{ 0x000115, 0x400000},
{ 0x000116, 0x616CB1},
{ 0x000117, 0x08408D},
{ 0x000118, 0x08408D},
{ 0x000119, 0x3298B0},
{ 0x00011A, 0x100000},
{ 0x00011B, 0x100000},
{ 0x00011C, 0x011111},
{ 0x00011D, 0x011111},
{ 0x00011E, 0x000000},
{ 0x00011F, 0x0B3333},
{ 0x000120, 0xD9999A},
{ 0x000121, 0xE66666},
{ 0x000122, 0x333333},
{ 0x000123, 0x19999A},
{ 0x000124, 0x355555},
{ 0x000125, 0x355555},
{ 0x000126, 0x0AAAAB},
{ 0x000127, 0x0AAAAB},
{ 0x000128, 0xA56208},
{ 0x000129, 0x800000},
{ 0x00012A, 0x100000},
{ 0x00012B, 0x100000},
{ 0x00012C, 0x011111},
{ 0x00012D, 0x011111},
{ 0x00012E, 0x000000},
{ 0x00012F, 0x0B3333},
{ 0x000130, 0xD9999A},
{ 0x000131, 0xE66666},
{ 0x000132, 0x333333},
{ 0x000133, 0x19999A},
{ 0x000134, 0x000000},
{ 0x000135, 0x355555},
{ 0x000136, 0x7641B1},
{ 0x000137, 0x355555},
{ 0x000138, 0x0AAAAB},
{ 0x000139, 0x0AAAAB},
{ 0x00013A, 0xA56208},
{ 0x00013B, 0x800000},
{ 0x00013C, 0x000000},
#ifdef CONFIG_SOUND_DAC_SSP
{ 0x020001, 0xC0000E},
#else
{ 0x020001, 0xC0000F},
#endif
{ 0x00013E, 0x000000},
{ 0x00013F, 0x400000},
{ 0x000140, 0x1C73D5},
{ 0x000141, 0x23D1CD},
{ 0x000142, 0x3FDF7B},
{ 0x000143, 0x3FF060},
{ 0x000144, 0x7FEF00},
{ 0x000145, 0x7FEF00},
{ 0x000146, 0x4010E5},
{ 0x000147, 0x000000},
{ 0x000148, 0x000000},
{ 0x000149, 0x000000},
{ 0x00014A, 0x000001},
{ 0x00014B, 0x000003},
{ 0x00014C, 0x000000},
{ 0x00014D, 0x000000},
{ 0x00014E, 0x000000},
{ 0x00014F, 0x000000},
{ 0x000150, 0x000000},
{ 0x000151, 0x000000},
{ 0x000152, 0x4CCCCD},
{ 0x000153, 0x400000},
{ 0x000154, 0x7FFFFF},
{ 0x000155, 0x5AE148},
{ 0x000156, 0x266666},
{ 0x000157, 0x266666},
{ 0x000158, 0x400000},
{ 0x000159, 0x266666},
{ 0x00015A, 0x266666},
{ 0x00015B, 0x000003},
{ 0x00015C, 0x000000},
{ 0x00015D, 0x000000},
{ 0x00015E, 0x000000},
{ 0x00015F, 0x000000},
{ 0x000160, 0x000000},
{ 0x000161, 0x000002},
{ 0x000162, 0x666666},
{ 0x000163, 0x400000},
{ 0x000164, 0x333333},
{ 0x000165, 0x600000},
{ 0x000166, 0x0000FF},
{ 0x000167, 0x00027F},
{ 0x800000, 0x000000}
 };


#else
reg_data const d2_reg[NUM_REGS_MAX] = {
{ 0x000000, 0x800000},
{ 0x000001, 0xE00000},
{ 0x000002, 0xE00000},
{ 0x000003, 0xE00000},
{ 0x000004, 0xE00000},
{ 0x000005, 0xE00000},
{ 0x000006, 0x800000},
{ 0x000007, 0x000000},
{ 0x000008, 0x000000},
{ 0x000009, 0x800000},
{ 0x00000A, 0x834ED4},
{ 0x00000B, 0x000000},
{ 0x00000C, 0x9DC83B},
{ 0x00000D, 0x000000},
{ 0x00000E, 0x834ED4},
{ 0x00000F, 0x000000},
{ 0x000010, 0x9DC83B},
{ 0x000011, 0x000000},
{ 0x000012, 0x0000E4},
{ 0x000013, 0x24011D},
{ 0x000014, 0x7C0000},
{ 0x000015, 0x7EB852},
{ 0x000016, 0x04322A},
{ 0x000017, 0x006D0B},
{ 0x000018, 0x020000},
{ 0x000019, 0x24011D},
{ 0x00001A, 0x7C0000},
{ 0x00001B, 0x7EB852},
{ 0x00001C, 0x04322A},
{ 0x00001D, 0x006D0B},
{ 0x00001E, 0x020000},
{ 0x00001F, 0x022222},
{ 0x000020, 0x0ECE1F},
{ 0x000021, 0x000000},
{ 0x000022, 0x022222},
{ 0x000023, 0x0ECE1F},
{ 0x000024, 0x000000},
{ 0x000025, 0x022222},
{ 0x000026, 0x0ECE1F},
{ 0x000027, 0x000000},
{ 0x000028, 0x022222},
{ 0x000029, 0x0ECE1F},
{ 0x00002A, 0x000000},
{ 0x00002B, 0x022222},
{ 0x00002C, 0x0ECE1F},
{ 0x00002D, 0x000000},
{ 0x00002E, 0x022222},
{ 0x00002F, 0x0ECE1F},
{ 0x000030, 0x000000},
{ 0x000031, 0x022222},
{ 0x000032, 0x0ECE1F},
{ 0x000033, 0x000000},
{ 0x000034, 0x022222},
{ 0x000035, 0x0ECE1F},
{ 0x000036, 0x000000},
{ 0x000037, 0x022222},
{ 0x000038, 0x0ECE1F},
{ 0x000039, 0x000000},
{ 0x00003A, 0x022222},
{ 0x00003B, 0x0ECE1F},
{ 0x00003C, 0x000000},
{ 0x00003D, 0x055555},
{ 0x00003E, 0x0ECE1F},
{ 0x00003F, 0x000000},
{ 0x000040, 0x055555},
{ 0x000041, 0x0ECE1F},
{ 0x000042, 0x000000},
{ 0x000043, 0x055555},
{ 0x000044, 0x0ECE1F},
{ 0x000045, 0x000000},
{ 0x000046, 0x055555},
{ 0x000047, 0x0ECE1F},
{ 0x000048, 0x000000},
{ 0x000049, 0x055555},
{ 0x00004A, 0x0ECE1F},
{ 0x00004B, 0x000000},
{ 0x00004C, 0x055555},
{ 0x00004D, 0x0ECE1F},
{ 0x00004E, 0x000000},
{ 0x00004F, 0x055555},
{ 0x000050, 0x0ECE1F},
{ 0x000051, 0x000000},
{ 0x000052, 0x055555},
{ 0x000053, 0x0ECE1F},
{ 0x000054, 0x000000},
{ 0x000055, 0x055555},
{ 0x000056, 0x0ECE1F},
{ 0x000057, 0x000000},
{ 0x000058, 0x055555},
{ 0x000059, 0x0ECE1F},
{ 0x00005A, 0x000000},
{ 0x00005B, 0x055555},
{ 0x00005C, 0x0ECE1F},
{ 0x00005D, 0x000000},
{ 0x00005E, 0x055555},
{ 0x00005F, 0x0ECE1F},
{ 0x000060, 0x000000},
{ 0x000061, 0x022222},
{ 0x000062, 0x0ECE1F},
{ 0x000063, 0x000000},
{ 0x000064, 0x022222},
{ 0x000065, 0x0ECE1F},
{ 0x000066, 0x000000},
{ 0x000067, 0x022222},
{ 0x000068, 0x0ECE1F},
{ 0x000069, 0x000000},
{ 0x00006A, 0x022222},
{ 0x00006B, 0x0ECE1F},
{ 0x00006C, 0x000000},
{ 0x00006D, 0x022222},
{ 0x00006E, 0x0ECE1F},
{ 0x00006F, 0x000000},
{ 0x000070, 0x022222},
{ 0x000071, 0x0ECE1F},
{ 0x000072, 0x000000},
{ 0x000073, 0x022222},
{ 0x000074, 0x0ECE1F},
{ 0x000075, 0x000000},
{ 0x000076, 0x022222},
{ 0x000077, 0x0ECE1F},
{ 0x000078, 0x000000},
{ 0x000079, 0x022222},
{ 0x00007A, 0x0ECE1F},
{ 0x00007B, 0x000000},
{ 0x00007C, 0x022222},
{ 0x00007D, 0x0ECE1F},
{ 0x00007E, 0x000000},
{ 0x00007F, 0x800000},
{ 0x000080, 0x7FFFFF},
{ 0x000081, 0x7641AF},
{ 0x000082, 0x800000},
{ 0x000083, 0x7FFFFF},
{ 0x000084, 0x30FBC5},
{ 0x000085, 0x800000},
{ 0x000086, 0x7FFFFF},
{ 0x000087, 0x7641AF},
{ 0x000088, 0x800000},
{ 0x000089, 0x7FFFFF},
{ 0x00008A, 0x30FBC5},
{ 0x00008B, 0x800000},
{ 0x00008C, 0x7FFFFF},
{ 0x00008D, 0x7641AF},
{ 0x00008E, 0x800000},
{ 0x00008F, 0x7FFFFF},
{ 0x000090, 0x30FBC5},
{ 0x000091, 0x800000},
{ 0x000092, 0x7FFFFF},
{ 0x000093, 0x7641AF},
{ 0x000094, 0x800000},
{ 0x000095, 0x7FFFFF},
{ 0x000096, 0x30FBC5},
{ 0x000097, 0x800000},
{ 0x000098, 0x7FFFFF},
{ 0x000099, 0x7641AF},
{ 0x00009A, 0x800000},
{ 0x00009B, 0x7FFFFF},
{ 0x00009C, 0x30FBC5},
{ 0x00009D, 0x800000},
{ 0x00009E, 0x7FFFFF},
{ 0x00009F, 0x7641AF},
{ 0x0000A0, 0x800000},
{ 0x0000A1, 0x7FFFFF},
{ 0x0000A2, 0x30FBC5},
{ 0x0000A3, 0x800000},
{ 0x0000A4, 0x7FFFFF},
{ 0x0000A5, 0x7641AF},
{ 0x0000A6, 0x800000},
{ 0x0000A7, 0x7FFFFF},
{ 0x0000A8, 0x30FBC5},
{ 0x0000A9, 0x800000},
{ 0x0000AA, 0x7FFFFF},
{ 0x0000AB, 0x7641AF},
{ 0x0000AC, 0x800000},
{ 0x0000AD, 0x7FFFFF},
{ 0x0000AE, 0x30FBC5},
{ 0x0000AF, 0x400000},
{ 0x0000B0, 0x400000},
{ 0x0000B1, 0x800000},
{ 0x0000B2, 0x7FFFFF},
{ 0x0000B3, 0x7641AF},
{ 0x0000B4, 0x800000},
{ 0x0000B5, 0x7FFFFF},
{ 0x0000B6, 0x30FBC5},
{ 0x0000B7, 0x800000},
{ 0x0000B8, 0x7FFFFF},
{ 0x0000B9, 0x7641AF},
{ 0x0000BA, 0x800000},
{ 0x0000BB, 0x7FFFFF},
{ 0x0000BC, 0x30FBC5},
{ 0x0000BD, 0x24011D},
{ 0x0000BE, 0x7C0000},
{ 0x0000BF, 0x7EB852},
{ 0x0000C0, 0x04322A},
{ 0x0000C1, 0x006D0B},
{ 0x0000C2, 0x020000},
{ 0x0000C3, 0x24011D},
{ 0x0000C4, 0x7C0000},
{ 0x0000C5, 0x7EB852},
{ 0x0000C6, 0x04322A},
{ 0x0000C7, 0x006D0B},
{ 0x0000C8, 0x020000},
{ 0x0000C9, 0x24011D},
{ 0x0000CA, 0x7C0000},
{ 0x0000CB, 0x7EB852},
{ 0x0000CC, 0x04322A},
{ 0x0000CD, 0x006D0B},
{ 0x0000CE, 0x020000},
{ 0x0000CF, 0x24011D},
{ 0x0000D0, 0x7C0000},
{ 0x0000D1, 0x7EB852},
{ 0x0000D2, 0x04322A},
{ 0x0000D3, 0x006D0B},
{ 0x0000D4, 0x020000},
{ 0x0000D5, 0x24011D},
{ 0x0000D6, 0x7C0000},
{ 0x0000D7, 0x7EB852},
{ 0x0000D8, 0x04322A},
{ 0x0000D9, 0x006D0B},
{ 0x0000DA, 0x020000},
{ 0x0000DB, 0x000000},
{ 0x0000DC, 0x81AA26},
{ 0x0000DD, 0xEF2603},
{ 0x0000DE, 0x000000},
{ 0x0000DF, 0x81AA26},
{ 0x0000E0, 0xEF2603},
{ 0x0000E1, 0x000000},
{ 0x0000E2, 0x81AA26},
{ 0x0000E3, 0xEF2603},
{ 0x0000E4, 0x000000},
{ 0x0000E5, 0x81AA26},
{ 0x0000E6, 0xEF2603},
{ 0x0000E7, 0x000000},
{ 0x0000E8, 0x81AA26},
{ 0x0000E9, 0xEF2603},
#ifdef CONFIG_SOUND_DAC_SSP
{ 0x020001, 0xC0000E},
#else
{ 0x020001, 0xC0000F},
#endif
{ 0x0000EB, 0x000003},
{ 0x0000EC, 0xC0E09C},
{ 0x0000ED, 0x03E793},
{ 0x0000EE, 0x80AB20},
{ 0x0000EF, 0xF1FEB3},
{ 0x0000F0, 0xE00000},
{ 0x0000F1, 0x800000},
{ 0x0000F2, 0x7FFFFF},
{ 0x0000F3, 0x000002},
{ 0x0000F4, 0x11999A},
{ 0x0000F5, 0x5A85F9},
{ 0x0000F6, 0x000002},
{ 0x0000F7, 0x11999A},
{ 0x0000F8, 0x5A85F9},
{ 0x0000F9, 0x000006},
{ 0x0000FA, 0x000006},
{ 0x0000FB, 0x13BBBC},
{ 0x0000FC, 0x13BBBC},
{ 0x0000FD, 0x008889},
{ 0x0000FE, 0x008889},
{ 0x0000FF, 0x00369D},
{ 0x000100, 0x00369D},
{ 0x000101, 0x00CCCD},
{ 0x000102, 0x00CCCD},
{ 0x000103, 0x006D3A},
{ 0x000104, 0x006D3A},
{ 0x000105, 0x800000},
{ 0x000106, 0x800000},
{ 0x000107, 0x800000},
{ 0x000108, 0x800000},
{ 0x000109, 0x00CCCD},
{ 0x00010A, 0x001B4F},
{ 0x00010B, 0x00001F},
{ 0x00010C, 0x40348E},
{ 0x00010D, 0x08408D},
{ 0x00010E, 0x08408D},
{ 0x00010F, 0x140000},
{ 0x000110, 0x000000},
{ 0x000111, 0x11999A},
{ 0x000112, 0x000000},
{ 0x000113, 0xB33333},
{ 0x000114, 0x000000},
{ 0x000115, 0x400000},
{ 0x000116, 0x616CB1},
{ 0x000117, 0x08408D},
{ 0x000118, 0x08408D},
{ 0x000119, 0x3298B0},
{ 0x00011A, 0x100000},
{ 0x00011B, 0x100000},
{ 0x00011C, 0x011111},
{ 0x00011D, 0x011111},
{ 0x00011E, 0x000000},
{ 0x00011F, 0x0B3333},
{ 0x000120, 0xD9999A},
{ 0x000121, 0xE66666},
{ 0x000122, 0x333333},
{ 0x000123, 0x19999A},
{ 0x000124, 0x355555},
{ 0x000125, 0x355555},
{ 0x000126, 0x0AAAAB},
{ 0x000127, 0x0AAAAB},
{ 0x000128, 0xA56208},
{ 0x000129, 0x800000},
{ 0x00012A, 0x100000},
{ 0x00012B, 0x100000},
{ 0x00012C, 0x011111},
{ 0x00012D, 0x011111},
{ 0x00012E, 0x000000},
{ 0x00012F, 0x0B3333},
{ 0x000130, 0xD9999A},
{ 0x000131, 0xE66666},
{ 0x000132, 0x333333},
{ 0x000133, 0x19999A},
{ 0x000134, 0x000000},
{ 0x000135, 0x355555},
{ 0x000136, 0x7641B1},
{ 0x000137, 0x355555},
{ 0x000138, 0x0AAAAB},
{ 0x000139, 0x0AAAAB},
{ 0x00013A, 0xA56208},
{ 0x00013B, 0x800000},
{ 0x00013C, 0x000000},
#ifdef CONFIG_SOUND_DAC_SSP
{ 0x020001, 0xC0000E},
#else
{ 0x020001, 0xC0000F},
#endif
{ 0x00013E, 0x000000},
{ 0x00013F, 0x400000},
{ 0x000140, 0x1C73D5},
{ 0x000141, 0x23D1CD},
{ 0x000142, 0x3FDF7B},
{ 0x000143, 0x3FF060},
{ 0x000144, 0x7FEF00},
{ 0x000145, 0x7FEF00},
{ 0x000146, 0x4010E5},
{ 0x000147, 0x000000},
{ 0x000148, 0x000000},
{ 0x000149, 0x000000},
{ 0x00014A, 0x000001},
{ 0x00014B, 0x000003},
{ 0x00014C, 0x000000},
{ 0x00014D, 0x000000},
{ 0x00014E, 0x000000},
{ 0x00014F, 0x000000},
{ 0x000150, 0x000000},
{ 0x000151, 0x000000},
{ 0x000152, 0x4CCCCD},
{ 0x000153, 0x400000},
{ 0x000154, 0x7FFFFF},
{ 0x000155, 0x5AE148},
{ 0x000156, 0x266666},
{ 0x000157, 0x266666},
{ 0x000158, 0x400000},
{ 0x000159, 0x266666},
{ 0x00015A, 0x266666},
{ 0x00015B, 0x000003},
{ 0x00015C, 0x000000},
{ 0x00015D, 0x000000},
{ 0x00015E, 0x000000},
{ 0x00015F, 0x000000},
{ 0x000160, 0x000000},
{ 0x000161, 0x000002},
{ 0x000162, 0x666666},
{ 0x000163, 0x400000},
{ 0x000164, 0x333333},
{ 0x000165, 0x600000},
{ 0x000166, 0x0000FF},
{ 0x000167, 0x00027F},
{ 0x800000, 0x000000}
 };

#endif
