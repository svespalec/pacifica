BOOL APIENTRY DllMain( HMODULE module, DWORD reason, LPVOID ) {
  if ( reason == DLL_PROCESS_ATTACH ) {
    g_module = module;
  }

  return TRUE;
}


