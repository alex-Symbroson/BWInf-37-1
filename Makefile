
SUBDIRS = Junior1 Junior2 Aufgabe1 Aufgabe2 Aufgabe4

ifneq ($(ARGS),)
	OP += ARGS=$(ARGS)
endif

.PHONY: all clean $(SUBDIRS)

all: $(SUBDIRS)

clean:
	for dir in $(SUBDIRS);
		do make -C $$dir clean;
	done

$(SUBDIRS):
	make -C $@ $(OP)
	@echo

help:
	# Variables:
	#     OP:     make operation      OP=[all,clean,run]
	#     ARGS:   program arguments   ARGS="arg1 arg2 ..."
