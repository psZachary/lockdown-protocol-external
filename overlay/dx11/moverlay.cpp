#include "../overlay.h"
#include <tuple>

template <typename T>
inline VOID c_overlay::safe_release(T*& p) {
    if (NULL != p) {
        p->Release();
        p = NULL;
    }
}

HWND c_overlay::get_window_handle() {
    return window_handle;
}

BOOL c_overlay::init_device() {
    DXGI_SWAP_CHAIN_DESC vSwapChainDesc;
    ZeroMemory(&vSwapChainDesc, sizeof(vSwapChainDesc));
    vSwapChainDesc.BufferCount = 2;
    vSwapChainDesc.BufferDesc.Width = 0;
    vSwapChainDesc.BufferDesc.Height = 0;
    vSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    vSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    vSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    vSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    vSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    vSwapChainDesc.OutputWindow = window_handle;
    vSwapChainDesc.SampleDesc.Count = 1;
    vSwapChainDesc.SampleDesc.Quality = 0;
    vSwapChainDesc.Windowed = TRUE;
    vSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL vFeatureLevel;
    D3D_FEATURE_LEVEL vFeatureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, vFeatureLevelArray, 2, D3D11_SDK_VERSION, &vSwapChainDesc, &swap_chain, &d3d_device, &vFeatureLevel, &device_context) != S_OK)
        return false;

    init_render_target();

    ShowWindow(window_handle, SW_SHOWNORMAL);
    UpdateWindow(window_handle);
    return true;
}



VOID c_overlay::dest_device() {
    dest_render_target();
    safe_release(swap_chain);
    safe_release(device_context);
    safe_release(d3d_device);
}

VOID c_overlay::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().WantSaveIniSettings = false;
    ImGui::GetIO().IniFilename = "";

    //ImGuiStyle* style = &ImGui::GetStyle();
    ImGui::StyleColorsDark();

    // Customize Style
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 7.5f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 10.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 9.699999809265137f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(5.0f, 2.0f);
    style.FrameRounding = 6.400000095367432f;
    style.FrameBorderSize = 1.0f;
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.CellPadding = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 16.20000076293945f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 1.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.9200000166893005f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.2899999916553497f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.239999994635582f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481f, 0.3372549116611481f, 0.3372549116611481f, 0.5400000214576721f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.5400000214576721f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779f, 0.5568627715110779f, 0.5568627715110779f, 0.5400000214576721f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7896995544433594f, 0.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.7882353067398071f, 0.0f, 1.0f, 0.6866952776908875f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7882353067398071f, 0.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776f, 0.0470588244497776f, 0.0470588244497776f, 0.5400000214576721f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.5400000214576721f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.3600000143051147f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 0.3300000131130219f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.2899999916553497f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645f, 0.4392156898975372f, 0.4666666686534882f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3600000143051147f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.5199999809265137f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389f, 0.2784313857555389f, 0.2784313857555389f, 0.2899999916553497f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2274509817361832f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335f, 0.6666666865348816f, 0.8588235378265381f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.3499999940395355f);


    style.WindowTitleAlign = ImVec2(0.500f, 0.500f);

    ImGuiIO& io = ImGui::GetIO();
    font->im_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 13.f);
    font->font_size = 24.f;

    ImGui_ImplWin32_Init(window_handle);
    ImGui_ImplDX11_Init(d3d_device, device_context);
}

VOID c_overlay::dest_imgui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

VOID c_overlay::init_render_target() {
    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

    if (back_buffer) {
        d3d_device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
    }
    back_buffer->Release();
}

VOID c_overlay::dest_render_target() {
    if (!render_target_view)
        return;

    render_target_view->Release();
    render_target_view = NULL;
}

c_overlay::c_overlay() :
    exit_ready(false),
    window_handle(nullptr),
    window_class( {}),
              window_width((FLOAT)GetSystemMetrics(SM_CXSCREEN)),
              window_height((FLOAT)GetSystemMetrics(SM_CYSCREEN)),
              font(nullptr),
              d3d_device(nullptr),
              device_context(nullptr),
              swap_chain(nullptr),
              render_target_view(nullptr),
              draw_list(nullptr),
              breath(NULL),
