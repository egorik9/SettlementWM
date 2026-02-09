// ============================================================================
// SETTLEMENT WM - ЭЛЕГАНТНАЯ ВЕРСИЯ С ПОИСКОМ ПРИЛОЖЕНИЙ
// ============================================================================

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xresource.h>

// Cairo для графики
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>

// Библиотеки для поиска приложений
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <functional>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <sys/wait.h>
#include <libgen.h>
#include <sys/stat.h>
#include <dirent.h>
#include <thread>
#include <mutex>
#include <queue>
#include <deque>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <pwd.h>
#include <unordered_set>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

// ============================================================================
// КОНСТАНТЫ И КОНФИГУРАЦИЯ
// ============================================================================

#define WM_NAME "SettlementWM"
#define WM_VERSION "3.0.0"

namespace Config {
    // Размеры
    const int BORDER_WIDTH = 2;
    const int TITLEBAR_HEIGHT = 32;
    const int BUTTON_SIZE = 16;
    const int BUTTON_MARGIN = 8;
    const int GAP = 12;
    const int RESIZE_HANDLE_SIZE = 10;
    const int SHADOW_SIZE = 30;
    const int ACTIVE_SHADOW_SIZE = 40;
    const int CORNER_RADIUS = 12;
    const int PANEL_HEIGHT = 42;
    const int ANIMATION_DURATION = 250;
    const float ANIMATION_CURVE = 0.8f;
    const int WORKSPACE_COUNT = 9;
    const int MAX_LAUNCHER_RESULTS = 10;

    // Цвета (RGB) - Современная цветовая схема
    namespace Colors {
        // Основные цвета
        const unsigned long PRIMARY = 0xFF5E7CE2;     // Синий
        const unsigned long SECONDARY = 0xFF8A6FE2;   // Фиолетовый
        const unsigned long ACCENT = 0xFF4AC6B0;      // Бирюзовый

        // Активные цвета
        const unsigned long ACTIVE_BORDER = PRIMARY;
        const unsigned long ACTIVE_TITLEBAR = 0xFF2D2F36;
        const unsigned long ACTIVE_TITLE = 0xFFF0F0F0;

        // Неактивные цвета
        const unsigned long INACTIVE_BORDER = 0xFF444752;
        const unsigned long INACTIVE_TITLEBAR = 0xFF252730;
        const unsigned long INACTIVE_TITLE = 0xFFA0A0A8;

        // Цвета кнопок
        const unsigned long CLOSE_BUTTON = 0xFFFF6B6B;
        const unsigned long CLOSE_BUTTON_HOVER = 0xFFFF5252;
        const unsigned long MAXIMIZE_BUTTON = 0xFF4ECDC4;
        const unsigned long MAXIMIZE_BUTTON_HOVER = 0xFF45B7B1;
        const unsigned long MINIMIZE_BUTTON = 0xFFFFD166;
        const unsigned long MINIMIZE_BUTTON_HOVER = 0xFFFFC145;

        // Панель
        const unsigned long PANEL_BG = 0xCC1E1F28;
        const unsigned long PANEL_FG = 0xFFE8E8F0;
        const unsigned long PANEL_ACCENT = PRIMARY;
        const unsigned long PANEL_WORKSPACE_ACTIVE = PRIMARY;
        const unsigned long PANEL_WORKSPACE_INACTIVE = 0x70707080;
        const unsigned long PANEL_WORKSPACE_OCCUPIED = SECONDARY;

        // Рабочий стол
        const unsigned long DESKTOP_BG = 0xFF12121A;
        const unsigned long DESKTOP_GRADIENT_START = 0xFF12121A;
        const unsigned long DESKTOP_GRADIENT_END = 0xFF1A1B24;

        // Launcher (поиск приложений)
        const unsigned long LAUNCHER_BG = 0xEE1E1F28;
        const unsigned long LAUNCHER_FG = 0xFFE8E8F0;
        const unsigned long LAUNCHER_SELECTED = 0x505E7CE2;
        const unsigned long LAUNCHER_BORDER = PRIMARY;
        const unsigned long LAUNCHER_SHADOW = 0x80000000;

        // Состояния
        const unsigned long URGENT_BORDER = 0xFFFF7B7B;
        const unsigned long SUCCESS = 0xFF4ECDC4;
        const unsigned long WARNING = 0xFFFFD166;
        const unsigned long ERROR = 0xFFFF6B6B;
    }

    // Горячие клавиши
    namespace Keys {
        const unsigned int SUPER = Mod4Mask;
        const unsigned int SHIFT = ShiftMask;
        const unsigned int CTRL = ControlMask;
        const unsigned int ALT = Mod1Mask;

        // Основные команды
        const int TERMINAL = XK_Return;
        const int CLOSE = XK_q;
        const int FULLSCREEN = XK_f;
        const int TOGGLE_FLOAT = XK_space;
        const int MAXIMIZE = XK_m;
        const int MINIMIZE = XK_n;
        const int LAUNCHER = XK_d;
        const int LAUNCHER_ALT = XK_p;
        const int EXIT = XK_Escape;

        // Навигация
        const int NEXT_WINDOW = XK_Tab;
        const int PREV_WINDOW = XK_grave;
        const int LEFT = XK_Left;
        const int RIGHT = XK_Right;
        const int UP = XK_Up;
        const int DOWN = XK_Down;

        // Управление окнами
        const int SNAP_LEFT = XK_h;
        const int SNAP_RIGHT = XK_l;
        const int SNAP_UP = XK_k;
        const int SNAP_DOWN = XK_j;
        const int CENTER = XK_c;
        const int TOGGLE_STICKY = XK_s;
        const int TOGGLE_ALWAYS_ON_TOP = XK_t;
        const int KILL_CLIENT = XK_x;

        // Режимы
        const int RESIZE_MODE = XK_r;
        const int MOVE_MODE = XK_v;

        // Менеджеры и утилиты
        const int FILE_MANAGER = XK_e;
        const int WEB_BROWSER = XK_w;
        const int SCREENSHOT = XK_Print;
        const int LOCK_SCREEN = XK_l;
        const int CALCULATOR = XK_c;
        const int SETTINGS = XK_comma;

        // Рабочие столы
        const int WS_1 = XK_1;
        const int WS_2 = XK_2;
        const int WS_3 = XK_3;
        const int WS_4 = XK_4;
        const int WS_5 = XK_5;
        const int WS_6 = XK_6;
        const int WS_7 = XK_7;
        const int WS_8 = XK_8;
        const int WS_9 = XK_9;
        const int NEXT_WS = XK_period;
        const int PREV_WS = XK_comma;
        const int MOVE_TO_WS = XK_Shift_L;
    }
}

// ============================================================================
// СТРУКТУРЫ ДАННЫХ
// ============================================================================

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}

    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }

    Point operator*(float scale) const {
        return Point(static_cast<int>(x * scale), static_cast<int>(y * scale));
    }
};

struct Size {
    unsigned int width, height;
    Size(unsigned int w = 0, unsigned int h = 0) : width(w), height(h) {}

    Size operator+(const Size& other) const {
        return Size(width + other.width, height + other.height);
    }

    Size operator-(const Size& other) const {
        return Size(width - other.width, height - other.height);
    }

    Size operator*(float scale) const {
        return Size(static_cast<unsigned int>(width * scale),
                   static_cast<unsigned int>(height * scale));
    }
};

struct Rectangle {
    int x, y;
    unsigned int width, height;

    Rectangle(int x = 0, int y = 0, unsigned int w = 0, unsigned int h = 0)
        : x(x), y(y), width(w), height(h) {}

    bool contains(int px, int py) const {
        return px >= x && px < x + (int)width &&
               py >= y && py < y + (int)height;
    }

    Point center() const { return Point(x + width / 2, y + height / 2); }

    int left() const { return x; }
    int right() const { return x + width; }
    int top() const { return y; }
    int bottom() const { return y + height; }

    Rectangle snapped_left(int screen_width) const {
        return Rectangle(0, y, screen_width / 2, height);
    }

    Rectangle snapped_right(int screen_width) const {
        return Rectangle(screen_width / 2, y, screen_width / 2, height);
    }

    Rectangle snapped_top(int screen_height) const {
        return Rectangle(x, Config::PANEL_HEIGHT, width, screen_height / 2 - Config::PANEL_HEIGHT);
    }

    Rectangle snapped_bottom(int screen_height) const {
        return Rectangle(x, screen_height / 2, width, screen_height / 2 - Config::PANEL_HEIGHT);
    }

    Rectangle centered(int screen_width, int screen_height) const {
        int new_x = (screen_width - static_cast<int>(width)) / 2;
        int new_y = (screen_height - static_cast<int>(height)) / 2;
        return Rectangle(new_x, new_y + Config::PANEL_HEIGHT, width, height);
    }
};

// Структура для приложения в лаунчере
struct AppEntry {
    std::string name;
    std::string exec;
    std::string icon;
    std::string description;
    std::string category;

    bool operator<(const AppEntry& other) const {
        return name < other.name;
    }
};

// ============================================================================
// КЛАСС ПОИСКА ПРИЛОЖЕНИЙ
// ============================================================================

class ApplicationLauncher {
private:
    std::vector<AppEntry> m_applications;
    std::vector<std::string> m_searchPaths;

public:
    ApplicationLauncher() {
        // Стандартные пути к .desktop файлам
        const char* home = getenv("HOME");
        if (home) {
            m_searchPaths = {
                "/usr/share/applications",
                "/usr/local/share/applications",
                fs::path(home) / ".local/share/applications"
            };
        } else {
            m_searchPaths = {
                "/usr/share/applications",
                "/usr/local/share/applications"
            };
        }

        loadApplications();
    }

