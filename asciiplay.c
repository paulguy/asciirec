#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int MAGICNUMBER = 0x54585441; //ATXT
const int MAXREVISION = 1;

int main(int argc, char **argv) {
	FILE *ifile;

	int maxfs;
	int period;

	if(argc > 1) {
		ifile = fopen(argv[1], "rb");
	} else {
		fprintf(stderr, "ERROR: You must specify an input filename.\n");
		return(-1);
	}

	int t;
	fread(&t, sizeof(int), 1, ifile);
	if(t != MAGICNUMBER) {
		fprintf(stderr, "ERROR: This does not appear to be a valid ATXT file.  Change the first 4 bytes to contain ATXT and try again.\n");
		return(-1);
	} else {
		fprintf(stderr, "ATXT file format detected.\n");
	}
	fread(&t, sizeof(int), 1, ifile);
	if(t > MAXREVISION) {
		fprintf(stderr, "ERROR: This file appears to be from a newer version of asciirec, it will likely not play.  Try changing the version in the header to &d (bytes 4 to 7).\n");
		return(-1);
	}
	fread(&maxfs, sizeof(int), 1, ifile);
	fread(&period, sizeof(int), 1, ifile);
	fprintf(stderr, "Frame period is %d with a max frame size of %d.\n", period, maxfs);

	char* fb = (char*)malloc(maxfs * sizeof(char));
	if(fb == NULL) {
		fprintf(stderr, "ERROR: Couldn't allocate the requested %d bytes of memory.  Maybe free up some memory, and try again.  Otherwise, your computer doesn't have enough RAM or the file is malformed.\n", maxfs);
		return(-1);
	}
	
	fprintf(stderr, "Starting playback...\n");

	int fs;
	while(!feof(ifile)) {
		fread(&fs, sizeof(int), 1, ifile);
		fread(fb, sizeof(char), fs, ifile);
		fwrite(fb, sizeof(char), fs, stdout);
		usleep(period);
	}
	return(0);
}
