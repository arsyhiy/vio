# NOTE: 
это модофицированая версия vi с коментариями на руском что бы разобрать его на куски - изучить
# NOTE: что бы запустить vi придется её портировать с unix (system V ) на линукс так как я её использую

  This directory and its subdirectories contain the files necessary
for making ex/vi in the BTL UNIX environment: 

	This is a list of all the important directories and what's
	hidden under each.

	misc - contains files used in the "make" of ex/vi
	      { mkstr.c, xstr.c ...}
	
	vax - ex/vi source directory for the vax, 3B, C70, 370, &
	      pdp 11/70 (UNIX type=ov --  with overlays)

	pdp11  - ex/vi source directory for the pdp11/70 (UNIX type=od
	      without overlays)

	local - contains "uparm.h" which must be edited to
		reflect your system setup!
		EXAMPLE:
		--------
		#define libpath(file) "/usr/lib/file"
		#define loclibpath(file) "/usr/lib/file"
		#define binpath(file) "/usr/lbin/file"
		#define usrpath(file) "/usr/file"
		** libpath is where ex/vi expects the strings file
		(ex2.16strings or ex3.9strings...this file is
		made as the ex/vi source is compiled), and
		ex/vi 's recover & preserve commands (ex2.16preserve
		or ex3.9preserve).
		** usrpath is where the preserved files are held after
		a hangup or fail..."preserve" is tacked onto
		the end of the usrpath string .


	Look through the source, make all necessary changes to
	local/uparm.h (& ex source if necessary) and then do
	a "makevi".  makevi is a shell which directs you through
	a "make" of ex/vi.

	PATHs to look at (and possibly change) include: 

	  vax/makefile.u3   ---|
	  pdp11/makefile.u3 ---|  the editor's string file,
	  preserve and recover commands all are installed in
	  ${DESTDIR}${LIBDIR}....make sure this is where you
	  want them to go.  This path must be the same as
	  the path defined in local/uparm.h ...c variable
	  name is usrpath.

	     In the event of a hangup, the editor will place a
	  copy of the temporary file in the place defined as
	  ${DESTDIR}/usr/preserve.
					