    void loadApplications() {
        m_applications.clear();

        for (const auto& path : m_searchPaths) {
            try {
                if (fs::exists(path) && fs::is_directory(path)) {
                    for (const auto& entry : fs::directory_iterator(path)) {
                        if (entry.path().extension() == ".desktop") {
                            parseDesktopFile(entry.path());
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error loading applications from " << path << ": " << e.what() << std::endl;
            }
        }

        std::sort(m_applications.begin(), m_applications.end());
        std::cout << "Loaded " << m_applications.size() << " applications" << std::endl;
    }

    void parseDesktopFile(const fs::path& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) return;

        AppEntry app;
        std::string line;
        bool isDesktopEntry = false;

        while (std::getline(file, line)) {
            // Пропускаем комментарии и пустые строки
            if (line.empty() || line[0] == '#') continue;

            // Проверяем секцию
            if (line[0] == '[') {
                isDesktopEntry = (line == "[Desktop Entry]");
                continue;
            }

            if (!isDesktopEntry) continue;

            // Парсим ключи
            size_t eqPos = line.find('=');
            if (eqPos == std::string::npos) continue;

            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);

            if (key == "Name") {
                app.name = value;
            } else if (key == "Exec") {
                app.exec = value;
            } else if (key == "Icon") {
                app.icon = value;
            } else if (key == "Comment") {
                app.description = value;
            } else if (key == "Categories") {
                app.category = value;
            }
        }

        // Добавляем только если есть имя и команда запуска
        if (!app.name.empty() && !app.exec.empty()) {
            // Очищаем команду от параметров
            cleanExecString(app.exec);
            m_applications.push_back(app);
        }
    }

    void cleanExecString(std::string& exec) {
        // Удаляем специфичные параметры desktop файлов
        std::regex paramRegex("%[a-zA-Z]");
        exec = std::regex_replace(exec, paramRegex, "");

        // Удаляем лишние пробелы
        exec.erase(std::remove(exec.begin(), exec.end(), '\n'), exec.end());
        exec.erase(std::remove(exec.begin(), exec.end(), '\r'), exec.end());
        exec = std::regex_replace(exec, std::regex("\\s+"), " ");

        // Обрезаем пробелы в начале и конце
        size_t start = exec.find_first_not_of(" ");
        if (start != std::string::npos) {
            exec = exec.substr(start);
        }
        size_t end = exec.find_last_not_of(" ");
        if (end != std::string::npos) {
            exec = exec.substr(0, end + 1);
        }
    }

    std::vector<AppEntry> search(const std::string& query, int maxResults = Config::MAX_LAUNCHER_RESULTS) {
        std::vector<AppEntry> results;

        if (query.empty()) {
            // Возвращаем популярные приложения при пустом запросе
            std::vector<std::string> popular = {
                "terminal", "firefox", "chrome", "files", "editor",
                "calculator", "settings", "browser", "text", "code"
            };

            for (const auto& app : m_applications) {
                bool isPopular = false;
                std::string lowerName = app.name;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

                for (const auto& popularTerm : popular) {
                    if (lowerName.find(popularTerm) != std::string::npos) {
                        isPopular = true;
                        break;
                    }
                }

                if (isPopular && results.size() < static_cast<size_t>(maxResults)) {
                    results.push_back(app);
                }
            }

            return results;
        }

        // Поиск по имени и описанию
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        for (const auto& app : m_applications) {
            std::string lowerName = app.name;
            std::string lowerDesc = app.description;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
            std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);

            if (lowerName.find(lowerQuery) != std::string::npos ||
                lowerDesc.find(lowerQuery) != std::string::npos ||
                app.category.find(lowerQuery) != std::string::npos) {
                results.push_back(app);

                if (results.size() >= static_cast<size_t>(maxResults)) {
                    break;
                }
            }
        }

        return results;
    }

    void launch(const AppEntry& app) {
        std::string command = app.exec + " &";
        std::cout << "Launching: " << app.name << " (" << command << ")" << std::endl;

        pid_t pid = fork();
        if (pid == 0) {
            setsid();
            execl("/bin/sh", "/bin/sh", "-c", command.c_str(), nullptr);
            _exit(0);
        } else if (pid > 0) {
            waitpid(pid, nullptr, WNOHANG);
        }
    }

    const std::vector<AppEntry>& getAllApplications() const {
        return m_applications;
    }
};

// ============================================================================
// КЛАСС ОКНА С АНИМАЦИЯМИ
// ============================================================================

class Client {
public:
    Window window;
    Window frame;
    std::string title;
    std::string class_name;
    std::string instance_name;
    Rectangle geometry;
    Rectangle normal_geometry;
    bool is_mapped;
    bool is_fullscreen;
    bool is_maximized;
    bool is_minimized;
    bool is_floating;
    bool is_sticky;
    bool is_always_on_top;
    bool has_focus;
    bool is_urgent;
    unsigned int workspace;
    unsigned int border_width;
    unsigned long border_color;

    // Состояние для анимаций
    struct Animation {
        Point start_pos;
        Point target_pos;
        Size start_size;
        Size target_size;
        unsigned long start_time;
        bool active;

        Animation() : active(false), start_time(0) {}
    };

    Animation animation;

    // Cairo
    cairo_surface_t* cairo_surface;
    cairo_t* cairo_context;

    Client(Window w = 0)
        : window(w), frame(0), is_mapped(false), is_fullscreen(false),
          is_maximized(false), is_minimized(false), is_floating(false),
          is_sticky(false), is_always_on_top(false), has_focus(false),
          is_urgent(false), workspace(1), border_width(Config::BORDER_WIDTH),
          border_color(Config::Colors::INACTIVE_BORDER),
          cairo_surface(nullptr), cairo_context(nullptr) {}

    ~Client() {
        if (cairo_context) cairo_destroy(cairo_context);
        if (cairo_surface) cairo_surface_destroy(cairo_surface);
    }

    unsigned long getBorderColor() const {
        if (is_urgent) return Config::Colors::URGENT_BORDER;
        if (!is_mapped) return Config::Colors::INACTIVE_BORDER;
        if (has_focus) return Config::Colors::ACTIVE_BORDER;
        return Config::Colors::INACTIVE_BORDER;
    }

    unsigned long getTitlebarColor() const {
        if (!is_mapped) return Config::Colors::INACTIVE_TITLEBAR;
        if (has_focus) return Config::Colors::ACTIVE_TITLEBAR;
        return Config::Colors::INACTIVE_TITLEBAR;
    }

    unsigned long getTitleColor() const {
        if (!is_mapped) return Config::Colors::INACTIVE_TITLE;
        if (has_focus) return Config::Colors::ACTIVE_TITLE;
        return Config::Colors::INACTIVE_TITLE;
    }

    bool shouldShowInWorkspace(unsigned int ws) const {
        return is_sticky || workspace == ws;
    }

    void saveNormalGeometry() {
        normal_geometry = geometry;
    }

    void restoreNormalGeometry() {
        geometry = normal_geometry;
    }

    void startAnimation(const Point& target_pos, const Size& target_size) {
        animation.start_pos = Point(geometry.x, geometry.y);
        animation.target_pos = target_pos;
        animation.start_size = Size(geometry.width, geometry.height);
        animation.target_size = target_size;
        animation.start_time = getCurrentTime();
        animation.active = true;
    }

    bool updateAnimation() {
        if (!animation.active) return false;

        unsigned long current_time = getCurrentTime();
        float t = static_cast<float>(current_time - animation.start_time) / Config::ANIMATION_DURATION;

        if (t >= 1.0f) {
            geometry.x = animation.target_pos.x;
            geometry.y = animation.target_pos.y;
            geometry.width = animation.target_size.width;
            geometry.height = animation.target_size.height;
            animation.active = false;
            return false;
        }

        // Кубическая интерполяция с плавным ускорением/замедлением
        t = 1.0f - std::pow(1.0f - t, 3.0f); // Кубическое ускорение
        t = t * Config::ANIMATION_CURVE + t * t * (1.0f - Config::ANIMATION_CURVE);

        geometry.x = animation.start_pos.x + static_cast<int>((animation.target_pos.x - animation.start_pos.x) * t);
        geometry.y = animation.start_pos.y + static_cast<int>((animation.target_pos.y - animation.start_pos.y) * t);
        geometry.width = animation.start_size.width +
                         static_cast<unsigned int>((animation.target_size.width - animation.start_size.width) * t);
        geometry.height = animation.start_size.height +
                          static_cast<unsigned int>((animation.target_size.height - animation.start_size.height) * t);

        return true;
    }

    static unsigned long getCurrentTime() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
};

// ============================================================================
// КЛАСС ПАНЕЛИ С ГРАДИЕНТАМИ
// ============================================================================

class Panel {
private:
    Display* m_display;
    Window m_window;
    Window m_parent;
    int m_screen;
    Visual* m_visual;
    Colormap m_colormap;

    cairo_surface_t* m_cairo_surface;
    cairo_t* m_cairo_context;

    int m_width;
    int m_height;

    // Анимация часов
    unsigned long m_last_clock_update;
    std::string m_current_time;

    std::vector<std::string> m_workspaces;
    int m_current_workspace;
    std::unordered_set<int> m_occupied_workspaces;

    static void cairo_rounded_rectangle(cairo_t* cr, double x, double y, double width, double height, double radius) {
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(cr);
        cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(cr);
    }

    static unsigned long getCurrentTime() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

public:
    Panel(Display* display, Window parent, Visual* visual, Colormap colormap, int screen)
        : m_display(display), m_window(0), m_parent(parent),
          m_screen(screen), m_visual(visual), m_colormap(colormap),
          m_cairo_surface(nullptr), m_cairo_context(nullptr),
          m_width(0), m_height(Config::PANEL_HEIGHT),
          m_last_clock_update(0), m_current_workspace(0) {

        // Стильные названия рабочих столов
        m_workspaces = {"一", "二", "三", "四", "五", "六", "七", "八", "九"};
        createWindow();
    }

    ~Panel() {
        if (m_cairo_context) cairo_destroy(m_cairo_context);
        if (m_cairo_surface) cairo_surface_destroy(m_cairo_surface);
        if (m_window) XDestroyWindow(m_display, m_window);
    }

    void createWindow() {
        m_width = DisplayWidth(m_display, m_screen);

        XSetWindowAttributes attrs;
        attrs.background_pixel = Config::Colors::PANEL_BG;
        attrs.border_pixel = 0;
        attrs.override_redirect = True;

        m_window = XCreateWindow(m_display, m_parent,
                                0, 0, m_width, m_height,
                                0, CopyFromParent, InputOutput,
                                CopyFromParent,
                                CWBackPixel | CWBorderPixel | CWOverrideRedirect,
                                &attrs);

        // Устанавливаем тип окна как док
        Atom wtype = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_DOCK", False);
        XChangeProperty(m_display, m_window,
                       XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False),
                       XA_ATOM, 32, PropModeReplace,
                       reinterpret_cast<unsigned char*>(&wtype), 1);

        // Всегда поверх
        Atom above = XInternAtom(m_display, "_NET_WM_STATE_ABOVE", False);
        XChangeProperty(m_display, m_window,
                       XInternAtom(m_display, "_NET_WM_STATE", False),
                       XA_ATOM, 32, PropModeAppend,
                       reinterpret_cast<unsigned char*>(&above), 1);

        m_cairo_surface = cairo_xlib_surface_create(m_display, m_window,
                                                   m_visual, m_width, m_height);
        m_cairo_context = cairo_create(m_cairo_surface);

