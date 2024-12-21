#include "../overlay.h"
#include "../globals.h"

using namespace globals;

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

    void load_font() {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig cfg;
        cfg.OversampleH = cfg.OversampleV = 1;
        cfg.PixelSnapH = true;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0x1C80, 0x1C88, // Cyrillic Extended-C
            0x3040, 0x30FF, // Hiragana
            0x30A0, 0x30FF, // Katakana
            0x4E00, 0x9FFF, // CJK Unified Ideographs (Most common Han characters)
            0xAC00, 0xD7AF, // Hangul Syllables
            0x0,
        };

        default_font.im_font = io.Fonts->AddFontFromFileTTF("fonts/MSYH.TTC", 14.0f, &cfg, ranges); // Load msyh
       default_font.font_size = 14.0f; // Store the font size

        if (default_font.im_font == nullptr) {
            default_font.im_font = io.Fonts->AddFontDefault();
            if (default_font.im_font == nullptr) {
                std::cout << "Failed to load any font!" << std::endl;
                return;
            }
            std::cout << "Failed to load msyh.ttc, using default font" << std::endl;
        }
    }

    inline void draw_text(const ImVec2& pos, ImU32 col, const char* text_begin, bool outline = false, float font_size = 14.0f, const char* end = nullptr) {
              ImU8 alpha = (col >> IM_COL32_A_SHIFT) & 0xFF;
        ImU32 black_with_alpha = IM_COL32(0, 0, 0, alpha);

        if (outline) {
            draw_list->AddText(default_font.im_font, font_size, ImVec2(pos.x + 1, pos.y + 1), black_with_alpha, text_begin, end);
            draw_list->AddText(default_font.im_font, font_size, ImVec2(pos.x - 1, pos.y - 1), black_with_alpha, text_begin, end);
            draw_list->AddText(default_font.im_font, font_size, ImVec2(pos.x + 1, pos.y - 1), black_with_alpha, text_begin, end);
            draw_list->AddText(default_font.im_font, font_size, ImVec2(pos.x - 1, pos.y + 1), black_with_alpha, text_begin, end);
        }

        draw_list->AddText(default_font.im_font, font_size, pos, col, text_begin, end);
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