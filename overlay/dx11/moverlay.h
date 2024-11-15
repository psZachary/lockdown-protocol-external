#include "../overlay.h"

constexpr int menu_anim_time = 350;
constexpr int breath_anim_time = 1000;

constexpr ImVec4 clear_clr = { 0,0,0,0 };

class c_overlay {
public:
    HWND								window_handle;
    WNDCLASSEX							window_class;
    ID3D11Device* d3d_device;
    ID3D11DeviceContext* device_context;
    IDXGISwapChain* swap_chain;
    ID3D11RenderTargetView* render_target_view;

    struct s_font {
        ImFont* im_font;
        FLOAT font_size;
    };

    template <typename T>
    inline VOID safe_release(T*& p);

    BOOL init_device();
    VOID dest_device();

    VOID init_imgui();
    VOID dest_imgui();

    VOID init_render_target();
    VOID dest_render_target();
    const VOID render(FLOAT width, FLOAT height);

    void end_frame();
    MSG begin_frame();

    void input_handler();

    c_overlay();
    ~c_overlay();

    ImDrawList* draw_list;

    void bind_render_callback(std::function<void()> callback);
    std::function<void()> render_callback;

    BOOL msg_loop();
    HWND get_window_handle();
    VOID init_draw_list();

    BOOL in_screen(const ImVec2& pos);

    void exit();



    inline void draw_text(const ImVec2& pos, ImU32 col, const char* text_begin, bool outline = false, float font_size = 0.0f, const char* end = 0) {
        if (outline) {
            draw_list->AddText(NULL, font_size, ImVec2(pos.x + 1, pos.y + 1), IM_COL32_BLACK, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x - 1, pos.y - 1), IM_COL32_BLACK, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x + 1, pos.y - 1), IM_COL32_BLACK, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x - 1, pos.y + 1), IM_COL32_BLACK, text_begin, end);
        }

        draw_list->AddText(NULL, font_size, pos, col, text_begin);
    }
    inline void draw_rect(const ImVec2& pos1, const ImVec2& pos2, ImU32 col, bool outline = false, float rounding = 0.0f, int rounding_corners_flags = 0x0F, float thickness = 1.0f) {
        if (outline) {
            draw_list->AddRect(ImVec2(pos1.x + 1, pos1.y + 1), ImVec2(pos2.x + 1, pos2.y
                + 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x - 1, pos1.y - 1), ImVec2(pos2.x - 1, pos2.y - 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x + 1, pos1.y - 1), ImVec2(pos2.x + 1, pos2.y - 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x - 1, pos1.y + 1), ImVec2(pos2.x - 1, pos2.y + 1), col, rounding, rounding_corners_flags, thickness);
        }

        draw_list->AddRect(pos1, pos2, col, rounding, rounding_corners_flags, thickness);
    }

    FLOAT window_width, window_height;
    bool exit_ready, show_menu = true;
    bool clear_screen = false;
    INT breath, menu_ticks;
    s_font* font;
};
