#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

inline void fu_get_size(fpos_t *p_dst, FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	fgetpos(fp, p_dst);
	fseek(fp, 0, SEEK_SET);
}