#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <unistd.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "def.h"
#include "sdlutils.h"
#include "resourceManager.h"
#include "window.h"
#include "keyboard.h"

// ---------------------------------------------------------------------------
// Globals required by the DinguxCommander framework
// ---------------------------------------------------------------------------
SDL_Surface           *Globals::g_screen      = NULL;
SDL_Surface           *Globals::g_screen_real = NULL;
SDL_Window            *Globals::g_sdlwindow   = NULL;
SDL_Joystick          *Globals::g_joy         = NULL;
const SDL_Color        Globals::g_colorTextNormal   = {COLOR_TEXT_NORMAL};
const SDL_Color        Globals::g_colorTextTitle    = {COLOR_TEXT_TITLE};
const SDL_Color        Globals::g_colorTextDir      = {COLOR_TEXT_DIR};
const SDL_Color        Globals::g_colorTextSelected = {COLOR_TEXT_SELECTED};
std::vector<CWindow *> Globals::g_windows;

// ---------------------------------------------------------------------------
// Runtime paths (filled in main)
// ---------------------------------------------------------------------------
static std::string g_sdcard;
static std::string g_collectionsPath;
static std::string g_romsPath;

// ---------------------------------------------------------------------------
// Drawing helpers
// ---------------------------------------------------------------------------
static void drawHeaderBar(const std::string &title) {
    SDL_FillRect(Globals::g_screen, NULL,
        SDL_MapRGB(Globals::g_screen->format, COLOR_BG_2));
    SDL_Rect bar = {0, 0, SCREEN_WIDTH, LINE_HEIGHT + 2};
    SDL_FillRect(Globals::g_screen, &bar,
        SDL_MapRGB(Globals::g_screen->format, COLOR_BORDER));
    SDL_utils::applyText(SCREEN_WIDTH / 2, Y_HEADER, Globals::g_screen,
        CResourceManager::instance().getFont(), title,
        Globals::g_colorTextTitle, SDL_utils::T_TEXT_ALIGN_CENTER);
}

static void drawFooterBar(const std::string &hint) {
    SDL_Rect bar = {0, Y_FOOTER, SCREEN_WIDTH, H_FOOTER};
    SDL_FillRect(Globals::g_screen, &bar,
        SDL_MapRGB(Globals::g_screen->format, COLOR_BORDER));
    SDL_utils::applyText(SCREEN_WIDTH / 2, Y_FOOTER + 1, Globals::g_screen,
        CResourceManager::instance().getFont(), hint,
        Globals::g_colorTextTitle, SDL_utils::T_TEXT_ALIGN_CENTER);
}

static void drawListItems(const std::vector<std::string> &items, int cursor, int scroll) {
    TTF_Font *font = CResourceManager::instance().getFont();
    int visLines = NB_VISIBLE_LINES - 2;
    for (int i = 0; i < visLines && (scroll + i) < (int)items.size(); i++) {
        int idx = scroll + i;
        int y = Y_LIST + i * LINE_HEIGHT;
        if (idx == cursor) {
            SDL_Rect cur = {0, y, SCREEN_WIDTH, LINE_HEIGHT};
            SDL_FillRect(Globals::g_screen, &cur,
                SDL_MapRGB(Globals::g_screen->format, COLOR_CURSOR_1));
        }
        SDL_utils::applyText(4, y + 1, Globals::g_screen, font,
            items[idx], Globals::g_colorTextNormal);
    }
}

