MODULE_big=nanmedian
OBJS=nanmedian.o
DATA_built=nanmedian.sql
DOCS=README.nanmedian

OPT=-O3
OPT_LOW=-O2
#DEBUG=-g3 -ggdb -DQ3C_DEBUG

PG_CPPFLAGS =  $(DEBUG) $(OPT) -D_GNU_SOURCE -D__STDC_FORMAT_MACROS
SHLIB_LINK += $(filter -lm, $(LIBS))
#EXTRA_CLEAN=dump.c prepare prepare.o

ifdef NO_PGXS
subdir = contrib/nanmedian
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
else
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
ifndef PGXS
$(error You should have `pg_config` program in your PATH or compile Q3C with\
'make NO_PGXS=1' \
after putting it in the contrib subdirectory of Postgres sources)
endif
include $(PGXS)
endif
CPPFLAGS = $(CPPFLAGS) -D$(Q3CVERSION) -lm


oldclean: 
	rm -f *~ sql/*~


