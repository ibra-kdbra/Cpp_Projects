/* For PATH_MAX, remove and define PATH_MAX if unwanted */
#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

#include "xhtmlbasic1-0.h"
#include "staticpagegen.h"

static char *xhtmldirname = "xhtml";
static char *gopherdirname = "gopher";
char *pagetitle = "Page";
char *pagebaseaddr = "server";
enum Gentype gen = GEN_HG;
enum Htmlspec htmspec = HTM_XHTMLBASIC10;
enum Gopherspec gphspec = GPH_GEOMYIDAE;
int debuglvl = 0;
int ignoresymlinks = 0;

struct stat cwdstbuf;

/*
 * staticpagegen_main - the main function.
 *	argpath - SOURCE directory given as an argument, from main.
 * Initialises certain values and calls itersrc. Returns to main. See the
 * manpage for program details.
 */
int
staticpagegen_main(char *argpath)
{
	char cwd[PATH_MAX];
	struct stat stbuf;
	if (strlen(argpath) >= FILENAME_MAX) {
		(void)fprintf(stderr, "SOURCE folder name too long\n");
		return 1;
	}
	if (stat(argpath, &stbuf))
		return staticpagegen_perror(argpath);
	if (!S_ISDIR(stbuf.st_mode)) {
		errno = ENOTDIR;
		(void)fprintf(stderr, "staticpagegen: failed to use '%s': %s\n",
		    argpath, strerror(errno));
		return 1;
	}
	if (gen != GEN_GONLY) {
		if (mkdir(xhtmldirname, 0700))
			return staticpagegen_perror(xhtmldirname);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created directory '%s'\n", xhtmldirname);
	}
	if (gen != GEN_HONLY) {
		if (mkdir(gopherdirname, 0700)) 
			return staticpagegen_perror(gopherdirname);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created directory '%s'\n", gopherdirname);
	}
	if (!getcwd(cwd, PATH_MAX))
		return staticpagegen_perror(cwd);
	if (stat(cwd, &cwdstbuf))
		return staticpagegen_perror(cwd);
	return itersrc(argpath, "");
}


/*
 * itersrc  - iterates through directories and calls appropriate functions
 * depending on the file.
 *	path    - directory to iterate.
 *	genpath - partial relative path for the directories to generate.
 */
int
itersrc(char *path, char *genpath)
{
	struct stat pathstbuf;
	DIR *d;
	struct dirent *dir;
	int wrpages_rval = 0;
	if (stat(path, &pathstbuf))
		return staticpagegen_perror(path);
	if ((d = opendir(path))) {
		char *subdirs[SUBDIRN_MAX];
		char *subfils[SUBDIRN_MAX];
		unsigned int subdirs_n = 0;
		unsigned int subfils_n = 0;
		if (debuglvl > 1)
			(void)fprintf(stderr, "Iterating directory '%s'\n", path);
		while ((dir = readdir(d))) {
			struct stat stbuf;
			char nextpath[PATH_MAX];
			char nextgenpath[PATH_MAX];
			if (snprintf(nextpath, PATH_MAX, "%s/%s", path, dir->d_name) >=
			    PATH_MAX) {
				(void)fprintf(stderr, "staticpagegen: can't use '%s/%s': File"
				    " name too long'\n", path, dir->d_name);
				return 1;
			}
			if (!ignoresymlinks) {
				if (stat(nextpath, &stbuf))
					return staticpagegen_perror(nextpath);
			}
			else {
				if (lstat(nextpath, &stbuf))
					return staticpagegen_perror(nextpath);
			}
			if (snprintf(nextgenpath, PATH_MAX, "%s/%s", genpath, dir->d_name)
			    >= PATH_MAX) {
				(void)fprintf(stderr, "staticpagegen: can't use '%s/%s': File"
				    " name too long'\n", genpath, dir->d_name);
				return 1;
			}
			if (S_ISDIR(stbuf.st_mode) && strcmp(dir->d_name, ".") &&
			    strcmp(dir->d_name, "..") && !(pathstbuf.st_ino ==
			    cwdstbuf.st_ino && (!strcmp(dir->d_name, xhtmldirname) ||
			    !strcmp(dir->d_name, gopherdirname)))) {
				subdirs[subdirs_n] = dir->d_name;
				subdirs_n += 1;

				mkpagepaths(nextgenpath);
				if (itersrc(nextpath, nextgenpath))
					return -1; 
			} else if (S_ISREG(stbuf.st_mode)) {
				subfils[subfils_n] = dir->d_name;
				subfils_n += 1;
				cpfiles(nextpath, nextgenpath);
			} else {
				if (debuglvl > 1)
					(void)fprintf(stderr, "Ignoring node '%s'\n", dir->d_name);
			}
	   	}
		if ((wrpages_rval = wrpages(genpath, subdirs, subdirs_n, subfils,
		    subfils_n)) != 0)
			return wrpages_rval;
		if (closedir(d))
			return staticpagegen_perror(path);
	} else 
		return staticpagegen_perror(path);
	return 0;
} 