static bool pathIsDir(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

static std::string lastComponent(const std::string &path) {
    size_t pos = path.rfind('/');
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

// ---------------------------------------------------------------------------
// ROM Browser — lets the user navigate Roms/ and pick a file
// ---------------------------------------------------------------------------
class CRomBrowser : public CWindow {
public:
    std::string m_result; // path relative to sdcard root (e.g. "/Roms/GBA/game.gba")

    explicit CRomBrowser(const std::string &startPath)
        : CWindow(), m_path(startPath), m_cursor(0), m_scroll(0) {
        loadDir();
    }

    virtual const bool isFullScreen() const { return true; }

    virtual void render(const bool) const {
        std::string title = (m_path == g_romsPath) ? "Add Game" : lastComponent(m_path);
        drawHeaderBar(title);
        drawFooterBar("A:Select  B:Back");
        drawListItems(m_display, m_cursor, m_scroll);
    }

private:
    std::string              m_path;
    std::vector<std::string> m_fullPaths;
    std::vector<std::string> m_display;
    std::vector<bool>        m_isDir;
    int m_cursor;
    int m_scroll;

    void loadDir() {
        m_fullPaths.clear(); m_display.clear(); m_isDir.clear();
        m_cursor = 0; m_scroll = 0;

        if (m_path != g_romsPath) {
            std::string parent = m_path.substr(0, m_path.rfind('/'));
            m_fullPaths.push_back(parent);
            m_display.push_back("..");
            m_isDir.push_back(true);
        }

        DIR *dir = opendir(m_path.c_str());
        if (!dir) return;

        std::vector<std::string> dirs, files;
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL) {
            std::string name = dp->d_name;
            if (name[0] == '.') continue;
            std::string full = m_path + "/" + name;
            if (pathIsDir(full)) dirs.push_back(name);
            else                 files.push_back(name);
        }
        closedir(dir);
        std::sort(dirs.begin(), dirs.end());
        std::sort(files.begin(), files.end());

        // Collect filenames referenced inside .cue files (internal disc tracks)
        std::set<std::string> cueReferenced;
        for (auto &f : files) {
            if (f.size() <= 4) continue;
            std::string ext = f.substr(f.size() - 4);
            for (char &c : ext) c = tolower(c);
            if (ext != ".cue") continue;
            std::ifstream cueFile(m_path + "/" + f);
            std::string line;
            while (std::getline(cueFile, line)) {
                // CUE syntax: FILE "filename.bin" BINARY
                size_t q1 = line.find('"');
                size_t q2 = line.rfind('"');
                if (q1 != std::string::npos && q2 != std::string::npos && q1 != q2)
                    cueReferenced.insert(line.substr(q1 + 1, q2 - q1 - 1));
            }
        }

        for (auto &d : dirs)  { m_fullPaths.push_back(m_path+"/"+d); m_display.push_back(d+"/"); m_isDir.push_back(true); }
        for (auto &f : files) {
            if (cueReferenced.count(f)) continue; // hide tracks referenced by a .cue
            if (f.size() > 4) {
                std::string ext = f.substr(f.size() - 4);
                for (char &c : ext) c = tolower(c);
                if (ext == ".sub") continue; // hide PSX subchannel data files
            }
            m_fullPaths.push_back(m_path+"/"+f); m_display.push_back(f); m_isDir.push_back(false);
        }
    }

    void moveCursor(int delta) {
        int n = (int)m_fullPaths.size();
        int vis = NB_VISIBLE_LINES - 2;
        m_cursor = std::max(0, std::min(n > 0 ? n - 1 : 0, m_cursor + delta));
        if (m_cursor < m_scroll)            m_scroll = m_cursor;
        if (m_cursor >= m_scroll + vis)     m_scroll = m_cursor - vis + 1;
    }

    virtual const bool keyPress(const SDL_Event &ev) {
        CWindow::keyPress(ev);
        switch (ev.key.keysym.sym) {
            case MYKEY_UP:   moveCursor(-1); return true;
            case MYKEY_DOWN: moveCursor(+1); return true;
            case MYKEY_OPEN:
                if (!m_fullPaths.empty()) {
                    if (m_isDir[m_cursor]) {
                        m_path = m_fullPaths[m_cursor];
                        loadDir();
                    } else {
                        m_result  = m_fullPaths[m_cursor].substr(g_sdcard.size());
                        m_retVal  = 1;
                    }
                    return true;
                }
                break;
            case MYKEY_PARENT:
                if (m_path != g_romsPath) {
                    m_path = m_path.substr(0, m_path.rfind('/'));
                    loadDir();
                } else {
                    m_retVal = -1;
                }
                return true;
        }
        return false;
    }

    virtual const bool keyHold() {
        switch (m_lastPressed) {
            case MYKEY_UP:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_UP)]))
                    { moveCursor(-1); return true; }
                break;
            case MYKEY_DOWN:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_DOWN)]))
                    { moveCursor(+1); return true; }
                break;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// Collection View — shows the contents of one .txt collection file
