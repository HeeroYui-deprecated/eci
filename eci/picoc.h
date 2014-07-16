/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_H__
#define __ECI_H__

/* eci version number */
#define ECI_VERSION "v0.1"

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#include <setjmp.h>

/* mark where to end the program for platforms which require this */
extern jmp_buf PicocExitBuf;

/* this has to be a macro, otherwise errors will occur due to the stack being corrupt */
#define PicocPlatformSetExitPoint() setjmp(PicocExitBuf)

/* parse.c */
void PicocParse(const char *_fileName, const char *_source, int _sourceLen, int _runIt, int _cleanupNow, int _cleanupSource);
void PicocParseInteractive();

/* platform.c */
void PicocCallMain(int _argc, char **_argv);
void PicocInitialise(int _stackSize);
void PicocCleanup();
void PicocPlatformScanFile(const char *_fileName);

extern int PicocExitValue;

/* include.c */
void PicocIncludeAllSystemHeaders();

#endif
