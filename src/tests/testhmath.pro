TARGET = testhmath

DEPENDPATH += . \
              .. \
              ../core \
              ../gui \
              ../locale \
              ../math \
              ../resources

INCLUDEPATH += . .. ../math ../core ../gui

HEADERS += ../math/floatcommon.h \
           ../math/floatconst.h \
           ../math/floatconvert.h \
           ../math/floaterf.h \
           ../math/floatexp.h \
           ../math/floatgamma.h \
           ../math/floathmath.h \
           ../math/floatio.h \
           ../math/floatipower.h \
           ../math/floatlog.h \
           ../math/floatlogic.h \
           ../math/floatlong.h \
           ../math/floatnum.h \
           ../math/floatpower.h \
           ../math/floatseries.h \
           ../math/floattrig.h \
           ../math/hmath.h \
           ../math/number.h \
	   ../math/cmath.h \
	   ../math/cnumberparser.h

SOURCES += ../math/floatcommon.c \
           ../math/floatconst.c \
           ../math/floatconvert.c \
           ../math/floaterf.c \
           ../math/floatexp.c \
           ../math/floatgamma.c \
           ../math/floathmath.c \
           ../math/floatio.c \
           ../math/floatipower.c \
           ../math/floatlog.c \
           ../math/floatlogic.c \
           ../math/floatlong.c \
           ../math/floatnum.c \
           ../math/floatpower.c \
           ../math/floatseries.c \
           ../math/floattrig.c \
	   ../math/hmath.cpp \
           ../math/number.c \
	   ../math/cmath.cpp \
	   ../math/cnumberparser.cpp \
	   testhmath.cpp