// ---------------------------------------------------------------------------
class CCollectionView : public CWindow {
public:
    CCollectionView(const std::string &filepath, const std::string &name)
        : CWindow(), m_filepath(filepath), m_name(name),
          m_cursor(0), m_scroll(0), m_pendingDelete(false) {
        loadItems();
    }

    virtual const bool isFullScreen() const { return true; }

    virtual void render(const bool) const {
        drawHeaderBar(m_name + " (" + std::to_string((int)m_items.size()) + ")");
        std::string footer;
        if (m_pendingDelete)                      footer = "Y:Confirm  B:Cancel";
        else if (m_cursor == (int)m_items.size()) footer = "A:Add  B:Back";
        else                                      footer = "B:Back  Y:Remove";
        drawFooterBar(footer);

        std::vector<std::string> display;
        for (auto &it : m_items) display.push_back(lastComponent(it));
        display.push_back("[+ Add Game]");
        drawListItems(display, m_cursor, m_scroll);
    }

private:
    std::string              m_filepath;
    std::string              m_name;
    std::vector<std::string> m_items;
    int  m_cursor;
    int  m_scroll;
    bool m_pendingDelete;

    void loadItems() {
        m_items.clear(); m_pendingDelete = false;
        std::ifstream f(m_filepath);
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (!line.empty()) m_items.push_back(line);
        }
    }

    void saveItems() {
        std::ofstream f(m_filepath);
        for (auto &it : m_items) f << it << "\n";
    }

    void moveCursor(int delta) {
        int n   = (int)m_items.size() + 1; // +1 for "Add Game"
        int vis = NB_VISIBLE_LINES - 2;
        m_cursor = std::max(0, std::min(n - 1, m_cursor + delta));
        if (m_cursor < m_scroll)        m_scroll = m_cursor;
        if (m_cursor >= m_scroll + vis) m_scroll = m_cursor - vis + 1;
    }

    virtual const bool keyPress(const SDL_Event &ev) {
        CWindow::keyPress(ev);

        if (m_pendingDelete) {
            if (ev.key.keysym.sym == MYKEY_SYSTEM) { // Y: confirm remove
                m_items.erase(m_items.begin() + m_cursor);
                saveItems();
                int n = (int)m_items.size() + 1;
                if (m_cursor >= n) m_cursor = n - 1;
                if (m_cursor < m_scroll) m_scroll = m_cursor;
                m_pendingDelete = false;
                return true;
            }
            if (ev.key.keysym.sym == MYKEY_PARENT) { // B: cancel
                m_pendingDelete = false;
                return true;
            }
            return false;
        }

        switch (ev.key.keysym.sym) {
            case MYKEY_UP:   moveCursor(-1); return true;
            case MYKEY_DOWN: moveCursor(+1); return true;
            case MYKEY_OPEN: // A — only acts on the "Add Game" entry
                if (m_cursor == (int)m_items.size()) {
                    CRomBrowser browser(g_romsPath);
                    if (browser.execute() == 1 && !browser.m_result.empty()) {
                        m_items.push_back(browser.m_result);
                        saveItems();
                        moveCursor(0);
                    }
                    return true;
                }
                break;
            case MYKEY_PARENT: // B: go back
                m_retVal = -1;
                return true;
            case MYKEY_SYSTEM: // Y: begin remove
                if (m_cursor < (int)m_items.size()) {
                    m_pendingDelete = true;
                    return true;
                }
                break;
        }
        return false;
    }

    virtual const bool keyHold() {
        if (m_pendingDelete) return false;
        switch (m_lastPressed) {
            case MYKEY_UP:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_UP)]))
                    { moveCursor(-1); return true; }
                break;
            case MYKEY_DOWN:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_DOWN)]))
                    { moveCursor(+1); return true; }
                break;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// Collection List — main screen listing all .txt files in /Collections/
