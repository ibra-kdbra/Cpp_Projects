#ifndef _STATICPAGEGEN_H_
#define _STATICPAGEGEN_H_
#define STATICPAGEGEN_VERSION "0.1.1"
/* Maximum amount of files or directories accepted in a directory */
#define SUBDIRN_MAX 256
extern enum Gentype {
	GEN_HG,
	GEN_HONLY,
	GEN_GONLY
} gen;

extern enum Htmlspec {
	HTM_XHTMLBASIC10
} htmspec;

extern enum Gopherspec {
	GPH_GEOMYIDAE
	/* TODO: Implement gofish and others. */
} gphspec;
extern char *pagetitle;
extern char *pagebaseaddr;
extern struct stat cwdstbuf;
extern int debuglvl;
extern int ignoresymlinks;
int staticpagegen_main(char *);
int itersrc(char *, char *);
int mkpagepaths(char *);
int cpfiles(char *, char *);
int wrpages(char *, char **, unsigned int, char **, unsigned int);
int staticpagegen_perror(char *);
#endif /* _STATICPAGEGEN_H_ */
