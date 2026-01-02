add_library( cheat STATIC
  "pch.hxx"
)

# packages
find_package( minhook CONFIG REQUIRED )
find_package( imgui CONFIG REQUIRED )

target_link_libraries( cheat PUBLIC
  minhook::minhook
  imgui::imgui
)

# precompiled header
target_precompile_headers( cheat PUBLIC pch.hxx )
