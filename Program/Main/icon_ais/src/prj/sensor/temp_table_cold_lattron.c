#include "temp_table_common.h"
#include "temp_table_cold_lattron.h"

/****************************************************************************
 * Thermistor Tabel
 *
 * - Cold water 
 * - Lattron SEF3I145.0FS-P, 13QA-LS
 *   B(0/100) = 3970K(1.5%)
 *   R@4'C = 145 kohm
 * - Pull-Up register : 140kohm
 * - ADC:@10-bit resolution
 * - Range : 636 adc ~ 176 adc ( -5.0'C ~ 40.0'C ) 
 */

#define  MIN_ADC_COLD_WATER_LATTRON  176
#define  MAX_ADC_COLD_WATER_LATTRON  636

const static int temp_cold_water_lattron_table[] = 
{
    400, 398, 397, 395, 393, 392, 390, 388, 387, 385,
    383, 382, 380, 378, 377, 375, 373, 372, 370, 369,
    367, 366, 364, 363, 361, 360, 358, 357, 355, 353,
    352, 350, 349, 347, 346, 344, 343, 341, 340, 339,
    337, 336, 334, 333, 331, 330, 329, 328, 326, 325,
    324, 323, 321, 320, 319, 317, 316, 314, 313, 311,
    310, 309, 308, 306, 305, 304, 303, 301, 300, 299,
    298, 296, 295, 294, 293, 291, 290, 289, 288, 287,
    286, 284, 283, 282, 281, 280, 279, 278, 276, 275,
    274, 273, 271, 270, 269, 268, 267, 266, 264, 263,
    262, 261, 260, 259, 258, 257, 256, 254, 253, 252,
    251, 250, 249, 248, 247, 246, 244, 243, 242, 241,
    240, 239, 238, 237, 236, 234, 233, 232, 231, 230,
    229, 228, 227, 226, 225, 224, 223, 222, 221, 220,
    219, 218, 217, 216, 215, 214, 213, 212, 211, 210,
    209, 208, 207, 206, 205, 204, 203, 202, 201, 200,
    199, 198, 197, 196, 195, 194, 193, 192, 191, 190,
    189, 188, 187, 186, 185, 185, 184, 183, 182, 181,
    180, 179, 178, 177, 176, 175, 174, 173, 172, 171,
    170, 169, 168, 167, 166, 165, 165, 164, 163, 162,
    161, 160, 159, 158, 157, 156, 155, 155, 154, 153,
    152, 151, 150, 149, 148, 148, 147, 146, 145, 144,
    143, 143, 142, 141, 140, 139, 138, 137, 136, 135,
    135, 134, 133, 132, 131, 130, 129, 128, 128, 127,
    126, 125, 124, 123, 123, 122, 121, 120, 119, 118,
    118, 117, 116, 115, 114, 113, 113, 112, 111, 110,
    109, 108, 108, 107, 106, 105, 104, 103, 103, 102,
    101, 100, 99, 98, 98, 97, 96, 95, 94, 93,
    93, 92, 91, 90, 89, 88, 88, 87, 86, 85,
    84, 83, 83, 82, 81, 80, 79, 78, 78, 77,
    76, 75, 74, 73, 73, 72, 71, 70, 69, 68,
    68, 67, 66, 65, 65, 64, 63, 62, 62, 61,
    60, 59, 58, 58, 57, 56, 55, 54, 53, 53,
    52, 51, 50, 49, 48, 48, 47, 46, 45, 45,
    44, 43, 42, 42, 41, 40, 39, 38, 38, 37,
    36, 35, 35, 34, 33, 32, 32, 31, 30, 29,
    28, 28, 27, 26, 25, 24, 23, 23, 22, 21,
    20, 19, 18, 18, 17, 16, 15, 15, 14, 13,
    12, 12, 11, 10, 9, 8, 8, 7, 6, 5,
    5, 4, 3, 2,  2, 1, 0, -1, -2, -2,
    -3, -4, -5, -5, -6, -7, -8, -8, -9, -10,
    -11, -12, -12, -13, -14, -15, -15, -16, -17, -18,
    -18, -19, -20, -21, -22, -23, -23, -24, -25, -26,
    -27, -28, -28, -29, -30, -31, -32, -32, -33, -34,
    -35, -35, -36, -37, -38, -38, -39, -40, -41, -42,
    -42, -43, -44, -45, -45, -46, -47, -48, -48, -49,
    -50
};



