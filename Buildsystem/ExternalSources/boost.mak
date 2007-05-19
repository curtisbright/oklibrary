# Matthew Henderson, 19.7.2006 (Paderborn)
# filename : Buildsystem/ExternalSources/boost.mak 

# ##################################
# Directory Structure
# ##################################

# In the following text, ? denotes the GCC version number and % denotes the Boost version number, while & denotes the minor version number.
#./Boost : Contain locally stage built, various versions of Boost C++ Libraries.
#./Boost/boost_%_& : This is the original unarchived source directory for boost version %.
#./Boost/bjam : Contain the bjam tool. This binary is re-generated each time when boost is to be installed.
#./Boost/% : Locally installed boost using system-wide version of GCC.
#./Boost/%+?_Build : This is the staging directory where configuration and temporary files are stored. This Boost version % is build using GCC version ?. 
#./Boost/%+? : The built Boost Libraries using GCC version ?. Libraries in this directory are then copied to the GCC ? lib directory.

boost-base-directory := $(ExternalSources)/Boost
abbr_boost_targets := $(patsubst boost-%, %, $(boost_targets))
# creates e.g. 1_32_0 1_33_1

# These two lines are necessary - but also belong to makefile_gcc.mak
gcc_installation_directory_names := $(patsubst gcc-%, %, $(gcc_targets))
gcc-base-directory := $(ExternalSources)/Gcc

boost_installation_directory_names := $(foreach gccversion, $(gcc_installation_directory_names), $(addsuffix +$(gccversion), $(abbr_boost_targets)))
# creates e.g. 1_32_0+3.4.3 1_32_0+3.4.4 1_33_1+3.4.3 1_33_1+3.4.4
boost_installation_directory_names += $(abbr_boost_targets)
boost_installation_directory_paths := $(addprefix $(boost-base-directory)/,$(boost_installation_directory_names))

boost_build_directory_names := $(addsuffix _Build, $(boost_installation_directory_names))
boost_build_directory_paths := $(addprefix $(boost-base-directory)/,$(boost_build_directory_names))

bjam_directory_path := $(boost-base-directory)/bjam

boost_doc_dir := $(external_sources_doc_base_dir)/Boost

boost-directories := $(boost-base-directory) $(boost_build_directory_paths) $(boost_installation_directory_paths) $(bjam_directory_path) $(boost_doc_dir) $(addprefix $(boost_doc_dir)/, $(abbr_boost_targets))

boost_distribution_directories := $(addprefix $(boost-base-directory)/boost_, $(abbr_boost_targets))

boost_gcc_targets := $(foreach boostversion, $(boost_targets), $(addprefix $(boostversion)+, $(gcc_installation_directory_names)))
all_boost_targets := $(boost_targets) $(boost_gcc_targets)

# ####################################
# Documentation
# ####################################

boost_documentation := boost_1_34_0/boost.png \
                       boost_1_34_0/boost.css \
                       boost_1_34_0/index.htm \
                       boost_1_34_0/more \
                       boost_1_34_0/libs \
                       boost_1_34_0/doc

# This is just a temporary hack - there should already be a central
# organisation for the package names, or ???
# OK: Obviously, the library-version must not be hardcoded, but passed as parameter. ???

boost_package_name := boost_1_34_0.tar.bz2 

boost_doc : | $(boost_doc_dir)
	cd $(boost_doc_dir); $(postcondition) \
	tar -xf $(ExternalSources)/$(boost_package_name) $(boost_documentation)

# OK: This should not be a target on its own, but a command. ???


# ###############################
# General targets
# ###############################

.PHONY : boost boost_all boost_gcc_all $(all_boost_targets)

$(boost_installation_directory_paths) : % : | $(boost-base-directory) %_Build $(bjam_directory_path)

$(boost-directories) : % : 
	mkdir $@

# ###############################
# Making boost with the system gcc
# ###############################

define install-boost
	$(bjam_directory_path)/bjam --toolset=gcc --prefix=$(boost-base-directory)/$(1) --build-dir=$(boost-base-directory)/$(1)_Build install --without-python
