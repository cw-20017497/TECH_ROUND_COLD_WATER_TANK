#include "temp_table_common.h"
#include "temp_table_cold.h"

/****************************************************************************
 * Thermistor Tabel
 *
 * - Cold water / Room Water
 * - A3T-44J-JP 
 *   B(0/100) = 3970K(1.5%)
 *   R@4'C = 134 kohm
 * - Pull-Up register : 140kohm
 * - ADC:@10-bit resolution
 * - Range : 600 adc ~ 50 adc ( -3.5'C ~ 75.5'C ) 
 */

#define  MIN_ADC_COLD_WATER  50
#define  MAX_ADC_COLD_WATER  600

const static int temp_cold_water_table[] = 
{
    755, 750, 745, 735, 730, 725, 720, 715, 710, 705, 
    700, 695, 690, 685, 680, 675, 670, 665, 660, 655, 
    650, 645, 643, 640, 635, 630, 625, 623, 620, 615, 
    610, 608, 605, 600, 595, 593, 590, 585, 583, 580, 
    575, 573, 570, 565, 563, 560, 555, 553, 550, 548, 
    545, 540, 538, 535, 533, 530, 528, 525, 520, 518, 
    515, 513, 510, 508, 505, 503, 500, 498, 495, 493, 
    490, 488, 485, 483, 480, 478, 475, 473, 470, 468, 
    467, 465, 463, 460, 458, 455, 453, 450, 448, 447, 
    445, 443, 440, 438, 437, 435, 433, 430, 428, 427, 
    425, 423, 420, 418, 417, 415, 413, 410, 408, 407, 
    405, 403, 402, 400, 398, 397, 395, 393, 392, 390, 
    388, 385, 383, 382, 380, 378, 377, 375, 373, 372, 
    370, 369, 368, 366, 365, 363, 362, 360, 358, 357, 
    355, 353, 352, 350, 348, 347, 345, 344, 343, 341, 
    340, 338, 337, 335, 334, 333, 331, 330, 328, 327, 
    325, 324, 323, 321, 320, 318, 317, 315, 314, 313, 
    311, 310, 309, 308, 306, 305, 303, 302, 300, 299, 
    298, 296, 295, 294, 293, 291, 290, 289, 288, 286, 
    285, 284, 283, 281, 280, 279, 278, 276, 275, 274, 
    273, 271, 270, 269, 268, 266, 265, 264, 263, 262, 
    261, 260, 259, 258, 256, 255, 254, 253, 251, 250, 
    249, 248, 247, 246, 245, 244, 243, 241, 240, 239, 
    238, 237, 236, 235, 234, 233, 231, 230, 229, 228, 
    227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 
    216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 
    206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 
    196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 
    186, 185, 184, 183, 183, 182, 181, 180, 179, 178, 
    177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 
    167, 166, 165, 164, 163, 163, 162, 161, 160, 159, 
    158, 157, 156, 155, 154, 153, 153, 152, 151, 150, 
    149, 148, 147, 146, 145, 144, 143, 143, 142, 141, 
    140, 139, 138, 138, 137, 136, 135, 134, 133, 132, 
    131, 130, 129, 128, 128, 127, 126, 125, 124, 123, 
    123, 122, 121, 120, 119, 118, 118, 117, 116, 115, 
    114, 113, 113, 112, 111, 110, 109, 108, 108, 107, 
    106, 105, 104, 103, 103, 102, 101, 100, 99, 98, 
    98, 97, 96, 95, 94, 93, 93, 92, 91, 90, 
    89, 88, 88, 87, 86, 85, 84, 83, 83, 82, 
    81, 80, 79, 78, 78, 77, 76, 75, 74, 74, 
    73, 73, 72, 71, 70, 69, 68, 68, 67, 66, 
    65, 64, 63, 63, 62, 61, 60, 59, 59, 58, 
    58, 57, 56, 55, 54, 53, 53, 52, 51, 50, 
    49, 48, 48, 47, 46, 45, 44, 44, 43, 43, 
    42, 41, 40, 39, 38, 38, 37, 36, 35, 34, 
    34, 33, 33, 32, 31, 30, 29, 28, 28, 27, 
    26, 25, 24, 24, 23, 23, 22, 21, 20, 19, 
    18, 18, 17, 16, 15, 14, 14, 13, 13, 12, 
    11, 10, 9, 8, 8, 7, 6, 5, 4, 4, 
    3, 3, 2, 1, 0, -1, -2, -2, -3, -4,
    -5, -6, -6, -7, -8, -8, -9, -10, -11, -11,
    -12, -13, -14, -15, -16, -16, -17, -18, -18, -19,
    -20, -21, -21, -22, -23, -23, -24, -25, -26, -26,
    -27, -28, -29, -30, -31, -31, -32, -33, -33, -34,
    -35 
};



