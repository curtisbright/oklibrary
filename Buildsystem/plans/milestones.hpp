// Oliver Kullmann, 4.3.2006 (Swansea)

/*!
  \file Buildsystem/plans/milestones.hpp
  
  \module_version Buildsystem 0.1.5 (31.3.2007)

  \par Version 0.1.6 : THEME is "New infrastructure (version control, make, boost, gcc; planning for mailing lists)"

  \par
   In Buildsystem/plans/VersionControl.hpp the following topics are handled:
    - Building and using Git
    - New version control system

  \par
   In Buildsystem/plans/ExternalSources.hpp the following topics are handled:
    - Mailman
    - CMake
    - Boost
    - Building Ubcsat
    - bug "Building Boost 1_34_0" : DONE
    - bug "Building-Boost Errors" : DONE
    - bug "Building gcc_doc" : DONE

  \par
   In Buildsystem/plans/MakeSystem.hpp the following topics are handled:
    - CMake

  \par
   In Buildsystem/plans/MailingLists.hpp the following topics are handled:
    - Mailing lists
    - Notification list : DONE
    - Mailing list software : DONE

  \par
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - Special runs : DONE
    - Overhaul of the general targets : DONE
    - Design : DONE (moved to Buildsystem/plans/MakeSystem.hpp)

  \par
   In Buildsystem/plans/generic.hpp the following topics are handled:
    - .source_libraries : DONE
    - Directory structure : DONE
    - Compilation : DONE
    - Customisation : DONE


  \par Version 0.1.7 : THEME is "External libraries"

  \par
   In Buildsystem/plans/MailingLists.hpp the following topics are handled:
    - User mailing list

  \par
   In Buildsystem/Generic/documentation_building/plans/documentation_index.mak.hpp
   the following topics are handled
    - bug "Naming"
    - Boost-documentation
    - GCC-documentation
    - CMake documentation

  \par
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - system_definitions.mak
    - external_sources_versions.mak
    - Make-variables for external libraries

  \par
   In Buildsystem/plans/ExternalSources.hpp the following topics are handled:
    - Documenting ExternalSources.mak
    - Building documentation



  \par Version 0.1.8 : THEME is "A specified and testable build system" and "Build system consolidation"

  \par
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - Renaming module Buildsystem

  \par
   In Buildsystem/plans/generic.hpp the following topics are handled:
    - Mixing C and C++
    - Incorrect build of new tests I
    - Incorrect build of new tests II
    - Specification of buildsystem II
    - Setting the paths to GCC and Boost link libraries
    - User control of system versions
    - General_options
    - Dependency files
    - Verbosity
    - Test system
    - OKsystem/Buildsystem structure

  \par
   In Buildsystem/plans/ExternalSources.hpp the following topics are handled:
    - Building gcc_doc 
    - Corrections mhash
    - Building Boost Errors

  \par
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - system_definitions.mak
    - Testing the build system
    - Licence
    - Role of srcdir
    - System documentation
    - Documentation (Examples and Concepts)
    - Force make
    - Modes of Usage
    - makefile_recursive
    - Compilation information
    - Source code directory structure

  \par
   In Buildsystem/Generic/documentation_building/plans/documentation_building.hpp
   the following topics are handled:
    - Doxygen general

  \par
   In Buildsystem/Generic/documentation_building/plans/documentation_index.mak.hpp
   the following topics are handled
    - Page organisation
    - Using css
    - Meta tags
    - Improved logo

  \par Version 0.1.9 : THEME is "Build system extensions"
   In Buildsystem/plans/generic.hpp the following topics are handled:
    - Targets
    - Error messages of gcc should be processed
    - %Tools for testing
    - Log directory
    - Cleaning
    - Linking and options
    - Test cleaning

  \par
   In Buildsystem/plans/ExternalSources.hpp the following topics are handled:
    - Other sources
    - %Tools
    - PostgreSQL
    - General
    - Gcc
    - Boost

  \par
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - License
    - Notification list

  \par Version 0.1.10 :
   In Buildsystem/plans/README.hpp the following topics are handled:
    - Supported versions of gcc
    - Update

  \par Version 0.1.11 :
   In Buildsystem/plans/makefile.hpp the following topics are handled:
    - Directories with makefile-links

  \par Version 0.2 : THEME is "Basic functionality"
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - Full test
    - Nightly build
    
  \par Version 0.3 : THEME is "Complexity measurements"
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - Complexity system
    - Measurements

  \par Version 0.4 : THEME is "Packages and Distribution"
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - Package construction script
    - Competition extraction tools
    - Distributing the library
  
  \par Version 0.5 : THEME is "Rounding up"
   In Buildsystem/plans/Buildsystem.hpp the following topics are handled:
    - New targets
    - Higher-order build tools
  
  \par Version 1.0 :
   


  -------------------------------------------------------------------------------------------------------------------------------------

  \par Version history

   - 0.1   : 4.3.2006; initial version number (the build system is under work for around 2 years).
   - 0.1.1 : 3.11.2006; some smaller corrections and modifications regarding naming issues.
   - 0.1.2 : 9.12.2006; systematic naming, placement and decomposition of makefiles; documentation of generic.mak; documentation-building improved.
   - 0.1.3 : 14.1.2007; further improvements on naming and placement of makefiles, improved building of external libraries, improved html-documentation.
   - 0.1.4 : 8.2.2007; Steps towards a specified and testable buildsystem. Further documentation of generic.mak; corrections to makefile plans files suffixes; corrections to system-wide variable definitions.
   - 0.1.5 : 31.3.2007; further steps towards a specified and testable buildsystem; started documenting generic.mak in the doxygen-system. Further improvements in the general structure of the documentation-pages.

*/

