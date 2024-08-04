/*********************************************
*WINMAIN.CPP,                                *
*  Copyright © 2004, Jonathan Bradley Whited.*
*********************************************/
/*Content.                                  */
/*  Inclusions:        1.                   */
/*  WinMain.                                */
/*  Member functions:  1.                   */
/*                                          */
/*Purpose.                                  */
/*  EkoScape Version 1.0.                   */
/*                                          */
/*Thanks...                                 */
/*  to Ryan Witmer at                       */
/*    http://www.averagesoftware.com for    */
/*    making the Dantares engine.           */
/*  to http://www.gamehippo.com for all of  */
/*    those free games!                     */
/*  to Jeff Molofee at                      */
/*    http://www.nehe.gamedev.net for making*/
/*    those OpenGL tutorials!               */
/*  to you!  You must have a lot of time on */
/*    your hands to even read this!         */

//////////////////////////////////////////////
//This is for compilers that don't define   //
//these macros.                             //
//////////////////////////////////////////////
//#define __cplusplus
#define _WINDOWS

/***********
*Inclusions*
***********/
#include "WinMain.h"

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
    try
    {
        //Create variables
        WINMAIN::CWINMAIN CWinMain;

        //Create everything
        if(!CWinMain.Create(32,32,768,1024))
        {
            ERROR_MESSAGEBOX;

            return(0);
        }

        return(CWinMain.MessageLoop());
    }

    catch(...)
    {
        ERROR_MESSAGEBOX;

        return(0);
    }
}

/****************************
*Member function definitions*
****************************/
inline bool WINMAIN::CWINMAIN::BackgroundProcessing()
{
    //Introduction
    if(m_ulState == CWMS_INTRODUCTION)
    {
        //Draw m_uiTextureIntroduction
        glBindTexture(GL_TEXTURE_2D,m_uiTextureIntroduction);

        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex2f(-1,-1);
        glTexCoord2f(1,0);
        glVertex2f(1,-1);
        glTexCoord2f(1,1);
        glVertex2f(1,1);
        glTexCoord2f(0,1);
        glVertex2f(-1,1);
        glEnd();

        //Check for enter/return/space
        if(KEYDOWN(DIK_RETURN) || KEYDOWN(DIK_SPACE))
        {
            //Set OpenGL crap
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45,static_cast<GLfloat>(m_ulWidth)/static_cast<GLfloat>(m_ulHeight),.01,5);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glViewport(0,0,m_ulWidth,m_ulHeight);

            //m_szAuthor and m_szTitle
            char *pszBuffer = 0;

            pszBuffer = new char[strlen(m_szAuthor)+strlen(m_szTitle)+100];

            sprintf(pszBuffer,"%s by %s.\n\nThere are %u Eko('s).",m_szTitle,m_szAuthor,m_ulCellsTotal);

            MessageBox(m_hWnd,pszBuffer,"EkoScape.",MB_ICONINFORMATION | MB_OK | MB_TOPMOST);

            delete [] pszBuffer;
            pszBuffer = 0;

            //m_ulRobotDelay
            SetTimer(m_hWnd,WMT_ROBOTDELAY,m_ulRobotDelay,NULL);

            //Play
            m_ulState = CWMS_PLAYING;
        }
    }
    //Playing
    else if(m_ulState == CWMS_PLAYING)
    {
        //Draw
        m_CDantares.Draw();

        //Check for movement
        if(!(KEYDOWN(DIK_LEFT) && KEYDOWN(DIK_RIGHT)))
        {
            if(KEYDOWN(DIK_LEFT))
            {
                m_CDantares.TurnLeft();
            }

            if(KEYDOWN(DIK_RIGHT))
            {
                m_CDantares.TurnRight();
            }
        }

        if(!(KEYDOWN(DIK_DOWN) && KEYDOWN(DIK_UP)))
        {
            if(KEYDOWN(DIK_DOWN))
            {
                m_CDantares.StepBackward();
            }

            if(KEYDOWN(DIK_UP))
            {
                m_CDantares.StepForward();
            }
        }

        //Check for cell
        if(m_CDantares.GetCurrentSpace() == '@')
        {
            m_ppiMap[m_CDantares.GetPlayerX()][m_CDantares.GetPlayerY()]          = m_cCellSpace;
            m_ppiMapNoObjects[m_CDantares.GetPlayerX()][m_CDantares.GetPlayerY()] = m_cCellSpace;
            m_CDantares.ChangeSquare(m_CDantares.GetPlayerX(),m_CDantares.GetPlayerY(),m_cCellSpace);
            m_CDantares.MakeSpaceWalkable(m_CDantares.GetPlayerX(),m_CDantares.GetPlayerY());

            m_ulCells++;
        }

        //Check for end
        if(m_CDantares.GetCurrentSpace() == '$' && KEYDOWN(DIK_SPACE))
        {
            //Create variables
            char szBuffer[256] = "";

            sprintf(szBuffer,"Congratulations!\n\nYou freed %u Eko('s) out of a total of %u Eko('s).",m_ulCells,m_ulCellsTotal);

            MessageBox(m_hWnd,szBuffer,"EkoScape.",MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST);

            if(m_ulCells == m_ulCellsTotal)
            {
                MessageBox(m_hWnd,"You've unlocked a secret!\n\nSend \"GL_RGB\" without the quotations as a command line to see some weird colors.","EkoScape.",MB_ICONINFORMATION | MB_OK | MB_TOPMOST);
            }

            SendMessage(m_hWnd,WM_CLOSE,0,0);
        }

        //Check for robot
        switch(m_CDantares.GetCurrentSpace())
        {
            case '!':
            case '|':
            case 'Q':
            case '?':
                {
                    //Create variables
                    char szBuffer[256] = "";

                    sprintf(szBuffer,"You're dead!\n\nYou freed %u Eko('s) out of a total of %u Eko('s).",m_ulCells,m_ulCellsTotal);

                    MessageBox(m_hWnd,szBuffer,"EkoScape.",MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST);

                    SendMessage(m_hWnd,WM_CLOSE,0,0);
                }break;
        }
    }

    return(true);
}