// 소수점 2digit ( 2 deicmal place )
// ADC 1780 ~ 2241
// 8.5'C ~ -0.5'c
// 850 -> 8.50'C
#define  MIN_ADC_COLD_WATER_2DP  1780U
#define  MAX_ADC_COLD_WATER_2DP  2241U
const static int temp_cold_water_table_2dp[] = 
{
    850, 848, 846, 844, 842, 840, 838, 835, 833, 831, 
    829, 827, 825, 823, 821, 819, 817, 815, 812, 810, 
    808, 806, 804, 802, 800, 800, 798, 796, 794, 792, 
    790, 788, 786, 784, 782, 780, 778, 776, 774, 772, 
    770, 768, 766, 764, 762, 760, 758, 756, 754, 752, 
    750, 748, 746, 744, 742, 740, 738, 735, 733, 731, 
    729, 727, 725, 723, 721, 719, 717, 715, 713, 710, 
    708, 706, 704, 702, 700, 698, 696, 694, 692, 690, 
    688, 687, 685, 683, 681, 679, 677, 675, 673, 671, 
    669, 667, 665, 663, 662, 660, 658, 656, 654, 652, 
    650, 648, 646, 644, 642, 640, 638, 636, 634, 632, 
    630, 628, 626, 624, 622, 620, 618, 616, 614, 612, 
    610, 608, 606, 604, 602, 600, 598, 596, 594, 592, 
    590, 588, 586, 584, 582, 580, 578, 576, 574, 572, 
    570, 568, 566, 564, 562, 560, 558, 556, 554, 552, 
    550, 548, 546, 544, 542, 540, 538, 537, 535, 533, 
    531, 529, 527, 525, 523, 521, 519, 517, 515, 513, 
    512, 510, 508, 506, 504, 502, 500, 498, 496, 494, 
    492, 490, 488, 486, 484, 482, 480, 478, 476, 474, 
    472, 470, 468, 466, 464, 462, 460, 458, 456, 454, 
    452, 450, 448, 446, 444, 442, 440, 438, 437, 435, 
    433, 431, 429, 427, 425, 423, 421, 419, 417, 415, 
    413, 412, 410, 408, 406, 404, 402, 400, 398, 396, 
    394, 392, 390, 388, 387, 385, 383, 381, 379, 377, 
    375, 373, 371, 369, 367, 365, 363, 362, 360, 358, 
    356, 354, 352, 350, 348, 346, 344, 342, 340, 338, 
    337, 335, 333, 331, 329, 327, 325, 323, 321, 319, 
    317, 315, 313, 312, 310, 308, 306, 304, 302, 300, 
    298, 296, 294, 292, 290, 288, 287, 285, 283, 281, 
    279, 277, 275, 273, 271, 269, 267, 265, 263, 262, 
    260, 258, 256, 254, 252, 250, 248, 246, 244, 242, 
    240, 238, 237, 235, 233, 231, 229, 227, 225, 223, 
    221, 219, 217, 215, 213, 212, 210, 208, 206, 204, 
    202, 200, 198, 196, 194, 192, 190, 188, 187, 185, 
    183, 181, 179, 177, 175, 173, 171, 169, 167, 165, 
    163, 162, 160, 158, 156, 154, 152, 150, 148, 146, 
    144, 142, 140, 138, 137, 135, 133, 131, 129, 127, 
    125, 123, 121, 119, 117, 115, 113, 112, 110, 108, 
    106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 
    87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 
    67, 65, 63, 62, 60, 58, 56, 54, 52, 50, 
    48, 46, 44, 42, 40, 38, 37, 35, 33, 31, 
    29, 27, 25, 23, 21, 19, 17, 15, 13, 12, 
    10, 8, 6, 4, 2, 0, -2, -4, -6, -8, 
    -10, -12, -13, -15, -17, -19, -21, -23, -25, -27, 
    -29, -31, -33, -35, -37, -38, -40, -42, -44, -46, 
    -48, -50, 
};

TEMP_T   ConvAdc2Temp_ColdWater(U16 mu16Adc)
{
    U16 mu16Index;
    int val;
    TEMP_T temp;


    if( mu16Adc > MAX_ADC_COLD_WATER_2DP 
            ||  mu16Adc < MIN_ADC_COLD_WATER_2DP )
    {
        // 10bit
        mu16Index = GetTempTableIndex( 
                RES_ADC_TABLE_10BIT, 
                mu16Adc, 
                MIN_ADC_COLD_WATER, 
                MAX_ADC_COLD_WATER );
        val = temp_cold_water_table[ mu16Index ];
        temp = ConvInt2Temp( val );
    }
    else
    {
        // 12bit
        mu16Index = GetTempTableIndex( 
                RES_ADC_TABLE_12BIT, 
                mu16Adc, 
                MIN_ADC_COLD_WATER_2DP, 
                MAX_ADC_COLD_WATER_2DP );
        val = temp_cold_water_table_2dp[ mu16Index ];
        temp = ConvInt2Temp2dp( val );
    }
            
    return temp;
}