        XMapWindow(m_display, m_window);
        draw();
    }

    void draw() {
        if (!m_cairo_context) return;

        // Градиентный фон
        cairo_pattern_t* pattern = cairo_pattern_create_linear(0, 0, 0, m_height);
        cairo_pattern_add_color_stop_rgba(pattern, 0, 0.12, 0.12, 0.15, 0.95);
        cairo_pattern_add_color_stop_rgba(pattern, 1, 0.15, 0.15, 0.18, 0.95);
        cairo_set_source(m_cairo_context, pattern);
        cairo_paint(m_cairo_context);
        cairo_pattern_destroy(pattern);

        // Легкая тень снизу
        cairo_set_source_rgba(m_cairo_context, 0, 0, 0, 0.2);
        cairo_rectangle(m_cairo_context, 0, m_height - 2, m_width, 2);
        cairo_fill(m_cairo_context);

        // Рабочие столы с эффектом свечения
        int x = 20;
        int y = m_height / 2 + 3;
        int workspace_width = 50;

        for (size_t i = 0; i < m_workspaces.size(); i++) {
            const std::string& ws = m_workspaces[i];
            bool is_occupied = m_occupied_workspaces.find(static_cast<int>(i)) != m_occupied_workspaces.end();

            if (static_cast<int>(i) == m_current_workspace) {
                // Активный рабочий стол с подсветкой
                cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 1.0);

                // Фон активного рабочего стола
                cairo_rounded_rectangle(m_cairo_context, x - 5, y - 15, 40, 28, 8);
                cairo_fill(m_cairo_context);

                cairo_select_font_face(m_cairo_context, "Sans",
                                      CAIRO_FONT_SLANT_NORMAL,
                                      CAIRO_FONT_WEIGHT_BOLD);
                cairo_set_font_size(m_cairo_context, 14);

                cairo_set_source_rgba(m_cairo_context, 1.0, 1.0, 1.0, 1.0);
                cairo_move_to(m_cairo_context, x, y + 2);
                cairo_show_text(m_cairo_context, ws.c_str());
                x += workspace_width;
            } else if (is_occupied) {
                // Рабочий стол с окнами
                cairo_set_source_rgba(m_cairo_context, 0.54, 0.44, 0.89, 1.0);
                cairo_select_font_face(m_cairo_context, "Sans",
                                      CAIRO_FONT_SLANT_NORMAL,
                                      CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(m_cairo_context, 14);

                cairo_move_to(m_cairo_context, x, y + 2);
                cairo_show_text(m_cairo_context, ws.c_str());

                // Индикатор окон (точка)
                cairo_arc(m_cairo_context, x + 30, y - 8, 3, 0, 2 * M_PI);
                cairo_fill(m_cairo_context);
                x += workspace_width;
            } else {
                // Неактивный рабочий стол
                cairo_set_source_rgba(m_cairo_context, 0.44, 0.44, 0.49, 0.7);
                cairo_select_font_face(m_cairo_context, "Sans",
                                      CAIRO_FONT_SLANT_NORMAL,
                                      CAIRO_FONT_WEIGHT_NORMAL);
                cairo_set_font_size(m_cairo_context, 14);

                cairo_move_to(m_cairo_context, x, y + 2);
                cairo_show_text(m_cairo_context, ws.c_str());
                x += workspace_width;
            }
        }

        // Часы с плавной анимацией
        drawClock();

        // Имя WM с градиентом
        drawWMInfo();

        cairo_surface_flush(m_cairo_surface);
    }

    void drawClock() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        char time_buffer[64];
        strftime(time_buffer, sizeof(time_buffer), "%H:%M", timeinfo);

        std::string new_time = time_buffer;

        // Анимация изменения времени
        if (new_time != m_current_time) {
            m_current_time = new_time;
            m_last_clock_update = getCurrentTime();
        }

        // Эффект пульсации при изменении времени
        float pulse = 1.0f;
        if (getCurrentTime() - m_last_clock_update < 500) {
            pulse = 1.0f + 0.1f * sin((getCurrentTime() - m_last_clock_update) * M_PI / 250.0f);
        }

        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairo_context, 16 * pulse);

        // Градиент для текста часов
        cairo_text_extents_t extents;
        cairo_text_extents(m_cairo_context, m_current_time.c_str(), &extents);

        int x = m_width - static_cast<int>(extents.width) - 40;
        int y = m_height / 2 + 5;

        cairo_pattern_t* text_pattern = cairo_pattern_create_linear(x, y, x + extents.width, y);
        cairo_pattern_add_color_stop_rgba(text_pattern, 0, 0.37, 0.49, 0.89, 1.0);
        cairo_pattern_add_color_stop_rgba(text_pattern, 1, 0.54, 0.44, 0.89, 1.0);
        cairo_set_source(m_cairo_context, text_pattern);

        cairo_move_to(m_cairo_context, x, y);
        cairo_show_text(m_cairo_context, m_current_time.c_str());

        cairo_pattern_destroy(text_pattern);

        // Вторая строка с датой
        char date_buffer[64];
        strftime(date_buffer, sizeof(date_buffer), "%a %d %b", timeinfo);

        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(m_cairo_context, 11);
        cairo_set_source_rgba(m_cairo_context, 0.7, 0.7, 0.75, 0.8);

        cairo_text_extents(m_cairo_context, date_buffer, &extents);
        cairo_move_to(m_cairo_context, m_width - static_cast<int>(extents.width) - 40, y + 18);
        cairo_show_text(m_cairo_context, date_buffer);
    }

    void drawWMInfo() {
        std::string info = WM_NAME " " WM_VERSION;

        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairo_context, 12);

        cairo_text_extents_t extents;
        cairo_text_extents(m_cairo_context, info.c_str(), &extents);

        int x = m_width / 2 - static_cast<int>(extents.width) / 2;
        int y = m_height / 2 + 5;

        // Градиент для названия WM
        cairo_pattern_t* pattern = cairo_pattern_create_linear(x, y, x + extents.width, y);
        cairo_pattern_add_color_stop_rgba(pattern, 0, 0.37, 0.49, 0.89, 0.8);
        cairo_pattern_add_color_stop_rgba(pattern, 0.5, 0.54, 0.44, 0.89, 0.8);
        cairo_pattern_add_color_stop_rgba(pattern, 1, 0.29, 0.78, 0.69, 0.8);
        cairo_set_source(m_cairo_context, pattern);

        cairo_move_to(m_cairo_context, x, y);
        cairo_show_text(m_cairo_context, info.c_str());

        cairo_pattern_destroy(pattern);
    }

    void setCurrentWorkspace(int workspace) {
        if (workspace >= 0 && workspace < static_cast<int>(m_workspaces.size())) {
            m_current_workspace = workspace;
            draw();
        }
    }

    void setOccupiedWorkspaces(const std::unordered_set<int>& occupied) {
        m_occupied_workspaces = occupied;
        draw();
    }

    Window getWindow() const { return m_window; }
};

// ============================================================================
// КЛАСС ЛАУНЧЕРА (ПОИСК ПРИЛОЖЕНИЙ)
// ============================================================================

class Launcher {
private:
    Display* m_display;
    Window m_window;
    Window m_parent;
    int m_screen;
    Visual* m_visual;
    Colormap m_colormap;

    cairo_surface_t* m_cairo_surface;
    cairo_t* m_cairo_context;

    int m_width;
    int m_height;
    bool m_visible;

    std::string m_search_text;
    std::vector<AppEntry> m_current_results;
    int m_selected_index;

    ApplicationLauncher m_app_launcher;
    unsigned long m_last_key_time;

    static void cairo_rounded_rectangle(cairo_t* cr, double x, double y, double width, double height, double radius) {
        double degrees = M_PI / 180.0;

        cairo_new_sub_path(cr);
        cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path(cr);
    }

    static unsigned long getCurrentTime() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

public:
    Launcher(Display* display, Window parent, Visual* visual, Colormap colormap, int screen)
        : m_display(display), m_window(0), m_parent(parent),
          m_screen(screen), m_visual(visual), m_colormap(colormap),
          m_cairo_surface(nullptr), m_cairo_context(nullptr),
          m_width(600), m_height(500),
          m_visible(false), m_selected_index(0),
          m_last_key_time(0) {

        createWindow();
    }

    ~Launcher() {
        if (m_cairo_context) cairo_destroy(m_cairo_context);
        if (m_cairo_surface) cairo_surface_destroy(m_cairo_surface);
        if (m_window) XDestroyWindow(m_display, m_window);
    }

    void createWindow() {
        int screen_width = DisplayWidth(m_display, m_screen);
        int screen_height = DisplayHeight(m_display, m_screen);

        int x = (screen_width - m_width) / 2;
        int y = (screen_height - m_height) / 3;

        XSetWindowAttributes attrs;
        attrs.background_pixel = Config::Colors::LAUNCHER_BG;
        attrs.border_pixel = Config::Colors::LAUNCHER_BORDER;
        attrs.override_redirect = True;

        m_window = XCreateWindow(m_display, m_parent,
                                x, y, m_width, m_height,
                                2, CopyFromParent, InputOutput,
                                CopyFromParent,
                                CWBackPixel | CWBorderPixel | CWOverrideRedirect,
                                &attrs);

        // Устанавливаем тип окна как диалог
        Atom dialog = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
        XChangeProperty(m_display, m_window,
                       XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False),
                       XA_ATOM, 32, PropModeReplace,
                       reinterpret_cast<unsigned char*>(&dialog), 1);

        // Всегда поверх
        Atom above = XInternAtom(m_display, "_NET_WM_STATE_ABOVE", False);
        XChangeProperty(m_display, m_window,
                       XInternAtom(m_display, "_NET_WM_STATE", False),
                       XA_ATOM, 32, PropModeAppend,
                       reinterpret_cast<unsigned char*>(&above), 1);

        // Поддерживаем ввод
        XSelectInput(m_display, m_window,
                    KeyPressMask | KeyReleaseMask | ExposureMask | FocusChangeMask);

        m_cairo_surface = cairo_xlib_surface_create(m_display, m_window,
                                                   m_visual, m_width, m_height);
        m_cairo_context = cairo_create(m_cairo_surface);

