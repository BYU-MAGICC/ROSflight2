#ifdef __cplusplus
extern "C" {
#endif
#include <turbotrig/turbotrig.h>

int16_t atan_lookup_table[1001] =
{
  0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,
  36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,
  69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
  101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
  126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,143,144,145,146,147,148,149,
  150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,
  175,176,177,178,179,180,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,
  199,200,201,202,203,204,205,206,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,
  223,224,225,226,227,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,244,245,
  246,247,248,249,250,251,252,253,254,255,256,257,258,259,259,260,261,262,263,264,265,266,267,268,269,
  270,271,272,273,273,274,275,276,277,278,279,280,281,282,283,284,285,285,286,287,288,289,290,291,292,
  293,294,295,296,296,297,298,299,300,301,302,303,304,305,306,306,307,308,309,310,311,312,313,314,315,
  316,316,317,318,319,320,321,322,323,324,325,325,326,327,328,329,330,331,332,333,333,334,335,336,337,
  338,339,340,341,342,342,343,344,345,346,347,348,349,349,350,351,352,353,354,355,356,357,357,358,359,
  360,361,362,363,364,364,365,366,367,368,369,370,370,371,372,373,374,375,376,377,377,378,379,380,381,
  382,383,383,384,385,386,387,388,389,389,390,391,392,393,394,395,395,396,397,398,399,400,401,401,402,
  403,404,405,406,406,407,408,409,410,411,411,412,413,414,415,416,417,417,418,419,420,421,422,422,423,
  424,425,426,427,427,428,429,430,431,431,432,433,434,435,436,436,437,438,439,440,440,441,442,443,444,
  445,445,446,447,448,449,449,450,451,452,453,454,454,455,456,457,458,458,459,460,461,462,462,463,464,
  465,466,466,467,468,469,470,470,471,472,473,473,474,475,476,477,477,478,479,480,481,481,482,483,484,
  485,485,486,487,488,488,489,490,491,492,492,493,494,495,495,496,497,498,498,499,500,501,502,502,503,
  504,505,505,506,507,508,508,509,510,511,512,512,513,514,515,515,516,517,518,518,519,520,521,521,522,
  523,524,524,525,526,527,527,528,529,530,530,531,532,533,533,534,535,535,536,537,538,538,539,540,541,
  541,542,543,544,544,545,546,547,547,548,549,549,550,551,552,552,553,554,554,555,556,557,557,558,559,
  560,560,561,562,562,563,564,565,565,566,567,567,568,569,570,570,571,572,572,573,574,574,575,576,577,
  577,578,579,579,580,581,581,582,583,584,584,585,586,586,587,588,588,589,590,590,591,592,593,593,594,
  595,595,596,597,597,598,599,599,600,601,601,602,603,603,604,605,606,606,607,608,608,609,610,610,611,
  612,612,613,614,614,615,616,616,617,618,618,619,620,620,621,622,622,623,624,624,625,625,626,627,627,
  628,629,629,630,631,631,632,633,633,634,635,635,636,637,637,638,639,639,640,640,641,642,642,643,644,
  644,645,646,646,647,647,648,649,649,650,651,651,652,653,653,654,654,655,656,656,657,658,658,659,659,
  660,661,661,662,663,663,664,664,665,666,666,667,667,668,669,669,670,671,671,672,672,673,674,674,675,
  675,676,677,677,678,678,679,680,680,681,682,682,683,683,684,685,685,686,686,687,688,688,689,689,690,
  690,691,692,692,693,693,694,695,695,696,696,697,698,698,699,699,700,701,701,702,702,703,703,704,705,
  705,706,706,707,707,708,709,709,710,710,711,711,712,713,713,714,714,715,715,716,717,717,718,718,719,
  719,720,721,721,722,722,723,723,724,725,725,726,726,727,727,728,728,729,730,730,731,731,732,732,733,
  733,734,735,735,736,736,737,737,738,738,739,739,740,741,741,742,742,743,743,744,744,745,745,746,746,
  747,748,748,749,749,750,750,751,751,752,752,753,753,754,755,755,756,756,757,757,758,758,759,759,760,
  760,761,761,762,762,763,763,764,764,765,766,766,767,767,768,768,769,769,770,770,771,771,772,772,773,
  773,774,774,775,775,776,776,777,777,778,778,779,779,780,780,781,781,782,782,783,783,784,784,785
};

int16_t asin_lookup_table[1000] =
{
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
  44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
  86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
  106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
  123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
  140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156,
  157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173,
  174, 175, 176, 177, 178, 179, 180, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
  192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
  209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
  226, 227, 228, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243,
  244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260,
  261, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278,
  279, 280, 281, 282, 283, 284, 285, 286, 287, 289, 290, 291, 292, 293, 294, 295, 296,
  297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 312, 313, 314,
  315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 331, 332,
  333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 349, 350,
  351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 365, 366, 367, 368,
  369, 370, 371, 372, 373, 374, 375, 376, 377, 379, 380, 381, 382, 383, 384, 385, 386,
  387, 388, 389, 390, 391, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404,
  406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 418, 419, 420, 421, 422, 423,
  424, 425, 426, 427, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 440, 441, 442,
  443, 444, 445, 446, 447, 448, 450, 451, 452, 453, 454, 455, 456, 457, 458, 460, 461,
  462, 463, 464, 465, 466, 467, 469, 470, 471, 472, 473, 474, 475, 476, 477, 479, 480,
  481, 482, 483, 484, 485, 487, 488, 489, 490, 491, 492, 493, 494, 496, 497, 498, 499,
  500, 501, 502, 504, 505, 506, 507, 508, 509, 510, 512, 513, 514, 515, 516, 517, 518,
  520, 521, 522, 523, 524, 525, 527, 528, 529, 530, 531, 532, 534, 535, 536, 537, 538,
  539, 541, 542, 543, 544, 545, 546, 548, 549, 550, 551, 552, 553, 555, 556, 557, 558,
  559, 560, 562, 563, 564, 565, 566, 568, 569, 570, 571, 572, 574, 575, 576, 577, 578,
  579, 581, 582, 583, 584, 585, 587, 588, 589, 590, 591, 593, 594, 595, 596, 598, 599,
  600, 601, 602, 604, 605, 606, 607, 608, 610, 611, 612, 613, 615, 616, 617, 618, 619,
  621, 622, 623, 624, 626, 627, 628, 629, 631, 632, 633, 634, 636, 637, 638, 639, 641,
  642, 643, 644, 646, 647, 648, 649, 651, 652, 653, 654, 656, 657, 658, 659, 661, 662,
  663, 664, 666, 667, 668, 670, 671, 672, 673, 675, 676, 677, 678, 680, 681, 682, 684,
  685, 686, 688, 689, 690, 691, 693, 694, 695, 697, 698, 699, 701, 702, 703, 704, 706,
  707, 708, 710, 711, 712, 714, 715, 716, 718, 719, 720, 722, 723, 724, 726, 727, 728,
  730, 731, 732, 734, 735, 736, 738, 739, 740, 742, 743, 745, 746, 747, 749, 750, 751,
  753, 754, 755, 757, 758, 760, 761, 762, 764, 765, 767, 768, 769, 771, 772, 773, 775,
  776, 778, 779, 781, 782, 783, 785, 786, 788, 789, 790, 792, 793, 795, 796, 798, 799,
  800, 802, 803, 805, 806, 808, 809, 811, 812, 813, 815, 816, 818, 819, 821, 822, 824,
  825, 827, 828, 830, 831, 833, 834, 836, 837, 839, 840, 842, 843, 845, 846, 848, 849,
  851, 852, 854, 855, 857, 858, 860, 861, 863, 864, 866, 867, 869, 871, 872, 874, 875,
  877, 878, 880, 881, 883, 885, 886, 888, 889, 891, 893, 894, 896, 897, 899, 901, 902,
  904, 905, 907, 909, 910, 912, 914, 915, 917, 919, 920, 922, 923, 925, 927, 928, 930,
  932, 933, 935, 937, 939, 940, 942, 944, 945, 947, 949, 951, 952, 954, 956, 957, 959,
  961, 963, 964, 966, 968, 970, 971, 973, 975, 977, 979, 980, 982, 984, 986, 988, 989,
  991, 993, 995, 997, 999, 1000, 1002, 1004, 1006, 1008, 1010, 1012, 1014, 1015, 1017,
  1019, 1021, 1023, 1025, 1027, 1029, 1031, 1033, 1035, 1037, 1039, 1041, 1043, 1045,
  1047, 1049, 1051, 1053, 1055, 1057, 1059, 1061, 1063, 1065, 1067, 1069, 1071, 1073,
  1075, 1077, 1080, 1082, 1084, 1086, 1088, 1090, 1092, 1095, 1097, 1099, 1101, 1103,
  1106, 1108, 1110, 1112, 1115, 1117, 1119, 1122, 1124, 1126, 1129, 1131, 1133, 1136,
  1138, 1140, 1143, 1145, 1148, 1150, 1153, 1155, 1157, 1160, 1163, 1165, 1168, 1170,
  1173, 1175, 1178, 1181, 1183, 1186, 1189, 1191, 1194, 1197, 1199, 1202, 1205, 1208,
  1211, 1213, 1216, 1219, 1222, 1225, 1228, 1231, 1234, 1237, 1240, 1243, 1246, 1250,
  1253, 1256, 1259, 1262, 1266, 1269, 1273, 1276, 1279, 1283, 1287, 1290, 1294, 1297,
  1301, 1305, 1309, 1313, 1317, 1321, 1325, 1329, 1333, 1337, 1342, 1346, 1351, 1355,
  1360, 1365, 1370, 1375, 1380, 1386, 1391, 1397, 1403, 1409, 1415, 1422, 1429, 1436,
  1444, 1452, 1461, 1470, 1481, 1493, 1507, 1526
};


int32_t sign(int32_t y)
{
  return (0 < y) - (y < 0);
}


int32_t turboatan(int32_t x)
{
  if (x < 0)
  {
    return -1*turboatan(-1*x);
  }
  if (x > 1000)
  {
    return 1571 - turboatan(1000000/x);
  }

  return atan_lookup_table[x];
}


int32_t turboatan2(int32_t y, int32_t x)
{
  if (y == 0)
  {
    if (x < 0)
    {
      return 3142;
    }
    else
    {
      return 0;
    }
  }

  else if (x == 0)
  {
    return 1572*sign(y);
  }

  else
  {
    int32_t arctan = turboatan((1000*x)/y);

    if (y > 0)
    {
      return 1571 - arctan;
    }
    else if (y < 0)
    {
      return -1571 - arctan;
    }
    else if (x < 0)
    {
      return arctan + 3142;
    }
    else
    {
      return arctan;
    }
  }
}


int32_t turboatan_taylor(int32_t x)
{
  if (x > 1000)
  {
    return 1571-turboatan(1000000/x);
  }

  return (972*x/1000) - (((191*x*x)/1000)*x)/(1000*1000); // the weird order of operations is to prevent overflow
}


int32_t turbocos(int32_t x)
{
  return turbosin(x + 1571);
}


int32_t turbosin(int32_t x)
{
  // wrap to +/- PI
  if (x < -3142)
    x += 6283;
  else if (x >  3142)
    x -= 6283;

  if (x < 0)
  {
    return (1273 * x)/1000 + (405 * x * x)/(1000000);
  }
  else
  {
    return (1273 * x)/1000 - (405 * x * x)/(1000000);
  }

  return x;
}


int32_t turboasin(int32_t x)
{
  if (x < 0)
  {
    return -1*turboasin(-1*x);
  }
  else if (x > 999)
  {
    return 1ul;
  }
  return asin_lookup_table[x];
}
#ifdef __cplusplus
}
#endif