endef

# Does the target below really work with several versions of boost ???
# Shouldn't the following be replaced by the simpler use of "configure" ???

$(addprefix $(boost-base-directory)/, $(boost_targets)) : $(boost-base-directory)/boost-% : $(boost-base-directory)/% $(boost_doc_dir)/%
	$(call unarchive,boost_$*,$(boost-base-directory))
	cd $(boost-base-directory)/boost_$*; $(postcondition) \
	cd tools/jam/src; $(postcondition) \
	./build.sh; $(postcondition) \
	cp bin.*/bjam $(bjam_directory_path); $(postcondition) \
	cd $(boost-base-directory)/boost_$*; $(postcondition) \
	$(call install-boost,$*); $(postcondition) \
	cp -r boost $(boost-base-directory)/$*; $(postcondition) \
	cp -r $(boost_documentation) $(boost_doc_dir)/$*; $(postcondition) \
	touch $@

# ###############################
# Making boost with a local gcc
# ###############################

# NEEDS TO BE UPDATED ???

define install-boost_gcc
	$(bjam_directory_path)/bjam "-sTOOLS=gcc" "-sGCC_ROOT_DIRECTORY=$(gcc-base-directory)/$(2)" --prefix=$(boost-base-directory)/$(1)+$(2) --builddir=$(boost-base-directory)/$(1)+$(2)_Build install
#	if [ -d $(gcc-base-directory)/$(2)/boost-$(1) ]; then echo; else mkdir $(gcc-base-directory)/$(2)/boost-$(1); fi;
#	cp $(boost-base-directory)/$(1)+$(2)/lib/* $(gcc-base-directory)/$(2)/boost-$(1)
#	if [ -d $(gcc-base-directory)/$(2)/include/boost-$(1) ]; then echo; else mkdir $(gcc-base-directory)/$(2)/include/boost-$(1); fi;
#	cp -r $(boost-base-directory)/$(1)+$(2)/include/boost-$(1)/boost $(gcc-base-directory)/$(2)/include/boost-$(1)
endef

define boost_gcc_rule
$(boost-base-directory)/boost-$(1)+$(2) : $(boost-base-directory)/$(1)+$(2) | gcc-$(2) 
	$(call unarchive,boost_$(1),$(boost-base-directory))
	cd $(boost-base-directory)/boost_$(1); if [ $$$$? != 0 ]; then exit 1; fi; \
	cd tools/build/jam_src/;  if [ $$$$? != 0 ]; then exit 1; fi; \
	./build.sh;  if [ $$$$? != 0 ]; then exit 1; fi; \
	cp bin.*/bjam $(bjam_directory_path);  if [ $$$$? != 0 ]; then exit 1; fi; \
	cd $(boost-base-directory)/boost_$(1);  if [ $$$$? != 0 ]; then exit 1; fi; \
	$(call install-boost_gcc,$(1),$(2))
	touch $(boost-base-directory)/boost-$(1)+$(2)
endef

$(foreach boostversion, $(abbr_boost_targets), $(foreach gccversion, $(gcc_installation_directory_names), $(eval $(call boost_gcc_rule,$(boostversion),$(gccversion)))))

# ###############################
# The main targets for making boost
# ###############################

boost_gcc_all : $(all_boost_targets)

$(all_boost_targets) : % : $(boost-base-directory)/%

ifeq ($(gcc-version),all)
 boost_all : $(boost_gcc_targets)
 boost : $(addprefix $(boost_recommended)+,$(gcc_installation_directory_names))
else
 ifeq ($(gcc-version),)
  boost_all : $(boost_targets)
  boost : $(boost_recommended)
 else
  boost_all : $(addsuffix $(gcc-version),$(boost_targets))
  boost : $(boost_recommended)+$(gcc-version)
 endif
endif

# ####################################
# Cleaning
# ####################################

cleanboost :
	-rm -rf $(boost_build_directory_paths) $(boost_distribution_directories) $(bjam_directory_path)

cleanallboost : 
	-rm -rf $(boost-base-directory)
