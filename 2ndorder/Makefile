CC      = g++
C	= cpp
H	= h

CFLAGS      = -g

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -lglut -lGL -lGLU -lm
  endif
endif

INCFILES   = Strut.${H} Meter.${H} Stripchart.${H} Graphics.${H}
OBJFILES   = Strut.o Meter.o Stripchart.o Graphics.o
PROJECT     = 2ndorder

${PROJECT}: ${PROJECT}.o ${OBJFILES}
	${CC} ${CFLAGS} -o ${PROJECT} ${PROJECT}.o ${OBJFILES} ${LDFLAGS}

${PROJECT}.o:   ${PROJECT}.${C} ${INCFILES}
	${CC} ${CFLAGS} -c ${PROJECT}.${C}

Strut.o:    Strut.${C} Strut.${H}
	${CC} ${CFLAGS} -c Strut.${C}

Meter.o:    Meter.${C} Meter.${H}
	${CC} ${CFLAGS} -c Meter.${C}

Stripchart.o:   Stripchart.${C} Stripchart.${H}
	${CC} ${CFLAGS} -c Stripchart.${C}

Graphics.o:    Graphics.${C} Graphics.${H}
	${CC} ${CFLAGS} -c Graphics.${C}

debug:
	make 'DFLAGS = /usr/lib/debug/malloc.o'

clean:
	rm -f core.* *.o *~ ${PROJECT}