menu_ticks(NULL) {
    //window_handle = FindWindowW(L"HudSight 2Mirror2DesktopWindowClass", L"HudSight 2Mirror2DesktopWindowClass1");
    window_handle = FindWindowW(L"MedalOverlayClass", L"MedalOverlay");

    if (window_handle) {

        auto info = GetWindowLongA(window_handle, -20);
        if (!info)
            return;

        auto attrchange = SetWindowLongPtrA(window_handle, -20, (LONG_PTR)(info | 0x20));
        if (!attrchange)
            return;

        MARGINS margin;
        margin.cyBottomHeight = margin.cyTopHeight = margin.cxLeftWidth = margin.cxRightWidth = -1;

        if (DwmExtendFrameIntoClientArea(window_handle, &margin) != S_OK)
            return;

        if (!SetLayeredWindowAttributes(window_handle, 0x000000, 0xFF, 0x02))
            return;

        auto [x, y] = std::tuple<float, float>((float)GetSystemMetrics(SM_CXSCREEN), (float)GetSystemMetrics(SM_CYSCREEN));

        if (!SetWindowPos(window_handle, HWND_TOPMOST, 0, 0, static_cast<int>(x), static_cast<int>(y), 0))
            return;
    } else
        window_handle = FindWindowA(("0"), ("0"));

    font = new s_font();

    if (!init_device()) {
        return;
    }

    init_imgui();

    std::thread(&c_overlay::input_handler, this).detach();
}

c_overlay::~c_overlay() {
    dest_imgui();
    dest_device();

    delete font;
}

void c_overlay::bind_render_callback(std::function<void()> callback) {
    this->render_callback = (callback);
}


const void c_overlay::render(FLOAT width, FLOAT height) {
    SetWindowPos(window_handle, 0, 0, 0, static_cast<int>(window_width), static_cast<int>(window_height), 0);

    ImGuiStyle& style = ImGui::GetStyle();

    init_draw_list();

    if (menu_ticks == 1)
        return;

    this->render_callback();
}

void c_overlay::end_frame() {
    ImGui::Render();
    device_context->OMSetRenderTargets(1, &render_target_view, NULL);
    device_context->ClearRenderTargetView(render_target_view, (float*)&clear_clr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    swap_chain->Present(1, 0);
}

MSG c_overlay::begin_frame() {
    MSG msg { 0 };

    if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    return msg;
}

BOOL c_overlay::msg_loop() {
    if (exit_ready)
        return false;

    if (window_handle && !clear_screen) {
        auto msg = begin_frame();

        render(window_width, window_height);

        end_frame();

        return msg.message != WM_QUIT;
    } else if (clear_screen) {
        auto msg = begin_frame();
        end_frame();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        clear_screen = false;

        return msg.message != WM_QUIT;
    }
    return false;
}

void c_overlay::input_handler() {
    for (; !exit_ready; Sleep(1)) {
        ImGuiIO& io = ImGui::GetIO();

        POINT p {};
        GetCursorPos(&p);
        io.MousePos = ImVec2((float)p.x, (float)p.y);

        io.MouseDown[0] = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        io.MouseDown[1] = GetAsyncKeyState(VK_RBUTTON) & 0x8000;

        bool is_arrow_key_up_down = GetAsyncKeyState(VK_UP) != 0;
        bool is_arrow_key_down_down = GetAsyncKeyState(VK_DOWN) != 0;

        io.MouseWheel = is_arrow_key_up_down ? .5f : is_arrow_key_down_down ? -.5f : 0.0f;
    }
}

VOID c_overlay::init_draw_list() {
    draw_list = ImGui::GetBackgroundDrawList();
}

BOOL c_overlay::in_screen(const ImVec2& pos) {
    return !(pos.x > window_width || pos.x<0 || pos.y>window_height || pos.y < 0);
}

void c_overlay::exit() {
    begin_frame();
    end_frame();
    exit_ready = true;
}
