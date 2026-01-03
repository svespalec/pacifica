add_library( cheat SHARED
  "pch.hxx"
  "pch.cxx"
  "dllmain.cxx"
  "hook/hook.cxx"
  "utils/utils.cxx"
  "features/aim.cxx"
  "menu/menu.cxx"
  "render/render.cxx"
)

# include cheat dir so subdirs can find pch.hxx
target_include_directories( cheat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )

# packages
find_package( imgui CONFIG REQUIRED )
find_path( DETOURS_INCLUDE_DIR detours/detours.h )
find_library( DETOURS_LIBRARY detours )

target_include_directories( cheat PRIVATE ${DETOURS_INCLUDE_DIR} )
target_link_libraries( cheat PUBLIC
  imgui::imgui
  ${DETOURS_LIBRARY}
)

# precompiled header
target_precompile_headers( cheat PUBLIC pch.hxx )
