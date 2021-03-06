/* 
 * tclMain.c --
 *
 *	Main program for Tcl shells and other Tcl-based applications.
 *
 * Copyright (c) 1988-1994 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 * Copyright (c) 2000 Ajuba Solutions.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclMain.c,v 1.20.2.3 2006/05/05 18:08:58 dgp Exp $
 */

#include "tcl.h"
#include "tclInt.h"
#include <unistd.h> /* for getcwd */
#include <stdlib.h>
#include <getopt.h>

#define CLACKWISE_NAME "Clackwise"
#define CLACKWISE_VERSION "0.1.0 (alpha)"

# undef TCL_STORAGE_CLASS
# define TCL_STORAGE_CLASS DLLEXPORT

/*
 * Declarations for various library procedures and variables (don't want
 * to include tclPort.h here, because people might copy this file out of
 * the Tcl source directory to make their own modified versions).
 */

#if !defined(MAC_TCL)
extern int		isatty _ANSI_ARGS_((int fd));
#else
#include <unistd.h>
#endif

static Tcl_Obj *tclStartupScriptPath = NULL;

static Tcl_MainLoopProc *mainLoopProc = NULL;

/* 
 * Structure definition for information used to keep the state of
 * an interactive command processor that reads lines from standard
 * input and writes prompts and results to standard output.
 */

typedef enum {
    PROMPT_NONE,	/* Print no prompt */
    PROMPT_START,	/* Print prompt for command start */
    PROMPT_CONTINUE	/* Print prompt for command continuation */
} PromptType;

typedef struct InteractiveState {
    Tcl_Channel input;		/* The standard input channel from which
				 * lines are read. */
    int tty;                    /* Non-zero means standard input is a 
				 * terminal-like device.  Zero means it's
				 * a file. */
    Tcl_Obj *commandPtr;	/* Used to assemble lines of input into
				 * Tcl commands. */
    PromptType prompt;		/* Next prompt to print */
    Tcl_Interp *interp;		/* Interpreter that evaluates interactive
				 * commands. */
} InteractiveState;

/*
 * Forward declarations for procedures defined later in this file.
 */

static void		Prompt _ANSI_ARGS_((Tcl_Interp *interp,
			    PromptType *promptPtr));
static void		StdinProc _ANSI_ARGS_((ClientData clientData,
			    int mask));


/*
 *----------------------------------------------------------------------
 *
 * TclSetStartupScriptPath --
 *
 *	Primes the startup script VFS path, used to override the
 *      command line processing.
 *
 * Results:
 *	None. 
 *
 * Side effects:
 *	This procedure initializes the VFS path of the Tcl script to
 *      run at startup.
 *
 *----------------------------------------------------------------------
 */
void TclSetStartupScriptPath(pathPtr)
    Tcl_Obj *pathPtr;
{
    if (tclStartupScriptPath != NULL) {
	Tcl_DecrRefCount(tclStartupScriptPath);
    }
    tclStartupScriptPath = pathPtr;
    if (tclStartupScriptPath != NULL) {
	Tcl_IncrRefCount(tclStartupScriptPath);
    }
}


/*
 *----------------------------------------------------------------------
 *
 * TclGetStartupScriptPath --
 *
 *	Gets the startup script VFS path, used to override the
 *      command line processing.
 *
 * Results:
 *	The startup script VFS path, NULL if none has been set.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
Tcl_Obj *TclGetStartupScriptPath()
{
    return tclStartupScriptPath;
}


/*
 *----------------------------------------------------------------------
 *
 * TclSetStartupScriptFileName --
 *
 *	Primes the startup script file name, used to override the
 *      command line processing.
 *
 * Results:
 *	None. 
 *
 * Side effects:
 *	This procedure initializes the file name of the Tcl script to
 *      run at startup.
 *
 *----------------------------------------------------------------------
 */
