// Local copy of window.cpp with MY355-specific fix:
// On MY355 the d-pad emits both SDL_KEYDOWN and SDL_JOYHATMOTION for the same
// physical press. Processing both causes double movement, so we skip joystick
// events on that platform since the keyboard events already handle everything.

#include <iostream>
#include "window.h"
#include "def.h"
#include "sdlutils.h"

#define KEYHOLD_TIMER_FIRST   6
#define KEYHOLD_TIMER         2

CWindow::CWindow(void):
    m_timer(0),
    m_lastPressed(SDLK_0),
    m_retVal(0)
{
    Globals::g_windows.push_back(this);
}

CWindow::~CWindow(void)
{
    Globals::g_windows.pop_back();
}

const int CWindow::execute(void)
{
    m_retVal = 0;
    Uint32 l_time(0);
    SDL_Event l_event;
    bool l_loop(true);
    bool l_render(true);
    while (l_loop)
    {
        l_time = SDL_GetTicks();
        while (SDL_PollEvent(&l_event))
        {
            if (l_event.type == SDL_KEYDOWN)
            {
                l_event.key.keysym.sym = l_event.key.keysym.sym == MYKEY_MENU ? MYKEY_SYSTEM : l_event.key.keysym.sym;
                l_render = this->keyPress(l_event);
                if (m_retVal)
                    l_loop = false;
            }
            else if (l_event.type == SDL_QUIT)
            {
                SDL_PushEvent(&l_event);
                l_loop = false;
                break;
            }
#if !defined(PLATFORM_MY355)
            // MY355 maps all inputs to keyboard events — skip joystick to avoid double movement
            else if (l_event.type == SDL_JOYBUTTONDOWN)
            {
                SDL_Event key_event;
                key_event.key.keysym.sym = l_event.jbutton.button == MYKEY_MENU ? MYKEY_SYSTEM : l_event.jbutton.button;
                l_render = this->keyPress(key_event);
                l_render = true;
                if (m_retVal)
                    l_loop = false;
            }
            else if (l_event.type == SDL_JOYHATMOTION)
            {
                int hat = l_event.jhat.value;
                SDL_Event key_event;
                key_event.key.keysym.sym = 0;
                if (hat == SDL_HAT_UP)    key_event.key.keysym.sym = MYKEY_UP;
                if (hat == SDL_HAT_DOWN)  key_event.key.keysym.sym = MYKEY_DOWN;
                if (hat == SDL_HAT_LEFT)  key_event.key.keysym.sym = MYKEY_LEFT;
                if (hat == SDL_HAT_RIGHT) key_event.key.keysym.sym = MYKEY_RIGHT;
                if (key_event.key.keysym.sym)
                {
                    l_render = this->keyPress(key_event);
                    l_render = true;
                    if (m_retVal)
                        l_loop = false;
                }
            }
#endif
        }
        if (l_loop)
            l_render = this->keyHold() || l_render;
        if (l_render && l_loop)
        {
            SDL_utils::renderAll();
            SDL_UpdateWindowSurface(Globals::g_sdlwindow);
            SDL_UpdateWindowSurface(Globals::g_sdlwindow);
            l_render = false;
        }
        l_time = MS_PER_FRAME - (SDL_GetTicks() - l_time);
        if (l_time <= MS_PER_FRAME) SDL_Delay(l_time);
    }
    return m_retVal;
}

const bool CWindow::keyPress(const SDL_Event &p_event)
{
    if (m_timer) m_timer = 0;
    m_lastPressed = p_event.key.keysym.sym;
    return false;
}

const bool CWindow::keyHold(void)
{
    return false;
}

const bool CWindow::tick(const Uint8 p_held)
{
    bool l_ret(false);
    if (p_held)
    {
        if (m_timer)
        {
            --m_timer;
            if (!m_timer)
            {
                l_ret = true;
                m_timer = KEYHOLD_TIMER;
            }
        }
        else
        {
            m_timer = KEYHOLD_TIMER_FIRST;
        }
    }
    else
    {
        if (m_timer) m_timer = 0;
    }
    return l_ret;
}

const int CWindow::getReturnValue(void) const
{
    return m_retVal;
}

const bool CWindow::isFullScreen(void) const
{
    return false;
}
