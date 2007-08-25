# Oliver Kullmann, 12.7.2007 (Swansea)

# Special makefile for setting up the basic system; does not depend on
# other makefiles

ifndef OKplatform
  ifdef OKPLATFORM
    OKplatform := $(OKPLATFORM)
  else
    $(error Either OKplatform (a make-variable) or OKPLATFORM (an environment-variable) must be defined when calling this makefile (as the full directory path containing the OKplatform)!)
  endif
endif
ifndef OKanchor
  OKanchor = $(OKplatform)/.oklib
endif
ifndef OKconfiguration
  OKconfiguration = $(OKanchor)/Configuration
endif
ifndef OKoverrideconfig
  OKoverrideconfig = $(OKanchor)/override.mak
endif
ifndef oklib_call
  oklib_call = ~/bin/oklib
endif
ifndef OKlog
  OKlog = $(OKanchor)/log
endif
ifndef oklib_call
  oklib_call = ~/bin/oklib
endif


.PHONY : all oklibrary_initialisation

all : oklibrary_initialisation

oklibrary_initialisation : $(OKanchor) $(OKconfiguration) $(OKoverrideconfig) $(oklib_call) $(OKlog)

$(OKanchor) :
	mkdir $(OKanchor)

$(OKconfiguration) :
	cd $(OKanchor) && ln -s ../OKsystem/Transitional/Buildsystem/Configuration

$(OKoverrideconfig) :
	touch $(OKoverrideconfig)

$(OKlog) :
	cd $(OKanchor) && ln -s ../system_directories/log

$(oklib_call) :
	cd ~/bin; ln -s $(OKconfiguration)/../MasterScript/oklib

