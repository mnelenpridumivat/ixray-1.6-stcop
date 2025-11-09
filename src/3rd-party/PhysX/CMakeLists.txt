project(xrPhysX)

set(PHYSX_INCLUDE_LIBRARIES "${IXRAY_SDK_INC}/PhysX")

file(GLOB_RECURSE PHYSX_SOURCE_ALL_FILES
	"*.cpp"
    "*.h"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_CHARACTERKINEMATIC_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/characterkinematic/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_COLLISION_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/collision/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_COMMON_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/common/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_COMMON_WINDOWS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/common/windows*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_COOKING_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/cooking/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_CUDAMANAGER_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/cudamanager/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_EXTENSIONS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/extensions/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_FILEBUF_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/filebuf/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_FOUNDATION_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/foundation/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_FOUNDATION_UNIX_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/foundation/unix/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_FOUNDATION_WINDOWS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/foundation/windows/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_GEOMETRY_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/geometry/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_GEOMUTILS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/geomutils/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_GPU_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/gpu/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_OMNIPVD_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/omnipvd/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_PVD_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/pvd/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_SOLVER_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/solver/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_TASK_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/task/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_BRAKING_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/braking/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_COMMANDS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/commands/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_DRIVETRAIN_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/drivetrain/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXACTOR_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/physxActor/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXCONSTRAINTS_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/physxConstraints/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXROADGEOMETRY_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/physxRoadGeometry/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_PVD_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/pvd/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_RIGIDBODY_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/rigidBody/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_ROADGEOMETRY_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/roadGeometry/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_STEERING_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/steering/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_SUSPENSION_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/suspension/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_TIRE_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/tire/*.*"
)

file(GLOB_RECURSE PHYSX_SOURCE_INCLUDE_VEHICLE2_WHEEL_FILES
    "${PHYSX_INCLUDE_LIBRARIES}/vehicle2/wheel/*.*"
)

set(PHYSX_SRC_FILES "")
list(APPEND PHYSX_SRC_FILES	"${PHYSX_SOURCE_ALL_FILES}")
list(APPEND PHYSX_SRC_FILES	"${PHYSX_SOURCE_INCLUDE_FILES}")

add_library(xrPhysX SHARED ${PHYSX_SRC_FILES})

source_group("" FILES ${PHYSX_SOURCE_ALL_FILES})

#function(group_include_files)
#    foreach(file ${PHYSX_SOURCE_INCLUDE_ALL_FILES})
#        file(RELATIVE_PATH relative_path "${PHYSX_INCLUDE_LIBRARIES}" "${file}")
#        
#        get_filename_component(dir_path "${relative_path}" DIRECTORY)
#        
#        if(NOT dir_path)
#            string(REPLACE "/" "\\" group_name "include\\${dir_path}")
#        else()
#            set(group_name "include")
#        endif()
#        
#        source_group("${group_name}" FILES "${file}")
#    endforeach()
#endfunction()

function(add_include_files_with_groups)
    # Получаем список всех поддиректорий
    file(GLOB_RECURSE ALL_INCLUDE_FILES 
        "${PHYSX_INCLUDE_LIBRARIES}/*.h"
        "${PHYSX_INCLUDE_LIBRARIES}/*.hpp" 
        "${PHYSX_INCLUDE_LIBRARIES}/*.inl"
        "${PHYSX_INCLUDE_LIBRARIES}/*.c"
        "${PHYSX_INCLUDE_LIBRARIES}/*.cpp"
    )
    
    foreach(include_file ${ALL_INCLUDE_FILES})
        # Получаем относительный путь
        file(RELATIVE_PATH rel_path "${PHYSX_INCLUDE_LIBRARIES}" "${include_file}")
        
        if(rel_path)
            # Добавляем файл к цели
            target_sources(xrPhysX PRIVATE "${include_file}")
            
            # Создаем группу
            get_filename_component(dir_part "${rel_path}" DIRECTORY)
            if(dir_part)
                string(REPLACE "/" "\\" group_name "include\\${dir_part}")
                source_group(TREE "${group_name}" PREFIX "${PHYSX_INCLUDE_LIBRARIES}" FILES "${include_file}")
            else()
                source_group(TREE "include" PREFIX "${PHYSX_INCLUDE_LIBRARIES}" FILES "${include_file}")
            endif()
        endif()
    endforeach()
endfunction()

source_group("include" FILES ${PHYSX_SOURCE_INCLUDE_FILES})

add_include_files_with_groups()

#source_group("include\\characterkinematic" FILES ${PHYSX_SOURCE_INCLUDE_CHARACTERKINEMATIC_FILES})
#source_group("include\\collision" FILES ${PHYSX_SOURCE_INCLUDE_COLLISION_FILES})
#source_group("include\\common" FILES ${PHYSX_SOURCE_INCLUDE_COMMON_FILES})
#source_group("include\\common\\windows" FILES ${PHYSX_SOURCE_INCLUDE_COMMON_WINDOWS_FILES})
#source_group("include\\cooking" FILES ${PHYSX_SOURCE_INCLUDE_COOKING_FILES})
#source_group("include\\cudamanager" FILES ${PHYSX_SOURCE_INCLUDE_CUDAMANAGER_FILES})
#source_group("include\\extensions" FILES ${PHYSX_SOURCE_INCLUDE_EXTENSIONS_FILES})
#source_group("include\\filebuf" FILES ${PHYSX_SOURCE_INCLUDE_FILEBUF_FILES})
#source_group("include\\foundation" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_FILES})
#source_group("include\\foundation\\unix" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_UNIX_FILES})
#source_group("include\\foundation\\windows" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_WINDOWS_FILES})
#source_group("include\\geometry" FILES ${PHYSX_SOURCE_INCLUDE_GEOMETRY_FILES})
#source_group("include\\geomutils" FILES ${PHYSX_SOURCE_INCLUDE_GEOMUTILS_FILES})
#source_group("include\\gpu" FILES ${PHYSX_SOURCE_INCLUDE_GPU_FILES})
#source_group("include\\omnipvd" FILES ${PHYSX_SOURCE_INCLUDE_OMNIPVD_FILES})
#source_group("include\\pvd" FILES ${PHYSX_SOURCE_INCLUDE_PVD_FILES})
#source_group("include\\solver" FILES ${PHYSX_SOURCE_INCLUDE_SOLVER_FILES})
#source_group("include\\task" FILES ${PHYSX_SOURCE_INCLUDE_TASK_FILES})
#source_group("include\\vehicle2" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_FILES})
#source_group("include\\vehicle2\\braking" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_BRAKING_FILES})
#source_group("include\\vehicle2\\commands" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_COMMANDS_FILES})
#source_group("include\\vehicle2\\drivetrain" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_DRIVETRAIN_FILES})
#source_group("include\\vehicle2\\physxActor" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXACTOR_FILES})
#source_group("include\\vehicle2\\physxConstraints" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXCONSTRAINTS_FILES})
#source_group("include\\vehicle2\\physxRoadGeometry" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXROADGEOMETRY_FILES})
#source_group("include\\vehicle2\\pvd" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PVD_FILES})
#source_group("include\\vehicle2\\rigidBody" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_RIGIDBODY_FILES})
#source_group("include\\vehicle2\\roadGeometry" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_ROADGEOMETRY_FILES})
#source_group("include\\vehicle2\\steering" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_STEERING_FILES})
#source_group("include\\vehicle2\\suspension" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_SUSPENSION_FILES})
#source_group("include\\vehicle2\\tire" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_TIRE_FILES})
#source_group("include\\vehicle2\\wheel" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_WHEEL_FILES})


#source_group("include\\characterkinematic" FILES ${PHYSX_SOURCE_INCLUDE_CHARACTERKINEMATIC_FILES})
#source_group("include\\collision" FILES ${PHYSX_SOURCE_INCLUDE_COLLISION_FILES})
#source_group("include\\common" FILES ${PHYSX_SOURCE_INCLUDE_COMMON_FILES})
#source_group("include\\common\\windows" FILES ${PHYSX_SOURCE_INCLUDE_COMMON_WINDOWS_FILES})
#source_group("include\\cooking" FILES ${PHYSX_SOURCE_INCLUDE_COOKING_FILES})
#source_group("include\\cudamanager" FILES ${PHYSX_SOURCE_INCLUDE_CUDAMANAGER_FILES})
#source_group("include\\extensions" FILES ${PHYSX_SOURCE_INCLUDE_EXTENSIONS_FILES})
#source_group("include\\filebuf" FILES ${PHYSX_SOURCE_INCLUDE_FILEBUF_FILES})
#source_group("include\\foundation" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_FILES})
#source_group("include\\foundation\\unix" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_UNIX_FILES})
#source_group("include\\foundation\\windows" FILES ${PHYSX_SOURCE_INCLUDE_FOUNDATION_WINDOWS_FILES})
#source_group("include\\geometry" FILES ${PHYSX_SOURCE_INCLUDE_GEOMETRY_FILES})
#source_group("include\\geomutils" FILES ${PHYSX_SOURCE_INCLUDE_GEOMUTILS_FILES})
#source_group("include\\gpu" FILES ${PHYSX_SOURCE_INCLUDE_GPU_FILES})
#source_group("include\\omnipvd" FILES ${PHYSX_SOURCE_INCLUDE_OMNIPVD_FILES})
#source_group("include\\pvd" FILES ${PHYSX_SOURCE_INCLUDE_PVD_FILES})
#source_group("include\\solver" FILES ${PHYSX_SOURCE_INCLUDE_SOLVER_FILES})
#source_group("include\\task" FILES ${PHYSX_SOURCE_INCLUDE_TASK_FILES})
#source_group("include\\vehicle2" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_FILES})
#source_group("include\\vehicle2\\braking" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_BRAKING_FILES})
#source_group("include\\vehicle2\\commands" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_COMMANDS_FILES})
#source_group("include\\vehicle2\\drivetrain" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_DRIVETRAIN_FILES})
#source_group("include\\vehicle2\\physxActor" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXACTOR_FILES})
#source_group("include\\vehicle2\\physxConstraints" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXCONSTRAINTS_FILES})
#source_group("include\\vehicle2\\physxRoadGeometry" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PHYSXROADGEOMETRY_FILES})
#source_group("include\\vehicle2\\pvd" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_PVD_FILES})
#source_group("include\\vehicle2\\rigidBody" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_RIGIDBODY_FILES})
#source_group("include\\vehicle2\\roadGeometry" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_ROADGEOMETRY_FILES})
#source_group("include\\vehicle2\\steering" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_STEERING_FILES})
#source_group("include\\vehicle2\\suspension" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_SUSPENSION_FILES})
#source_group("include\\vehicle2\\tire" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_TIRE_FILES})
#source_group("include\\vehicle2\\wheel" FILES ${PHYSX_SOURCE_INCLUDE_VEHICLE2_WHEEL_FILES})

target_include_directories(xrPhysX PUBLIC "${PHYSX_INCLUDE_LIBRARIES}")

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
