#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "staticpagegen.h"

void usage(void);

int
main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "G:gH:hPT:U:Vv")) != -1) {
		switch (opt) {
		case 'G':
			if (strcmp(optarg, "geomyidae")) {
				(void)fprintf(stderr, "staticpagegen: Invalid or"\
				    " unsupported Gopher server\n");
				return EXIT_FAILURE;
			}
			break;
		case 'g':
			if (gen == GEN_HONLY)
				gen = GEN_HG;
			else
				gen = GEN_GONLY;
			break;
		case 'H':
			if (strcmp(optarg, "xhtml_basic_1-0")) {
				(void)fprintf(stderr, "staticpagegen: Invalid or"\
				    " unsupported XHTML standard\n");
				return EXIT_FAILURE;
			}
			break;
		case 'h':
			if (gen == GEN_GONLY)
				gen = GEN_HG;
			else
				gen = GEN_HONLY;
			break;
		case 'P':
			ignoresymlinks = 1;
			break;
		case 'T':
			pagetitle = optarg;
			break;
		case 'U':
			pagebaseaddr = optarg;
			break;
		case 'V':
			(void)fprintf(stdout, "staticpagegen-"STATICPAGEGEN_VERSION\
			    " (c) 2023-2024 Kaan Çırağ\n");
			return EXIT_SUCCESS;
			break;
		case 'v':
			debuglvl++;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc > 1) {
		(void)fprintf(stderr, "staticpagegen: too many arguments\n");
		return EXIT_FAILURE;
	}
	if (!argv[0]) {
		usage();
	}
	return staticpagegen_main(argv[0]);
}


/*
 * usage - prints usage information.
 */
void
usage(void)
{
	(void)fprintf(stderr,
	    "usage: staticpagegen [-ghPVv] [-G gopher-server] [-H html-standard]\n"
		"                     [-T page-title] [-U base-address] directory\n");
	exit(EXIT_FAILURE);
}