// ---------------------------------------------------------------------------
class CCollectionList : public CWindow {
public:
    CCollectionList()
        : CWindow(), m_cursor(0), m_scroll(0), m_pendingDelete(false) {
        loadCollections();
    }

    virtual const bool isFullScreen() const { return true; }

    virtual void render(const bool) const {
        drawHeaderBar("Collections");
        std::string footer;
        if (m_pendingDelete)       footer = "Y:Confirm  B:Cancel";
        else if (m_cols.empty())   footer = "A:New  B:Exit";
        else                       footer = "A:Open  X:Rename  B:Exit  Y:Delete";
        drawFooterBar(footer);

        std::vector<std::string> display;
        for (auto &c : m_cols) display.push_back(c.name);
        display.push_back("[+ New Collection]");
        drawListItems(display, m_cursor, m_scroll);
    }

private:
    struct Col { std::string name; std::string path; };
    std::vector<Col> m_cols;
    int  m_cursor;
    int  m_scroll;
    bool m_pendingDelete;

    void loadCollections() {
        m_cols.clear(); m_pendingDelete = false;
        DIR *dir = opendir(g_collectionsPath.c_str());
        if (!dir) return;
        std::vector<std::string> names;
        struct dirent *dp;
        while ((dp = readdir(dir)) != NULL) {
            std::string n = dp->d_name;
            if (n[0] == '.') continue; // skip hidden files and macOS ._metadata files
            if (n.size() > 4 && n.substr(n.size() - 4) == ".txt") names.push_back(n);
        }
        closedir(dir);
        std::sort(names.begin(), names.end());
        for (auto &n : names)
            m_cols.push_back({n.substr(0, n.size() - 4), g_collectionsPath + "/" + n});
    }

    void moveCursor(int delta) {
        int n   = (int)m_cols.size() + 1; // +1 for "New Collection"
        int vis = NB_VISIBLE_LINES - 2;
        m_cursor = std::max(0, std::min(n - 1, m_cursor + delta));
        if (m_cursor < m_scroll)        m_scroll = m_cursor;
        if (m_cursor >= m_scroll + vis) m_scroll = m_cursor - vis + 1;
    }

