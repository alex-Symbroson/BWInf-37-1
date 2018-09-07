
SUBDIRS = Junior1 Aufgabe1 Aufgabe2

ifneq ($(ARGS),)
	OP += ARGS=$(ARGS)
endif

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

clean:
	for dir in $(SUBDIRS); do make -C $$dir clean; done

$(SUBDIRS):
	make -C $@ $(OP)

help:
	# Variables:
	#     OP:     make operation      OP=[all,clean,run]
	#     ARGS:   program arguments   ARGS="arg1 arg2 ..."