/*
 * mkpagepaths  - makes the generated page directories.
 *	nextgenpath - partial relative path for the directories to generate.
 */
int
mkpagepaths(char *nextgenpath)
{
	char xhtmlgenpath[PATH_MAX];
	char gophergenpath[PATH_MAX];
	if (gen != GEN_GONLY) {
		if (snprintf(xhtmlgenpath, PATH_MAX, "%s%s", xhtmldirname, nextgenpath)
		    >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s': File"
			    " name too long'\n", xhtmldirname, nextgenpath);
			return 1;
		}
		if (mkdir(xhtmlgenpath, 0700))
			return staticpagegen_perror(xhtmlgenpath);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created directory '%s'\n", xhtmlgenpath);
	}
	if (gen != GEN_HONLY) {
		if (snprintf(gophergenpath, PATH_MAX, "%s%s", gopherdirname,
		    nextgenpath) >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s': File"
			    " name too long'\n", gopherdirname, nextgenpath);
			return 1;
		}
		if (mkdir(gophergenpath, 0700))
			return staticpagegen_perror(gophergenpath);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created directory '%s'\n", gophergenpath);
	}
	return 0;
}


/*
 * cpfiles      - copies the files from the iterated directories to the
 *     generated directories.
 *	nextpath    - path for the source file.
 *	nextgenpath - partial relative path for the destination files.
 */
int
cpfiles(char *nextpath, char *nextgenpath)
{
	char xhtmlgenpath[PATH_MAX];
	char gophergenpath[PATH_MAX];
	char byte;
	FILE *inptr;
	FILE *xhtmloutptr;
	FILE *gopheroutptr;
	if (!(inptr = fopen(nextpath, "r")))
		return staticpagegen_perror(nextpath);
	if (gen != GEN_GONLY) {
		if (snprintf(xhtmlgenpath, PATH_MAX, "%s%s", xhtmldirname, nextgenpath)
		    >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s': File"
			    " name too long'\n", xhtmldirname, nextgenpath);
			return 1;
		}
		if (!(xhtmloutptr = fopen(xhtmlgenpath, "w")))
			return staticpagegen_perror(xhtmlgenpath);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Copying file '%s' to '%s'\n", nextpath,
			    xhtmlgenpath);
		while (1) {
			if ((byte = fgetc(inptr)) == EOF && feof(inptr)) {
				clearerr(inptr);
				fseek(inptr, 0, SEEK_SET);
				break;
			} else if (byte == EOF && ferror(inptr))
				return staticpagegen_perror(nextpath);
			if (fputc(byte, xhtmloutptr) == EOF && ferror(xhtmloutptr))
				return staticpagegen_perror(xhtmlgenpath);
		}
		if (fclose(xhtmloutptr))
			return staticpagegen_perror(xhtmlgenpath);
	}
	if (gen != GEN_HONLY) {
		if (snprintf(gophergenpath, PATH_MAX, "%s%s", gopherdirname,
		    nextgenpath) >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s': File"
			    " name too long'\n", gopherdirname, nextgenpath);
			return 1;
		}
		if (!(gopheroutptr = fopen(gophergenpath, "w")))
			return staticpagegen_perror(gophergenpath);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Copying file '%s' to '%s'\n", nextpath,
			    gophergenpath);
		while (1) {
			if ((byte = fgetc(inptr)) == EOF && feof(inptr)) {
				clearerr(inptr);
				fseek(inptr, 0, SEEK_SET);
				break;
			} else if (byte == EOF && ferror(inptr))
				return 1;
			if (fputc(byte, gopheroutptr) == EOF && ferror(gopheroutptr))
				return staticpagegen_perror(gophergenpath);
		}
		if (fclose(gopheroutptr))
			return staticpagegen_perror(gophergenpath);
	}
	if (fclose(inptr))
		return staticpagegen_perror(nextpath);
	return 0;
}


