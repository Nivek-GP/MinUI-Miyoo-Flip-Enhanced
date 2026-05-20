// Simplified ResourceManager for colmgr — loads the font only, no icon images.
#include "resourceManager.h"
#include "sdlutils.h"
#include "def.h"

CResourceManager& CResourceManager::instance() {
    static CResourceManager l_singleton;
    return l_singleton;
}

CResourceManager::CResourceManager() : m_font(NULL) {
    for (int i = 0; i < NB_SURFACES; i++) m_surfaces[i] = NULL;
    m_font = SDL_utils::loadFont(RESDIR "/Fiery_Turk.ttf", 8);
}

void CResourceManager::sdlCleanup() {
    for (int i = 0; i < NB_SURFACES; i++) {
        if (m_surfaces[i]) { SDL_FreeSurface(m_surfaces[i]); m_surfaces[i] = NULL; }
    }
    if (m_font) { TTF_CloseFont(m_font); m_font = NULL; }
}

SDL_Surface* CResourceManager::getSurface(const T_SURFACE p_surface) const {
    return m_surfaces[p_surface];
}

TTF_Font* CResourceManager::getFont() const {
    return m_font;
}