void TclSetStartupScriptFileName(fileName)
    CONST char *fileName;
{
    Tcl_Obj *pathPtr = Tcl_NewStringObj(fileName,-1);
    TclSetStartupScriptPath(pathPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * TclGetStartupScriptFileName --
 *
 *	Gets the startup script file name, used to override the
 *      command line processing.
 *
 * Results:
 *	The startup script file name, NULL if none has been set.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */
CONST char *TclGetStartupScriptFileName()
{
    Tcl_Obj *pathPtr = TclGetStartupScriptPath();

    if (pathPtr == NULL) {
	return NULL;
    }
    return Tcl_GetString(pathPtr);
}

char *getClackwiseRoot(char *appPath) {
    char cwRoot[1024] = "";
    int i, prevDirEllipseCount, dirTextStartPos, lastSlashPos;
    dirTextStartPos = 0;
    prevDirEllipseCount = 0;
    for (i = 0; appPath[i] != 0; i++) {
        if (appPath[i] == '.' && appPath[i+1] == '.' && appPath[i+2] == '/') {
            prevDirEllipseCount++;
            dirTextStartPos = i + 3;
            i += 2;
        }
    }

    if (appPath[0] == '/') {
        strcpy(cwRoot, appPath + dirTextStartPos);
    } else {
        getcwd(cwRoot, 1024);
        if (cwRoot[0] != '/') {
            printf("Internal error: Cannot get current work directory. Exiting.\n");
            return 0;
        }
        for (i = 0; cwRoot[i] != 0; i++) {}
        for (i--; i > 0; i--) { // search backwards for slashes
            if (prevDirEllipseCount <= 0) {
                break;
            }
            if (cwRoot[i] == '/') {
                prevDirEllipseCount--;
                cwRoot[i] = 0;
            }
        }
        strcat(cwRoot, "/");
        strcat(cwRoot, appPath + dirTextStartPos);
    }
    lastSlashPos = 0; // the first char is always slash
    for (i = 0; cwRoot[i] != 0; i++) {
        if (cwRoot[i] == '/') {
            lastSlashPos = i;
        }
    }
    *(cwRoot + lastSlashPos) = 0;
    return cwRoot;
}


/*
 *----------------------------------------------------------------------
 *
 * Tcl_Main --
 *
 *	Main program for tclsh and most other Tcl-based applications.
 *
 * Results:
 *	None. This procedure never returns (it exits the process when
 *	it's done).
 *
 * Side effects:
 *	This procedure initializes the Tcl world and then starts
 *	interpreting commands;  almost anything could happen, depending
 *	on the script being interpreted.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_Main(argc, argv, appInitProc)
    int argc;			/* Number of arguments. */
    char **argv;		/* Array of argument strings. */
    Tcl_AppInitProc *appInitProc;
				/* Application-specific initialization
				 * procedure to call after most
				 * initialization but before starting to
				 * execute commands. */
{
    Tcl_Obj *resultPtr, *argvPtr, *commandPtr = NULL;
    PromptType prompt = PROMPT_START;
    int code, length, tty, exitCode = 0;
    Tcl_Channel inChannel, outChannel, errChannel;
    Tcl_Interp *interp;
    Tcl_DString appName;
    Tcl_Obj *objPtr;
	char cwInitTcl[255] = "";
	FILE *cwInitTcl_fp;

    /* Handle cw_shell command line options */
    int c;
    int cmdline_error = 0;
    static int cmdline_version = 0;
    static int cmdline_help = 0;
    static int cmdline_no_init = 0;
    char cmdline_f[1024] = "";
    char cmdline_x[1024] = "";
    int option_index = 0;
    static struct option long_options[] = {
        {"version", no_argument, &cmdline_version, 1},
        {"help", no_argument, &cmdline_help, 1},
        {"no_init", no_argument, &cmdline_no_init, 1},
        {0, 0, 0, 0}
    };
    while (1) {
        if ((c = getopt_long_only(argc, argv, "f:x:", long_options, &option_index)) == -1)
            break;
        switch (c) {
            case 'f':
                strcpy(cmdline_f, optarg);
                break;
            case 'x':
                strcpy(cmdline_x, optarg);
                break;
            case '?':
                cmdline_error = 1;
                break;
        }
    }
    if (cmdline_help) {
        printf("Usage: cw_shell [-no_init] [-f file] [-x command] [-version] [-help]\n", argv[0]);
        exit(0);
    }
    if (cmdline_version) {
        printf("%s %s\n", CLACKWISE_NAME, CLACKWISE_VERSION);
        exit(0);
    }
    if (cmdline_error) {
        exit(1);
    }

    interp = Tcl_CreateInterp();
    Tcl_InitMemory(interp);
#if defined(CLACKWISE_NAME) && defined(CLACKWISE_VERSION)
    Tcl_WriteChars(Tcl_GetStdChannel(TCL_STDOUT), CLACKWISE_NAME, sizeof(CLACKWISE_NAME));
    Tcl_WriteChars(Tcl_GetStdChannel(TCL_STDOUT), " ", 1);
    Tcl_WriteChars(Tcl_GetStdChannel(TCL_STDOUT), CLACKWISE_VERSION, sizeof(CLACKWISE_VERSION));
    Tcl_WriteChars(Tcl_GetStdChannel(TCL_STDOUT), "\n", 1);
#endif

    char *cwRoot = getClackwiseRoot(argv[0]);
    Tcl_DString cwRootDString;
    Tcl_ExternalToUtfDString(NULL, cwRoot, -1, &cwRootDString);
    Tcl_SetVar(interp, "clackwise_root_path", Tcl_DStringValue(&cwRootDString), TCL_GLOBAL_ONLY);
    strcat(cwInitTcl, cwRoot);
    strcat(cwInitTcl, "/lib/clackwise/init.tcl");
    if (cwInitTcl_fp = fopen(cwInitTcl, "r")) {
        fclose(cwInitTcl_fp);
        TclSetStartupScriptFileName(cwInitTcl);
    } else {
        printf("Internal error: Cannot open initialization file %s. Exiting.\n", cwInitTcl);
        goto done;
    }

    /*
     * Set Tcl variables based on command line arguments:
     *      -f file -> $init_eval_file
     *      -x command -> $init_eval_command
     *      -no_init -> $init_disable_clackwiserc
     */

    Tcl_DString init_eval_file, init_eval_command;
	Tcl_ExternalToUtfDString(NULL, cmdline_f, -1, &init_eval_file);
	Tcl_ExternalToUtfDString(NULL, cmdline_x, -1, &init_eval_command);
    Tcl_SetVar(interp, "init_eval_file", Tcl_DStringValue(&init_eval_file), TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "init_eval_command", Tcl_DStringValue(&init_eval_command), TCL_GLOBAL_ONLY);
    Tcl_DStringFree(&init_eval_file);
    Tcl_DStringFree(&init_eval_command);

    Tcl_Obj *init_disable_clackwiserc;
    init_disable_clackwiserc = Tcl_NewIntObj(cmdline_no_init);
    Tcl_IncrRefCount(init_disable_clackwiserc);
    Tcl_SetVar2Ex(interp, "init_disable_clackwiserc", NULL, init_disable_clackwiserc, TCL_GLOBAL_ONLY);
    Tcl_DecrRefCount(init_disable_clackwiserc);

    /*
     * Set the "tcl_interactive" variable.
     */

    tty = isatty(0);
    Tcl_SetVar(interp, "tcl_interactive",
	    ((TclGetStartupScriptPath() == NULL) && tty) ? "1" : "0",
	    TCL_GLOBAL_ONLY);
    
    /*
     * Invoke application-specific initialization.
     */

    Tcl_Preserve((ClientData) interp);
    if ((*appInitProc)(interp) != TCL_OK) {
	errChannel = Tcl_GetStdChannel(TCL_STDERR);
	if (errChannel) {
	    Tcl_WriteChars(errChannel,
		    "application-specific initialization failed: ", -1);
	    Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
	    Tcl_WriteChars(errChannel, "\n", 1);
	}
    }
    if (Tcl_InterpDeleted(interp)) {
	goto done;
    }

    /*
     * If a script file was specified then just source that file
     */

    if (TclGetStartupScriptPath() != NULL) {
	code = Tcl_FSEvalFile(interp, TclGetStartupScriptPath());
	if (code != TCL_OK) {
	    errChannel = Tcl_GetStdChannel(TCL_STDERR);
	    if (errChannel) {

		/*
		 * The following statement guarantees that the errorInfo
		 * variable is set properly.
		 */

		Tcl_AddErrorInfo(interp, "");
		Tcl_WriteObj(errChannel, Tcl_GetVar2Ex(interp, "errorInfo",
			NULL, TCL_GLOBAL_ONLY));
		Tcl_WriteChars(errChannel, "\n", 1);
	    }
	    exitCode = 1;
	}
    }

    /*
     * We're running interactively.  Source a user-specific startup
     * file if the application specified one and if the file exists.
     */

    Tcl_SourceRCFile(interp);

    /*
     * Process commands from stdin until there's an end-of-file.  Note
     * that we need to fetch the standard channels again after every
     * eval, since they may have been changed.
     */

    commandPtr = Tcl_NewObj();
    Tcl_IncrRefCount(commandPtr);

    /*
     * Get a new value for tty if anyone writes to ::tcl_interactive
     */
    Tcl_LinkVar(interp, "tcl_interactive", (char *) &tty, TCL_LINK_BOOLEAN);
    inChannel = Tcl_GetStdChannel(TCL_STDIN);
    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    while ((inChannel != (Tcl_Channel) NULL) && !Tcl_InterpDeleted(interp)) {
	if (mainLoopProc == NULL) {
	    if (tty) {
		Prompt(interp, &prompt);
		if (Tcl_InterpDeleted(interp)) {
		    break;
		}
		inChannel = Tcl_GetStdChannel(TCL_STDIN);
		if (inChannel == (Tcl_Channel) NULL) {
	            break;
		}
	    }
	    if (Tcl_IsShared(commandPtr)) {
		Tcl_DecrRefCount(commandPtr);
		commandPtr = Tcl_DuplicateObj(commandPtr);
		Tcl_IncrRefCount(commandPtr);
	    }
            length = Tcl_GetsObj(inChannel, commandPtr);
	    if (length < 0) {
		if (Tcl_InputBlocked(inChannel)) {

		    /*
		     * This can only happen if stdin has been set to
		     * non-blocking.  In that case cycle back and try
		     * again.  This sets up a tight polling loop (since
		     * we have no event loop running).  If this causes
		     * bad CPU hogging, we might try toggling the blocking
		     * on stdin instead.
		     */

		    continue;
		}

		/* 
		 * Either EOF, or an error on stdin; we're done
		 */

		break;
	    }

            /*
             * Add the newline removed by Tcl_GetsObj back to the string.
             */

	    if (Tcl_IsShared(commandPtr)) {
		Tcl_DecrRefCount(commandPtr);
		commandPtr = Tcl_DuplicateObj(commandPtr);
		Tcl_IncrRefCount(commandPtr);
	    }
	    Tcl_AppendToObj(commandPtr, "\n", 1);
	    if (!TclObjCommandComplete(commandPtr)) {
		prompt = PROMPT_CONTINUE;
		continue;
	    }

	    prompt = PROMPT_START;
	    code = Tcl_RecordAndEvalObj(interp, commandPtr, TCL_EVAL_GLOBAL);
	    inChannel = Tcl_GetStdChannel(TCL_STDIN);
	    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
	    errChannel = Tcl_GetStdChannel(TCL_STDERR);
	    Tcl_DecrRefCount(commandPtr);
	    commandPtr = Tcl_NewObj();
	    Tcl_IncrRefCount(commandPtr);
	    if (code != TCL_OK) {
		if (errChannel) {
		    Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
		    Tcl_WriteChars(errChannel, "\n", 1);
		}
	    } else if (tty) {
		resultPtr = Tcl_GetObjResult(interp);
		Tcl_IncrRefCount(resultPtr);
		char *resultStr = Tcl_GetStringFromObj(resultPtr, &length);
		if ((length > 0) && outChannel) {
			if (resultStr[0] == '_' && strstr(resultStr, "_p_Clackwise__") != 0) { // if result is a clackwise object list
				char *buffer = (char *) malloc((length + 50) * sizeof(char));
				sprintf(buffer, "show_objects {%s}", resultStr);
				Tcl_Eval(interp, buffer);
			} else { // if result is a tcl object, print it out
				Tcl_WriteObj(outChannel, resultPtr);
				Tcl_WriteChars(outChannel, "\n", 1);
			}
		}
		Tcl_DecrRefCount(resultPtr);
	    }
	} else {	/* (mainLoopProc != NULL) */
	    /*
	     * If a main loop has been defined while running interactively,
	     * we want to start a fileevent based prompt by establishing a
	     * channel handler for stdin.
	     */

	    InteractiveState *isPtr = NULL;

	    if (inChannel) {
	        if (tty) {
		    Prompt(interp, &prompt);
	        }
		isPtr = (InteractiveState *) 
			ckalloc((int) sizeof(InteractiveState));
		isPtr->input = inChannel;
		isPtr->tty = tty;
		isPtr->commandPtr = commandPtr;
		isPtr->prompt = prompt;
		isPtr->interp = interp;

		Tcl_UnlinkVar(interp, "tcl_interactive");
		Tcl_LinkVar(interp, "tcl_interactive", (char *) &(isPtr->tty),
			TCL_LINK_BOOLEAN);

		Tcl_CreateChannelHandler(inChannel, TCL_READABLE, StdinProc,
			(ClientData) isPtr);
	    }

	    (*mainLoopProc)();
	    mainLoopProc = NULL;

	    if (inChannel) {
		tty = isPtr->tty;
		Tcl_UnlinkVar(interp, "tcl_interactive");
		Tcl_LinkVar(interp, "tcl_interactive", (char *) &tty,
			TCL_LINK_BOOLEAN);
		prompt = isPtr->prompt;
		commandPtr = isPtr->commandPtr;
		if (isPtr->input != (Tcl_Channel) NULL) {
		    Tcl_DeleteChannelHandler(isPtr->input, StdinProc,
			    (ClientData) isPtr);
		}
		ckfree((char *)isPtr);
	    }
	    inChannel = Tcl_GetStdChannel(TCL_STDIN);
	    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
	    errChannel = Tcl_GetStdChannel(TCL_STDERR);
	}
#ifdef TCL_MEM_DEBUG

	/*
	 * This code here only for the (unsupported and deprecated)
	 * [checkmem] command.
	 */

	if (tclMemDumpFileName != NULL) {
	    mainLoopProc = NULL;
	    Tcl_DeleteInterp(interp);
	}
#endif
    }

    done:
    if ((exitCode == 0) && (mainLoopProc != NULL)) {

	/*
	 * If everything has gone OK so far, call the main loop proc,
	 * if it exists.  Packages (like Tk) can set it to start processing
	 * events at this point.
	 */

	(*mainLoopProc)();
	mainLoopProc = NULL;
    }
    if (commandPtr != NULL) {
	Tcl_DecrRefCount(commandPtr);
    }

    /*
     * Rather than calling exit, invoke the "exit" command so that
     * users can replace "exit" with some other command to do additional
     * cleanup on exit.  The Tcl_Eval call should never return.
     */

    if (!Tcl_InterpDeleted(interp)) {
        char buffer[TCL_INTEGER_SPACE + 5];
        sprintf(buffer, "exit %d", exitCode);
        Tcl_Eval(interp, buffer);

        /*
         * If Tcl_Eval returns, trying to eval [exit], something
         * unusual is happening.  Maybe interp has been deleted;
         * maybe [exit] was redefined.  We still want to cleanup
         * and exit.
         */

        if (!Tcl_InterpDeleted(interp)) {
            Tcl_DeleteInterp(interp);
        }
    }
    TclSetStartupScriptPath(NULL);

    /*
     * If we get here, the master interp has been deleted.  Allow
     * its destruction with the last matching Tcl_Release.
     */

    Tcl_Release((ClientData) interp);
    Tcl_Exit(exitCode);
}

/*
 *---------------------------------------------------------------
 *
 * Tcl_SetMainLoop --
 *
 *	Sets an alternative main loop procedure.
 *
 * Results:
 *	Returns the previously defined main loop procedure.
 *
 * Side effects:
 *	This procedure will be called before Tcl exits, allowing for
 *	the creation of an event loop.
 *
 *---------------------------------------------------------------
 */

void
Tcl_SetMainLoop(proc)
    Tcl_MainLoopProc *proc;
{
    mainLoopProc = proc;
}

/*
 *----------------------------------------------------------------------
 *
 * StdinProc --
 *
 *	This procedure is invoked by the event dispatcher whenever
 *	standard input becomes readable.  It grabs the next line of
 *	input characters, adds them to a command being assembled, and
 *	executes the command if it's complete.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Could be almost arbitrary, depending on the command that's
 *	typed.
 *
 *----------------------------------------------------------------------
 */

    /* ARGSUSED */
static void
StdinProc(clientData, mask)
    ClientData clientData;		/* The state of interactive cmd line */
    int mask;				/* Not used. */
{
    InteractiveState *isPtr = (InteractiveState *) clientData;
    Tcl_Channel chan = isPtr->input;
    Tcl_Obj *commandPtr = isPtr->commandPtr;
    Tcl_Interp *interp = isPtr->interp;
    int code, length;

    if (Tcl_IsShared(commandPtr)) {
	Tcl_DecrRefCount(commandPtr);
	commandPtr = Tcl_DuplicateObj(commandPtr);
	Tcl_IncrRefCount(commandPtr);
    }
    length = Tcl_GetsObj(chan, commandPtr);
    if (length < 0) {
	if (Tcl_InputBlocked(chan)) {
	    return;
	}
	if (isPtr->tty) {
	    /*
	     * Would be better to find a way to exit the mainLoop?
	     * Or perhaps evaluate [exit]?  Leaving as is for now due
	     * to compatibility concerns.
	     */
	    Tcl_Exit(0);
	}
	Tcl_DeleteChannelHandler(chan, StdinProc, (ClientData) isPtr);
	return;
    }

    if (Tcl_IsShared(commandPtr)) {
	Tcl_DecrRefCount(commandPtr);
	commandPtr = Tcl_DuplicateObj(commandPtr);
	Tcl_IncrRefCount(commandPtr);
    }
    Tcl_AppendToObj(commandPtr, "\n", 1);
    if (!TclObjCommandComplete(commandPtr)) {
        isPtr->prompt = PROMPT_CONTINUE;
        goto prompt;
    }
    isPtr->prompt = PROMPT_START;

    /*
     * Disable the stdin channel handler while evaluating the command;
     * otherwise if the command re-enters the event loop we might
     * process commands from stdin before the current command is
     * finished.  Among other things, this will trash the text of the
     * command being evaluated.
     */

    Tcl_CreateChannelHandler(chan, 0, StdinProc, (ClientData) isPtr);
    code = Tcl_RecordAndEvalObj(interp, commandPtr, TCL_EVAL_GLOBAL);
    isPtr->input = chan = Tcl_GetStdChannel(TCL_STDIN);
    Tcl_DecrRefCount(commandPtr);
    isPtr->commandPtr = commandPtr = Tcl_NewObj();
    Tcl_IncrRefCount(commandPtr);
    if (chan != (Tcl_Channel) NULL) {
	Tcl_CreateChannelHandler(chan, TCL_READABLE, StdinProc,
		(ClientData) isPtr);
    }
    if (code != TCL_OK) {
	Tcl_Channel errChannel = Tcl_GetStdChannel(TCL_STDERR);
	if (errChannel != (Tcl_Channel) NULL) {
	    Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
	    Tcl_WriteChars(errChannel, "\n", 1);
	}
    } else if (isPtr->tty) {
	Tcl_Obj *resultPtr = Tcl_GetObjResult(interp);
	Tcl_Channel outChannel = Tcl_GetStdChannel(TCL_STDOUT);
	Tcl_IncrRefCount(resultPtr);
	Tcl_GetStringFromObj(resultPtr, &length);
	if ((length >0) && (outChannel != (Tcl_Channel) NULL)) {
	    Tcl_WriteObj(outChannel, resultPtr);
	    Tcl_WriteChars(outChannel, "\n", 1);
	}
	Tcl_DecrRefCount(resultPtr);
    }

    /*
     * If a tty stdin is still around, output a prompt.
     */

    prompt:
    if (isPtr->tty && (isPtr->input != (Tcl_Channel) NULL)) {
	Prompt(interp, &(isPtr->prompt));
	isPtr->input = Tcl_GetStdChannel(TCL_STDIN);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Prompt --
 *
 *	Issue a prompt on standard output, or invoke a script
 *	to issue the prompt.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	A prompt gets output, and a Tcl script may be evaluated
 *	in interp.
 *
 *----------------------------------------------------------------------
 */

static void
Prompt(interp, promptPtr)
    Tcl_Interp *interp;			/* Interpreter to use for prompting. */
    PromptType *promptPtr;		/* Points to type of prompt to print.
					 * Filled with PROMPT_NONE after a
					 * prompt is printed. */
{
    Tcl_Obj *promptCmdPtr;
    int code;
    Tcl_Channel outChannel, errChannel;

    if (*promptPtr == PROMPT_NONE) {
	return;
    }

    promptCmdPtr = Tcl_GetVar2Ex(interp,
	    ((*promptPtr == PROMPT_CONTINUE) ? "tcl_prompt2" : "tcl_prompt1"),
	    NULL, TCL_GLOBAL_ONLY);
    if (Tcl_InterpDeleted(interp)) {
	return;
    }
    if (promptCmdPtr == NULL) {
	defaultPrompt:
	outChannel = Tcl_GetStdChannel(TCL_STDOUT);
	if ((*promptPtr == PROMPT_START)
		&& (outChannel != (Tcl_Channel) NULL)) {
	    Tcl_WriteChars(outChannel, "% ", 2);
	}
    } else {
	code = Tcl_EvalObjEx(interp, promptCmdPtr, TCL_EVAL_GLOBAL);
	if (code != TCL_OK) {
	    Tcl_AddErrorInfo(interp,
		    "\n    (script that generates prompt)");
	    errChannel = Tcl_GetStdChannel(TCL_STDERR);
            if (errChannel != (Tcl_Channel) NULL) {
                Tcl_WriteObj(errChannel, Tcl_GetObjResult(interp));
                Tcl_WriteChars(errChannel, "\n", 1);
            }
	    goto defaultPrompt;
	}
    }
    outChannel = Tcl_GetStdChannel(TCL_STDOUT);
    if (outChannel != (Tcl_Channel) NULL) {
	Tcl_Flush(outChannel);
    }
    *promptPtr = PROMPT_NONE;
}
