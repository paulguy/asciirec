#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DEFAULTFPS (15)
#define DEFAULTFILE "out.atxt"
const int MAGICNUMBER = 0x54585441; //ATXT
const int REVISION = 1;
#define DMAXFRAMESIZE (32767)

int main(int argc, char **argv) {
	int fps = DEFAULTFPS;
	int maxfs = DMAXFRAMESIZE;
	char *outfile = (char*)malloc((strlen(DEFAULTFILE)+1) * sizeof(char));
	strcpy(outfile, DEFAULTFILE);
	if(argc > 2) {
		int i;
		for(i = 1; i < argc - 1; i++) {
			if(strcmp(argv[i], "-maxfs") == 0) {
				maxfs = atoi(argv[i+1]);
				if(maxfs < 1) maxfs = DMAXFRAMESIZE;
			} else if(strcmp(argv[i], "-fps") == 0) {
				fps = atoi(argv[i+1]);
				if(fps < 1) fps = DEFAULTFPS;
			} else if (strcmp(argv[i], "-filename") == 0) {
				free(outfile);
				outfile = (char*)malloc((strlen(argv[i+1])+1) * sizeof(char));
				strcpy(outfile, argv[i+1]);
			}
		}
	}

	char* fb = (char*)malloc(maxfs * sizeof(char));
	if(fb == NULL) {
		fprintf(stderr, "WARNING: Couldn't allocate %d bytes for buffer, trying default of %d.\n", maxfs, DMAXFRAMESIZE);
		fb = (char*)malloc(DMAXFRAMESIZE * sizeof(char));
		maxfs = DMAXFRAMESIZE;
		if(fb == NULL) {
			fprintf(stderr, "ERROR: Couldn't allocate default buffer size, try specifying a lower size with -maxfs or freeing up some memory.\n");
			return(-1);
		}
	}
	
	fprintf(stderr, "Recording to %s at %d FPS.\n", outfile, fps);
	FILE *o = fopen(outfile, "wb");
	fwrite(&MAGICNUMBER, sizeof(int), 1, o);
	fwrite(&REVISION, sizeof(int), 1, o);
	int period = 1000/fps*1000; //To keep the value as multiple of 1000, and a sane minimum value.
	if(period == 0) {
		fprintf(stderr, "WARNING: INSANELY HIGH FPS!  Capped to 1000!  Performance WILL suffer.\n");
		period = 1;
	}
	fwrite(&maxfs, sizeof(int), 1, o);
	fwrite(&period, sizeof(int), 1, o);

	int flags = fcntl(0, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags);

	int fs, i;
	while(!feof(stdin)) {
		fs = fread(fb, sizeof(char), maxfs, stdin);
		fwrite(&fs, sizeof(int), 1, o);
		fwrite(fb, sizeof(char), fs, o);
		usleep(period);
	}
	return(0);
}
