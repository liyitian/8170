#----------------------------------------------
#    Example Makefile for compiling a program that uses
#    the Matrix and Vector classes
#----------------------------------------------
#
#  Programmer: Donald House
#  Date: March 8, 1999
#
#  Copyright (C) - Donald H. House. 2005
#

CC  = g++
C	= cpp
H	= h
CFLAGS 	= -g -Wno-deprecated-declarations

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = $(CFLAGS) -framework Foundation -framework GLUT -framework OpenGL -lm 
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -lglut -lGL -lm -L/usr/local/lib -lGLU
  endif
endif

HFILES 	= Matrix.${H} Vector.${H} Utility.${H} Camera.${H} Gauss.${H} StateVector.${H} Quaternion.${H}
OFILES 	= Matrix.o Vector.o Utility.o Camera.o Gauss.o StateVector.o Quaternion.o
PROJECT = Project5

${PROJECT}:	${PROJECT}.o $(OFILES)
	${CC} $(CFLAGS) -o ${PROJECT} ${PROJECT}.o $(OFILES) $(LDFLAGS)

${PROJECT}.o: ${PROJECT}.${C} $(HFILES)
	${CC} $(CFLAGS) -c ${PROJECT}.${C}

Camera.o: Camera.${C} Camera.${H} Matrix.${H} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Camera.${C}

StateVector.o: StateVector.${C} StateVector.${H} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c StateVector.${C}

Matrix.o: Matrix.${C} Matrix.${H} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Matrix.${C}

Vector.o: Vector.${C} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Vector.${C}

Utility.o: Utility.${C} Utility.${H}
	${CC} $(CFLAGS) -c Utility.${C}

Gauss.o: Gauss.${C} Gauss.${H}
	${CC} $(CFLAGS) -c Gauss.${C} $(LDFLAGS)
	
Quaternion.o: Quaternion.${C} Quaternion.${H} Matrix.${H} Vector.${H} Utility.${H}
	${CC} $(CFLAGS) -c Quaternion.${C}


debug:
	make 'DFLAGS = /usr/lib/debug/malloc.o'

clean:
	rm *.o ${PROJECT} #*~
