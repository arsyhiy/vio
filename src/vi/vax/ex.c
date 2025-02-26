// [a few]


#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include "ex.h"
#include "ex_argv.h"// заголовочный файл для выделения памяти и динамической памяти. переписать
#include "ex_temp.h"// работа с буферами и востановление памяти. переписать
#include "ex_tty.h"// взаимодействие с терминалом, управление курсором, изменение терминальных флагов и маппинг клавиш. переписать


/*
 * The code for ex is divided as follows:
 *
 * ex.c			Entry point and routines handling interrupt, hangup
 *			signals; initialization code.
 *
 * ex_addr.c		Address parsing routines for command mode decoding.
 *			Routines to set and check address ranges on commands.
 *
 * ex_cmds.c		Command mode command decoding.
 *
 * ex_cmds2.c		Subroutines for command decoding and processing of
 *			file names in the argument list.  Routines to print
 *			messages and reset state when errors occur.
 *
 * ex_cmdsub.c		Subroutines which implement command mode functions
 *			such as append, delete, join.
 *
 * ex_data.c		Initialization of options.
 *
 * ex_get.c		Command mode input routines.
 *
 * ex_io.c		General input/output processing: file i/o, unix
 *			escapes, filtering, source commands, preserving
 *			and recovering.
 *
 * ex_put.c		Terminal driving and optimizing routines for low-level
 *			output (cursor-positioning); output line formatting
 *			routines.
 *
 * ex_re.c		Global commands, substitute, regular expression
 *			compilation and execution.
 *
 * ex_set.c		The set command.
 *
 * ex_subr.c		Loads of miscellaneous subroutines.
 *
 * ex_temp.c		Editor buffer routines for main buffer and also
 *			for named buffers (Q registers if you will.)
 *
 * ex_tty.c		Terminal dependent initializations from termcap
 *			data base, grabbing of tty modes (at beginning
 *			and after escapes).
 *
 * ex_unix.c		Routines for the ! command and its variations.
 *
 * ex_v*.c		Visual/open mode routines... see ex_v.c for a
 *			guide to the overall organization.
 */

