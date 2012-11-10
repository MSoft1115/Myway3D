#pragma once

#define SHUFFLE_PS(x, y, z, w)		((w & 3) << 6 | (z & 3) << 4 | (y & 3) << 2 | (x & 3))

/*
����    ���Ƿ�      ���� 
0       eq          ��� 
1       lt          С�� 
2       le          С�ں���� 
3       unord       ���� 
4       neq         ���� 
5       nlt         ��С�� 
6       nle         ��С�ڲ����� 
7       ord         ���� 
*/
#define SSE_EQ          0
#define SSE_LT          1
#define SSE_LE          2
#define SSE_UNORD       3
#define SSE_NE          4
#define SSE_GE          5
#define SSE_GT          6
#define SSE_ORD         7
#define SSE_CMP_OK      0xFFFFFFFF
#define SSE_CMP_FAIL    0