/*
 * wrpages    - writes the contents of the gopher and xhtml pages.
 *	genpath   - partial relative path for the page files to be generated.
 *	subdirs   - directories located in where the generated pages will be.
 *	subdirs_n - subdir count.
 *	subflis   - files located in where the generated pages will be.
 *	subfils_n - subfil count.
 */
int
wrpages(char *genpath, char **subdirs, unsigned int
    subdirs_n, char **subfils, unsigned int subfils_n)
{
	if (gen != GEN_GONLY && htmspec == HTM_XHTMLBASIC10) {
		unsigned int i;
		FILE *idxptr;
		char xhtmlindexfil[PATH_MAX];

		if (snprintf(xhtmlindexfil, PATH_MAX, "%s%s/index.xhtml", xhtmldirname,
		    genpath) >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s/index.xhtml':"
			    " File name too long'\n", xhtmldirname, genpath);
			return 1;
		}
		if ((idxptr = fopen(xhtmlindexfil, "w")) == NULL)
			return staticpagegen_perror(xhtmlindexfil);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created file '%s'\n", xhtmlindexfil);
		if (fprintf(idxptr, "%s%s%s%s%s", xhtmlbasic10top, pagetitle,
		    xhtmlbasic10mid1, pagetitle, xhtmlbasic10mid2) !=
		    (int) (strlen(xhtmlbasic10top) + 2 * strlen(pagetitle) +
		    strlen(xhtmlbasic10mid1) + strlen(xhtmlbasic10mid2))) {
			(void)fprintf(stderr, "staticpagegen: can't write to file '%s'\n",
			    xhtmlindexfil);
			return 1;
		}
		if (strcmp(genpath, "")) {
			char prevgenpath[PATH_MAX];
			unsigned int lastslashi = 0;
			for (i = 1; i < strlen(genpath); i++) {
				if (genpath[i] == '/')
					lastslashi = i;
			}
			(void)strncpy(prevgenpath, genpath, lastslashi);
			prevgenpath[lastslashi] = '\0';
			if (fprintf(idxptr, "<a href=\"%s/\">../</a>\n",
			    prevgenpath) != 20 + (int) strlen(prevgenpath)) {
				(void)fprintf(stderr, "staticpagegen: can't write to file"
				    " '%s'\n", xhtmlindexfil);
				return 1;
			}
		}
		for (i = 0; i < subdirs_n; i++) {
			if (fprintf(idxptr, "<a href=\"%s/%s/index.xhtml\">%s/</a>\n",
			    genpath, subdirs[i], subdirs[i]) != 30 + (int)
			    (strlen(genpath) + 2 * strlen(subdirs[i]))) {
				(void)fprintf(stderr, "staticpagegen: can't write to file"
				    " '%s'\n", xhtmlindexfil);
				return 1;
			}
		}
		for (i = 0; i < subfils_n; i++) {
			if (fprintf(idxptr, "<a href=\"%s/%s\">%s</a>\n",
			    genpath, subfils[i], subfils[i]) != 17 + (int)
			    (strlen(genpath) + 2 * strlen(subfils[i]))) {
				(void)fprintf(stderr, "staticpagegen: can't write to file"
				    " '%s'\n", xhtmlindexfil);
				return 1;
			}
		}
		if (fprintf(idxptr, "%s", xhtmlbasic10bot) !=
		    (int) strlen(xhtmlbasic10bot)) {
			(void)fprintf(stderr, "staticpagegen: can't write to file"
			    " '%s'\n", xhtmlindexfil);
			return 1;
		}
		if (fclose(idxptr) != 0)
			return staticpagegen_perror(xhtmlindexfil);
	}
	if (gen != GEN_HONLY && gphspec == GPH_GEOMYIDAE) {
		unsigned int i;
		FILE *idxptr;
		char gopherindexfil[PATH_MAX];
		if (snprintf(gopherindexfil, PATH_MAX, "%s%s/index.gph", gopherdirname,
		    genpath) >= PATH_MAX) {
			(void)fprintf(stderr, "staticpagegen: can't use '%s%s/index.gph':"
			    " File name too long'\n", gopherdirname, genpath);
			return 1;
		}
		if ((idxptr = fopen(gopherindexfil, "w")) == NULL)
			return staticpagegen_perror(gopherindexfil);
		if (debuglvl > 0)
			(void)fprintf(stderr, "Created file '%s'\n", gopherindexfil);
		if (fprintf(idxptr, "[1|%s|/|%s|port]\n", pagetitle,
		    pagebaseaddr) != 13 +
		    (int) (strlen(pagetitle) + strlen(pagebaseaddr))) {
			(void)fprintf(stderr, "staticpagegen: can't write to file '%s'\n",
			    gopherindexfil);
			return 1;
		}
		if (strcmp(genpath, "")) {
			char prevgenpath[PATH_MAX];
			unsigned int lastslashi = 0;
			for (i = 1; i < strlen(genpath); i++) {
				if (genpath[i] == '/')
					lastslashi = i;
			}
			(void)strncpy(prevgenpath, genpath, lastslashi);
			prevgenpath[lastslashi] = '\0';
			if (fprintf(idxptr, "[1|../|%s/|%s|port]\n", prevgenpath,
			    pagebaseaddr) != 16 + (int) (strlen(prevgenpath) +
			    strlen(pagebaseaddr))) {
				(void)fprintf(stderr, "staticpagegen: can't write to file '%s'\n",
				    gopherindexfil);
				return 1;
			}
		}
		for (i = 0; i < subdirs_n; i++) {
			if (fprintf(idxptr, "[1|%s/|%s/%s/|%s|port]\n", subdirs[i], genpath,
			    subdirs[i], pagebaseaddr) != 15 + (int) (2 *
			    strlen(subdirs[i]) + strlen(genpath) +
			    strlen(pagebaseaddr))) {
				(void)fprintf(stderr, "staticpagegen: can't write to file '%s'\n",
				    gopherindexfil);
				return 1;
			}
		}
		for (i = 0; i < subfils_n; i++) {
			if (fprintf(idxptr, "[0|%s|%s/%s|%s|port]\n", subfils[i], genpath,
			    subfils[i], pagebaseaddr) != 13 + (int) (2 *
			    strlen(subfils[i]) + strlen(genpath) +
			    strlen(pagebaseaddr))) {
				(void)fprintf(stderr, "staticpagegen: can't write to file '%s'\n",
				    gopherindexfil);
				return 1;
			}
		}
		if (fclose(idxptr) != 0)
			return staticpagegen_perror(gopherindexfil);
	}
	return 0;
}

/*
 * staticpagegen_perror - prints the program name alongside the error message.
 *	probstr - string related to the problem.
 */
int
staticpagegen_perror(char *probstr)
{
	(void)fprintf(stderr, "staticpagegen: ");
	perror(probstr);
	return errno;
}