/*
 * Main procedure.  Process arguments and then
 * transfer control to the main command processing loop
 * in the routine commands.  We are entered as either "ex", "edit", "vi"
 * or "view" and the distinction is made here.  Actually, we are "vi" if
 * there is a 'v' in our name, "view" is there is a 'w', and "edit" if
 * there is a 'd' in our name.  For edit we just diddle options;
 * for vi we actually force an early visual command.
 */


 main(ac, av)
	register int ac;
	register char *av[];
{


	char *erpath = EXSTRINGS;
	register char *cp;
	register int c;
	bool recov = 0;
	bool ivis;
	bool itag = 0;
	bool fast = 0;
	extern int onemt();
	extern int oncore();
	extern int verbose;
	struct termios old_termios, new_termios;
	normf = tcgetattr(STDIN_FILENO, &old_termios);  // Получаем текущие настройки терминала

	/*
	В целом, код совместим с Linux. Однако, чтобы сделать его более гибким и 
	современным, можно использовать библиотеки ncurses или terminfo для динамического 
	получения информации о терминале. Также стоит убедиться, что вспомогательные функции,
	 такие как any(), работают правильно для строковых данных.
	*/

	ppid = getpid();
	/* Note - this will core dump if you didn't -DSINGLE in CFLAGS */
	lines = 24;
	columns = 80;	/* until defined right by setupterm */
	/*
	 * Defend against d's, v's, w's, and a's in directories of
	 * path leading to our true name.
	 */
	av[0] = tailpath(av[0]);

	/*
	 * Figure out how we were invoked: ex, edit, vi, view.
	 */
	ivis = any('v', av[0]);	/* "vi" */
	if (any('w', av[0]))	/* "view" */
		value(READONLY) = 1;
	if (any('d', av[0])) {	/* "edit" or "vedit" */

		value(NOVICE) = 1;
		value(REPORT) = 1;
		value(MAGIC) = 0;
		value(SHOWMODE) = 1;
	}
	
#ifndef __linux__
	/*
	 * For debugging take files out of . if name is a.out.
	 */
	if (av[0][0] == 'a')
		erpath = tailpath(erpath);
#endif
	/*
	 * Open the error message file.
	 */
	draino();
#ifndef __linux__
	erfile = open(erpath+4, 0);
	if (erfile < 0) {
		erfile = open(erpath, 0);
	}
#endif

	pstop(); // NOTE:  посмотреть реализацию
	// /*
	//  * Initialize interrupt handling.
	//  */
	oldhup = sigaction(SIGHUP, SIG_IGN);  
	if (oldhup == SIG_DFL)
		sigaction(SIGHUP, onhup);
	oldquit = sigaction(SIGQUIT, SIG_IGN);
	ruptible = sigaction(SIGINT, SIG_IGN) == SIG_DFL;
	if (sigaction(SIGTERM, SIG_IGN) == SIG_DFL)
		sigaction(SIGTERM, onhup);
	sigaction(SIGILL, oncore);
	sigaction(SIGTRAP, oncore);
	sigaction(SIGIOT, oncore);// надо будет смотреть имплементацию этих обработчиков ошибок
	sigaction(SIGFPE, oncore);
	sigaction(SIGBUS, oncore);
	sigaction(SIGSEGV, oncore);
	sigaction(SIGPIPE, oncore);

	/*
	 * Initialize end of core pointers.
	 * Normally we avoid breaking back to fendcore after each
	 * file since this can be expensive (much core-core copying).
	 * If your system can scatter load processes you could do
	 * this as ed does, saving a little core, but it will probably
	 * not often make much difference.
	 */

	/*
	NOTE: эти две строки кода работают с паматью их надо переписать под linux так как sbrk устарел. надо перепесать под malloc() free() 
	*/
	// fendcore = (line *) sbrk(0); 
	// endcore = fendcore - 2;

	// Предположим, что нужно выделить память для массива line
	size_t initial_size = 1000; // Размер памяти, который нужно выделить для line (можно настроить по необходимости)

	fendcore = (line *) malloc(sizeof(line) * initial_size); // Выделяем память
	if (fendcore == NULL) {
		// Обработка ошибки выделения памяти
		perror("malloc failed");
		exit(1);
	}

	// Для endcore, предположим, что необходимо сместить указатель на два элемента назад
	endcore = fendcore + initial_size - 2;

	/*
		в этом куске кода приводится флаги при вызове vi  по типу : $ vi -l
		обработка ошибок и выделение памяти также как и верхние строки кода должен быть редактирован
	*/

	/*
	 * Process flag arguments.
	 */
	ac--, av++;
	while (ac && av[0][0] == '-') {
		c = av[0][1];
		if (c == 0) {
			hush = 1;
			value(AUTOPRINT) = 0;
			fast++;
		} else switch (c) {

		case 'R':
			value(READONLY) = 1;
			break;
		case 'l':
			value(LISP) = 1;
			value(SHOWMATCH) = 1;
			break;

		case 'r':
			printf("recover is under construction");
			break;

		case 'V':
			verbose = 1;
			break;

		case 't':
			if (ac > 1 && av[1][0] != '-') {
				ac--, av++;
				itag = 1;
				/* BUG: should check for too long tag. */
				CP(lasttag, av[0]);
			}
			break;

		case 'v':
			ivis = 1;
			break;

		case 'w':
			defwind = 0;
			if (av[0][2] == 0) defwind = 3;
			else for (cp = &av[0][2]; isdigit(*cp); cp++)
				defwind = 10*defwind + *cp - '0';
			break;


		default:
			printf("Unknown option  %s\n", av[0]); // пересмотреть потом реализацию
			break;
		}
		ac--, av++;
	}

#ifdef SIGTSTP
	if (!hush && signal(SIGTSTP, SIG_IGN) == SIG_DFL)
		signal(SIGTSTP, onsusp), dosusp++;
#endif

	if (ac && av[0][0] == '+') {
		firstpat = &av[0][1];
		ac--, av++;
	}

	/*
	 * If we are doing a recover and no filename
	 * was given, then execute an exrecover command with
	 * the -r option to type out the list of saved file names.
	 * Otherwise set the remembered file name to the first argument
	 * file name so the "recover" initial command will find it.
	 */

	 /* было решено закоментировать систему востановления файла а не удалить.
	   переделать систему что бы она создавала swap файл в месте с новым буфером и чтобы можно отключить создание swap файла
	*/
	// if (recov) { 
	// 	if (ac == 0) {
	// 		ppid = 0;
	// 		setrupt();
	// 		execl(EXRECOVER, "exrecover", "-r", 0);
	// 		filioerr(EXRECOVER);
	// 		exit(1);
	// 	}
	// 	CP(savedfile, *av++), ac--;
	// }

	/*
	 * Initialize the argument list.
	 */
	argv0 = av;
	argc0 = ac;
	args0 = av[0];
	erewind();

	/*
	 * Initialize a temporary file (buffer) and
	 * set up terminal environment.  Read user startup commands.
	 */
	if (setexit() == 0) {
		setrupt();
		intty = isatty(0);
		value(PROMPT) = intty;
		if (cp = getenv("SHELL")) 
			CP(shell, cp);
		if (fast)
			setterm("dumb");
		else {
			gettmode();
			cp = getenv("TERM");
			if (cp == NULL || *cp == '\0')
				cp = "unknown";
			setterm(cp);
		}
	}
	if (setexit() == 0 && !fast) {
		if ((globp = getenv("EXINIT")) && *globp)
			commands(1,1);
		else {
			globp = 0;
			if ((cp = getenv("HOME")) != 0 && *cp)
				source(strcat(strcpy(genbuf, cp), "/.exrc"), 1);
		}
		/*
		 * Allow local .exrc too.  This loses if . is $HOME,
		 * but nobody should notice unless they do stupid things
		 * like putting a version command in .exrc.  Besides,
		 * they should be using EXINIT, not .exrc, right?
		 */
		source(".exrc", 1);
	}
	init();	/* moved after prev 2 chunks to fix directory option */

	/*
	 * Initial processing.  Handle tag, recover, and file argument
	 * implied next commands.  If going in as 'vi', then don't do
	 * anything, just set initev so we will do it later (from within
	 * visual).
	 */
	if (setexit() == 0) {
		// if (recov)
		// 	globp = "recover";
		// else if (itag)
		if (itag)
			globp = ivis ? "tag" : "tag|p";
		else if (argc)
			globp = "next";
		if (ivis)
			initev = globp;
		else if (globp) {
			inglobal = 1;
			commands(1, 1);
			inglobal = 0;
		}
	}

	/*
	 * Vi command... go into visual.
	 * Strange... everything in vi usually happens
	 * before we ever "start".
	 */
	if (ivis) {
		/*
		 * Don't have to be upward compatible with stupidity
		 * of starting editing at line $.
		 */
		if (dol > zero)
			dot = one;
		globp = "visual";
		if (setexit() == 0)
			commands(1, 1);
	}

	/*
	 * Clear out trash in state accumulated by startup,
	 * and then do the main command loop for a normal edit.
	 * If you quit out of a 'vi' command by doing Q or ^\,
	 * you also fall through to here.
	 */
	seenprompt = 1;
	ungetchar(0);
	globp = 0;
	initev = 0;
	setlastchar('\n');
	setexit();
	commands(0, 0);
	cleanup(1);
	exit(0);
}

/*
 * Initialization, before editing a new file.
 * Main thing here is to get a new buffer (in fileinit),
 * rest is peripheral state resetting.
 */
init()
{
	register int i;

	fileinit();
	dot = zero = truedol = unddol = dol = fendcore;
	one = zero+1;
	undkind = UNDNONE;
	chng = 0;
	edited = 0;
	for (i = 0; i <= 'z'-'a'+1; i++)
		names[i] = 1;
	anymarks = 0;
}

/*
 * Return last component of linux path name p.
 */
char *
tailpath(p)
register char *p;
{
	register char *r;

	for (r=p; *p; p++)
		if (*p == '/')
			r = p+1;
	return(r);
}
