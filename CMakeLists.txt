
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(pfRICH)

set(CMAKE_BUILD_TYPE Debug)

#----------------------------------------------------------------------------

find_package(ROOT 6.0.00 REQUIRED COMPONENTS RIO Hist Tree)
include(${ROOT_USE_FILE})

#----------------------------------------------------------------------------

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------

include(CheckCXXCompilerFlag)

SET(CMAKE_CXX_FLAGS "-Wall -std=c++0x -Wno-misleading-indentation")

check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
  # Have -std=c++17, use it;	
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
  check_cxx_compiler_flag(-std=c++11 HAVE_FLAG_STD_CXX11)
  if(HAVE_FLAG_STD_CXX11)
    # Have -std=c++11, use it;	
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
endif()

#----------------------------------------------------------------------------

include_directories( 
  ${Geant4_INCLUDE_DIR} 
  ${ROOT_INCLUDE_DIRS}	

  ${IRT}/include/IRT 
  ${BMF}/include 
  ${HEPMC3}/include 
  ${XERCES}/include 

  ${PROJECT_SOURCE_DIR}/include 
  ${PROJECT_SOURCE_DIR}/g4irt/include 
)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h )

SET(G4IRT_LIB g4irt)
SET(PFRICH_EXE pfrich)
#set(XML_EXE xmlreader)

SET( G4IRT_SRC
   ${PROJECT_SOURCE_DIR}/g4irt/source/G4Object.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/G4RadiatorMaterial.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovMirror.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovSteppingAction.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovStackingAction.cc
)

SET( sources
   ${PROJECT_SOURCE_DIR}/source/TofPetMultiColumn.cc
   ${PROJECT_SOURCE_DIR}/source/MagneticField.cc
   ${PROJECT_SOURCE_DIR}/source/Materials.cc
   ${PROJECT_SOURCE_DIR}/source/PrimaryGeneratorAction.cc
   ${PROJECT_SOURCE_DIR}/source/RunAction.cc

   ${PROJECT_SOURCE_DIR}/source/Vessel.cc
   ${PROJECT_SOURCE_DIR}/source/Aerogel.cc
   ${PROJECT_SOURCE_DIR}/source/Acrylic.cc
   ${PROJECT_SOURCE_DIR}/source/Mirrors.cc
   ${PROJECT_SOURCE_DIR}/source/PhotonDetectors.cc
   ${PROJECT_SOURCE_DIR}/source/DetectorConstruction.cc
)

SET( XML_SRC
   ${PROJECT_SOURCE_DIR}/source/XmlReader.cc
)

ADD_LIBRARY( ${G4IRT_LIB} SHARED ${G4IRT_SRC} )
TARGET_LINK_LIBRARIES(${G4IRT_LIB} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} -L${IRT}/lib IRT)

#----------------------------------------------------------------------------

add_executable(${PFRICH_EXE} pfRICH.cc ${sources} ${headers})
target_link_libraries(${PFRICH_EXE} ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${G4IRT_LIB}) 
if(DEFINED BMF)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBMF")
  target_link_libraries(${PFRICH_EXE} -L${BMF}/lib bmf)
endif()
if(DEFINED HEPMC3)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHEPMC3")
  target_link_libraries(${PFRICH_EXE} -L${HEPMC3}/lib -L${HEPMC3}/lib64 HepMC3)
endif()
if(DEFINED XERCES)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXERCES")
  target_link_libraries(${PFRICH_EXE} -L${XERCES}/lib -L${XERCES}/lib64 xerces-c)
endif()

#----------------------------------------------------------------------------

#add_executable( xmlreader ${XML_SRC})
#target_link_libraries( xmlreader xerces-c)

#----------------------------------------------------------------------------

install(TARGETS ${PFRICH_EXE}   DESTINATION bin)
install(TARGETS ${G4IRT_LIB}    DESTINATION lib)