        // Скрываем по умолчанию
        XUnmapWindow(m_display, m_window);
    }

    void show() {
        if (!m_visible) {
            m_visible = true;
            m_search_text = "";
            m_selected_index = 0;
            updateSearch();

            XMapRaised(m_display, m_window);
            XSetInputFocus(m_display, m_window, RevertToParent, CurrentTime);
            draw();
        }
    }

    void hide() {
        if (m_visible) {
            m_visible = false;
            XUnmapWindow(m_display, m_window);
        }
    }

    bool isVisible() const {
        return m_visible;
    }

    void handleKeyPress(XKeyEvent& event) {
        char buffer[256];
        KeySym keysym;
        XLookupString(&event, buffer, sizeof(buffer), &keysym, nullptr);

        // Защита от слишком быстрого ввода
        unsigned long current_time = getCurrentTime();
        if (current_time - m_last_key_time < 50) {
            return;
        }
        m_last_key_time = current_time;

        switch (keysym) {
            case XK_Escape:
                hide();
                break;

            case XK_Return:
                if (!m_current_results.empty() && m_selected_index < static_cast<int>(m_current_results.size())) {
                    m_app_launcher.launch(m_current_results[m_selected_index]);
                    hide();
                }
                break;

            case XK_BackSpace:
                if (!m_search_text.empty()) {
                    m_search_text.pop_back();
                    updateSearch();
                    draw();
                }
                break;

            case XK_Up:
                if (m_selected_index > 0) {
                    m_selected_index--;
                    draw();
                }
                break;

            case XK_Down:
                if (!m_current_results.empty() && m_selected_index < static_cast<int>(m_current_results.size()) - 1) {
                    m_selected_index++;
                    draw();
                }
                break;

            case XK_Tab:
                if (!m_current_results.empty()) {
                    m_selected_index = (m_selected_index + 1) % static_cast<int>(m_current_results.size());
                    draw();
                }
                break;

            default:
                if (buffer[0] != 0 && isprint(buffer[0]) && !(event.state & ControlMask)) {
                    m_search_text += buffer[0];
                    updateSearch();
                    draw();
                }
                break;
        }
    }

    void updateSearch() {
        m_current_results = m_app_launcher.search(m_search_text);
        if (m_selected_index >= static_cast<int>(m_current_results.size())) {
            m_selected_index = std::max(0, static_cast<int>(m_current_results.size()) - 1);
        }
    }

    void draw() {
        if (!m_cairo_context || !m_visible) return;

        // Очищаем
        cairo_set_source_rgba(m_cairo_context, 0.12, 0.12, 0.15, 0.98);
        cairo_paint(m_cairo_context);

        // Скругленные углы
        cairo_rounded_rectangle(m_cairo_context, 0, 0, m_width, m_height, 16);
        cairo_clip(m_cairo_context);

        // Градиентный фон
        cairo_pattern_t* bg_pattern = cairo_pattern_create_linear(0, 0, 0, m_height);
        cairo_pattern_add_color_stop_rgba(bg_pattern, 0, 0.12, 0.12, 0.15, 0.98);
        cairo_pattern_add_color_stop_rgba(bg_pattern, 1, 0.15, 0.15, 0.18, 0.98);
        cairo_set_source(m_cairo_context, bg_pattern);
        cairo_paint(m_cairo_context);
        cairo_pattern_destroy(bg_pattern);

        // Тень
        cairo_set_source_rgba(m_cairo_context, 0, 0, 0, 0.3);
        cairo_rounded_rectangle(m_cairo_context, 2, 2, m_width - 4, m_height - 4, 14);
        cairo_stroke(m_cairo_context);

        // Заголовок
        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairo_context, 18);

        cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 1.0);
        cairo_move_to(m_cairo_context, 30, 50);
        cairo_show_text(m_cairo_context, "Launch Application");

        // Поле поиска
        drawSearchBox();

        // Результаты поиска
        drawResults();

        cairo_surface_flush(m_cairo_surface);
    }

    void drawSearchBox() {
        int box_x = 30;
        int box_y = 80;
        int box_width = m_width - 60;
        int box_height = 50;

        // Фон поля поиска
        cairo_set_source_rgba(m_cairo_context, 0.08, 0.08, 0.10, 1.0);
        cairo_rounded_rectangle(m_cairo_context, box_x, box_y, box_width, box_height, 12);
        cairo_fill(m_cairo_context);

        // Подсветка при фокусе
        cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 0.3);
        cairo_rounded_rectangle(m_cairo_context, box_x, box_y, box_width, box_height, 12);
        cairo_set_line_width(m_cairo_context, 2);
        cairo_stroke(m_cairo_context);

        // Иконка поиска
        cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 0.8);
        cairo_arc(m_cairo_context, box_x + 30, box_y + box_height/2, 8, 0, 2 * M_PI);
        cairo_fill(m_cairo_context);

        cairo_move_to(m_cairo_context, box_x + 35, box_y + box_height/2 + 8);
        cairo_line_to(m_cairo_context, box_x + 45, box_y + box_height/2 + 15);
        cairo_set_line_width(m_cairo_context, 3);
        cairo_stroke(m_cairo_context);

        // Текст поиска
        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(m_cairo_context, 16);

        std::string display_text = m_search_text.empty() ? "Type to search..." : m_search_text;
        cairo_set_source_rgba(m_cairo_context, 0.9, 0.9, 0.95, m_search_text.empty() ? 0.5 : 1.0);
        cairo_move_to(m_cairo_context, box_x + 60, box_y + box_height/2 + 6);
        cairo_show_text(m_cairo_context, display_text.c_str());

        // Курсор
        if ((getCurrentTime() / 500) % 2 == 0) {
            cairo_text_extents_t extents;
            cairo_text_extents(m_cairo_context, m_search_text.c_str(), &extents);

            cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 1.0);
            cairo_move_to(m_cairo_context, box_x + 60 + extents.width + 2, box_y + box_height/2 - 10);
            cairo_line_to(m_cairo_context, box_x + 60 + extents.width + 2, box_y + box_height/2 + 10);
            cairo_set_line_width(m_cairo_context, 2);
            cairo_stroke(m_cairo_context);
        }
    }

    void drawResults() {
        int start_y = 150;
        int item_height = 60;
        int max_visible = 6;

        if (m_current_results.empty()) {
            cairo_select_font_face(m_cairo_context, "Sans",
                                  CAIRO_FONT_SLANT_NORMAL,
                                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(m_cairo_context, 14);
            cairo_set_source_rgba(m_cairo_context, 0.6, 0.6, 0.65, 0.8);
            cairo_move_to(m_cairo_context, 50, start_y + 30);

            if (m_search_text.empty()) {
                cairo_show_text(m_cairo_context, "Popular applications will appear here...");
            } else {
                cairo_show_text(m_cairo_context, "No applications found. Try another search.");
            }
            return;
        }

        // Определяем диапазон для отображения
        int start_idx = std::max(0, m_selected_index - max_visible / 2);
        start_idx = std::min(start_idx, static_cast<int>(m_current_results.size()) - max_visible);
        start_idx = std::max(0, start_idx);

        int end_idx = std::min(start_idx + max_visible, static_cast<int>(m_current_results.size()));

        for (int i = start_idx; i < end_idx; i++) {
            const AppEntry& app = m_current_results[i];
            bool is_selected = (i == m_selected_index);
            int y = start_y + (i - start_idx) * item_height;

            drawAppItem(app, y, is_selected);
        }

        // Индикатор количества результатов
        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(m_cairo_context, 12);
        cairo_set_source_rgba(m_cairo_context, 0.6, 0.6, 0.65, 0.8);

        std::string count_text = std::to_string(m_selected_index + 1) + "/" +
                                std::to_string(m_current_results.size());
        cairo_text_extents_t extents;
        cairo_text_extents(m_cairo_context, count_text.c_str(), &extents);

        cairo_move_to(m_cairo_context, m_width - static_cast<int>(extents.width) - 30, start_y + (end_idx - start_idx) * item_height + 20);
        cairo_show_text(m_cairo_context, count_text.c_str());
    }

    void drawAppItem(const AppEntry& app, int y, bool selected) {
        int item_width = m_width - 60;
        int item_height = 50;

        // Фон элемента
        if (selected) {
            cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 0.15);
            cairo_rounded_rectangle(m_cairo_context, 30, y, item_width, item_height, 10);
            cairo_fill(m_cairo_context);

            // Подсветка выбранного элемента
            cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 0.3);
            cairo_rounded_rectangle(m_cairo_context, 30, y, item_width, item_height, 10);
            cairo_set_line_width(m_cairo_context, 2);
            cairo_stroke(m_cairo_context);
        }

        // Иконка (заглушка - цветной круг)
        cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 0.8);
        cairo_arc(m_cairo_context, 60, y + item_height/2, 15, 0, 2 * M_PI);
        cairo_fill(m_cairo_context);

        // Буква из названия внутри круга
        if (!app.name.empty()) {
            cairo_select_font_face(m_cairo_context, "Sans",
                                  CAIRO_FONT_SLANT_NORMAL,
                                  CAIRO_FONT_WEIGHT_BOLD);
            cairo_set_font_size(m_cairo_context, 14);
            cairo_set_source_rgba(m_cairo_context, 1.0, 1.0, 1.0, 1.0);

            std::string initial = std::string(1, app.name[0]);
            cairo_text_extents_t extents;
            cairo_text_extents(m_cairo_context, initial.c_str(), &extents);

            cairo_move_to(m_cairo_context, 60 - extents.width/2, y + item_height/2 + extents.height/2);
            cairo_show_text(m_cairo_context, initial.c_str());
        }

        // Название приложения
        cairo_select_font_face(m_cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(m_cairo_context, 16);
        cairo_set_source_rgba(m_cairo_context, selected ? 0.95 : 0.9,
                                             selected ? 0.95 : 0.9,
                                             selected ? 1.0 : 0.95, 1.0);

        cairo_move_to(m_cairo_context, 90, y + 20);
        cairo_show_text(m_cairo_context, app.name.c_str());

        // Описание
        if (!app.description.empty()) {
            cairo_select_font_face(m_cairo_context, "Sans",
                                  CAIRO_FONT_SLANT_NORMAL,
                                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(m_cairo_context, 12);
            cairo_set_source_rgba(m_cairo_context, 0.7, 0.7, 0.75, 0.9);

            std::string desc = app.description;
            if (desc.length() > 50) {
                desc = desc.substr(0, 47) + "...";
            }

            cairo_move_to(m_cairo_context, 90, y + 40);
            cairo_show_text(m_cairo_context, desc.c_str());
        }

        // Категория
        if (!app.category.empty()) {
            cairo_select_font_face(m_cairo_context, "Sans",
                                  CAIRO_FONT_SLANT_NORMAL,
                                  CAIRO_FONT_WEIGHT_NORMAL);
            cairo_set_font_size(m_cairo_context, 11);
            cairo_set_source_rgba(m_cairo_context, 0.54, 0.44, 0.89, 0.8);

            std::string category = app.category;
            size_t semicolon = category.find(';');
            if (semicolon != std::string::npos) {
                category = category.substr(0, semicolon);
            }

            cairo_text_extents_t extents;
            cairo_text_extents(m_cairo_context, category.c_str(), &extents);

            cairo_move_to(m_cairo_context, m_width - static_cast<int>(extents.width) - 40, y + 25);
            cairo_show_text(m_cairo_context, category.c_str());
        }

        // Индикатор выбора
        if (selected) {
            cairo_set_source_rgba(m_cairo_context, 0.37, 0.49, 0.89, 1.0);
            cairo_arc(m_cairo_context, m_width - 40, y + item_height/2, 5, 0, 2 * M_PI);
            cairo_fill(m_cairo_context);
        }
    }

    Window getWindow() const { return m_window; }
};

// ============================================================================
// ГЛАВНЫЙ КЛАСС WM
// ============================================================================

class SettlementWM {
private:
    Display* m_display;
    Window m_root;
    int m_screen;
    Visual* m_visual;
    Colormap m_colormap;
    std::atomic<bool> m_running;
    bool m_resize_mode;
    bool m_move_mode;
    bool m_moving_window;
    bool m_resizing_window;
    Point m_resize_start;
    Rectangle m_resize_start_geom;

    // Компоненты
    Panel* m_panel;
    Launcher* m_launcher;

    // Окна
    std::map<Window, Client> m_clients;
    std::map<Window, Client*> m_frame_to_client;
    Window m_focused_window;
    std::vector<Window> m_window_stack;

    // Режимы
    int m_current_workspace;
    std::unordered_set<int> m_occupied_workspaces;

    // Ресурсы
    GC m_gc;
    Cursor m_cursor_normal;
    Cursor m_cursor_move;
    Cursor m_cursor_resize;
    Cursor m_cursor_arrow;

    // Состояние
    bool m_dragging;
    Window m_drag_window;
    Point m_drag_start;
    Point m_window_start;
    int m_drag_edge;

    // Atoms
    Atom m_wm_protocols;
    Atom m_wm_delete_window;
    Atom m_wm_state;
    Atom m_net_wm_name;
    Atom m_net_wm_state;
    Atom m_net_wm_state_fullscreen;
    Atom m_net_wm_state_hidden;
    Atom m_net_wm_state_above;
    Atom m_net_wm_state_sticky;
    Atom m_net_wm_window_type;
    Atom m_net_wm_window_type_dialog;
    Atom m_utf8_string;
    Atom m_net_active_window;
    Atom m_net_supported;
    Atom m_net_current_desktop;
    Atom m_net_number_of_desktops;
    Atom m_net_wm_desktop;
    Atom m_wm_change_state;
    Atom m_wm_protocols_atom;
    Atom m_wm_take_focus;
    Atom m_net_wm_window_opacity;
    Atom m_wm_colormap_windows;

    // Утилиты
    void runCommand(const char* cmd) {
        pid_t pid = fork();
        if (pid == 0) {
            setsid();
            execl("/bin/sh", "/bin/sh", "-c", cmd, nullptr);
            _exit(0);
        } else if (pid > 0) {
            waitpid(pid, nullptr, WNOHANG);
        }
    }

