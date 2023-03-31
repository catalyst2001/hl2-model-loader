#include <stdio.h>
#include "smd_loader.h"

smd_model model;

int main()
{
	int status = smd_load_model(&model, "model/ct_gign_reference.smd");
	if (status != SMDLDR_STATUS_OK) {
		printf("Loading SMD model failed. Error: %d\n", status);
		return 1;
	}




	return 0;
}