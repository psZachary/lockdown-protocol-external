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
        // Extract the alpha value from 'col'
        ImU8 alpha = (col >> IM_COL32_A_SHIFT) & 0xFF;  // Extract the alpha channel (0-255)

        // Create a black color with the same alpha as 'col'
        ImU32 black_with_alpha = IM_COL32(0, 0, 0, alpha);

        if (outline) {
            draw_list->AddText(NULL, font_size, ImVec2(pos.x + 1, pos.y + 1), black_with_alpha, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x - 1, pos.y - 1), black_with_alpha, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x + 1, pos.y - 1), black_with_alpha, text_begin, end);
            draw_list->AddText(NULL, font_size, ImVec2(pos.x - 1, pos.y + 1), black_with_alpha, text_begin, end);
        }

        // Draw the main text with the original color
        draw_list->AddText(NULL, font_size, pos, col, text_begin);
    }
    inline void draw_rect(const ImVec2& pos1, const ImVec2& pos2, ImU32 col, bool outline = false, float rounding = 0.0f, int rounding_corners_flags = 0x0F, float thickness = 0.1f) {
        if (outline) {
            draw_list->AddRect(ImVec2(pos1.x + 1, pos1.y + 1), ImVec2(pos2.x + 1, pos2.y
                + 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x - 1, pos1.y - 1), ImVec2(pos2.x - 1, pos2.y - 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x + 1, pos1.y - 1), ImVec2(pos2.x + 1, pos2.y - 1), col, rounding, rounding_corners_flags, thickness);
            draw_list->AddRect(ImVec2(pos1.x - 1, pos1.y + 1), ImVec2(pos2.x - 1, pos2.y + 1), col, rounding, rounding_corners_flags, thickness);
        }

        draw_list->AddRect(pos1, pos2, col, rounding, rounding_corners_flags, thickness);
    }
    inline void draw_rect_filled(const ImVec2& pos1, const ImVec2& pos2, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = 0x0F) {
        draw_list->AddRectFilled(pos1, pos2, col, rounding, rounding_corners_flags);
    }

    inline void draw_rect_with_fill(const ImVec2& pos1, const ImVec2& pos2, ImU32 fill_color, ImU32 outline_color, float thickness = 1.0f, float rounding = 0.0f, int rounding_corners_flags = 0x0F) {
        // Draw filled rectangle if alpha in fill_color > 0
        if ((fill_color >> 24) & 0xFF) { // Checks if the alpha component of fill_color is non-zero
            draw_list->AddRectFilled(pos1, pos2, fill_color, rounding, rounding_corners_flags);
        }

        // Draw the outline
        if (thickness > 0.0f) {
            draw_list->AddRect(pos1, pos2, outline_color, rounding, rounding_corners_flags, thickness);
        }
    }

    inline void draw_circle(const ImVec2& center, float radius, ImU32 color, int num_segments = 32, float thickness = 1.0f) {
        draw_list->AddCircle(center, radius, color, num_segments, thickness);
    }

    inline void draw_circle_filled(const ImVec2& center, float radius, ImU32 color, int num_segments = 32) {
        draw_list->AddCircleFilled(center, radius, color, num_segments);
    }

    inline void draw_circle_with_fill(const ImVec2& center, float radius, ImU32 fill_color, ImU32 outline_color, int num_segments = 32, float thickness = 1.0f) {
        // Draw filled circle if alpha in fill_color > 0
        if ((fill_color >> 24) & 0xFF) { // Check if the alpha component of fill_color is non-zero
            draw_list->AddCircleFilled(center, radius, fill_color, num_segments);
        }

        // Draw outline if thickness > 0
        if (thickness > 0.0f) {
            draw_list->AddCircle(center, radius, outline_color, num_segments, thickness);
        }
    }

    inline void draw_triangle_filled(const ImVec2& point1, const ImVec2& point2, const ImVec2& point3, ImU32 color) {
        draw_list->AddTriangleFilled(point1, point2, point3, color);
    }

    FLOAT window_width, window_height;
    bool exit_ready, show_menu = true;
    bool clear_screen = false;
    INT breath, menu_ticks;
    s_font* font;
};