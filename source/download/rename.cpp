#include <stdio.h>

int main() {
	for(int i=0;i<131;i++) {
		char dec[32], hex[32];
		snprintf(dec, sizeof(dec), "%d.bmp", i);
		snprintf(hex, sizeof(hex), "%02x.bmp", i);

		printf("Renaming %s to %s\n", hex, dec)
		rename(hex, dec);
	}
}