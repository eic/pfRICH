
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(pfRICH)

set(CMAKE_BUILD_TYPE Debug)

cmake_policy(SET CMP0079 NEW) # allow `target_link_libraries` from any dir

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

  ${PROJECT_SOURCE_DIR}/share/include 
  ${PROJECT_SOURCE_DIR}/epic/include 
  ${PROJECT_SOURCE_DIR}/ftbf/include 
  ${PROJECT_SOURCE_DIR}/tstand/include 
)

file(GLOB SHARE_HEADERS ${PROJECT_SOURCE_DIR}/share/include/*.h)
file(GLOB EPIC_HEADERS  ${PROJECT_SOURCE_DIR}/epic/include/*.h)
file(GLOB FTBF_HEADERS  ${PROJECT_SOURCE_DIR}/ftbf/include/*.h)

SET(G4IRT_LIB g4irt)
SET(SHARE_LIB pfrich-share)
SET(EPIC_EXE pfrich-epic)
SET(FTBF_EXE pfrich-ftbf)
SET(TSTAND_EXE pfrich-tstand)

SET( G4IRT_SOURCES
   ${PROJECT_SOURCE_DIR}/g4irt/source/G4Object.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/G4RadiatorMaterial.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovMirror.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovSteppingAction.cc
   ${PROJECT_SOURCE_DIR}/g4irt/source/CherenkovStackingAction.cc
)

SET( SHARE_SOURCES
   ${PROJECT_SOURCE_DIR}/pfRICH.cc

   ${PROJECT_SOURCE_DIR}/share/source/TofPetMultiColumn.cc

   ${PROJECT_SOURCE_DIR}/share/source/Materials.cc
   ${PROJECT_SOURCE_DIR}/share/source/RunAction.cc
   ${PROJECT_SOURCE_DIR}/share/source/AsphericLens.cc

   ${PROJECT_SOURCE_DIR}/share/source/HRPPD.cc
   ${PROJECT_SOURCE_DIR}/share/source/DetectorConstruction.cc
)

SET( EPIC_SOURCES
   ${PROJECT_SOURCE_DIR}/epic/source/Vessel.cc
   ${PROJECT_SOURCE_DIR}/epic/source/Aerogel.cc
   ${PROJECT_SOURCE_DIR}/epic/source/Acrylic.cc
   ${PROJECT_SOURCE_DIR}/epic/source/Mirrors.cc
   ${PROJECT_SOURCE_DIR}/epic/source/MagneticField.cc

   ${PROJECT_SOURCE_DIR}/epic/source/EpicSetup.cc
   ${PROJECT_SOURCE_DIR}/epic/source/EpicDetectorConstruction.cc
   ${PROJECT_SOURCE_DIR}/epic/source/EpicPrimaryGeneratorAction.cc
)

SET( FTBF_SOURCES
   #${PROJECT_SOURCE_DIR}/share/source/AsphericLens.cc

   ${PROJECT_SOURCE_DIR}/ftbf/source/FTBFSetup.cc
   ${PROJECT_SOURCE_DIR}/ftbf/source/FTBFDetectorConstruction.cc
   ${PROJECT_SOURCE_DIR}/ftbf/source/FTBFPrimaryGeneratorAction.cc
)

SET( TSTAND_SOURCES
   #${PROJECT_SOURCE_DIR}/share/source/AsphericLens.cc
   # FIXME: unify, please;
   #${PROJECT_SOURCE_DIR}/tstand/source/TestStandAsphericLens.cc

   ${PROJECT_SOURCE_DIR}/tstand/source/TestStandSetup.cc
   #${PROJECT_SOURCE_DIR}/tstand/source/TestStandPhotonDetectors.cc
   ${PROJECT_SOURCE_DIR}/tstand/source/TestStandDetectorConstruction.cc
   ${PROJECT_SOURCE_DIR}/tstand/source/TestStandPrimaryGeneratorAction.cc
)

ADD_LIBRARY( ${G4IRT_LIB} SHARED ${G4IRT_SOURCES} )
TARGET_LINK_LIBRARIES(${G4IRT_LIB} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} -L${IRT}/lib IRT)

ADD_LIBRARY( ${SHARE_LIB} SHARED ${SHARE_SOURCES} )
TARGET_LINK_LIBRARIES(${SHARE_LIB} ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------

add_executable(${EPIC_EXE} ${EPIC_SOURCES} ${SHARE_HEADERS} ${EPIC_HEADERS})
target_link_libraries(${EPIC_EXE} ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${G4IRT_LIB} ${SHARE_LIB}) 
add_executable(${FTBF_EXE} ${FTBF_SOURCES} ${SHARE_HEADERS} ${FTBF_HEADERS})
target_link_libraries(${FTBF_EXE} ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${G4IRT_LIB} ${SHARE_LIB}) 
add_executable(${TSTAND_EXE} ${TSTAND_SOURCES} ${SHARE_HEADERS} ${TSTAND_HEADERS})
target_link_libraries(${TSTAND_EXE} ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} ${G4IRT_LIB} ${SHARE_LIB}) 

if(DEFINED BMF)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBMF")
  target_link_libraries(${EPIC_EXE} -L${BMF}/lib bmf)
endif()
if(DEFINED HEPMC3)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHEPMC3")
  target_link_libraries(${EPIC_EXE} -L${HEPMC3}/lib -L${HEPMC3}/lib64 HepMC3)
endif()
if(DEFINED XERCES)
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DXERCES")
  target_link_libraries(${EPIC_EXE} -L${XERCES}/lib -L${XERCES}/lib64 xerces-c)
  target_link_libraries(${FTBF_EXE} -L${XERCES}/lib -L${XERCES}/lib64 xerces-c)
  # FIXME: this stuff is only needed for aerogel;
  target_link_libraries(${TSTAND_EXE} -L${XERCES}/lib -L${XERCES}/lib64 xerces-c)
endif()

#----------------------------------------------------------------------------

install(TARGETS ${EPIC_EXE}   DESTINATION bin)
install(TARGETS ${FTBF_EXE}   DESTINATION bin)
install(TARGETS ${TSTAND_EXE} DESTINATION bin)
install(TARGETS ${G4IRT_LIB}  DESTINATION lib)
install(TARGETS ${SHARE_LIB}  DESTINATION lib)