    virtual const bool keyPress(const SDL_Event &ev) {
        CWindow::keyPress(ev);

        if (m_pendingDelete) {
            if (ev.key.keysym.sym == MYKEY_SYSTEM) { // Y: confirm delete
                unlink(m_cols[m_cursor].path.c_str());
                int savedCursor = m_cursor;
                loadCollections();
                m_cursor = std::min(savedCursor, (int)m_cols.size());
                if (m_cursor < m_scroll) m_scroll = m_cursor;
                return true;
            }
            if (ev.key.keysym.sym == MYKEY_PARENT) { // B: cancel
                m_pendingDelete = false;
                return true;
            }
            return false;
        }

        switch (ev.key.keysym.sym) {
            case MYKEY_UP:   moveCursor(-1); return true;
            case MYKEY_DOWN: moveCursor(+1); return true;
            case MYKEY_OPEN: { // A
                if (m_cursor == (int)m_cols.size()) {
                    // New collection: open keyboard to get name
                    CKeyboard kb("Collection " + std::to_string(m_cols.size() + 1));
                    if (kb.execute() == 1 && !kb.getInputText().empty()) {
                        std::string name = kb.getInputText();
                        for (char &c : name)
                            if (c=='/'||c=='\\'||c==':'||c=='*'||c=='?'||c=='"'||c=='<'||c=='>'||c=='|')
                                c = '-';
                        std::ofstream(g_collectionsPath + "/" + name + ".txt");
                        int savedCursor = m_cursor;
                        loadCollections();
                        // Position cursor on the newly created collection
                        m_cursor = savedCursor; // start from where we were
                        for (int i = 0; i < (int)m_cols.size(); i++)
                            if (m_cols[i].name == name) { m_cursor = i; break; }
                        int vis = NB_VISIBLE_LINES - 2;
                        if (m_cursor < m_scroll)        m_scroll = m_cursor;
                        if (m_cursor >= m_scroll + vis) m_scroll = m_cursor - vis + 1;
                    }
                    return true;
                } else {
                    // Open existing collection
                    CCollectionView view(m_cols[m_cursor].path, m_cols[m_cursor].name);
                    view.execute();
                    int savedCursor = m_cursor;
                    loadCollections();
                    m_cursor = std::min(savedCursor, (int)m_cols.size());
                    if (m_cursor < m_scroll) m_scroll = m_cursor;
                    return true;
                }
            }
            case MYKEY_PARENT: // B: exit app
                m_retVal = -1;
                return true;
            case MYKEY_OPERATION: // X: rename collection
                if (m_cursor < (int)m_cols.size()) {
                    CKeyboard kb(m_cols[m_cursor].name);
                    if (kb.execute() == 1 && !kb.getInputText().empty()) {
                        std::string newName = kb.getInputText();
                        for (char &c : newName)
                            if (c=='/'||c=='\\'||c==':'||c=='*'||c=='?'||c=='"'||c=='<'||c=='>'||c=='|')
                                c = '-';
                        std::string newPath = g_collectionsPath + "/" + newName + ".txt";
                        rename(m_cols[m_cursor].path.c_str(), newPath.c_str());
                        int savedCursor = m_cursor;
                        loadCollections();
                        m_cursor = savedCursor;
                        for (int i = 0; i < (int)m_cols.size(); i++)
                            if (m_cols[i].name == newName) { m_cursor = i; break; }
                        int vis = NB_VISIBLE_LINES - 2;
                        if (m_cursor < m_scroll)        m_scroll = m_cursor;
                        if (m_cursor >= m_scroll + vis) m_scroll = m_cursor - vis + 1;
                    }
                    return true;
                }
                break;
            case MYKEY_SYSTEM: // Y: begin delete
                if (m_cursor < (int)m_cols.size()) {
                    m_pendingDelete = true;
                    return true;
                }
                break;
        }
        return false;
    }

    virtual const bool keyHold() {
        if (m_pendingDelete) return false;
        switch (m_lastPressed) {
            case MYKEY_UP:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_UP)]))
                    { moveCursor(-1); return true; }
                break;
            case MYKEY_DOWN:
                if (tick(SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(MYKEY_DOWN)]))
                    { moveCursor(+1); return true; }
                break;
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char **argv) {
    const char *sdcard  = getenv("SDCARD_PATH");
    g_sdcard            = sdcard ? sdcard : "/mnt/SDCARD";
    g_collectionsPath   = g_sdcard + "/Collections";
    g_romsPath          = g_sdcard + "/Roms";
    mkdir(g_collectionsPath.c_str(), 0755);

    { char s[] = "SDL_NOMOUSE=1"; putenv(s); }
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    Globals::g_sdlwindow = SDL_CreateWindow("Collections",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    Globals::g_screen_real = SDL_GetWindowSurface(Globals::g_sdlwindow);

    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask=0xff000000; gmask=0x00ff0000; bmask=0x0000ff00; amask=0x000000ff;
#else
    rmask=0x000000ff; gmask=0x0000ff00; bmask=0x00ff0000; amask=0xff000000;
#endif
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    Globals::g_screen = SDL_CreateRGBSurface(0, 320, 240, 32, rmask, gmask, bmask, amask);

    if (SDL_NumJoysticks() > 0)
        Globals::g_joy = SDL_JoystickOpen(0);
    SDL_ShowCursor(SDL_DISABLE);

    if (TTF_Init() == -1) return 1;
    CResourceManager::instance();

    {
        CCollectionList list;
        list.execute();
    }

    if (Globals::g_joy && SDL_JoystickGetAttached(Globals::g_joy))
        SDL_JoystickClose(Globals::g_joy);

    CResourceManager::instance().sdlCleanup();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
