// ex11
// Builtin types

typedef unsigned int word;
typedef unsigned char byte;

/*** PT01AD - Port AD Data Register; 0x00000270 ***/
typedef union {
    word Word;
    /* Overlapped registers: */
    struct {
        /*** PT0AD - Port ADH Data Register; 0x00000270 ***/
        union {
            byte Byte;
            struct {
                byte PT0AD0 : 1;                                       /* Port AD data bit 0 */
                byte PT0AD1 : 1;                                       /* Port AD data bit 1 */
                byte PT0AD2 : 1;                                       /* Port AD data bit 2 */
                byte PT0AD3 : 1;                                       /* Port AD data bit 3 */
                byte : 1;
                byte : 1;
                byte : 1;
                byte : 1;
            } Bits;
            struct {
                byte grpPT0AD : 4;
                byte : 1;
                byte : 1;
                byte : 1;
                byte : 1;
            } MergedBits;
        } PT0ADSTR;
#define PT0AD                       _PT01AD.Overlap_STR.PT0ADSTR.Byte
#define PT0AD_PT0AD0                _PT01AD.Overlap_STR.PT0ADSTR.Bits.PT0AD0
#define PT0AD_PT0AD1                _PT01AD.Overlap_STR.PT0ADSTR.Bits.PT0AD1
#define PT0AD_PT0AD2                _PT01AD.Overlap_STR.PT0ADSTR.Bits.PT0AD2
#define PT0AD_PT0AD3                _PT01AD.Overlap_STR.PT0ADSTR.Bits.PT0AD3
#define PT0AD_PT0AD                 _PT01AD.Overlap_STR.PT0ADSTR.MergedBits.grpPT0AD

#define PT0AD_PT0AD0_MASK           1U
#define PT0AD_PT0AD1_MASK           2U
#define PT0AD_PT0AD2_MASK           4U
#define PT0AD_PT0AD3_MASK           8U
#define PT0AD_PT0AD_MASK            15U
#define PT0AD_PT0AD_BITNUM          0U


        /*** PT1AD - Port ADL Data Register; 0x00000271 ***/
        union {
            byte Byte;
            struct {
                byte PT1AD0 : 1;                                       /* Port AD data bit 0 */
                byte PT1AD1 : 1;                                       /* Port AD data bit 1 */
                byte PT1AD2 : 1;                                       /* Port AD data bit 2 */
                byte PT1AD3 : 1;                                       /* Port AD data bit 3 */
                byte PT1AD4 : 1;                                       /* Port AD data bit 4 */
                byte PT1AD5 : 1;                                       /* Port AD data bit 5 */
                byte PT1AD6 : 1;                                       /* Port AD data bit 6 */
                byte PT1AD7 : 1;                                       /* Port AD data bit 7 */
            } Bits;
        } PT1ADSTR;
#define PT1AD                       _PT01AD.Overlap_STR.PT1ADSTR.Byte
#define PT1AD_PT1AD0                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD0
#define PT1AD_PT1AD1                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD1
#define PT1AD_PT1AD2                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD2
#define PT1AD_PT1AD3                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD3
#define PT1AD_PT1AD4                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD4
#define PT1AD_PT1AD5                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD5
#define PT1AD_PT1AD6                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD6
#define PT1AD_PT1AD7                _PT01AD.Overlap_STR.PT1ADSTR.Bits.PT1AD7

#define PT1AD_PT1AD0_MASK           1U
#define PT1AD_PT1AD1_MASK           2U
#define PT1AD_PT1AD2_MASK           4U
#define PT1AD_PT1AD3_MASK           8U
#define PT1AD_PT1AD4_MASK           16U
#define PT1AD_PT1AD5_MASK           32U
#define PT1AD_PT1AD6_MASK           64U
#define PT1AD_PT1AD7_MASK           128U

    } Overlap_STR;

    struct {
        word PT1AD0 : 1;                                       /* Port AD data bit 0 */
        word PT1AD1 : 1;                                       /* Port AD data bit 1 */
        word PT1AD2 : 1;                                       /* Port AD data bit 2 */
        word PT1AD3 : 1;                                       /* Port AD data bit 3 */
        word PT1AD4 : 1;                                       /* Port AD data bit 4 */
        word PT1AD5 : 1;                                       /* Port AD data bit 5 */
        word PT1AD6 : 1;                                       /* Port AD data bit 6 */
        word PT1AD7 : 1;                                       /* Port AD data bit 7 */
        word PT0AD0 : 1;                                       /* Port AD data bit 8 */
        word PT0AD1 : 1;                                       /* Port AD data bit 9 */
        word PT0AD2 : 1;                                       /* Port AD data bit 10 */
        word PT0AD3 : 1;                                       /* Port AD data bit 11 */
        word : 1;
        word : 1;
        word : 1;
        word : 1;
    } Bits;
    struct {
        word grpPT1AD : 8;
        word grpPT0AD : 4;
        word : 1;
        word : 1;
        word : 1;
        word : 1;
    } MergedBits;
} PT01ADSTR;
extern volatile PT01ADSTR _PT01AD /*@(REG_BASE + 0x00000270UL)*/;
#define PT01AD                          _PT01AD.Word
#define PT01AD_PT1AD0                   _PT01AD.Bits.PT1AD0
#define PT01AD_PT1AD1                   _PT01AD.Bits.PT1AD1
#define PT01AD_PT1AD2                   _PT01AD.Bits.PT1AD2
#define PT01AD_PT1AD3                   _PT01AD.Bits.PT1AD3
#define PT01AD_PT1AD4                   _PT01AD.Bits.PT1AD4
#define PT01AD_PT1AD5                   _PT01AD.Bits.PT1AD5
#define PT01AD_PT1AD6                   _PT01AD.Bits.PT1AD6
#define PT01AD_PT1AD7                   _PT01AD.Bits.PT1AD7
#define PT01AD_PT0AD0                   _PT01AD.Bits.PT0AD0
#define PT01AD_PT0AD1                   _PT01AD.Bits.PT0AD1
#define PT01AD_PT0AD2                   _PT01AD.Bits.PT0AD2
#define PT01AD_PT0AD3                   _PT01AD.Bits.PT0AD3
#define PT01AD_PT1AD                    _PT01AD.MergedBits.grpPT1AD
#define PT01AD_PT0AD                    _PT01AD.MergedBits.grpPT0AD

#define PT01AD_PT1AD0_MASK              1U
#define PT01AD_PT1AD1_MASK              2U
#define PT01AD_PT1AD2_MASK              4U
#define PT01AD_PT1AD3_MASK              8U
#define PT01AD_PT1AD4_MASK              16U
#define PT01AD_PT1AD5_MASK              32U
#define PT01AD_PT1AD6_MASK              64U
#define PT01AD_PT1AD7_MASK              128U
#define PT01AD_PT0AD0_MASK              256U
#define PT01AD_PT0AD1_MASK              512U
#define PT01AD_PT0AD2_MASK              1024U
#define PT01AD_PT0AD3_MASK              2048U
#define PT01AD_PT1AD_MASK               255U
#define PT01AD_PT1AD_BITNUM             0U
#define PT01AD_PT0AD_MASK               3840U
#define PT01AD_PT0AD_BITNUM             8U

PT01ADSTR var1;
const PT01ADSTR var2;

int f1()
{
    PT1AD_PT1AD0 = 1;
    var1.Overlap_STR.PT0ADSTR.Bits.PT0AD2 = 1;

	return 0;
}