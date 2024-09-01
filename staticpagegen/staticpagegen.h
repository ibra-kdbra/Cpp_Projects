/*
 * Copyright (c) 2023-2024 Kaan Çırağ
 *
 * This file is part of staticpagegen.
 *
 * staticpagegen is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation, version 3 of the License only.
 * 
 * staticpagegen is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with staticpagegen. If not, see <https://www.gnu.org/licenses/>.
 */
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