    // Отрисовка красивого фона
    void drawDesktopBackground() {
        XSetWindowBackground(m_display, m_root, Config::Colors::DESKTOP_BG);
        XClearWindow(m_display, m_root);
    }

    void updateAnimations() {
        bool needs_update = false;
        for (auto& pair : m_clients) {
            Client& client = pair.second;
            if (client.updateAnimation()) {
                needs_update = true;
                XMoveResizeWindow(m_display, client.frame,
                                 client.geometry.x,
                                 client.geometry.y + Config::PANEL_HEIGHT,
                                 client.geometry.width + Config::BORDER_WIDTH * 2,
                                 client.geometry.height + Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH);
            }
        }
        if (needs_update) {
            XFlush(m_display);
        }
    }

    void updateOccupiedWorkspaces() {
        m_occupied_workspaces.clear();
        for (const auto& pair : m_clients) {
            const Client& client = pair.second;
            if (client.is_mapped && !client.is_minimized) {
                m_occupied_workspaces.insert(client.workspace);
            }
        }
        m_panel->setOccupiedWorkspaces(m_occupied_workspaces);
    }

    void handleMapRequest(XMapRequestEvent& event) {
        Client* client = findClient(event.window);
        if (!client) {
            // Создаем нового клиента
            Client new_client(event.window);

            // Получаем атрибуты окна
            XWindowAttributes attrs;
            XGetWindowAttributes(m_display, event.window, &attrs);

            new_client.geometry = Rectangle(attrs.x, attrs.y, attrs.width, attrs.height);
            new_client.saveNormalGeometry();

            // Получаем заголовок окна
            char* name = nullptr;
            XFetchName(m_display, event.window, &name);
            if (name) {
                new_client.title = name;
                XFree(name);
            }

            // Получаем класс окна
            XClassHint class_hint;
            if (XGetClassHint(m_display, event.window, &class_hint)) {
                new_client.class_name = class_hint.res_class ? class_hint.res_class : "";
                new_client.instance_name = class_hint.res_name ? class_hint.res_name : "";
                XFree(class_hint.res_name);
                XFree(class_hint.res_class);
            }

            // Добавляем в список клиентов
            m_clients[event.window] = new_client;
            client = &m_clients[event.window];

            // Создаем рамку
            createFrame(*client);
        }

        // Отображаем окно
        client->is_mapped = true;
        client->is_minimized = false;
        XMapWindow(m_display, client->frame);
        XMapWindow(m_display, client->window);

        // Фокусируем окно
        focusWindow(event.window);

        updateOccupiedWorkspaces();
    }

    void handleConfigureRequest(XConfigureRequestEvent& event) {
        Client* client = findClient(event.window);
        if (client && !client->is_fullscreen && !client->is_maximized) {
            client->geometry.x = event.x;
            client->geometry.y = event.y;
            client->geometry.width = event.width;
            client->geometry.height = event.height;

            XMoveResizeWindow(m_display, client->frame,
                            client->geometry.x,
                            client->geometry.y + Config::PANEL_HEIGHT,
                            client->geometry.width + Config::BORDER_WIDTH * 2,
                            client->geometry.height + Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH);
        }
    }

    void handleKeyRelease(XKeyEvent& event) {
        // Обработка отпускания клавиш (если нужно)
    }

    void handleButtonPress(XButtonEvent& event) {
        if (event.subwindow == m_root || event.window == m_root) {
            return;
        }

        Client* client = findClientByFrame(event.window);
        if (!client) {
            client = findClient(event.window);
        }

        if (client) {
            focusWindow(client->window);

            if (event.button == Button1) {
                // Начало перемещения
                m_dragging = true;
                m_drag_window = client->window;
                m_drag_start = Point(event.x_root, event.y_root);
                m_window_start = Point(client->geometry.x, client->geometry.y);
                XRaiseWindow(m_display, client->frame);
            } else if (event.button == Button3) {
                // Начало изменения размера
                m_resizing_window = true;
                m_drag_window = client->window;
                m_resize_start = Point(event.x_root, event.y_root);
                m_resize_start_geom = client->geometry;
            }
        }
    }

    void handleMotionNotify(XMotionEvent& event) {
        if (m_dragging && m_drag_window) {
            Client* client = findClient(m_drag_window);
            if (client) {
                int dx = event.x_root - m_drag_start.x;
                int dy = event.y_root - m_drag_start.y;

                client->geometry.x = m_window_start.x + dx;
                client->geometry.y = m_window_start.y + dy;

                XMoveWindow(m_display, client->frame,
                           client->geometry.x,
                           client->geometry.y + Config::PANEL_HEIGHT);
            }
        } else if (m_resizing_window && m_drag_window) {
            Client* client = findClient(m_drag_window);
            if (client) {
                int dx = event.x_root - m_resize_start.x;
                int dy = event.y_root - m_resize_start.y;

                client->geometry.width = std::max(100u, m_resize_start_geom.width + dx);
                client->geometry.height = std::max(100u, m_resize_start_geom.height + dy);

                XResizeWindow(m_display, client->frame,
                            client->geometry.width + Config::BORDER_WIDTH * 2,
                            client->geometry.height + Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH);
                XResizeWindow(m_display, client->window,
                            client->geometry.width,
                            client->geometry.height);
            }
        }
    }

    void handleButtonRelease(XButtonEvent& event) {
        m_dragging = false;
        m_resizing_window = false;
        m_drag_window = 0;
    }

    void handleClientMessage(XClientMessageEvent& event) {
        Client* client = findClient(event.window);
        if (!client) return;

        if (event.message_type == m_wm_protocols) {
            Atom protocol = event.data.l[0];
            if (protocol == m_wm_delete_window) {
                closeWindow(event.window);
            }
        } else if (event.message_type == m_net_wm_state) {
            // Обработка изменения состояния окна
        }
    }

    void handleDestroyNotify(XDestroyWindowEvent& event) {
        Client* client = findClient(event.window);
        if (client) {
            if (client->frame) {
                XDestroyWindow(m_display, client->frame);
                m_frame_to_client.erase(client->frame);
            }
            m_clients.erase(event.window);

            if (m_focused_window == event.window) {
                m_focused_window = 0;
                if (!m_clients.empty()) {
                    focusWindow(m_clients.begin()->first);
                }
            }

            updateOccupiedWorkspaces();
        }
    }

    void handleUnmapNotify(XUnmapEvent& event) {
        Client* client = findClient(event.window);
        if (client) {
            client->is_mapped = false;
            XUnmapWindow(m_display, client->frame);
            updateOccupiedWorkspaces();
        }
    }

    void handleExpose(XExposeEvent& event) {
        if (event.window == m_root) {
            drawDesktopBackground();
        } else {
            Client* client = findClientByFrame(event.window);
            if (client) {
                drawEnhancedTitlebar(*client);
            }
        }
    }

    void handlePropertyNotify(XPropertyEvent& event) {
        Client* client = findClient(event.window);
        if (client && event.atom == XA_WM_NAME) {
            char* name = nullptr;
            XFetchName(m_display, event.window, &name);
            if (name) {
                client->title = name;
                XFree(name);
                drawEnhancedTitlebar(*client);
            }
        }
    }

    void handleFocusIn(XFocusInEvent& event) {
        // Обработка получения фокуса
    }

    void handleFocusOut(XFocusOutEvent& event) {
        // Обработка потери фокуса
    }

    void handleEnterNotify(XCrossingEvent& event) {
        // Обработка входа мыши в окно
    }

    void handleLeaveNotify(XCrossingEvent& event) {
        // Обработка выхода мыши из окна
    }

public:
    SettlementWM()
        : m_display(nullptr), m_root(0), m_screen(0),
          m_visual(nullptr), m_colormap(0), m_running(true),
          m_resize_mode(false), m_move_mode(false),
          m_moving_window(false), m_resizing_window(false),
          m_panel(nullptr), m_launcher(nullptr),
          m_focused_window(0), m_current_workspace(0), m_gc(0),
          m_dragging(false), m_drag_window(0), m_drag_edge(0) {}

    ~SettlementWM() {
        cleanup();
    }

    bool initialize() {
        m_running = true;

        m_display = XOpenDisplay(nullptr);
        if (!m_display) {
            std::cerr << "Failed to open X display" << std::endl;
            return false;
        }

        m_screen = DefaultScreen(m_display);
        m_root = RootWindow(m_display, m_screen);
        m_visual = DefaultVisual(m_display, m_screen);
        m_colormap = DefaultColormap(m_display, m_screen);

        std::cout << "==================================" << std::endl;
        std::cout << "  " << WM_NAME << " v" << WM_VERSION << std::endl;
        std::cout << "  Elegant Window Manager" << std::endl;
        std::cout << "==================================" << std::endl;

        // Инициализируем компоненты
        drawDesktopBackground();
        m_panel = new Panel(m_display, m_root, m_visual, m_colormap, m_screen);
        m_launcher = new Launcher(m_display, m_root, m_visual, m_colormap, m_screen);

        // X11 инициализация
        initAtoms();
        createGraphicsResources();
        createCursors();
        setupGrabs();
        setSupported();

        std::cout << "Initialization complete" << std::endl;
        std::cout << "Display: " << DisplayString(m_display) << std::endl;

        return true;
    }

    void run() {
        std::cout << "\nWindow Manager running..." << std::endl;
        printHelp();

        XEvent event;

        while (m_running) {
            while (XPending(m_display) > 0) {
                XNextEvent(m_display, &event);
                handleEvent(event);
            }

            // Обновляем анимации
            updateAnimations();

            // Обновляем панель
            static time_t last_update = 0;
            time_t now = time(nullptr);
            if (now != last_update) {
                updateOccupiedWorkspaces();
                m_panel->draw();
                last_update = now;
            }

            usleep(10000);
        }

        std::cout << "Shutting down..." << std::endl;
    }

    static SettlementWM& getInstance() {
        static SettlementWM instance;
        return instance;
    }

private:
    void initAtoms() {
        m_wm_protocols = XInternAtom(m_display, "WM_PROTOCOLS", False);
        m_wm_delete_window = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
        m_wm_state = XInternAtom(m_display, "WM_STATE", False);
        m_net_wm_name = XInternAtom(m_display, "_NET_WM_NAME", False);
        m_net_wm_state = XInternAtom(m_display, "_NET_WM_STATE", False);
        m_net_wm_state_fullscreen = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);
        m_net_wm_state_hidden = XInternAtom(m_display, "_NET_WM_STATE_HIDDEN", False);
        m_net_wm_state_above = XInternAtom(m_display, "_NET_WM_STATE_ABOVE", False);
        m_net_wm_state_sticky = XInternAtom(m_display, "_NET_WM_STATE_STICKY", False);
        m_net_wm_window_type = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE", False);
        m_net_wm_window_type_dialog = XInternAtom(m_display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
        m_utf8_string = XInternAtom(m_display, "UTF8_STRING", False);
        m_net_active_window = XInternAtom(m_display, "_NET_ACTIVE_WINDOW", False);
        m_net_supported = XInternAtom(m_display, "_NET_SUPPORTED", False);
        m_net_current_desktop = XInternAtom(m_display, "_NET_CURRENT_DESKTOP", False);
        m_net_number_of_desktops = XInternAtom(m_display, "_NET_NUMBER_OF_DESKTOPS", False);
        m_net_wm_desktop = XInternAtom(m_display, "_NET_WM_DESKTOP", False);
        m_wm_change_state = XInternAtom(m_display, "WM_CHANGE_STATE", False);
        m_wm_protocols_atom = XInternAtom(m_display, "WM_PROTOCOLS", False);
        m_wm_take_focus = XInternAtom(m_display, "WM_TAKE_FOCUS", False);
        m_net_wm_window_opacity = XInternAtom(m_display, "_NET_WM_WINDOW_OPACITY", False);
        m_wm_colormap_windows = XInternAtom(m_display, "WM_COLORMAP_WINDOWS", False);
    }

