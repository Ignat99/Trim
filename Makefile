# $Id: Makefile,v 3.1 1998/04/21 23:58:11 brianp Exp $

# Mesa 3-D graphics library
# Version:  3.0
# Copyright (C) 1995-1998  Brian Paul
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


# Makefile for programs from the OpenGL Programming Guide


# $Log: Makefile,v $
# Revision 3.1  1998/04/21 23:58:11  brianp
# added LIBDIR variable
#
# Revision 3.0  1998/02/14 18:27:58  brianp
# initial rev
#


##### MACROS #####

INCDIR = /usr/local/include
LIBDIR = /usr/local/lib

GL_LIBS = -L$(LIBDIR)  -lm  -lglut -lGL -lGLU -lXt -lXaw  $(XLIBS) -lXaw

LIB_DEP = $(LIBDIR)/$(GL_LIB) $(LIBDIR)/$(GLU_LIB) $(LIBDIR)/$(GLUT_LIB)

#PROGS = trim01
PROGS = trim_lamanga

##### RULES #####

.SUFFIXES:
.SUFFIXES: .cc

.cc: $(LIB_DEP)
	g++ -I$(INCDIR) $(CFLAGS) $< $(GL_LIBS) -o $@



##### TARGETS ######

default:
	@echo "Specify a target configuration"

clean:
	-rm *.o *~

realclean:
	-rm $(PROGS)
	-rm *.o *~

targets: $(PROGS)

# execute all programs
exec: $(PROGS)
	@for prog in $(PROGS) ;			\
	do					\
		echo -n "Running $$prog ..." ;	\
		$$prog ;			\
		echo ;				\
	done


# include /usr/lib/Mesa/Make-config