// 소수점 2digit ( 2 deicmal place )
// ADC 1780 ~ 2241
// 10.00'C ~ -5.00'c
// 850 -> 8.50'C
#define  MIN_ADC_COLD_WATER_LATTRON_2DP  1786U
#define  MAX_ADC_COLD_WATER_LATTRON_2DP  2543U
const static int temp_cold_water_lattron_table_2dp[] = 
{
    1000, 998, 996, 994, 992, 990, 988, 986, 984, 982, 
    980, 978, 976, 973, 971, 969, 967, 965, 963, 961, 
    959, 957, 955, 953, 951, 949, 947, 945, 943, 941, 
    939, 937, 935, 933, 931, 929, 927, 924, 922, 920, 
    918, 916, 914, 912, 910, 908, 906, 904, 902, 900, 
    898, 896, 894, 892, 890, 888, 886, 884, 882, 880, 
    878, 876, 873, 871, 869, 867, 865, 863, 861, 859, 
    857, 855, 853, 851, 849, 847, 845, 843, 841, 839, 
    837, 835, 833, 831, 829, 827, 824, 822, 820, 818, 
    816, 814, 812, 810, 808, 806, 804, 802, 800, 798, 
    796, 794, 792, 790, 788, 786, 784, 782, 780, 778, 
    776, 773, 771, 769, 767, 765, 763, 761, 759, 757, 
    755, 753, 751, 749, 747, 745, 743, 741, 739, 737, 
    735, 733, 731, 729, 727, 724, 722, 720, 718, 716, 
    714, 712, 710, 708, 706, 704, 702, 700, 698, 696, 
    694, 692, 690, 688, 686, 684, 682, 680, 678, 676, 
    674, 672, 670, 668, 666, 664, 662, 660, 658, 656, 
    654, 652, 650, 648, 646, 644, 642, 640, 638, 636, 
    634, 632, 630, 628, 626, 624, 622, 620, 618, 616, 
    614, 612, 610, 608, 606, 604, 602, 600, 598, 596, 
    594, 592, 590, 588, 586, 584, 582, 580, 578, 576, 
    574, 572, 570, 568, 566, 564, 562, 560, 558, 556, 
    554, 552, 550, 548, 546, 544, 542, 540, 538, 536, 
    534, 532, 530, 528, 526, 524, 522, 520, 518, 516, 
    514, 512, 510, 508, 506, 504, 502, 500, 498, 496, 
    494, 492, 490, 488, 486, 484, 482, 480, 478, 476, 
    475, 473, 471, 469, 467, 465, 463, 461, 459, 457, 
    455, 453, 451, 449, 447, 445, 443, 441, 439, 437, 
    435, 433, 431, 429, 427, 425, 424, 422, 420, 418, 
    416, 414, 412, 410, 408, 406, 404, 402, 400, 398, 
    396, 394, 392, 390, 388, 386, 384, 382, 380, 378, 
    376, 375, 373, 371, 369, 367, 365, 363, 361, 359, 
    357, 355, 353, 351, 349, 347, 345, 343, 341, 339, 
    337, 335, 333, 331, 329, 327, 325, 324, 322, 320, 
    318, 316, 314, 312, 310, 308, 306, 304, 302, 300, 
    298, 296, 294, 292, 290, 288, 286, 284, 282, 280, 
    278, 276, 275, 273, 271, 269, 267, 265, 263, 261, 
    259, 257, 255, 253, 251, 249, 247, 245, 243, 241, 
    239, 237, 235, 233, 231, 229, 227, 225, 224, 222, 
    220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 
    200, 198, 196, 194, 192, 190, 188, 186, 184, 182, 
    180, 178, 176, 175, 173, 171, 169, 167, 165, 163, 
    161, 159, 157, 155, 153, 151, 149, 147, 145, 143, 
    141, 139, 137, 135, 133, 131, 129, 127, 125, 124, 
    122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 
    102, 100, 98, 96, 94, 92, 90, 88, 86, 84, 
    82, 80, 78, 76, 75, 73, 71, 69, 67, 65, 
    63, 61, 59, 57, 55, 53, 51, 49, 47, 45, 
    43, 41, 39, 37, 35, 33, 31, 29, 27, 25, 
    24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 
    4, 2, 0, -2, -4, -6, -8, -10, -12, -13, 
    -15, -17, -19, -21, -23, -25, -27, -29, -31, -33, 
    -35, -37, -38, -40, -42, -44, -46, -48, -50, -52, 
    -54, -56, -58, -60, -62, -63, -65, -67, -69, -71, 
    -73, -75, -77, -79, -81, -83, -85, -87, -88, -90, 
    -92, -94, -96, -98, -100, -102, -104, -106, -108, -110, 
    -112, -114, -116, -118, -120, -122, -124, -125, -127, -129, 
    -131, -133, -135, -137, -139, -141, -143, -145, -147, -149, 
    -151, -153, -155, -157, -159, -161, -163, -165, -167, -169, 
    -171, -173, -175, -176, -178, -180, -182, -184, -186, -188, 
    -190, -192, -194, -196, -198, -200, -202, -204, -206, -208, 
    -210, -212, -214, -216, -218, -220, -222, -224, -225, -227, 
    -229, -231, -233, -235, -237, -239, -241, -243, -245, -247, 
    -249, -251, -253, -255, -257, -259, -261, -263, -265, -267, 
    -269, -271, -273, -275, -276, -278, -280, -282, -284, -286, 
    -288, -290, -292, -294, -296, -298, -300, -302, -304, -306, 
    -308, -310, -312, -314, -316, -318, -320, -322, -324, -325, 
    -327, -329, -331, -333, -335, -337, -339, -341, -343, -345, 
    -347, -349, -351, -353, -355, -357, -359, -361, -363, -365, 
    -367, -369, -371, -373, -375, -376, -378, -380, -382, -384, 
    -386, -388, -390, -392, -394, -396, -398, -400, -402, -404, 
    -406, -408, -410, -412, -414, -416, -418, -420, -422, -424, 
    -426, -428, -430, -432, -434, -436, -438, -440, -442, -444, 
    -446, -448, -450, -452, -454, -456, -458, -460, -462, -464, 
    -466, -468, -470, -472, -474, -476, -478, -480, -482, -484, 
    -486, -488, -490, -492, -494, -496, -498, -500
};

TEMP_T   ConvAdc2Temp_Lattron_ColdWater(U16 mu16Adc)
{
    U16 mu16Index;
    int val;
    TEMP_T temp;


    if( mu16Adc > MAX_ADC_COLD_WATER_LATTRON_2DP 
            ||  mu16Adc < MIN_ADC_COLD_WATER_LATTRON_2DP )
    {
        // 10bit
        mu16Index = GetTempTableIndex( 
                RES_ADC_TABLE_10BIT, 
                mu16Adc, 
                MIN_ADC_COLD_WATER_LATTRON, 
                MAX_ADC_COLD_WATER_LATTRON );
        val = temp_cold_water_lattron_table[ mu16Index ];
        temp = ConvInt2Temp( val );
    }
    else
    {
        // 12bit
        mu16Index = GetTempTableIndex( 
                RES_ADC_TABLE_12BIT, 
                mu16Adc, 
                MIN_ADC_COLD_WATER_LATTRON_2DP, 
                MAX_ADC_COLD_WATER_LATTRON_2DP );
        val = temp_cold_water_lattron_table_2dp[ mu16Index ];
        temp = ConvInt2Temp2dp( val );
    }
            
    return temp;
}