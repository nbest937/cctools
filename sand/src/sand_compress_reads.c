/*
Copyright (C) 2009- The University of Notre Dame & University of Cambridge
This software is distributed under the GNU General Public License.
See the file COPYING for details.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "compressed_sequence.h"

#include "cctools.h"
#include "debug.h"

static void show_help(const char *cmd)
{
        printf("Use: %s [options] [infile] [outfile]\n", cmd);
        printf("where options are:\n");
	printf(" -q  Quiet mode: suppress summary line.\n");
	printf(" -v  Show version string.\n");
	printf(" -c  Remove Celera read_ids if file came from Celera's gatekeeper\n");
	printf(" -i  Remove read_ids but leave the Celera internal ids if the file came from Celera's gatekeeper\n"); 
	printf(" -h  Show this help screen\n");
}

int main(int argc, char ** argv)
{
	const char *progname = "sand_compress_reads";
	FILE * infile;
	FILE * outfile;
	int quiet_mode = 0;
	struct seq *s;
	struct cseq *c;
	signed char d;
	int clip = 0;
	int internal = 0; 
	char tmp_id[128];
	int count = 0;

        while((d=getopt(argc,argv,"cvqhi")) > -1) {
                switch(d) {
		case 'c':
			clip = 1;
			break;
		case 'i':
			internal = 1;
			break; 
		case 'q':
			quiet_mode = 1;
			break;
		case 'v':
			cctools_version_print(stdout, progname);
			exit(0);
			break;
                case 'h':
		default:
                        show_help(progname);
                        exit(0);
                        break;
                }
        }

	cctools_version_debug(D_DEBUG, argv[0]);

	if( optind<argc ) {
		infile = fopen(argv[optind], "r");
		if(!infile) {
			fprintf(stderr,"%s: couldn't open %s: %s\n",progname,argv[optind],strerror(errno));
			return 1;
		}
		optind++;
	} else {
		infile = stdin;
	}

	if( optind<argc ) {
		outfile = fopen(argv[optind],"w");
		if(!outfile) {
			fprintf(stderr,"%s: couldn't open %s: %s\n",progname,argv[optind],strerror(errno));
			return 1;
		}
		optind++;
	} else {
		outfile = stdout;
	}

	while((s = seq_read(infile))) {
		if(clip != 0 || internal != 0){
			strcpy(tmp_id, s->name);
			strcpy(s->name, strtok(tmp_id,","));
			if(internal != 0){
				strcpy(s->name, strtok(NULL,","));
			}
		}

		c = seq_compress(s);
		cseq_write(outfile,c);
		cseq_free(c);
		seq_free(s);
		count++;
	}

	if(!quiet_mode) {
		fprintf(stderr,"%d sequences compressed.\n",count);
	}

	fclose(infile);
	fclose(outfile);

	return 0;
}
