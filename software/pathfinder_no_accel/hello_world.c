/*
 * main.c
 *
 *  Created on: 6 May 2024
 *      Author: andre
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 50

int main() {
	FILE* fp_ascii = NULL;
	char buffer[BUF_SIZE];
	int read_size, i;

	fp_ascii = fopen("/mnt/host/input.txt", "r");
	if (fp_ascii == NULL) {
		printf("failed to open");
		exit(1);
	}

	fgets(buffer, sizeof(buffer), fp_ascii);
	printf("%s", buffer);
	fclose(fp_ascii);

	fp_ascii = fopen("/mnt/host/output.txt", "a");

		if (fp_ascii == NULL) {
		printf("failed to open");
		exit(1);
	}

	fputs(buffer, fp_ascii);
	fclose(fp_ascii);



	return 0;
}