    void createGraphicsResources() {
        XGCValues gcv;
        gcv.foreground = Config::Colors::DESKTOP_BG;
        gcv.background = BlackPixel(m_display, m_screen);
        gcv.line_width = 1;
        m_gc = XCreateGC(m_display, m_root, GCForeground | GCBackground | GCLineWidth, &gcv);
    }

    void createCursors() {
        m_cursor_normal = XCreateFontCursor(m_display, XC_left_ptr);
        m_cursor_move = XCreateFontCursor(m_display, XC_fleur);
        m_cursor_resize = XCreateFontCursor(m_display, XC_sizing);
        m_cursor_arrow = XCreateFontCursor(m_display, XC_arrow);

        XDefineCursor(m_display, m_root, m_cursor_normal);
    }

    void setupGrabs() {
        XUngrabKey(m_display, AnyKey, AnyModifier, m_root);

        // Регистрируем горячие клавиши
        struct KeyBinding {
            int keysym;
            unsigned int mod;
            const char* description;
        };

        KeyBinding bindings[] = {
            // Основные команды
            {XK_Return, Config::Keys::SUPER, "Super+Enter: Terminal"},
            {XK_q, Config::Keys::SUPER, "Super+Q: Close window"},
            {XK_f, Config::Keys::SUPER, "Super+F: Fullscreen"},
            {XK_m, Config::Keys::SUPER, "Super+M: Maximize"},
            {XK_n, Config::Keys::SUPER, "Super+N: Minimize"},
            {XK_d, Config::Keys::SUPER, "Super+D: Application launcher"},
            {XK_p, Config::Keys::SUPER, "Super+P: Alternative launcher"},
            {XK_space, Config::Keys::SUPER, "Super+Space: Toggle floating"},
            {XK_s, Config::Keys::SUPER, "Super+S: Toggle sticky"},
            {XK_t, Config::Keys::SUPER, "Super+T: Toggle always on top"},
            {XK_x, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+X: Kill client"},

            // Навигация
            {XK_Tab, Config::Keys::SUPER, "Super+Tab: Next window"},
            {XK_grave, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+`: Previous window"},

            // Режимы
            {XK_r, Config::Keys::SUPER, "Super+R: Resize mode"},
            {XK_v, Config::Keys::SUPER, "Super+V: Move mode"},

            // Привязка окон
            {XK_h, Config::Keys::SUPER, "Super+H: Snap left"},
            {XK_l, Config::Keys::SUPER, "Super+L: Snap right"},
            {XK_k, Config::Keys::SUPER, "Super+K: Snap top"},
            {XK_j, Config::Keys::SUPER, "Super+J: Snap bottom"},
            {XK_c, Config::Keys::SUPER, "Super+C: Center window"},

            // Навигация стрелками
            {XK_Left, Config::Keys::SUPER, "Super+Left: Move focus left"},
            {XK_Right, Config::Keys::SUPER, "Super+Right: Move focus right"},
            {XK_Up, Config::Keys::SUPER, "Super+Up: Move focus up"},
            {XK_Down, Config::Keys::SUPER, "Super+Down: Move focus down"},

            // Рабочие столы
            {XK_1, Config::Keys::SUPER, "Super+1: Workspace 1"},
            {XK_2, Config::Keys::SUPER, "Super+2: Workspace 2"},
            {XK_3, Config::Keys::SUPER, "Super+3: Workspace 3"},
            {XK_4, Config::Keys::SUPER, "Super+4: Workspace 4"},
            {XK_5, Config::Keys::SUPER, "Super+5: Workspace 5"},
            {XK_6, Config::Keys::SUPER, "Super+6: Workspace 6"},
            {XK_7, Config::Keys::SUPER, "Super+7: Workspace 7"},
            {XK_8, Config::Keys::SUPER, "Super+8: Workspace 8"},
            {XK_9, Config::Keys::SUPER, "Super+9: Workspace 9"},
            {XK_period, Config::Keys::SUPER, "Super+.: Next workspace"},
            {XK_comma, Config::Keys::SUPER, "Super+,: Previous workspace"},

            // Перемещение окон
            {XK_1, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+1: Move to WS 1"},
            {XK_2, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+2: Move to WS 2"},
            {XK_3, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+3: Move to WS 3"},
            {XK_4, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+4: Move to WS 4"},
            {XK_5, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+5: Move to WS 5"},
            {XK_6, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+6: Move to WS 6"},
            {XK_7, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+7: Move to WS 7"},
            {XK_8, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+8: Move to WS 8"},
            {XK_9, Config::Keys::SUPER | Config::Keys::SHIFT, "Super+Shift+9: Move to WS 9"},

            // Утилиты
            {XK_e, Config::Keys::SUPER, "Super+E: File manager"},
            {XK_w, Config::Keys::SUPER, "Super+W: Web browser"},
            {XK_Print, 0, "Print: Screenshot"},
            {XK_l, Config::Keys::SUPER | Config::Keys::CTRL, "Super+Ctrl+L: Lock screen"},
            {XK_c, Config::Keys::SUPER, "Super+C: Calculator"},
            {XK_comma, Config::Keys::SUPER, "Super+,: Settings"},

            // Выход
            {XK_Escape, Config::Keys::CTRL | Config::Keys::ALT, "Ctrl+Alt+Escape: Exit WM"},
        };

        std::cout << "\nRegistered Hotkeys:" << std::endl;
        for (const auto& binding : bindings) {
            KeyCode keycode = XKeysymToKeycode(m_display, binding.keysym);
            if (keycode) {
                XGrabKey(m_display, keycode, binding.mod, m_root,
                        True, GrabModeAsync, GrabModeAsync);
                std::cout << "  " << binding.description << std::endl;
            }
        }

        // Привязки мыши
        XGrabButton(m_display, Button1, Config::Keys::SUPER, m_root, True,
                   ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
                   GrabModeAsync, GrabModeAsync, None, m_cursor_move);

        XGrabButton(m_display, Button3, Config::Keys::SUPER, m_root, True,
                   ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
                   GrabModeAsync, GrabModeAsync, None, m_cursor_resize);

        XSelectInput(m_display, m_root,
                    SubstructureRedirectMask | SubstructureNotifyMask |
                    KeyPressMask | KeyReleaseMask |
                    ButtonPressMask | ButtonReleaseMask |
                    PointerMotionMask | EnterWindowMask | LeaveWindowMask |
                    FocusChangeMask | PropertyChangeMask |
                    StructureNotifyMask);
    }

    void setSupported() {
        Atom supported[] = {
            m_net_wm_name,
            m_net_wm_state,
            m_net_wm_state_fullscreen,
            m_net_wm_state_hidden,
            m_net_wm_state_above,
            m_net_wm_state_sticky,
            m_net_active_window,
            m_net_current_desktop,
            m_net_number_of_desktops,
            m_net_wm_desktop,
            m_wm_delete_window,
            m_net_wm_window_type,
            m_net_wm_window_type_dialog,
        };

        XChangeProperty(m_display, m_root, m_net_supported, XA_ATOM, 32,
                       PropModeReplace, reinterpret_cast<unsigned char*>(supported),
                       sizeof(supported) / sizeof(supported[0]));
    }

    void handleEvent(XEvent& event) {
        // Проверяем, не для лаунчера ли событие
        if (m_launcher && m_launcher->isVisible() && event.type == KeyPress) {
            m_launcher->handleKeyPress(event.xkey);
            return;
        }

        switch (event.type) {
            case MapRequest:
                handleMapRequest(event.xmaprequest);
                break;
            case ConfigureRequest:
                handleConfigureRequest(event.xconfigurerequest);
                break;
            case KeyPress:
                handleKeyPress(event.xkey);
                break;
            case KeyRelease:
                handleKeyRelease(event.xkey);
                break;
            case ButtonPress:
                handleButtonPress(event.xbutton);
                break;
            case MotionNotify:
                handleMotionNotify(event.xmotion);
                break;
            case ButtonRelease:
                handleButtonRelease(event.xbutton);
                break;
            case ClientMessage:
                handleClientMessage(event.xclient);
                break;
            case DestroyNotify:
                handleDestroyNotify(event.xdestroywindow);
                break;
            case UnmapNotify:
                handleUnmapNotify(event.xunmap);
                break;
            case Expose:
                handleExpose(event.xexpose);
                break;
            case PropertyNotify:
                handlePropertyNotify(event.xproperty);
                break;
            case FocusIn:
                handleFocusIn(event.xfocus);
                break;
            case FocusOut:
                handleFocusOut(event.xfocus);
                break;
            case EnterNotify:
                handleEnterNotify(event.xcrossing);
                break;
            case LeaveNotify:
                handleLeaveNotify(event.xcrossing);
                break;
            default:
                break;
        }
    }

    void handleKeyPress(XKeyEvent& event) {
        char buffer[256];
        KeySym keysym;
        XLookupString(&event, buffer, sizeof(buffer), &keysym, nullptr);

        bool super = (event.state & Config::Keys::SUPER);
        bool shift = (event.state & Config::Keys::SHIFT);
        bool ctrl = (event.state & Config::Keys::CTRL);
        bool alt = (event.state & Config::Keys::ALT);

        if (super) {
            switch (keysym) {
                // Основные команды
                case XK_Return:
                    runCommand("alacritty &");
                    break;

                case XK_q:
                    if (m_focused_window) {
                        closeWindow(m_focused_window);
                    }
                    break;

                case XK_f:
                    if (m_focused_window) {
                        toggleFullscreen(m_focused_window);
                    }
                    break;

                case XK_m:
                    if (m_focused_window) {
                        toggleMaximize(m_focused_window);
                    }
                    break;

                case XK_n:
                    if (m_focused_window) {
                        toggleMinimize(m_focused_window);
                    }
                    break;

                case XK_d:
                case XK_p:
                    if (m_launcher) {
                        m_launcher->show();
                    }
                    break;

                case XK_space:
                    if (m_focused_window) {
                        toggleFloating(m_focused_window);
                    }
                    break;

                case XK_s:
                    if (m_focused_window) {
                        toggleSticky(m_focused_window);
                    }
                    break;

                case XK_t:
                    if (m_focused_window) {
                        toggleAlwaysOnTop(m_focused_window);
                    }
                    break;

                case XK_c:
                    if (shift) {
                        // Центрирование окна
                        if (m_focused_window) {
                            centerWindow(m_focused_window);
                        }
                    } else {
                        // Калькулятор
                        runCommand("gnome-calculator &");
                    }
                    break;

                // Навигация
                case XK_Tab:
                    if (shift) {
                        focusPreviousWindow();
                    } else {
                        focusNextWindow();
                    }
                    break;

                case XK_grave:
                    focusPreviousWindow();
                    break;

                // Режимы
                case XK_r:
                    toggleResizeMode();
                    break;

                case XK_v:
                    toggleMoveMode();
                    break;

                // Привязка окон
                case XK_h:
                    if (m_focused_window) {
                        snapWindowLeft(m_focused_window);
                    }
                    break;

                case XK_l:
                    if (m_focused_window) {
                        snapWindowRight(m_focused_window);
                    }
                    break;

                case XK_k:
                    if (m_focused_window) {
                        snapWindowTop(m_focused_window);
                    }
                    break;

                case XK_j:
                    if (m_focused_window) {
                        snapWindowBottom(m_focused_window);
                    }
                    break;

                // Навигация стрелками
                case XK_Left:
                    if (m_focused_window) {
                        moveWindowRelative(m_focused_window, -20, 0);
                    }
                    break;

                case XK_Right:
                    if (m_focused_window) {
                        moveWindowRelative(m_focused_window, 20, 0);
                    }
                    break;

                case XK_Up:
                    if (m_focused_window) {
                        moveWindowRelative(m_focused_window, 0, -20);
                    }
                    break;

                case XK_Down:
                    if (m_focused_window) {
                        moveWindowRelative(m_focused_window, 0, 20);
                    }
                    break;

                // Рабочие столы
                case XK_1:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 0);
                    } else {
                        switchToWorkspace(0);
                    }
                    break;
                case XK_2:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 1);
                    } else {
                        switchToWorkspace(1);
                    }
                    break;
                case XK_3:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 2);
                    } else {
                        switchToWorkspace(2);
                    }
                    break;
                case XK_4:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 3);
                    } else {
                        switchToWorkspace(3);
                    }
                    break;
                case XK_5:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 4);
                    } else {
                        switchToWorkspace(4);
                    }
                    break;
                case XK_6:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 5);
                    } else {
                        switchToWorkspace(5);
                    }
                    break;
                case XK_7:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 6);
                    } else {
                        switchToWorkspace(6);
                    }
                    break;
                case XK_8:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 7);
                    } else {
                        switchToWorkspace(7);
                    }
                    break;
                case XK_9:
                    if (shift) {
                        moveWindowToWorkspace(m_focused_window, 8);
                    } else {
                        switchToWorkspace(8);
                    }
                    break;

                case XK_period:
                    nextWorkspace();
                    break;

                case XK_comma:
                    if (shift) {
                        previousWorkspace();
                    } else {
                        runCommand("gnome-control-center &");
                    }
                    break;

                // Утилиты
                case XK_e:
                    runCommand("nautilus &");
                    break;

                case XK_w:
                    runCommand("firefox &");
                    break;
            }
        }

        if (super && shift && keysym == XK_x) {
            if (m_focused_window) {
                killClient(m_focused_window);
            }
        }

        if (keysym == XK_Print) {
            runCommand("flameshot gui &");
        }

        if (super && ctrl && keysym == XK_l) {
            runCommand("i3lock -c 000000 &");
        }

        if (ctrl && alt && keysym == XK_Escape) {
            m_running = false;
        }
    }

    void createFrame(Client& client) {
        unsigned long border_color = client.getBorderColor();
        unsigned long titlebar_color = client.getTitlebarColor();

        client.frame = XCreateSimpleWindow(m_display, m_root,
                                          client.geometry.x,
                                          client.geometry.y + Config::PANEL_HEIGHT,
                                          client.geometry.width + Config::BORDER_WIDTH * 2,
                                          client.geometry.height + Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH,
                                          Config::BORDER_WIDTH,
                                          border_color,
                                          titlebar_color);

        XReparentWindow(m_display, client.window, client.frame,
                       Config::BORDER_WIDTH, Config::TITLEBAR_HEIGHT);

        m_frame_to_client[client.frame] = &client;

        XSelectInput(m_display, client.frame,
                    ExposureMask | ButtonPressMask | ButtonReleaseMask |
                    ButtonMotionMask | EnterWindowMask | LeaveWindowMask |
                    FocusChangeMask | StructureNotifyMask);

        client.cairo_surface = cairo_xlib_surface_create(
            m_display, client.frame, m_visual,
            client.geometry.width + Config::BORDER_WIDTH * 2,
            client.geometry.height + Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH);
        client.cairo_context = cairo_create(client.cairo_surface);

        // Отрисовываем красивый заголовок
        drawEnhancedTitlebar(client);
    }

    void drawEnhancedTitlebar(Client& client) {
        if (!client.frame || !client.cairo_context) return;

        int width = client.geometry.width + Config::BORDER_WIDTH * 2;
        int height = Config::TITLEBAR_HEIGHT + Config::BORDER_WIDTH;

        // Градиентный фон заголовка
        cairo_pattern_t* pattern = cairo_pattern_create_linear(0, 0, 0, height);

        if (client.has_focus) {
            cairo_pattern_add_color_stop_rgba(pattern, 0, 0.18, 0.19, 0.22, 1.0);
            cairo_pattern_add_color_stop_rgba(pattern, 1, 0.22, 0.23, 0.26, 1.0);
        } else {
            cairo_pattern_add_color_stop_rgba(pattern, 0, 0.15, 0.16, 0.19, 1.0);
            cairo_pattern_add_color_stop_rgba(pattern, 1, 0.18, 0.19, 0.22, 1.0);
        }

        cairo_set_source(client.cairo_context, pattern);
        cairo_rectangle(client.cairo_context, 0, 0, width, height);
        cairo_fill(client.cairo_context);
        cairo_pattern_destroy(pattern);

        // Легкая тень снизу
        cairo_set_source_rgba(client.cairo_context, 0, 0, 0, 0.2);
        cairo_rectangle(client.cairo_context, 0, height - 1, width, 1);
        cairo_fill(client.cairo_context);

        // Акцентная полоска сверху
        if (client.has_focus) {
            cairo_set_source_rgba(client.cairo_context,
                                0.37, 0.49, 0.89, 1.0);
            cairo_rectangle(client.cairo_context, 0, 0, width, 2);
            cairo_fill(client.cairo_context);
        }

        // Рисуем текст заголовка
        std::string title = client.title.empty() ? "Untitled" : client.title;
        if (title.length() > 40) title = title.substr(0, 37) + "...";

        // Индикаторы состояния
        std::string indicators = "";
        if (client.is_fullscreen) indicators += "🖥 ";
        if (client.is_maximized) indicators += "🗖 ";
        if (client.is_floating) indicators += "✈ ";
        if (client.is_sticky) indicators += "📌 ";
        if (client.is_always_on_top) indicators += "⬆ ";

        std::string full_title = indicators + title;

        cairo_select_font_face(client.cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              client.has_focus ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(client.cairo_context, 12);

        if (client.has_focus) {
            cairo_set_source_rgba(client.cairo_context, 0.95, 0.95, 1.0, 1.0);
        } else {
            cairo_set_source_rgba(client.cairo_context, 0.7, 0.7, 0.75, 0.9);
        }

        cairo_move_to(client.cairo_context, Config::BORDER_WIDTH + 10,
                     Config::TITLEBAR_HEIGHT / 2 + 4);
        cairo_show_text(client.cairo_context, full_title.c_str());

        // Кнопки управления
        drawEnhancedButtons(client);

        cairo_surface_flush(client.cairo_surface);
    }

    void drawEnhancedButtons(Client& client) {
        int button_size = Config::BUTTON_SIZE;
        int button_margin = Config::BUTTON_MARGIN;
        int button_y = button_margin;

        // Позиции кнопок справа
        int close_x = client.geometry.width + Config::BORDER_WIDTH - button_margin - button_size;
        int max_x = close_x - button_size - button_margin;
        int min_x = max_x - button_size - button_margin;

        // Кнопка закрытия
        drawButton(client, close_x, button_y, button_size,
                  Config::Colors::CLOSE_BUTTON, "✕");

        // Кнопка максимизации
        drawButton(client, max_x, button_y, button_size,
                  Config::Colors::MAXIMIZE_BUTTON, "🗖");

        // Кнопка сворачивания
        drawButton(client, min_x, button_y, button_size,
                  Config::Colors::MINIMIZE_BUTTON, "🗕");
    }

    void drawButton(Client& client, int x, int y, int size, unsigned long color, const char* symbol) {
        // Фон кнопки
        cairo_set_source_rgba(client.cairo_context,
                            ((color >> 16) & 0xFF) / 255.0,
                            ((color >> 8) & 0xFF) / 255.0,
                            (color & 0xFF) / 255.0,
                            0.9);
        cairo_arc(client.cairo_context, x + size/2, y + size/2, size/2 - 1, 0, 2 * M_PI);
        cairo_fill(client.cairo_context);

        // Иконка
        cairo_select_font_face(client.cairo_context, "Sans",
                              CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(client.cairo_context, size * 0.6);
        cairo_set_source_rgba(client.cairo_context, 1.0, 1.0, 1.0, 1.0);

        cairo_text_extents_t extents;
        cairo_text_extents(client.cairo_context, symbol, &extents);

        cairo_move_to(client.cairo_context,
                     x + size/2 - extents.width/2,
                     y + size/2 + extents.height/2);
        cairo_show_text(client.cairo_context, symbol);
    }

    void printHelp() {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "  " << WM_NAME << " v" << WM_VERSION << std::endl;
        std::cout << "  Elegant Window Manager with Application Launcher" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        std::cout << "\n🎯 APPLICATION LAUNCHER:" << std::endl;
        std::cout << "  Super+D/P        Open application launcher" << std::endl;
        std::cout << "  Type             Search applications by name" << std::endl;
        std::cout << "  Up/Down          Navigate results" << std::endl;
        std::cout << "  Enter            Launch selected app" << std::endl;
        std::cout << "  Escape           Close launcher" << std::endl;

        std::cout << "\n🖥️  WINDOW MANAGEMENT:" << std::endl;
        std::cout << "  Super+Enter      Terminal (Alacritty)" << std::endl;
        std::cout << "  Super+Q          Close window" << std::endl;
        std::cout << "  Super+F          Toggle fullscreen" << std::endl;
        std::cout << "  Super+M          Maximize window" << std::endl;
        std::cout << "  Super+N          Minimize window" << std::endl;
        std::cout << "  Super+Space      Toggle floating" << std::endl;
        std::cout << "  Super+S          Toggle sticky (all workspaces)" << std::endl;
        std::cout << "  Super+T          Toggle always on top" << std::endl;
        std::cout << "  Super+Shift+X    Force close window" << std::endl;

        std::cout << "\n🔍 WINDOW NAVIGATION:" << std::endl;
        std::cout << "  Super+Tab        Next window" << std::endl;
        std::cout << "  Super+Shift+`    Previous window" << std::endl;
        std::cout << "  Super+Arrow      Move window by 20px" << std::endl;

        std::cout << "\n📐 WINDOW SNAPPING:" << std::endl;
        std::cout << "  Super+H          Snap left" << std::endl;
        std::cout << "  Super+L          Snap right" << std::endl;
        std::cout << "  Super+K          Snap top" << std::endl;
        std::cout << "  Super+J          Snap bottom" << std::endl;
        std::cout << "  Super+C          Center window" << std::endl;

        std::cout << "\n🎨 MODES:" << std::endl;
        std::cout << "  Super+R          Resize mode" << std::endl;
        std::cout << "  Super+V          Move mode" << std::endl;

        std::cout << "\n🗂️  WORKSPACES (1-9):" << std::endl;
        std::cout << "  Super+1-9        Switch to workspace" << std::endl;
        std::cout << "  Super+Shift+1-9  Move window to workspace" << std::endl;
        std::cout << "  Super+,          Previous workspace" << std::endl;
        std::cout << "  Super+.          Next workspace" << std::endl;

        std::cout << "\n🔧 UTILITIES:" << std::endl;
        std::cout << "  Super+E          File manager (Nautilus)" << std::endl;
        std::cout << "  Super+W          Web browser (Firefox)" << std::endl;
        std::cout << "  Super+C          Calculator" << std::endl;
        std::cout << "  Super+,          Settings" << std::endl;
        std::cout << "  Print            Screenshot (Flameshot)" << std::endl;
        std::cout << "  Super+Ctrl+L     Lock screen" << std::endl;

        std::cout << "\n🖱️  MOUSE CONTROLS:" << std::endl;
        std::cout << "  Super+LMB        Move window" << std::endl;
        std::cout << "  Super+RMB        Resize window" << std::endl;
        std::cout << "  Super+MMB        Center window" << std::endl;
        std::cout << "  Titlebar LMB     Move window" << std::endl;
        std::cout << "  Titlebar buttons Close/Maximize/Minimize" << std::endl;

        std::cout << "\n✨ FEATURES:" << std::endl;
        std::cout << "  • Modern gradient design with animations" << std::endl;
        std::cout << "  • Application launcher with instant search" << std::endl;
        std::cout << "  • Smooth window animations" << std::endl;
        std::cout << "  • Dynamic workspace indicators" << std::endl;
        std::cout << "  • Enhanced titlebars with emoji indicators" << std::endl;
        std::cout << "  • Sticky and always-on-top windows" << std::endl;
        std::cout << "  • Window snapping and tiling" << std::endl;
        std::cout << "  • 9 workspaces with visual indicators" << std::endl;

        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Window Manager is now running..." << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }

    void cleanup() {
        for (auto& pair : m_clients) {
            Client& client = pair.second;
            if (client.frame) {
                XDestroyWindow(m_display, client.frame);
            }
        }

        m_clients.clear();
        m_frame_to_client.clear();
        m_window_stack.clear();

        delete m_panel;
        delete m_launcher;

        if (m_gc) XFreeGC(m_display, m_gc);

        XFreeCursor(m_display, m_cursor_normal);
        XFreeCursor(m_display, m_cursor_move);
        XFreeCursor(m_display, m_cursor_resize);
        XFreeCursor(m_display, m_cursor_arrow);

        if (m_display) {
            XCloseDisplay(m_display);
            m_display = nullptr;
        }
    }

    Client* findClient(Window window) {
        auto it = m_clients.find(window);
        return it != m_clients.end() ? &it->second : nullptr;
    }

    Client* findClientByFrame(Window frame) {
        auto it = m_frame_to_client.find(frame);
        return it != m_frame_to_client.end() ? it->second : nullptr;
    }

    void focusWindow(Window window) {
        if (window == m_focused_window) return;

        Client* old_client = findClient(m_focused_window);
        if (old_client) {
            old_client->has_focus = false;
            drawEnhancedTitlebar(*old_client);
        }

        m_focused_window = window;

        Client* new_client = findClient(window);
        if (new_client) {
            new_client->has_focus = true;
            XRaiseWindow(m_display, new_client->frame);
            XSetInputFocus(m_display, window, RevertToParent, CurrentTime);
            drawEnhancedTitlebar(*new_client);

            auto it = std::find(m_window_stack.begin(), m_window_stack.end(), window);
            if (it != m_window_stack.end()) {
                m_window_stack.erase(it);
            }
            m_window_stack.insert(m_window_stack.begin(), window);
        }
    }

    void closeWindow(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        XEvent msg;
        memset(&msg, 0, sizeof(msg));
        msg.xclient.type = ClientMessage;
        msg.xclient.window = window;
        msg.xclient.message_type = m_wm_protocols;
        msg.xclient.format = 32;
        msg.xclient.data.l[0] = m_wm_delete_window;
        msg.xclient.data.l[1] = CurrentTime;

        XSendEvent(m_display, window, False, NoEventMask, &msg);
        XFlush(m_display);
    }

    // Реализация недостающих методов

    void toggleFullscreen(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_fullscreen = !client->is_fullscreen;
        if (client->is_fullscreen) {
            client->saveNormalGeometry();

            int screen_width = DisplayWidth(m_display, m_screen);
            int screen_height = DisplayHeight(m_display, m_screen);

            client->startAnimation(Point(0, 0),
                                   Size(screen_width, screen_height - Config::PANEL_HEIGHT));
        } else {
            client->restoreNormalGeometry();
            client->startAnimation(Point(client->normal_geometry.x, client->normal_geometry.y),
                                   Size(client->normal_geometry.width, client->normal_geometry.height));
        }
    }

    void toggleMaximize(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_maximized = !client->is_maximized;
        if (client->is_maximized) {
            client->saveNormalGeometry();

            int screen_width = DisplayWidth(m_display, m_screen);
            int screen_height = DisplayHeight(m_display, m_screen);

            client->startAnimation(Point(0, 0),
                                   Size(screen_width, screen_height - Config::PANEL_HEIGHT));
        } else {
            client->restoreNormalGeometry();
            client->startAnimation(Point(client->normal_geometry.x, client->normal_geometry.y),
                                   Size(client->normal_geometry.width, client->normal_geometry.height));
        }
    }

    void toggleMinimize(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_minimized = !client->is_minimized;
        if (client->is_minimized) {
            XUnmapWindow(m_display, client->frame);
            XUnmapWindow(m_display, client->window);
        } else {
            XMapWindow(m_display, client->frame);
            XMapWindow(m_display, client->window);
            focusWindow(window);
        }
        updateOccupiedWorkspaces();
    }

    void toggleFloating(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_floating = !client->is_floating;
    }

    void toggleSticky(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_sticky = !client->is_sticky;
    }

    void toggleAlwaysOnTop(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        client->is_always_on_top = !client->is_always_on_top;
    }

    void centerWindow(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        int screen_width = DisplayWidth(m_display, m_screen);
        int screen_height = DisplayHeight(m_display, m_screen);

        Rectangle centered = client->geometry.centered(screen_width, screen_height);
        client->startAnimation(Point(centered.x, centered.y),
                               Size(centered.width, centered.height));
    }

    void focusPreviousWindow() {
        if (m_window_stack.size() > 1) {
            Window prev = m_window_stack[1];
            focusWindow(prev);
        }
    }

    void focusNextWindow() {
        if (!m_window_stack.empty()) {
            // Ищем текущее окно в карте клиентов
            Window current = m_window_stack[0];

            // Находим итератор на текущее окно
            auto it = m_clients.find(current);
            if (it != m_clients.end()) {
                // Переходим к следующему окну
                ++it;
                if (it == m_clients.end()) {
                    // Если достигли конца, переходим к началу
                    it = m_clients.begin();
                }
                focusWindow(it->first);
            } else {
                // Если текущего окна нет в карте, фокусируем первое окно
                if (!m_clients.empty()) {
                    focusWindow(m_clients.begin()->first);
                }
            }
        } else {
            // Если стек пуст, фокусируем первое окно
            if (!m_clients.empty()) {
                focusWindow(m_clients.begin()->first);
            }
        }
    }

    void toggleResizeMode() {
        m_resize_mode = !m_resize_mode;
        if (m_resize_mode) {
            XDefineCursor(m_display, m_root, m_cursor_resize);
        } else {
            XDefineCursor(m_display, m_root, m_cursor_normal);
        }
    }

    void toggleMoveMode() {
        m_move_mode = !m_move_mode;
        if (m_move_mode) {
            XDefineCursor(m_display, m_root, m_cursor_move);
        } else {
            XDefineCursor(m_display, m_root, m_cursor_normal);
        }
    }

    void snapWindowLeft(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        int screen_width = DisplayWidth(m_display, m_screen);
        Rectangle snapped = client->geometry.snapped_left(screen_width);
        client->startAnimation(Point(snapped.x, snapped.y),
                               Size(snapped.width, snapped.height));
    }

    void snapWindowRight(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        int screen_width = DisplayWidth(m_display, m_screen);
        Rectangle snapped = client->geometry.snapped_right(screen_width);
        client->startAnimation(Point(snapped.x, snapped.y),
                               Size(snapped.width, snapped.height));
    }

    void snapWindowTop(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        int screen_height = DisplayHeight(m_display, m_screen);
        Rectangle snapped = client->geometry.snapped_top(screen_height);
        client->startAnimation(Point(snapped.x, snapped.y),
                               Size(snapped.width, snapped.height));
    }

    void snapWindowBottom(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        int screen_height = DisplayHeight(m_display, m_screen);
        Rectangle snapped = client->geometry.snapped_bottom(screen_height);
        client->startAnimation(Point(snapped.x, snapped.y),
                               Size(snapped.width, snapped.height));
    }

    void moveWindowRelative(Window window, int dx, int dy) {
        Client* client = findClient(window);
        if (!client) return;

        Point new_pos = Point(client->geometry.x + dx, client->geometry.y + dy);
        client->startAnimation(new_pos, Size(client->geometry.width, client->geometry.height));
    }

    void moveWindowToWorkspace(Window window, int workspace) {
        Client* client = findClient(window);
        if (!client) return;

        client->workspace = static_cast<unsigned int>(workspace);
        if (client->workspace != static_cast<unsigned int>(m_current_workspace)) {
            XUnmapWindow(m_display, client->frame);
            XUnmapWindow(m_display, client->window);
        }
        updateOccupiedWorkspaces();
    }

    void switchToWorkspace(int workspace) {
        if (workspace >= 0 && workspace < Config::WORKSPACE_COUNT) {
            m_current_workspace = workspace;
            m_panel->setCurrentWorkspace(workspace);

            for (auto& pair : m_clients) {
                Client& client = pair.second;
                if (client.shouldShowInWorkspace(static_cast<unsigned int>(workspace))) {
                    XMapWindow(m_display, client.frame);
                    XMapWindow(m_display, client.window);
                } else {
                    XUnmapWindow(m_display, client.frame);
                    XUnmapWindow(m_display, client.window);
                }
            }
        }
    }

    void nextWorkspace() {
        int next = (m_current_workspace + 1) % Config::WORKSPACE_COUNT;
        switchToWorkspace(next);
    }

    void previousWorkspace() {
        int prev = (m_current_workspace - 1 + Config::WORKSPACE_COUNT) % Config::WORKSPACE_COUNT;
        switchToWorkspace(prev);
    }

    void killClient(Window window) {
        Client* client = findClient(window);
        if (!client) return;

        XKillClient(m_display, window);
        m_clients.erase(window);
        if (m_focused_window == window) {
            m_focused_window = 0;
        }
        updateOccupiedWorkspaces();
    }
};

// ============================================================================
// ТОЧКА ВХОДА
// ============================================================================

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "Starting " << WM_NAME << " v" << WM_VERSION << "..." << std::endl;

    SettlementWM& wm = SettlementWM::getInstance();

    if (!wm.initialize()) {
        std::cerr << "Failed to initialize window manager" << std::endl;
        return 1;
    }

    try {
        wm.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << WM_NAME << " stopped successfully." << std::endl;
    return 0;
}