


#include "ICOMFramework.h"
#include "TranscoderIBM01141.h"
#include "TranscoderException.h"
#include "ErrorHandler.h"

namespace ICOM{ 
 namespace Framework{
 

// ---------------------------------------------------------------------------
//  Local const data
//
//  gFromTable
//      This is the translation table for EBCDIC 1141 to Unicode. This
//      table contains 256 entries. The entry for EBCDIC 1141 byte x is the
//      Unicode translation of that byte.
//
//  gToTable
//  gToTableSz
//      This is the translation table for Unicode to EBCDIC 1141. This one
//      contains a list of records, sorted by the Unicode code point. We do
//      a binary search to find the Unicode point, and that record's other
//      field is the EBCDIC 1141 code point to translate to.
// ---------------------------------------------------------------------------
static const ICF_uint32 gFromTable[256] =
{
	0x0000, /* 0x00 - NUL - NUL */
	0x0001, /* 0x01 - SOH - SOH */
	0x0002, /* 0x02 - STX - STX */
	0x0003, /* 0x03 - ETX - ETX */
	0x009C, /* 0x04 - SEL - SEL */
	0x0009, /* 0x05 - HAT - HAT */
	0x0086, /* 0x06 - RNL - RNL */
	0x007F, /* 0x07 - DEL - DEL */
	0x0097, /* 0x08 - GE - GE */
	0x008D, /* 0x09 - SPS - SPS */
	0x008E, /* 0x0A - RPT - RPT */
	0x000B, /* 0x0B - VT - VT */
	0x000C, /* 0x0C - FF - FF */
	0x000D, /* 0x0D - CR - CR */
	0x000E, /* 0x0E - SO - SO */
	0x000F, /* 0x0F - SI - SI */
	0x0010, /* 0x10 - DLE - DLE */
	0x0011, /* 0x11 - DC1 - DC1 */
	0x0012, /* 0x12 - DC2 - DC2 */
	0x0013, /* 0x13 - DC3 - DC3 */
	0x009D, /* 0x14 - RES ENP - RES ENP */
	0x0085, /* 0x15 - NL - NL */
	0x0008, /* 0x16 - BS - BS */
	0x0087, /* 0x17 - POC - POC */
	0x0018, /* 0x18 - CAN - CAN */
	0x0019, /* 0x19 - EM - EM */
	0x0092, /* 0x1A - UBS - UBS */
	0x008F, /* 0x1B - CUI - CUI */
	0x001C, /* 0x1C - IFS - IFS */
	0x001D, /* 0x1D - IGS - IGS */
	0x001E, /* 0x1E - IRS - IRS */
	0x001F, /* 0x1F - IUS ITB - IUS ITB */
	0x0080, /* 0x20 - DS - DS */
	0x0081, /* 0x21 - SOS - SOS */
	0x0082, /* 0x22 - FS - FS */
	0x0083, /* 0x23 - WUS - WUS */
	0x0084, /* 0x24 - BYP INP - BYP INP */
	0x000A, /* 0x25 - LF - LF */
	0x0017, /* 0x26 - ETB - ETB */
	0x001B, /* 0x27 - ESC - ESC */
	0x0088, /* 0x28 - SA - SA */
	0x0089, /* 0x29 - SFE - SFE */
	0x008A, /* 0x2A - SM SW - SM SW */
	0x008B, /* 0x2B - CSP - CSP */
	0x008C, /* 0x2C - MFA - MFA */
	0x0005, /* 0x2D - ENQ - ENQ */
	0x0006, /* 0x2E - ACK - ACK */
	0x0007, /* 0x2F - BEL - BEL */
	0x0090, /* 0x30 - 0x30 - 0x30 */
	0x0091, /* 0x31 - 0x31 - 0x31 */
	0x0016, /* 0x32 - SYN - SYN */
	0x0093, /* 0x33 - IR - IR */
	0x0094, /* 0x34 - PP - PP */
	0x0095, /* 0x35 - TRN - TRN */
	0x0096, /* 0x36 - NBS - NBS */
	0x0004, /* 0x37 - EOT - EOT */
	0x0098, /* 0x38 - SBS - SBS */
	0x0099, /* 0x39 - IT - IT */
	0x009A, /* 0x3A - RFF - RFF */
	0x009B, /* 0x3B - CU3 - CU3 */
	0x0014, /* 0x3C - DC4 - DC4 */
	0x0015, /* 0x3D - NAK - NAK */
	0x009E, /* 0x3E - 0x3E - 0x3E */
	0x001A, /* 0x3F - SUB - SUB */
	0x0020, /* 0x40 - SP010000 - Space */
	0x00A0, /* 0x41 - SP300000 - Required Space */
	0x00E2, /* 0x42 - LA150000 - a Circumflex Small */
	0x007B, /* 0x43 - SM110000 - Left Brace */
	0x00E0, /* 0x44 - LA130000 - a Grave Small */
	0x00E1, /* 0x45 - LA110000 - a Acute Small */
	0x00E3, /* 0x46 - LA190000 - a Tilde Small */
	0x00E5, /* 0x47 - LA270000 - a Overcircle Small */
	0x00E7, /* 0x48 - LC410000 - c Cedilla Small */
	0x00F1, /* 0x49 - LN190000 - n Tilde Small */
	0x00C4, /* 0x4A - LA180000 - A Diaeresis Capital */
	0x002E, /* 0x4B - SP110000 - Period/Full Stop */
	0x003C, /* 0x4C - SA030000 - Less Than Sign/Greater Than Sign (Arabic) */
	0x0028, /* 0x4D - SP060000 - Left Parenthesis */
	0x002B, /* 0x4E - SA010000 - Plus Sign */
	0x0021, /* 0x4F - SP020000 - Exclamation Point */
	0x0026, /* 0x50 - SM030000 - Ampersand */
	0x00E9, /* 0x51 - LE110000 - e Acute Small */
	0x00EA, /* 0x52 - LE150000 - e Circumflex Small */
	0x00EB, /* 0x53 - LE170000 - e Diaeresis Small */
	0x00E8, /* 0x54 - LE130000 - e Grave Small */
	0x00ED, /* 0x55 - LI110000 - i Acute Small */
	0x00EE, /* 0x56 - LI150000 - i Circumflex Small */
	0x00EF, /* 0x57 - LI170000 - i Diaeresis Small */
	0x00EC, /* 0x58 - LI130000 - i Grave Small */
	0x007E, /* 0x59 - SD190000 - Tilde Accent */
	0x00DC, /* 0x5A - LU180000 - U Diaeresis Capital */
	0x0024, /* 0x5B - SC030000 - Dollar Sign */
	0x002A, /* 0x5C - SM040000 - Asterisk */
	0x0029, /* 0x5D - SP070000 - Right Parenthesis */
	0x003B, /* 0x5E - SP140000 - Semicolon */
	0x005E, /* 0x5F - SD150000 - Circumflex Accent */
	0x002D, /* 0x60 - SP100000 - Hyphen/Minus Sign */
	0x002F, /* 0x61 - SP120000 - Slash */
	0x00C2, /* 0x62 - LA160000 - A Circumflex Capital */
	0x005B, /* 0x63 - SM060000 - Left Bracket */
	0x00C0, /* 0x64 - LA140000 - A Grave Capital */
	0x00C1, /* 0x65 - LA120000 - A Acute Capital */
	0x00C3, /* 0x66 - LA200000 - A Tilde Capital */
	0x00C5, /* 0x67 - LA280000 - A Overcircle Capital */
	0x00C7, /* 0x68 - LC420000 - C Cedilla Capital */
	0x00D1, /* 0x69 - LN200000 - N Tilde Capital */
	0x00F6, /* 0x6A - LO170000 - o Diaeresis Small */
	0x002C, /* 0x6B - SP080000 - Comma */
	0x0025, /* 0x6C - SM020000 - Percent Sign */
	0x005F, /* 0x6D - SP090000 - Underline/Continuous Underscore */
	0x003E, /* 0x6E - SA050000 - Greater Than Sign/Less Than Sign (Arabic) */
	0x003F, /* 0x6F - SP150000 - Question Mark */
	0x00F8, /* 0x70 - LO610000 - o Slash Small */
	0x00C9, /* 0x71 - LE120000 - E Acute Capital */
	0x00CA, /* 0x72 - LE160000 - E Circumflex Capital */
	0x00CB, /* 0x73 - LE180000 - E Diaeresis Capital */
	0x00C8, /* 0x74 - LE140000 - E Grave Capital */
	0x00CD, /* 0x75 - LI120000 - I Acute Capital */
	0x00CE, /* 0x76 - LI160000 - I Circumflex Capital */
	0x00CF, /* 0x77 - LI180000 - I Diaeresis Capital */
	0x00CC, /* 0x78 - LI140000 - I Grave Capital */
	0x0060, /* 0x79 - SD130000 - Grave Accent */
	0x003A, /* 0x7A - SP130000 - Colon */
	0x0023, /* 0x7B - SM010000 - Number Sign */
	0x00A7, /* 0x7C - SM240000 - Section Symbol (USA)/Paragraph Symbol (Europe) */
	0x0027, /* 0x7D - SP050000 - Apostrophe */
	0x003D, /* 0x7E - SA040000 - Equal Sign */
	0x0022, /* 0x7F - SP040000 - Quotation Marks */
	0x00D8, /* 0x80 - LO620000 - O Slash Capital */
	0x0061, /* 0x81 - LA010000 - a Small */
	0x0062, /* 0x82 - LB010000 - b Small */
	0x0063, /* 0x83 - LC010000 - c Small */
	0x0064, /* 0x84 - LD010000 - d Small */
	0x0065, /* 0x85 - LE010000 - e Small */
	0x0066, /* 0x86 - LF010000 - f Small */
	0x0067, /* 0x87 - LG010000 - g Small */
	0x0068, /* 0x88 - LH010000 - h Small */
	0x0069, /* 0x89 - LI010000 - i Small */
	0x00AB, /* 0x8A - SP170000 - Left Angle Quotes */
	0x00BB, /* 0x8B - SP180000 - Right Angle Quotes */
	0x00F0, /* 0x8C - LD630000 - eth Icelandic Small */
	0x00FD, /* 0x8D - LY110000 - y Acute Small */
	0x00FE, /* 0x8E - LT630000 - Thorn Icelandic Small */
	0x00B1, /* 0x8F - SA020000 - Plus or Minus Sign */
	0x00B0, /* 0x90 - SM190000 - Degree Symbol */
	0x006A, /* 0x91 - LJ010000 - j Small */
	0x006B, /* 0x92 - LK010000 - k Small */
	0x006C, /* 0x93 - LL010000 - l Small */
	0x006D, /* 0x94 - LM010000 - m Small */
	0x006E, /* 0x95 - LN010000 - n Small */
	0x006F, /* 0x96 - LO010000 - o Small */
	0x0070, /* 0x97 - LP010000 - p Small */
	0x0071, /* 0x98 - LQ010000 - q Small */
	0x0072, /* 0x99 - LR010000 - r Small */
	0x00AA, /* 0x9A - SM210000 - Ordinal Indicator, Feminine */
	0x00BA, /* 0x9B - SM200000 - Ordinal Indicator, Masculine */
	0x00E6, /* 0x9C - LA510000 - ae Diphthong Small */
	0x00B8, /* 0x9D - SD410000 - Cedilla or Sedila Accent */
	0x00C6, /* 0x9E - LA520000 - ae Diphthong Capital */
	0x20AC, /* 0x9F - SC200000 - Euro symbol */
	0x00B5, /* 0xA0 - SM170000 - Micro Symbol */
	0x00DF, /* 0xA1 - LS610000 - Sharp s Small */
	0x0073, /* 0xA2 - LS010000 - s Small */
	0x0074, /* 0xA3 - LT010000 - t Small */
	0x0075, /* 0xA4 - LU010000 - u Small */
	0x0076, /* 0xA5 - LV010000 - v Small */
	0x0077, /* 0xA6 - LW010000 - w Small */
	0x0078, /* 0xA7 - LX010000 - x Small */
	0x0079, /* 0xA8 - LY010000 - y Small */
	0x007A, /* 0xA9 - LZ010000 - z Small */
	0x00A1, /* 0xAA - SP030000 - Exclamation Point, Inverted */
	0x00BF, /* 0xAB - SP160000 - Question Mark, Inverted */
	0x00D0, /* 0xAC - LD620000 - D Stroke Capital/Eth Icelandic Capital */
	0x00DD, /* 0xAD - LY120000 - Y Acute Capital */
	0x00DE, /* 0xAE - LT640000 - Thorn Icelandic Capital */
	0x00AE, /* 0xAF - SM530000 - Registered Trademark Symbol */
	0x00A2, /* 0xB0 - SC040000 - Cent Sign */
	0x00A3, /* 0xB1 - SC020000 - Pound Sterling Sign */
	0x00A5, /* 0xB2 - SC050000 - Yen Sign */
	0x00B7, /* 0xB3 - SD630000 - Middle Dot */
	0x00A9, /* 0xB4 - SM520000 - Copyright Symbol */
	0x0040, /* 0xB5 - SM050000 - At Sign */
	0x00B6, /* 0xB6 - SM250000 - Paragraph Symbol (USA) */
	0x00BC, /* 0xB7 - NF040000 - One Quarter */
	0x00BD, /* 0xB8 - NF010000 - One Half */
	0x00BE, /* 0xB9 - NF050000 - Three Quarters */
	0x00AC, /* 0xBA - SM660000 - Logical NOT/End Of Line Symbol */
	0x007C, /* 0xBB - SM130000 - Vertical Line/Logical OR */
	0x00AF, /* 0xBC - SM150000 - Overline */
	0x00A8, /* 0xBD - SD170000 - Diaeresis/Umlaut Accent */
	0x00B4, /* 0xBE - SD110000 - Acute Accent */
	0x00D7, /* 0xBF - SA070000 - Multiply Sign */
	0x00E4, /* 0xC0 - LA170000 - a Diaeresis Small */
	0x0041, /* 0xC1 - LA020000 - A Capital */
	0x0042, /* 0xC2 - LB020000 - B Capital */
	0x0043, /* 0xC3 - LC020000 - C Capital */
	0x0044, /* 0xC4 - LD020000 - D Capital */
	0x0045, /* 0xC5 - LE020000 - E Capital */
	0x0046, /* 0xC6 - LF020000 - F Capital */
	0x0047, /* 0xC7 - LG020000 - G Capital */
	0x0048, /* 0xC8 - LH020000 - H Capital */
	0x0049, /* 0xC9 - LI020000 - I Capital */
	0x00AD, /* 0xCA - SP320000 - Syllable Hyphen */
	0x00F4, /* 0xCB - LO150000 - o Circumflex Small */
	0x00A6, /* 0xCC - SM650000 - Vertical Line, Broken */
	0x00F2, /* 0xCD - LO130000 - o Grave Small */
	0x00F3, /* 0xCE - LO110000 - o Acute Small */
	0x00F5, /* 0xCF - LO190000 - o Tilde Small */
	0x00FC, /* 0xD0 - LU170000 - u Diaeresis Small */
	0x004A, /* 0xD1 - LJ020000 - J Capital */
	0x004B, /* 0xD2 - LK020000 - K Capital */
	0x004C, /* 0xD3 - LL020000 - L Capital */
	0x004D, /* 0xD4 - LM020000 - M Capital */
	0x004E, /* 0xD5 - LN020000 - N Capital */
	0x004F, /* 0xD6 - LO020000 - O Capital */
	0x0050, /* 0xD7 - LP020000 - P Capital */
	0x0051, /* 0xD8 - LQ020000 - Q Capital */
	0x0052, /* 0xD9 - LR020000 - R Capital */
	0x00B9, /* 0xDA - ND011000 - One Superscript */
	0x00FB, /* 0xDB - LU150000 - u Circumflex Small */
	0x007D, /* 0xDC - SM140000 - Right Brace */
	0x00F9, /* 0xDD - LU130000 - u Grave Small */
	0x00FA, /* 0xDE - LU110000 - u Acute Small */
	0x00FF, /* 0xDF - LY170000 - y Diaeresis Small */
	0x00D6, /* 0xE0 - LO180000 - O Diaeresis Capital */
	0x00F7, /* 0xE1 - SA060000 - Divide Sign */
	0x0053, /* 0xE2 - LS020000 - S Capital */
	0x0054, /* 0xE3 - LT020000 - T Capital */
	0x0055, /* 0xE4 - LU020000 - U Capital */
	0x0056, /* 0xE5 - LV020000 - V Capital */
	0x0057, /* 0xE6 - LW020000 - W Capital */
	0x0058, /* 0xE7 - LX020000 - X Capital */
	0x0059, /* 0xE8 - LY020000 - Y Capital */
	0x005A, /* 0xE9 - LZ020000 - Z Capital */
	0x00B2, /* 0xEA - ND021000 - Two Superscript */
	0x00D4, /* 0xEB - LO160000 - O Circumflex Capital */
	0x005C, /* 0xEC - SM070000 - Backslash */
	0x00D2, /* 0xED - LO140000 - O Grave Capital */
	0x00D3, /* 0xEE - LO120000 - O Acute Capital */
	0x00D5, /* 0xEF - LO200000 - O Tilde Capital */
	0x0030, /* 0xF0 - ND100000 - Zero */
	0x0031, /* 0xF1 - ND010000 - One */
	0x0032, /* 0xF2 - ND020000 - Two */
	0x0033, /* 0xF3 - ND030000 - Three */
	0x0034, /* 0xF4 - ND040000 - Four */
	0x0035, /* 0xF5 - ND050000 - Five */
	0x0036, /* 0xF6 - ND060000 - Six */
	0x0037, /* 0xF7 - ND070000 - Seven */
	0x0038, /* 0xF8 - ND080000 - Eight */
	0x0039, /* 0xF9 - ND090000 - Nine */
	0x00B3, /* 0xFA - ND031000 - Three Superscript */
	0x00DB, /* 0xFB - LU160000 - U Circumflex Capital */
	0x005D, /* 0xFC - SM080000 - Right Bracket */
	0x00D9, /* 0xFD - LU140000 - U Grave Capital */
	0x00DA, /* 0xFE - LU120000 - U Acute Capital */
	0x009F  /* 0xFF - EO - EO */
};

static const Transcoder256Table::Transcoder256TableRecord gToTable[] =
{
	{0x0000, 0x00}, /* NUL - NUL */
	{0x0001, 0x01}, /* SOH - SOH */
	{0x0002, 0x02}, /* STX - STX */
	{0x0003, 0x03}, /* ETX - ETX */
	{0x0004, 0x37}, /* EOT - EOT */
	{0x0005, 0x2D}, /* ENQ - ENQ */
	{0x0006, 0x2E}, /* ACK - ACK */
	{0x0007, 0x2F}, /* BEL - BEL */
	{0x0008, 0x16}, /* BS - BS */
	{0x0009, 0x05}, /* HAT - HAT */
	{0x000A, 0x25}, /* LF - LF */
	{0x000B, 0x0B}, /* VT - VT */
	{0x000C, 0x0C}, /* FF - FF */
	{0x000D, 0x0D}, /* CR - CR */
	{0x000E, 0x0E}, /* SO - SO */
	{0x000F, 0x0F}, /* SI - SI */
	{0x0010, 0x10}, /* DLE - DLE */
	{0x0011, 0x11}, /* DC1 - DC1 */
	{0x0012, 0x12}, /* DC2 - DC2 */
	{0x0013, 0x13}, /* DC3 - DC3 */
	{0x0014, 0x3C}, /* DC4 - DC4 */
	{0x0015, 0x3D}, /* NAK - NAK */
	{0x0016, 0x32}, /* SYN - SYN */
	{0x0017, 0x26}, /* ETB - ETB */
	{0x0018, 0x18}, /* CAN - CAN */
	{0x0019, 0x19}, /* EM - EM */
	{0x001A, 0x3F}, /* SUB - SUB */
	{0x001B, 0x27}, /* ESC - ESC */
	{0x001C, 0x1C}, /* IFS - IFS */
	{0x001D, 0x1D}, /* IGS - IGS */
	{0x001E, 0x1E}, /* IRS - IRS */
	{0x001F, 0x1F}, /* IUS ITB - IUS ITB */
	{0x0020, 0x40}, /* SP010000 - Space */
	{0x0021, 0x4F}, /* SP020000 - Exclamation Point */
	{0x0022, 0x7F}, /* SP040000 - Quotation Marks */
	{0x0023, 0x7B}, /* SM010000 - Number Sign */
	{0x0024, 0x5B}, /* SC030000 - Dollar Sign */
	{0x0025, 0x6C}, /* SM020000 - Percent Sign */
	{0x0026, 0x50}, /* SM030000 - Ampersand */
	{0x0027, 0x7D}, /* SP050000 - Apostrophe */
	{0x0028, 0x4D}, /* SP060000 - Left Parenthesis */
	{0x0029, 0x5D}, /* SP070000 - Right Parenthesis */
	{0x002A, 0x5C}, /* SM040000 - Asterisk */
	{0x002B, 0x4E}, /* SA010000 - Plus Sign */
	{0x002C, 0x6B}, /* SP080000 - Comma */
	{0x002D, 0x60}, /* SP100000 - Hyphen/Minus Sign */
	{0x002E, 0x4B}, /* SP110000 - Period/Full Stop */
	{0x002F, 0x61}, /* SP120000 - Slash */
	{0x0030, 0xF0}, /* ND100000 - Zero */
	{0x0031, 0xF1}, /* ND010000 - One */
	{0x0032, 0xF2}, /* ND020000 - Two */
	{0x0033, 0xF3}, /* ND030000 - Three */
	{0x0034, 0xF4}, /* ND040000 - Four */
	{0x0035, 0xF5}, /* ND050000 - Five */
	{0x0036, 0xF6}, /* ND060000 - Six */
	{0x0037, 0xF7}, /* ND070000 - Seven */
	{0x0038, 0xF8}, /* ND080000 - Eight */
	{0x0039, 0xF9}, /* ND090000 - Nine */
	{0x003A, 0x7A}, /* SP130000 - Colon */
	{0x003B, 0x5E}, /* SP140000 - Semicolon */
	{0x003C, 0x4C}, /* SA030000 - Less Than Sign/Greater Than Sign (Arabic) */
	{0x003D, 0x7E}, /* SA040000 - Equal Sign */
	{0x003E, 0x6E}, /* SA050000 - Greater Than Sign/Less Than Sign (Arabic) */
	{0x003F, 0x6F}, /* SP150000 - Question Mark */
	{0x0040, 0xB5}, /* SM050000 - At Sign */
	{0x0041, 0xC1}, /* LA020000 - A Capital */
	{0x0042, 0xC2}, /* LB020000 - B Capital */
	{0x0043, 0xC3}, /* LC020000 - C Capital */
	{0x0044, 0xC4}, /* LD020000 - D Capital */
	{0x0045, 0xC5}, /* LE020000 - E Capital */
	{0x0046, 0xC6}, /* LF020000 - F Capital */
	{0x0047, 0xC7}, /* LG020000 - G Capital */
	{0x0048, 0xC8}, /* LH020000 - H Capital */
	{0x0049, 0xC9}, /* LI020000 - I Capital */
	{0x004A, 0xD1}, /* LJ020000 - J Capital */
	{0x004B, 0xD2}, /* LK020000 - K Capital */
	{0x004C, 0xD3}, /* LL020000 - L Capital */
	{0x004D, 0xD4}, /* LM020000 - M Capital */
	{0x004E, 0xD5}, /* LN020000 - N Capital */
	{0x004F, 0xD6}, /* LO020000 - O Capital */
	{0x0050, 0xD7}, /* LP020000 - P Capital */
	{0x0051, 0xD8}, /* LQ020000 - Q Capital */
	{0x0052, 0xD9}, /* LR020000 - R Capital */
	{0x0053, 0xE2}, /* LS020000 - S Capital */
	{0x0054, 0xE3}, /* LT020000 - T Capital */
	{0x0055, 0xE4}, /* LU020000 - U Capital */
	{0x0056, 0xE5}, /* LV020000 - V Capital */
	{0x0057, 0xE6}, /* LW020000 - W Capital */
	{0x0058, 0xE7}, /* LX020000 - X Capital */
	{0x0059, 0xE8}, /* LY020000 - Y Capital */
	{0x005A, 0xE9}, /* LZ020000 - Z Capital */
	{0x005B, 0x63}, /* SM060000 - Left Bracket */
	{0x005C, 0xEC}, /* SM070000 - Backslash */
	{0x005D, 0xFC}, /* SM080000 - Right Bracket */
	{0x005E, 0x5F}, /* SD150000 - Circumflex Accent */
	{0x005F, 0x6D}, /* SP090000 - Underline/Continuous Underscore */
	{0x0060, 0x79}, /* SD130000 - Grave Accent */
	{0x0061, 0x81}, /* LA010000 - a Small */
	{0x0062, 0x82}, /* LB010000 - b Small */
	{0x0063, 0x83}, /* LC010000 - c Small */
	{0x0064, 0x84}, /* LD010000 - d Small */
	{0x0065, 0x85}, /* LE010000 - e Small */
	{0x0066, 0x86}, /* LF010000 - f Small */
	{0x0067, 0x87}, /* LG010000 - g Small */
	{0x0068, 0x88}, /* LH010000 - h Small */
	{0x0069, 0x89}, /* LI010000 - i Small */
	{0x006A, 0x91}, /* LJ010000 - j Small */
	{0x006B, 0x92}, /* LK010000 - k Small */
	{0x006C, 0x93}, /* LL010000 - l Small */
	{0x006D, 0x94}, /* LM010000 - m Small */
	{0x006E, 0x95}, /* LN010000 - n Small */
	{0x006F, 0x96}, /* LO010000 - o Small */
	{0x0070, 0x97}, /* LP010000 - p Small */
	{0x0071, 0x98}, /* LQ010000 - q Small */
	{0x0072, 0x99}, /* LR010000 - r Small */
	{0x0073, 0xA2}, /* LS010000 - s Small */
	{0x0074, 0xA3}, /* LT010000 - t Small */
	{0x0075, 0xA4}, /* LU010000 - u Small */
	{0x0076, 0xA5}, /* LV010000 - v Small */
	{0x0077, 0xA6}, /* LW010000 - w Small */
	{0x0078, 0xA7}, /* LX010000 - x Small */
	{0x0079, 0xA8}, /* LY010000 - y Small */
	{0x007A, 0xA9}, /* LZ010000 - z Small */
	{0x007B, 0x43}, /* SM110000 - Left Brace */
	{0x007C, 0xBB}, /* SM130000 - Vertical Line/Logical OR */
	{0x007D, 0xDC}, /* SM140000 - Right Brace */
	{0x007E, 0x59}, /* SD190000 - Tilde Accent */
	{0x007F, 0x07}, /* DEL - DEL */
	{0x0080, 0x20}, /* DS - DS */
	{0x0081, 0x21}, /* SOS - SOS */
	{0x0082, 0x22}, /* FS - FS */
	{0x0083, 0x23}, /* WUS - WUS */
	{0x0084, 0x24}, /* BYP INP - BYP INP */
	{0x0085, 0x15}, /* NL - NL */
	{0x0086, 0x06}, /* RNL - RNL */
	{0x0087, 0x17}, /* POC - POC */
	{0x0088, 0x28}, /* SA - SA */
	{0x0089, 0x29}, /* SFE - SFE */
	{0x008A, 0x2A}, /* SM SW - SM SW */
	{0x008B, 0x2B}, /* CSP - CSP */
	{0x008C, 0x2C}, /* MFA - MFA */
	{0x008D, 0x09}, /* SPS - SPS */
	{0x008E, 0x0A}, /* RPT - RPT */
	{0x008F, 0x1B}, /* CUI - CUI */
	{0x0090, 0x30}, /* 0x30 - 0x30 */
	{0x0091, 0x31}, /* 0x31 - 0x31 */
	{0x0092, 0x1A}, /* UBS - UBS */
	{0x0093, 0x33}, /* IR - IR */
	{0x0094, 0x34}, /* PP - PP */
	{0x0095, 0x35}, /* TRN - TRN */
	{0x0096, 0x36}, /* NBS - NBS */
	{0x0097, 0x08}, /* GE - GE */
	{0x0098, 0x38}, /* SBS - SBS */
	{0x0099, 0x39}, /* IT - IT */
	{0x009A, 0x3A}, /* RFF - RFF */
	{0x009B, 0x3B}, /* CU3 - CU3 */
	{0x009C, 0x04}, /* SEL - SEL */
	{0x009D, 0x14}, /* RES ENP - RES ENP */
	{0x009E, 0x3E}, /* 0x3E - 0x3E */
	{0x009F, 0xFF}, /* EO - EO */
	{0x00A0, 0x41}, /* SP300000 - Required Space */
	{0x00A1, 0xAA}, /* SP030000 - Exclamation Point, Inverted */
	{0x00A2, 0xB0}, /* SC040000 - Cent Sign */
	{0x00A3, 0xB1}, /* SC020000 - Pound Sterling Sign */
	{0x00A5, 0xB2}, /* SC050000 - Yen Sign */
	{0x00A6, 0xCC}, /* SM650000 - Vertical Line, Broken */
	{0x00A7, 0x7C}, /* SM240000 - Section Symbol (USA)/Paragraph Symbol (Europe) */
	{0x00A8, 0xBD}, /* SD170000 - Diaeresis/Umlaut Accent */
	{0x00A9, 0xB4}, /* SM520000 - Copyright Symbol */
	{0x00AA, 0x9A}, /* SM210000 - Ordinal Indicator, Feminine */
	{0x00AB, 0x8A}, /* SP170000 - Left Angle Quotes */
	{0x00AC, 0xBA}, /* SM660000 - Logical NOT/End Of Line Symbol */
	{0x00AD, 0xCA}, /* SP320000 - Syllable Hyphen */
	{0x00AE, 0xAF}, /* SM530000 - Registered Trademark Symbol */
	{0x00AF, 0xBC}, /* SM150000 - Overline */
	{0x00B0, 0x90}, /* SM190000 - Degree Symbol */
	{0x00B1, 0x8F}, /* SA020000 - Plus or Minus Sign */
	{0x00B2, 0xEA}, /* ND021000 - Two Superscript */
	{0x00B3, 0xFA}, /* ND031000 - Three Superscript */
	{0x00B4, 0xBE}, /* SD110000 - Acute Accent */
	{0x00B5, 0xA0}, /* SM170000 - Micro Symbol */
	{0x00B6, 0xB6}, /* SM250000 - Paragraph Symbol (USA) */
	{0x00B7, 0xB3}, /* SD630000 - Middle Dot */
	{0x00B8, 0x9D}, /* SD410000 - Cedilla or Sedila Accent */
	{0x00B9, 0xDA}, /* ND011000 - One Superscript */
	{0x00BA, 0x9B}, /* SM200000 - Ordinal Indicator, Masculine */
	{0x00BB, 0x8B}, /* SP180000 - Right Angle Quotes */
	{0x00BC, 0xB7}, /* NF040000 - One Quarter */
	{0x00BD, 0xB8}, /* NF010000 - One Half */
	{0x00BE, 0xB9}, /* NF050000 - Three Quarters */
	{0x00BF, 0xAB}, /* SP160000 - Question Mark, Inverted */
	{0x00C0, 0x64}, /* LA140000 - A Grave Capital */
	{0x00C1, 0x65}, /* LA120000 - A Acute Capital */
	{0x00C2, 0x62}, /* LA160000 - A Circumflex Capital */
	{0x00C3, 0x66}, /* LA200000 - A Tilde Capital */
	{0x00C4, 0x4A}, /* LA180000 - A Diaeresis Capital */
	{0x00C5, 0x67}, /* LA280000 - A Overcircle Capital */
	{0x00C6, 0x9E}, /* LA520000 - ae Diphthong Capital */
	{0x00C7, 0x68}, /* LC420000 - C Cedilla Capital */
	{0x00C8, 0x74}, /* LE140000 - E Grave Capital */
	{0x00C9, 0x71}, /* LE120000 - E Acute Capital */
	{0x00CA, 0x72}, /* LE160000 - E Circumflex Capital */
	{0x00CB, 0x73}, /* LE180000 - E Diaeresis Capital */
	{0x00CC, 0x78}, /* LI140000 - I Grave Capital */
	{0x00CD, 0x75}, /* LI120000 - I Acute Capital */
	{0x00CE, 0x76}, /* LI160000 - I Circumflex Capital */
	{0x00CF, 0x77}, /* LI180000 - I Diaeresis Capital */
	{0x00D0, 0xAC}, /* LD620000 - D Stroke Capital/Eth Icelandic Capital */
	{0x00D1, 0x69}, /* LN200000 - N Tilde Capital */
	{0x00D2, 0xED}, /* LO140000 - O Grave Capital */
	{0x00D3, 0xEE}, /* LO120000 - O Acute Capital */
	{0x00D4, 0xEB}, /* LO160000 - O Circumflex Capital */
	{0x00D5, 0xEF}, /* LO200000 - O Tilde Capital */
	{0x00D6, 0xE0}, /* LO180000 - O Diaeresis Capital */
	{0x00D7, 0xBF}, /* SA070000 - Multiply Sign */
	{0x00D8, 0x80}, /* LO620000 - O Slash Capital */
	{0x00D9, 0xFD}, /* LU140000 - U Grave Capital */
	{0x00DA, 0xFE}, /* LU120000 - U Acute Capital */
	{0x00DB, 0xFB}, /* LU160000 - U Circumflex Capital */
	{0x00DC, 0x5A}, /* LU180000 - U Diaeresis Capital */
	{0x00DD, 0xAD}, /* LY120000 - Y Acute Capital */
	{0x00DE, 0xAE}, /* LT640000 - Thorn Icelandic Capital */
	{0x00DF, 0xA1}, /* LS610000 - Sharp s Small */
	{0x00E0, 0x44}, /* LA130000 - a Grave Small */
	{0x00E1, 0x45}, /* LA110000 - a Acute Small */
	{0x00E2, 0x42}, /* LA150000 - a Circumflex Small */
	{0x00E3, 0x46}, /* LA190000 - a Tilde Small */
	{0x00E4, 0xC0}, /* LA170000 - a Diaeresis Small */
	{0x00E5, 0x47}, /* LA270000 - a Overcircle Small */
	{0x00E6, 0x9C}, /* LA510000 - ae Diphthong Small */
	{0x00E7, 0x48}, /* LC410000 - c Cedilla Small */
	{0x00E8, 0x54}, /* LE130000 - e Grave Small */
	{0x00E9, 0x51}, /* LE110000 - e Acute Small */
	{0x00EA, 0x52}, /* LE150000 - e Circumflex Small */
	{0x00EB, 0x53}, /* LE170000 - e Diaeresis Small */
	{0x00EC, 0x58}, /* LI130000 - i Grave Small */
	{0x00ED, 0x55}, /* LI110000 - i Acute Small */
	{0x00EE, 0x56}, /* LI150000 - i Circumflex Small */
	{0x00EF, 0x57}, /* LI170000 - i Diaeresis Small */
	{0x00F0, 0x8C}, /* LD630000 - eth Icelandic Small */
	{0x00F1, 0x49}, /* LN190000 - n Tilde Small */
	{0x00F2, 0xCD}, /* LO130000 - o Grave Small */
	{0x00F3, 0xCE}, /* LO110000 - o Acute Small */
	{0x00F4, 0xCB}, /* LO150000 - o Circumflex Small */
	{0x00F5, 0xCF}, /* LO190000 - o Tilde Small */
	{0x00F6, 0x6A}, /* LO170000 - o Diaeresis Small */
	{0x00F7, 0xE1}, /* SA060000 - Divide Sign */
	{0x00F8, 0x70}, /* LO610000 - o Slash Small */
	{0x00F9, 0xDD}, /* LU130000 - u Grave Small */
	{0x00FA, 0xDE}, /* LU110000 - u Acute Small */
	{0x00FB, 0xDB}, /* LU150000 - u Circumflex Small */
	{0x00FC, 0xD0}, /* LU170000 - u Diaeresis Small */
	{0x00FD, 0x8D}, /* LY110000 - y Acute Small */
	{0x00FE, 0x8E}, /* LT630000 - Thorn Icelandic Small */
	{0x00FF, 0xDF}, /* LY170000 - y Diaeresis Small */
	{0x203E, 0xBC}, /* SM150000 - Overline */
	{0x20AC, 0x9F}, /* SC200000 - Euro symbol */
	{0xFF01, 0x4F}, /* SP020000 - Exclamation Point */
	{0xFF02, 0x7F}, /* SP040000 - Quotation Marks */
	{0xFF03, 0x7B}, /* SM010000 - Number Sign */
	{0xFF04, 0x5B}, /* SC030000 - Dollar Sign */
	{0xFF05, 0x6C}, /* SM020000 - Percent Sign */
	{0xFF06, 0x50}, /* SM030000 - Ampersand */
	{0xFF07, 0x7D}, /* SP050000 - Apostrophe */
	{0xFF08, 0x4D}, /* SP060000 - Left Parenthesis */
	{0xFF09, 0x5D}, /* SP070000 - Right Parenthesis */
	{0xFF0A, 0x5C}, /* SM040000 - Asterisk */
	{0xFF0B, 0x4E}, /* SA010000 - Plus Sign */
	{0xFF0C, 0x6B}, /* SP080000 - Comma */
	{0xFF0D, 0x60}, /* SP100000 - Hyphen/Minus Sign */
	{0xFF0E, 0x4B}, /* SP110000 - Period/Full Stop */
	{0xFF0F, 0x61}, /* SP120000 - Slash */
	{0xFF10, 0xF0}, /* ND100000 - Zero */
	{0xFF11, 0xF1}, /* ND010000 - One */
	{0xFF12, 0xF2}, /* ND020000 - Two */
	{0xFF13, 0xF3}, /* ND030000 - Three */
	{0xFF14, 0xF4}, /* ND040000 - Four */
	{0xFF15, 0xF5}, /* ND050000 - Five */
	{0xFF16, 0xF6}, /* ND060000 - Six */
	{0xFF17, 0xF7}, /* ND070000 - Seven */
	{0xFF18, 0xF8}, /* ND080000 - Eight */
	{0xFF19, 0xF9}, /* ND090000 - Nine */
	{0xFF1A, 0x7A}, /* SP130000 - Colon */
	{0xFF1B, 0x5E}, /* SP140000 - Semicolon */
	{0xFF1C, 0x4C}, /* SA030000 - Less Than Sign/Greater Than Sign (Arabic) */
	{0xFF1D, 0x7E}, /* SA040000 - Equal Sign */
	{0xFF1E, 0x6E}, /* SA050000 - Greater Than Sign/Less Than Sign (Arabic) */
	{0xFF1F, 0x6F}, /* SP150000 - Question Mark */
	{0xFF20, 0xB5}, /* SM050000 - At Sign */
	{0xFF21, 0xC1}, /* LA020000 - A Capital */
	{0xFF22, 0xC2}, /* LB020000 - B Capital */
	{0xFF23, 0xC3}, /* LC020000 - C Capital */
	{0xFF24, 0xC4}, /* LD020000 - D Capital */
	{0xFF25, 0xC5}, /* LE020000 - E Capital */
	{0xFF26, 0xC6}, /* LF020000 - F Capital */
	{0xFF27, 0xC7}, /* LG020000 - G Capital */
	{0xFF28, 0xC8}, /* LH020000 - H Capital */
	{0xFF29, 0xC9}, /* LI020000 - I Capital */
	{0xFF2A, 0xD1}, /* LJ020000 - J Capital */
	{0xFF2B, 0xD2}, /* LK020000 - K Capital */
	{0xFF2C, 0xD3}, /* LL020000 - L Capital */
	{0xFF2D, 0xD4}, /* LM020000 - M Capital */
	{0xFF2E, 0xD5}, /* LN020000 - N Capital */
	{0xFF2F, 0xD6}, /* LO020000 - O Capital */
	{0xFF30, 0xD7}, /* LP020000 - P Capital */
	{0xFF31, 0xD8}, /* LQ020000 - Q Capital */
	{0xFF32, 0xD9}, /* LR020000 - R Capital */
	{0xFF33, 0xE2}, /* LS020000 - S Capital */
	{0xFF34, 0xE3}, /* LT020000 - T Capital */
	{0xFF35, 0xE4}, /* LU020000 - U Capital */
	{0xFF36, 0xE5}, /* LV020000 - V Capital */
	{0xFF37, 0xE6}, /* LW020000 - W Capital */
	{0xFF38, 0xE7}, /* LX020000 - X Capital */
	{0xFF39, 0xE8}, /* LY020000 - Y Capital */
	{0xFF3A, 0xE9}, /* LZ020000 - Z Capital */
	{0xFF3B, 0x63}, /* SM060000 - Left Bracket */
	{0xFF3C, 0xEC}, /* SM070000 - Backslash */
	{0xFF3D, 0xFC}, /* SM080000 - Right Bracket */
	{0xFF3E, 0x5F}, /* SD150000 - Circumflex Accent */
	{0xFF3F, 0x6D}, /* SP090000 - Underline/Continuous Underscore */
	{0xFF40, 0x79}, /* SD130000 - Grave Accent */
	{0xFF41, 0x81}, /* LA010000 - a Small */
	{0xFF42, 0x82}, /* LB010000 - b Small */
	{0xFF43, 0x83}, /* LC010000 - c Small */
	{0xFF44, 0x84}, /* LD010000 - d Small */
	{0xFF45, 0x85}, /* LE010000 - e Small */
	{0xFF46, 0x86}, /* LF010000 - f Small */
	{0xFF47, 0x87}, /* LG010000 - g Small */
	{0xFF48, 0x88}, /* LH010000 - h Small */
	{0xFF49, 0x89}, /* LI010000 - i Small */
	{0xFF4A, 0x91}, /* LJ010000 - j Small */
	{0xFF4B, 0x92}, /* LK010000 - k Small */
	{0xFF4C, 0x93}, /* LL010000 - l Small */
	{0xFF4D, 0x94}, /* LM010000 - m Small */
	{0xFF4E, 0x95}, /* LN010000 - n Small */
	{0xFF4F, 0x96}, /* LO010000 - o Small */
	{0xFF50, 0x97}, /* LP010000 - p Small */
	{0xFF51, 0x98}, /* LQ010000 - q Small */
	{0xFF52, 0x99}, /* LR010000 - r Small */
	{0xFF53, 0xA2}, /* LS010000 - s Small */
	{0xFF54, 0xA3}, /* LT010000 - t Small */
	{0xFF55, 0xA4}, /* LU010000 - u Small */
	{0xFF56, 0xA5}, /* LV010000 - v Small */
	{0xFF57, 0xA6}, /* LW010000 - w Small */
	{0xFF58, 0xA7}, /* LX010000 - x Small */
	{0xFF59, 0xA8}, /* LY010000 - y Small */
	{0xFF5A, 0xA9}, /* LZ010000 - z Small */
	{0xFF5B, 0x43}, /* SM110000 - Left Brace */
	{0xFF5C, 0xBB}, /* SM130000 - Vertical Line/Logical OR */
	{0xFF5D, 0xDC}, /* SM140000 - Right Brace */
	{0xFF5E, 0x59}, /* SD190000 - Tilde Accent */
	{0xFFE0, 0xB0}, /* SC040000 - Cent Sign */
	{0xFFE1, 0xB1}, /* SC020000 - Pound Sterling Sign */
	{0xFFE2, 0xBA}, /* SM660000 - Logical NOT/End Of Line Symbol */
	{0xFFE3, 0xBC}, /* SM150000 - Overline */
	{0xFFE4, 0xCC}, /* SM650000 - Vertical Line, Broken */
	{0xFFE5, 0xB2}  /* SC050000 - Yen Sign */
};
static const int gToTableSz = 357;
 

TranscoderIBM01141::TranscoderIBM01141() :
	Transcoder256Table(gFromTable,gToTable,gToTableSz)
{

}


TranscoderIBM01141::~TranscoderIBM01141()
{

}

}
}
