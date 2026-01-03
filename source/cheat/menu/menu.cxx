#include "pch.hxx"
#include "menu.hxx"

#include <hook/hook.hxx>
#include <features/aim.hxx>

#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace features::menu {
  using present_fn = HRESULT( __stdcall* )( IDXGISwapChain*, UINT, UINT );
  using wndproc_fn = LRESULT( __stdcall* )( HWND, UINT, WPARAM, LPARAM );

  static present_fn original_present = nullptr;
  static wndproc_fn original_wndproc = nullptr;
  static ID3D11Device* device = nullptr;
  static ID3D11DeviceContext* context = nullptr;
  static IDXGISwapChain* swapchain = nullptr;
  static HWND hwnd = nullptr;

  static bool initialized = false;

  static LRESULT __stdcall hooked_wndproc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    // toggle menu with HOME
    if ( msg == WM_KEYDOWN && wParam == VK_HOME )
      show_menu = !show_menu;

    // only let imgui process input when menu is shown
    if ( show_menu ) {
      if ( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
        return true;
    }

    return CallWindowProcA( reinterpret_cast< WNDPROC >( original_wndproc ), hWnd, msg, wParam, lParam );
  }

  static HRESULT __stdcall hooked_present( IDXGISwapChain* sc, UINT sync_interval, UINT flags ) {
    swapchain = sc;

    if ( !initialized ) {
      if ( SUCCEEDED( swapchain->GetDevice( __uuidof( ID3D11Device ), reinterpret_cast< void** >( &device ) ) ) ) {
        device->GetImmediateContext( &context );

        // get hwnd from swapchain
        DXGI_SWAP_CHAIN_DESC desc;
        swapchain->GetDesc( &desc );

        hwnd = desc.OutputWindow;

        // hook wndproc for input
        original_wndproc = reinterpret_cast< wndproc_fn >( SetWindowLongPtrA( hwnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( hooked_wndproc ) ) );

        // initialize imgui
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplWin32_Init( hwnd );
        ImGui_ImplDX11_Init( device, context );

        initialized = true;

        LOG( "d3d11 + imgui initialized" );
      }
    }

    // safety check
    if ( !context )
      return original_present( swapchain, sync_interval, flags );

    // create render target fresh each frame (don't hold persistent reference to back buffer)
    ID3D11RenderTargetView* render_target = nullptr;
    ID3D11Texture2D* back_buffer = nullptr;

    if ( FAILED( swapchain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >( &back_buffer ) ) ) )
      return original_present( swapchain, sync_interval, flags );

    if ( FAILED( device->CreateRenderTargetView( back_buffer, nullptr, &render_target ) ) ) {
      back_buffer->Release();
      return original_present( swapchain, sync_interval, flags );
    }

    back_buffer->Release();

    // save current D3D11 state
    ID3D11RenderTargetView* old_rtv = nullptr;
    ID3D11DepthStencilView* old_dsv = nullptr;

    context->OMGetRenderTargets( 1, &old_rtv, &old_dsv );

    // render imgui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGui::NewFrame();

    auto& io = ImGui::GetIO();
    io.MouseDrawCursor = show_menu;

    if ( show_menu ) {
      ImGui::Begin( "pacifica", &show_menu, ImGuiWindowFlags_AlwaysAutoResize );

      if ( ImGui::CollapsingHeader( "aim assist", ImGuiTreeNodeFlags_DefaultOpen ) ) {
        ImGui::SeparatorText( "settings" );

        ImGui::DragFloat( "pull", &aim::config::pull, 0.001f, 0.0f, 0.1f, "%.3f" );

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip( "how fast cursor pulls toward target" );

        ImGui::DragFloat( "decay", &aim::config::decay, 0.005f, 0.9f, 1.0f, "%.3f" );

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip( "how fast offset decays (higher = stickier)" );

        ImGui::DragFloat( "fov", &aim::config::fov, 0.5f, 1.0f, 50.0f, "%.1f px" );

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip( "assist range in pixels beyond hitcircle" );

        ImGui::SeparatorText( "debug" );

        ImGui::Checkbox( "display overlay", &aim::config::display_overlay );

        if ( aim::config::display_overlay ) 
          ImGui::SliderFloat( "line thickness", &aim::config::offset_thickness, 1.0f, 8.0f, "%.1f" );
        
        if ( aim::config::display_overlay && ImGui::TreeNode( "overlay colors" ) ) {
          ImGui::ColorEdit4( "fov", aim::config::col_fov, ImGuiColorEditFlags_AlphaBar );
          ImGui::ColorEdit4( "target", aim::config::col_target, ImGuiColorEditFlags_AlphaBar );
          ImGui::ColorEdit4( "offset", aim::config::col_offset, ImGuiColorEditFlags_AlphaBar );
          ImGui::ColorEdit4( "cursor", aim::config::col_cursor, ImGuiColorEditFlags_AlphaBar );

          ImGui::TreePop();
        }
      }

      ImGui::End();
    }

    // draw aim assist overlay
    if ( initialized ) {
      aim::draw();
    }

    ImGui::Render();
    context->OMSetRenderTargets( 1, &render_target, nullptr );

    ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );

    // restore original D3D11 state
    context->OMSetRenderTargets( 1, &old_rtv, old_dsv );
    if ( old_rtv )
      old_rtv->Release();

    if ( old_dsv )
      old_dsv->Release();

    // release our render target (created fresh each frame)
    render_target->Release();

    return original_present( swapchain, sync_interval, flags );
  }

  void init() {
    // wait for a valid foreground window
    HWND fg_hwnd = nullptr;

    while ( !fg_hwnd ) {
      fg_hwnd = GetForegroundWindow();

      if ( !fg_hwnd )
        Sleep( 100 );
    }

    LOG( "found foreground window @ %p", fg_hwnd );

    // create dummy swapchain to get vtable address
    DXGI_SWAP_CHAIN_DESC sd {};

    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = fg_hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* dummy_swapchain = nullptr;
    ID3D11Device* dummy_device = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      0,
      nullptr,
      0,
      D3D11_SDK_VERSION,
      &sd,
      &dummy_swapchain,
      &dummy_device,
      nullptr,
      nullptr
    );

    if ( FAILED( hr ) ) {
      LOG( "failed to create dummy swapchain (hr=0x%08X)", hr );
      return;
    }

    // get vtable and Present address (index 8)
    void** vtable = *reinterpret_cast< void*** >( dummy_swapchain );
    void* present_addr = vtable[ 8 ];

    LOG( "present @ %p", present_addr );

    dummy_swapchain->Release();
    dummy_device->Release();

    // hook Present using Detours
    if ( !SETUP_HOOK( present_addr, hooked_present, original_present ) ) {
      LOG( "failed to hook present" );
      return;
    }

    LOG( "present hooked via detours" );
  }

  void shutdown() {
    // restore wndproc
    if ( original_wndproc && hwnd )
      SetWindowLongPtrA( hwnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( original_wndproc ) );

    // cleanup imgui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    if ( context )
      context->Release();

    if ( device )
      device->Release();
  }
} // namespace features::menu
