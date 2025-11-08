project(xrPhysX)


file(GLOB_RECURSE PHYSX_SOURCE_ALL_FILES
	"*.cpp"
    "*.h"
)

set(PHYSX_SRC_FILES "")
list(APPEND PHYSX_SRC_FILES
	${PHYSX_SOURCE_ALL_FILES}
)

add_library(xrPhysX SHARED ${PHYSX_SRC_FILES})

target_include_directories(xrPhysX PUBLIC "${IXRAY_SDK_INC}/include/PhysX")

set(PHYSX_LINK_LIBRARIES "${IXRAY_SDK_LIB}/$<CONFIG>")
set(PHYSX_BINARIES "${IXRAY_SDK_BIN}/$<CONFIG>")

add_custom_command(TARGET xrPhysX POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHYSX_BINARIES}/freeglut.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHYSX_BINARIES}/PhysX_64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHYSX_BINARIES}/PhysXCommon_64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHYSX_BINARIES}/PhysXCooking_64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PHYSX_BINARIES}/PhysXFoundation_64.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/
)
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/LowLevel_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/LowLevelAABB_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/LowLevelDynamics_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysX_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXCharacterKinematic_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXCommon_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXCooking_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXExtensions_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXFoundation_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXPvdSDK_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXTask_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/PhysXVehicle2_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/SceneQuery_static_64.lib")
target_link_libraries(xrPhysX PUBLIC "${PHYSX_LINK_LIBRARIES}/SimulationController_static_64.lib")
