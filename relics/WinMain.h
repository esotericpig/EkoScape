/*********************************************
*WINMAIN.H,                                  *
*  Copyright © 2004, Jonathan Bradley Whited.*
*********************************************/
/*Content.                                  */
/*  Pragmas:               5.               */
/*  Inclusions:            8.               */
/*  Macros:                25.              */
/*  Namespaces:            1.               */
/*    Classes:             2.               */
/*      Constructors:      4.               */
/*      Destructors:       1.               */
/*      Member functions:  23.              */
/*      Member variables:  40.              */
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

#ifndef __cplusplus

#error "This is not a C++ compiler."

#endif

#ifndef _WINDOWS

#error "This is not a Windows compiler."

#endif

#ifndef WINMAIN_H

#define WINMAIN_H

/********
*Pragmas*
********/
#ifndef WINMAIN_H_NO_PRAGMAS

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")

#endif

#pragma message("Dantares Engine Version 0.82, Copyright © Ryan Witmer.")
#pragma message("EkoScape, Copyright © 2004, Jonathan Bradley Whited.")

/***********
*Inclusions*
***********/
#ifndef WINMAIN_H_NO_INCLUSIONS

#define INITGUID

#include "Dantares/Dantares.h"
#include <dinput.h>
#include <dmusici.h>
#include <fstream.h>
#include "Resource.h"
#include <stdio.h>
#include <time.h>
#include <windowsx.h>

#if DIRECTINPUT_VERSION < 0x0800

#error "This is not DirectX Version 8.0+."

#endif

/********************************
*#ifndef WINMAIN_H_NO_INCLUSIONS*
********************************/
#endif

/*******
*Macros*
*******/
#ifndef WINMAIN_H_NO_MACROS

/*CROBOT::m_iX*/
#define CRX_VELOCITY 1

/*CROBOT::m_iY*/
#define CRY_VELOCITY 1

/*CROBOT::m_ulType*/
//Do not use 0
#define CRT_GHOST  (1)
#define CRT_NORMAL (2)
#define CRT_SNAKE  (3)
#define CRT_WORM   (4)

/*CWINMAIN::(char)(Array)*/
#define CWMCA_MAXIMUM (256)

/*CWINMAIN::BackgroundProcessing(...){...Refresh rate...}*/
//Refresh rate in milliseconds
//  60 frames/1 second = 16.6r milliseconds/1 frame
#define CWMBP_REFRESHRATE (17)

/*CWINMAIN::m_CDantares*/
//Facing (direction)
#define CWMDF_EAST  (1)
#define CWMDF_NORTH (0)
#define CWMDF_SOUTH (2)
#define CWMDF_WEST  (3)

//Initialization
#define CWMDI_CEILINGHEIGHT (.04f)
#define CWMDI_FLOORHEIGHT   (-.04f)
#define CWMDI_SQUARESIZE    (.125f)

//Maximum map sizes
#define CWMDM_MAXIMUMHEIGHT (200)
#define CWMDM_MAXIMUMWIDTH  (200)

/*CWINMAIN::m_CRobots[]*/
#define CWMRS_MAXIMUM (200)

/*CWINMAIN::m_ulState*/
#define CWMS_INTRODUCTION (1)
#define CWMS_PLAYING      (2)

/*Errors*/
#define ERROR_MESSAGEBOX                                                                                      \
                                                                                                              \
        MessageBox(NULL,"An unknown error occurred.","EkoScape Error.",MB_ICONERROR | MB_OK | MB_SYSTEMMODAL)

/*KEYDOWN(KEY)*/
#define KEYDOWN(KEY) (m_uchKey[(KEY)] != 0)

/*KEYUP(KEY)*/
#define KEYUP(KEY) (m_uchKey[(KEY)] == 0)

/*WM_TIMER*/
#define WMT_BACKGROUNDPROCESSING (101)
#define WMT_ROBOTDELAY           (102)

#endif

/***********
*Namespaces*
***********/
#ifndef WINMAIN_H_NO_NAMESPACES

namespace WINMAIN
{

#endif

/******************
*Class definitions*
******************/
#ifndef WINMAIN_H_NO_CLASSES

class CROBOT
{
protected:
    //////////////////////////////
    //Member variable prototypes//
    //////////////////////////////
    int           m_iX,
                  m_iY;
    unsigned long m_ulType;

public:
    //////////////////////////
    //Constructor prototypes//
    //////////////////////////
    CROBOT();
    CROBOT(CROBOT *p_pCRobot);
    CROBOT(int p_iX,int p_iY,unsigned long p_ulType);

    //////////////////////////////
    //Member function prototypes//
    //////////////////////////////
    virtual bool          Draw();
    virtual unsigned long GetType();
    virtual int           GetX();
    virtual int           GetY();
    virtual bool          Move();
    virtual bool          operator =(CROBOT *p_pCRobot);
    virtual bool          Set(CROBOT *p_pCRobot);
    virtual bool          Set(int p_iX,int p_iY,unsigned long p_ulType);
    virtual bool          SetPosition(int p_iX,int p_iY);
    virtual bool          SetType(unsigned long p_ulType);
    virtual bool          SetX(int p_iX);
    virtual bool          SetY(int p_iY);
};

class CWINMAIN
{
public:
    //////////////////////////
    //Constructor prototypes//
    //////////////////////////
    CWINMAIN();

    ////////////////////////
    //Destructor prototype//
    ////////////////////////
    virtual ~CWINMAIN();

    //////////////////////////////
    //Member function prototypes//
    //////////////////////////////
    virtual bool             BackgroundProcessing();
    virtual bool             BitmapGetBytes(HBITMAP *p_phBitmap,unsigned char *p_puszBytes);
    virtual bool             BitmapGetDIBSection(DIBSECTION *p_pDIBSection,HBITMAP *p_phBitmap);
    virtual bool             BitmapLoad(const char *p_pszFile,unsigned int *p_puiTexture,bool p_bFile = true);
    virtual bool             Create(unsigned long p_ulColorBitsPerPixel,unsigned long p_ulDepthBitsPerPixel,unsigned long p_ulHeight,unsigned long p_ulWidth);
    virtual void             Delete();
    virtual bool             MapCreate();
    virtual bool             MapLoad(const char *p_pszFile);
    virtual WPARAM           MessageLoop();
    virtual bool             MusicLoad(const char *p_pszFile,IDirectMusicSegment8 **p_ppIDMSegment8,bool p_bFile = true,const char *p_pszType = "");
    static  LRESULT CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    //////////////////////////////
    //Member variable prototypes//
    //////////////////////////////
    static bool                       m_bBackgroundProcessing;
    static char                       m_cCellSpace,
                                      m_cPlayerSpace,
                                      m_cRobotSpace,
                                      m_szAuthor[CWMCA_MAXIMUM],
                                      m_szTitle[CWMCA_MAXIMUM];
    static CROBOT                     m_CRobots[CWMRS_MAXIMUM];
    static Dantares                   m_CDantares;
    static HDC                        m_hDC;
    static HGLRC                      m_hGLRC;
    static HINSTANCE                  m_hInstance;
    static HWND                       m_hWnd;
    static IDirectMusicLoader8       *m_pIDMLoader8;
    static IDirectMusicPerformance8  *m_pIDMPerformance8;
    static IDirectMusicSegment8      *m_pIDMSt8Matrix;
    static int                        m_iMapID,
                                    **m_ppiMap,
                                    **m_ppiMapNoObjects;
    static LPDIRECTINPUT8             m_lpDirectInput8;
    static LPDIRECTINPUTDEVICE8       m_lpDIDe8Keyboard;
    static unsigned char              m_uchKey[256];
    static unsigned int               m_uiTextureCeiling,
                                      m_uiTextureCell,
                                      m_uiTextureEnd,
                                      m_uiTextureFloor,
                                      m_uiTextureIntroduction,
                                      m_uiTextureRobot,
                                      m_uiTextureWall;
    static unsigned long              m_ulBitsPerPixel,
                                      m_ulCells,
                                      m_ulCellsTotal,
                                      m_ulHeight,
                                      m_ulMapHeight,
                                      m_ulMapWidth,
                                      m_ulRobotDelay,
                                      m_ulState,
                                      m_ulWidth;
};

/****************************
*Member variable definitions*
****************************/
bool                       CWINMAIN::m_bBackgroundProcessing          = true;
char                       CWINMAIN::m_cCellSpace                     = 0,
                           CWINMAIN::m_cPlayerSpace                   = 0,
                           CWINMAIN::m_cRobotSpace                    = 0,
                           CWINMAIN::m_szAuthor[CWMCA_MAXIMUM]        = "",
                           CWINMAIN::m_szTitle[CWMCA_MAXIMUM]         = "";
CROBOT                     CWINMAIN::m_CRobots[CWMRS_MAXIMUM];
Dantares                   CWINMAIN::m_CDantares(CWMDI_SQUARESIZE,
                                                 CWMDI_FLOORHEIGHT,
                                                 CWMDI_CEILINGHEIGHT);
HDC                        CWINMAIN::m_hDC                            = NULL;
HGLRC                      CWINMAIN::m_hGLRC                          = NULL;
HINSTANCE                  CWINMAIN::m_hInstance                      = GetModuleHandle(NULL);
HWND                       CWINMAIN::m_hWnd                           = NULL;
IDirectMusicLoader8       *CWINMAIN::m_pIDMLoader8                    = NULL;
IDirectMusicPerformance8  *CWINMAIN::m_pIDMPerformance8               = NULL;
IDirectMusicSegment8      *CWINMAIN::m_pIDMSt8Matrix                  = NULL;
int                        CWINMAIN::m_iMapID                         = -1,
                         **CWINMAIN::m_ppiMap                         = 0,
                         **CWINMAIN::m_ppiMapNoObjects                = 0;
LPDIRECTINPUT8             CWINMAIN::m_lpDirectInput8                 = NULL;
LPDIRECTINPUTDEVICE8       CWINMAIN::m_lpDIDe8Keyboard                = NULL;
unsigned char              CWINMAIN::m_uchKey[256];
unsigned int               CWINMAIN::m_uiTextureCeiling               = 0,
                           CWINMAIN::m_uiTextureCell                  = 0,
                           CWINMAIN::m_uiTextureEnd                   = 0,
                           CWINMAIN::m_uiTextureFloor                 = 0,
                           CWINMAIN::m_uiTextureIntroduction          = 0,
                           CWINMAIN::m_uiTextureRobot                 = 0,
                           CWINMAIN::m_uiTextureWall                  = 0;
unsigned long              CWINMAIN::m_ulBitsPerPixel                 = 0,
                           CWINMAIN::m_ulCells                        = 0,
                           CWINMAIN::m_ulCellsTotal                   = 0,
                           CWINMAIN::m_ulHeight                       = 0,
                           CWINMAIN::m_ulMapHeight                    = 0,
                           CWINMAIN::m_ulMapWidth                     = 0,
                           CWINMAIN::m_ulRobotDelay                   = 0,
                           CWINMAIN::m_ulState                        = CWMS_INTRODUCTION,
                           CWINMAIN::m_ulWidth                        = 0;

/************************
*Constructor definitions*
************************/
inline CROBOT::CROBOT() : m_iX(-1),m_iY(-1),m_ulType(0)
{
}

inline CROBOT::CROBOT(CROBOT *p_pCRobot)
{
    m_iX     = m_iY = -1;
    m_ulType = 0;

    if(p_pCRobot != 0)
    {
        SetType(p_pCRobot->GetType());
        SetX(p_pCRobot->GetX());
        SetY(p_pCRobot->GetY());
    }
}

inline CROBOT::CROBOT(int p_iX,int p_iY,unsigned long p_ulType)
{
    m_iX     = m_iY = -1;
    m_ulType = 0;

    SetType(p_ulType);
    SetX(p_iX);
    SetY(p_iY);
}

inline CWINMAIN::CWINMAIN()
{
}

/***********************
*Destructor definitions*
***********************/
inline CWINMAIN::~CWINMAIN()
{
    Delete();
}

/****************************
*Member function definitions*
****************************/
inline bool CROBOT::Draw()
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(!SetType(m_ulType) || !SetX(m_iX) || !SetY(m_iY))
    {
        return(false);
    }

    /////////////////////////////////////////////
    //Check for invalid static member variables//
    /////////////////////////////////////////////
    if(CWINMAIN::m_CDantares.GetCurrentMap()            <  0 ||
       !CWINMAIN::m_CDantares.IsMap(CWINMAIN::m_iMapID)      ||
       CWINMAIN::m_ppiMap                               == 0)
    {
        return(false);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    int iType = 0;

    switch(m_ulType)
    {
        case CRT_GHOST:
            {
                iType = '|';
            }break;

        case CRT_NORMAL:
            {
                iType = '!';
            }break;

        case CRT_SNAKE:
            {
                iType = 'Q';
            }break;

        case CRT_WORM:
            {
                iType = '?';
            }break;
    }

    CWINMAIN::m_ppiMap[m_iX][m_iY] = iType;
    CWINMAIN::m_CDantares.ChangeSquare(m_iX,m_iY,iType);
    CWINMAIN::m_CDantares.MakeSpaceWalkable(m_iX,m_iY);

    return(true);
}

inline unsigned long CROBOT::GetType()
{
    return(m_ulType);
}

inline int CROBOT::GetX()
{
    return(m_iX);
}

inline int CROBOT::GetY()
{
    return(m_iY);
}

inline bool CROBOT::Move()
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(!SetType(m_ulType) || !SetX(m_iX) || !SetY(m_iY))
    {
        return(false);
    }

    /////////////////////////////////////////////
    //Check for invalid static member variables//
    /////////////////////////////////////////////
    if(CWINMAIN::m_CDantares.GetCurrentMap()            <  0 ||
       !CWINMAIN::m_CDantares.IsMap(CWINMAIN::m_iMapID)      ||
       CWINMAIN::m_ppiMap                               == 0)
    {
        return(false);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    int iXOld = m_iX,
        iYOld = m_iY,
        iXY   = 0;

    m_iX = iXOld+((m_iX < CWINMAIN::m_CDantares.GetPlayerX()) ? (CRX_VELOCITY) : (-(CRX_VELOCITY)));
    m_iY = iYOld+((m_iY < CWINMAIN::m_CDantares.GetPlayerY()) ? (CRY_VELOCITY) : (-(CRY_VELOCITY)));

    if(m_iX < 0 || m_iX >= static_cast<int>(CWINMAIN::m_ulMapWidth))
    {
        m_iX = iXOld;
    }
    if(m_iY < 0 || m_iY >= static_cast<int>(CWINMAIN::m_ulMapHeight))
    {
        m_iY = iYOld;
    }

    switch(m_ulType)
    {
        case CRT_NORMAL:
        case CRT_WORM:
            {
                switch(CWINMAIN::m_ppiMap[m_iX][iYOld])
                {
                    //DeadSpace
                    //EndWall
                    //Wall
                    //White
                    case 'x':
                    case '&':
                    case '#':
                    case 'W':
                        {
                        }break;

                    default:
                        {
                            iXY = 1;
                        }break;
                }

                switch(CWINMAIN::m_ppiMap[iXOld][m_iY])
                {
                    //DeadSpace
                    //EndWall
                    //Wall
                    //White
                    case 'x':
                    case '&':
                    case '#':
                    case 'W':
                        {
                        }break;

                    default:
                        {
                            iXY = (iXY == 1) ? 3 : 2;
                        }break;
                }

                switch(iXY)
                {
                    case 0:
                        {
                            m_iX = iXOld;
                            m_iY = iYOld;
                        }break;

                    case 1:
                        {
                            m_iY = iYOld;
                        }break;

                    case 2:
                        {
                            m_iX = iXOld;
                        }break;

                    case 3:
                        {
                            if((rand()%2) == 0)
                            {
                                m_iX = iXOld;
                            }
                            else
                            {
                                m_iY = iYOld;
                            }
                        }break;
                }
            }break;

        default:
            {
                if((rand()%2) == 0)
                {
                    m_iX = iXOld;
                }
                else
                {
                    m_iY = iYOld;
                }

                iXY = 4;
            }break;
    }

    if(iXY != 0)
    {
        switch(m_ulType)
        {
            case CRT_GHOST:
            case CRT_NORMAL:
                {
                    CWINMAIN::m_ppiMap[iXOld][iYOld] = CWINMAIN::m_ppiMapNoObjects[iXOld][iYOld];
                    CWINMAIN::m_CDantares.ChangeSquare(iXOld,iYOld,CWINMAIN::m_ppiMapNoObjects[iXOld][iYOld]);

                    switch(CWINMAIN::m_ppiMap[iXOld][iYOld])
                    {
                        //DeadSpace
                        //EndWall
                        //Wall
                        //White
                        case 'x':
                        case '&':
                        case '#':
                        case 'W':
                            {
                                CWINMAIN::m_CDantares.MakeSpaceNonWalkable(iXOld,iYOld);
                            }break;

                        default:
                            {
                                CWINMAIN::m_CDantares.MakeSpaceWalkable(iXOld,iYOld);
                            }break;
                    }
                }break;
        }
    }

    return(true);
}

inline bool CROBOT::operator =(CROBOT *p_pCRobot)
{
    return(Set(p_pCRobot));
}

inline bool CROBOT::Set(CROBOT *p_pCRobot)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_pCRobot == 0)
    {
        return(false);
    }

    if(!SetType(p_pCRobot->GetType()) || !SetX(p_pCRobot->GetX()) || !SetY(p_pCRobot->GetY()))
    {
        return(false);
    }

    return(true);
}

inline bool CROBOT::Set(int p_iX,int p_iY,unsigned long p_ulType)
{
    if(!SetType(p_ulType) || !SetX(p_iX) || !SetY(p_iY))
    {
        return(false);
    }

    return(true);
}

inline bool CROBOT::SetPosition(int p_iX,int p_iY)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_iX <  0                                         ||
       p_iX >= static_cast<int>(CWINMAIN::m_ulMapWidth)  ||
       p_iY <  0                                         ||
       p_iY >= static_cast<int>(CWINMAIN::m_ulMapHeight))
    {
        return(false);
    }

    m_iX = p_iX;
    m_iY = p_iY;

    return(true);
}

inline bool CROBOT::SetType(unsigned long p_ulType)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    switch(p_ulType)
    {
        case CRT_GHOST:
        case CRT_NORMAL:
        case CRT_SNAKE:
        case CRT_WORM:
            {
                m_ulType = p_ulType;
            }break;

        default:
            {
                return(false);
            }break;
    }

    return(true);
}

inline bool CROBOT::SetX(int p_iX)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_iX <  0                                        ||
       p_iX >= static_cast<int>(CWINMAIN::m_ulMapWidth))
    {
        return(false);
    }

    m_iX = p_iX;

    return(true);
}

inline bool CROBOT::SetY(int p_iY)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_iY <  0                                         ||
       p_iY >= static_cast<int>(CWINMAIN::m_ulMapHeight))
    {
        return(false);
    }

    m_iY = p_iY;

    return(true);
}

inline bool CWINMAIN::BitmapGetBytes(HBITMAP *p_phBitmap,unsigned char *p_puszBytes)
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_hDC == NULL || m_hWnd == NULL)
    {
        return(false);
    }

    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_phBitmap == 0 || *p_phBitmap == NULL)
    {
        return(false);
    }

    try
    {
        strcat(reinterpret_cast<char *>(p_puszBytes),"");
    }

    catch(...)
    {
        return(false);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    BITMAPINFO BitmapInfo;
    DIBSECTION DIBSection;
    HBITMAP    hBpOld     = NULL;
    HDC        hDCBitmap  = NULL;

    //////////////////
    //Get DIBSection//
    //////////////////
    ZeroMemory(&DIBSection,sizeof(DIBSECTION));

    if(!BitmapGetDIBSection(&DIBSection,p_phBitmap))
    {
        return(false);
    }

    //////////////////
    //Get BitmapInfo//
    //////////////////
    ZeroMemory(&BitmapInfo,sizeof(BITMAPINFO));
    CopyMemory(&BitmapInfo.bmiHeader,&DIBSection.dsBmih,sizeof(BITMAPINFOHEADER));

    ////////////////////////////////
    //Create hDCBitmap using m_hDC//
    ////////////////////////////////
    if((hDCBitmap = CreateCompatibleDC(m_hDC)) == NULL)
    {
        return(false);
    }

    //////////////
    //Get hBpOld//
    //////////////
    if((hBpOld = SelectBitmap(hDCBitmap,*p_phBitmap)) == NULL)
    {
        DeleteDC(hDCBitmap);
        hDCBitmap = NULL;

        return(false);
    }

    ////////////////////
    //Get *p_puszBytes//
    ////////////////////
    try
    {
        if(GetDIBits(hDCBitmap,*p_phBitmap,0,BitmapInfo.bmiHeader.biHeight,
                     static_cast<LPVOID>(p_puszBytes),&BitmapInfo,DIB_RGB_COLORS) == 0)
        {
            SelectBitmap(hDCBitmap,hBpOld);
            DeleteDC(hDCBitmap);
            hDCBitmap = NULL;

            return(false);
        }
    }

    catch(...)
    {
        SelectBitmap(hDCBitmap,hBpOld);
        DeleteDC(hDCBitmap);
        hDCBitmap = NULL;

        return(false);
    }

    ////////////////////
    //Delete variables//
    ////////////////////
    if(SelectBitmap(hDCBitmap,hBpOld) == NULL)
    {
        DeleteDC(hDCBitmap);
        hDCBitmap = NULL;

        return(false);
    }

    if(DeleteDC(hDCBitmap) == FALSE)
    {
        hDCBitmap = NULL;

        return(false);
    }

    hDCBitmap = NULL;

    return(true);
}

inline bool CWINMAIN::BitmapGetDIBSection(DIBSECTION *p_pDIBSection,HBITMAP *p_phBitmap)
{
    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_pDIBSection == 0 || p_phBitmap == 0 || *p_phBitmap == NULL)
    {
        return(false);
    }

    //////////////////////
    //Get *p_pDIBSection//
    //////////////////////
    if(GetObject(static_cast<HGDIOBJ>(*p_phBitmap),sizeof(DIBSECTION),
                 static_cast<LPVOID>(p_pDIBSection)) == 0)
    {
        return(false);
    }

    return(true);
}

inline bool CWINMAIN::BitmapLoad(const char *p_pszFile,unsigned int *p_puiTexture,bool p_bFile)
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_hInstance == NULL)
    {
        return(false);
    }

    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_puiTexture == 0)
    {
        return(false);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    DIBSECTION     DIBSection;
    char          *pszCommandLine = 0;
    GLenum         GLEnum         = GL_BGR_EXT;
    HBITMAP        hBitmap        = NULL;
    unsigned char *puszBytes      = 0;

    ///////////////////////
    //Get *pszCommandLine//
    ///////////////////////
    if((pszCommandLine = new char[lstrlen(GetCommandLine())+1]) == 0)
    {
        return(false);
    }

    strcpy(pszCommandLine,GetCommandLine());

    if(*pszCommandLine == '"')
    {
        while(*pszCommandLine != '\0')
        {
            strcpy(pszCommandLine,pszCommandLine+1);

            if(*pszCommandLine == '"')
            {
                strcpy(pszCommandLine,pszCommandLine+1);

                if(*pszCommandLine == ' ')
                {
                    strcpy(pszCommandLine,pszCommandLine+1);
                }

                break;
            }
        }
    }
    else
    {
        while(*pszCommandLine != '\0')
        {
            strcpy(pszCommandLine,pszCommandLine+1);

            if(*pszCommandLine == ' ')
            {
                strcpy(pszCommandLine,pszCommandLine+1);

                break;
            }
        }
    }

    //////////////
    //Get GLEnum//
    //////////////
    if(strcmp(strupr(pszCommandLine),"GL_RGB") == 0)
    {
        GLEnum = GL_RGB;
    }

    ////////////////////
    //Delete variables//
    ////////////////////
    delete [] pszCommandLine;
    pszCommandLine = 0;

    ///////////////
    //Get hBitmap//
    ///////////////
    if(p_bFile)
    {
        if((hBitmap = static_cast<HBITMAP>(LoadImage(NULL,p_pszFile,IMAGE_BITMAP,0,0,
                                                     LR_CREATEDIBSECTION | LR_LOADFROMFILE))) == NULL)
        {
            return(false);
        }
    }
    else
    {
        if((hBitmap = static_cast<HBITMAP>(LoadImage(m_hInstance,p_pszFile,IMAGE_BITMAP,0,0,
                                                     LR_CREATEDIBSECTION))) == NULL)
        {
            return(false);
        }
    }

    //////////////////
    //Get DIBSection//
    //////////////////
    ZeroMemory(&DIBSection,sizeof(DIBSECTION));

    if(!BitmapGetDIBSection(&DIBSection,&hBitmap))
    {
        DeleteBitmap(hBitmap);
        hBitmap = NULL;

        return(false);
    }

    //////////////////
    //Get *puszBytes//
    //////////////////
    if((puszBytes = new unsigned char[DIBSection.dsBmih.biSizeImage+1]) == 0)
    {
        DeleteBitmap(hBitmap);
        hBitmap = NULL;

        return(false);
    }

    if(!BitmapGetBytes(&hBitmap,puszBytes))
    {
        DeleteBitmap(hBitmap);
        hBitmap = NULL;

        delete [] puszBytes;
        puszBytes = 0;

        return(false);
    }

    ////////////////////
    //Delete variables//
    ////////////////////
    DeleteBitmap(hBitmap);
    hBitmap = NULL;

    ////////////////////////
    //Create *p_puiTexture//
    ////////////////////////
    glGenTextures(1,p_puiTexture);
    glBindTexture(GL_TEXTURE_2D,*p_puiTexture);
    glTexImage2D(GL_TEXTURE_2D,0,3,DIBSection.dsBmih.biWidth,DIBSection.dsBmih.biHeight,0,GLEnum,
                 GL_UNSIGNED_BYTE,static_cast<GLvoid *>(puszBytes));
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    ////////////////////
    //Delete variables//
    ////////////////////
    delete [] puszBytes;
    puszBytes = 0;

    return(true);
}

inline bool CWINMAIN::Create(unsigned long p_ulColorBitsPerPixel,unsigned long p_ulDepthBitsPerPixel,unsigned long p_ulHeight,unsigned long p_ulWidth)
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_CDantares.GetCurrentMap() >= 0    || m_hDC                   != NULL ||
       m_hGLRC                     != NULL || m_hInstance             == NULL ||
       m_hWnd                      != NULL || m_pIDMLoader8           != NULL ||
       m_pIDMPerformance8          != NULL || m_pIDMSt8Matrix         != NULL ||
       m_iMapID                    >= 0    || m_lpDirectInput8        != NULL ||
       m_lpDIDe8Keyboard           != NULL || m_uiTextureCeiling      != 0    ||
       m_uiTextureCell             != 0    || m_uiTextureEnd          != 0    ||
       m_uiTextureFloor            != 0    || m_uiTextureIntroduction != 0    ||
       m_uiTextureWall             != 0)
    {
        return(false);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    DEVMODE               DevMode;
    int                   iPixelFormat          = 0;
    PIXELFORMATDESCRIPTOR PixelFormatDescriptor;
    WNDCLASSEX            WndClassEx;

    m_ulBitsPerPixel = p_ulColorBitsPerPixel;
    m_ulHeight       = p_ulHeight;
    m_ulWidth        = p_ulWidth;

    ZeroMemory(&DevMode,sizeof(DEVMODE));
    DevMode.dmBitsPerPel = m_ulBitsPerPixel;
    DevMode.dmFields     = DM_BITSPERPEL | DM_PELSHEIGHT | DM_PELSWIDTH;
    DevMode.dmPelsHeight = m_ulHeight;
    DevMode.dmPelsWidth  = m_ulWidth;
    DevMode.dmSize       = sizeof(DEVMODE);

    ZeroMemory(&PixelFormatDescriptor,sizeof(PIXELFORMATDESCRIPTOR));
    PixelFormatDescriptor.cColorBits = static_cast<BYTE>(m_ulBitsPerPixel);
    PixelFormatDescriptor.cDepthBits = static_cast<BYTE>(p_ulDepthBitsPerPixel);
    PixelFormatDescriptor.dwFlags    = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
    PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDescriptor.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    PixelFormatDescriptor.nVersion   = 1;

    ZeroMemory(&WndClassEx,sizeof(WNDCLASSEX));
    WndClassEx.cbSize        = sizeof(WNDCLASSEX);
    WndClassEx.hbrBackground = GetStockBrush(BLACK_BRUSH);
    WndClassEx.hCursor       = LoadCursor(NULL,IDC_ARROW);
    WndClassEx.hInstance     = m_hInstance;
    WndClassEx.hIcon         = LoadIcon(WndClassEx.hInstance,MAKEINTRESOURCE(IDI_EYE));
    WndClassEx.lpfnWndProc   = WndProc;
    WndClassEx.lpszClassName = "CEkoScape";
    WndClassEx.style         = CS_OWNDC;

    ///////////////////////////////////////////////////
    //Check if WndClassEx.lpszClassName is being used//
    ///////////////////////////////////////////////////
    if(FindWindow(WndClassEx.lpszClassName,NULL) != NULL)
    {
        return(false);
    }

    ///////////////////////
    //Register WndClassEx//
    ///////////////////////
    if(RegisterClassEx(&WndClassEx) == 0)
    {
        return(false);
    }

    //////////////////////////////////////////
    //Change the display settings to DevMode//
    //////////////////////////////////////////
    if(ChangeDisplaySettings(&DevMode,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
        return(false);
    }

    /////////////////
    //Create m_hWnd//
    /////////////////
    if((m_hWnd = CreateWindowEx(WS_EX_APPWINDOW,WndClassEx.lpszClassName,"EkoScape",
                                WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP,0,0,m_ulWidth,
                                m_ulHeight,NULL,NULL,WndClassEx.hInstance,NULL)) == NULL)
    {
        return(false);
    }

    /////////////
    //Get m_hDC//
    /////////////
    if((m_hDC = GetDC(m_hWnd)) == NULL)
    {
        return(false);
    }

    ////////////////////
    //Get iPixelFormat//
    ////////////////////
    if((iPixelFormat = ChoosePixelFormat(m_hDC,&PixelFormatDescriptor)) == 0)
    {
        return(false);
    }

    ////////////////////////////////////////////
    //Set m_hDC's pixel format to iPixelFormat//
    ////////////////////////////////////////////
    if(SetPixelFormat(m_hDC,iPixelFormat,&PixelFormatDescriptor) == FALSE)
    {
        return(false);
    }

    //////////////////////////////
    //Create m_hGLRC using m_hDC//
    //////////////////////////////
    if((m_hGLRC = wglCreateContext(m_hDC)) == NULL)
    {
        return(false);
    }

    //////////////////////////////////////////////
    //Make m_hGLRC the current rendering context//
    //////////////////////////////////////////////
    if(wglMakeCurrent(m_hDC,m_hGLRC) == FALSE)
    {
        return(false);
    }

    ///////////////////
    //Set OpenGL crap//
    ///////////////////
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0,0,0,1);
    glClearDepth(1);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(0,0,0,0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glShadeModel(GL_SMOOTH);
    glViewport(0,-258,1024,1024);

    /////////////////
    //Load textures//
    /////////////////
    if(!BitmapLoad("Files\\Graphics\\Ceiling.BMP",&m_uiTextureCeiling)               ||
       !BitmapLoad("Files\\Graphics\\Cell.BMP",&m_uiTextureCell)                     ||
       !BitmapLoad("Files\\Graphics\\End.BMP",&m_uiTextureEnd)                       ||
       !BitmapLoad("Files\\Graphics\\Floor.BMP",&m_uiTextureFloor)                   ||
       !BitmapLoad(MAKEINTRESOURCE(IDB_INTRODUCTION),&m_uiTextureIntroduction,false) ||
       !BitmapLoad("Files\\Graphics\\Robot.BMP",&m_uiTextureRobot)                   ||
       !BitmapLoad("Files\\Graphics\\Wall.BMP",&m_uiTextureWall))
    {
        return(false);
    }

    ///////////////////////////
    //Create m_lpDirectInput8//
    ///////////////////////////
    if(FAILED(DirectInput8Create(m_hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
                                 reinterpret_cast<LPVOID *>(&m_lpDirectInput8),NULL)))
    {
        return(false);
    }

    ////////////////////////////
    //Create m_lpDIDe8Keyboard//
    ////////////////////////////
    if(FAILED(m_lpDirectInput8->CreateDevice(GUID_SysKeyboard,&m_lpDIDe8Keyboard,NULL)))
    {
        return(false);
    }

    if(FAILED(m_lpDIDe8Keyboard->SetDataFormat(&c_dfDIKeyboard)))
    {
        return(false);
    }

    if(FAILED(m_lpDIDe8Keyboard->SetCooperativeLevel(m_hWnd,DISCL_FOREGROUND |
                                                     DISCL_NONEXCLUSIVE)))
    {
        return(false);
    }

    /////////////////////////
    //Create *m_pIDMLoader8//
    /////////////////////////
    if(FAILED(CoInitialize(NULL)))
    {
        return(false);
    }

    if(FAILED(CoCreateInstance(CLSID_DirectMusicLoader,NULL,CLSCTX_INPROC,
                               IID_IDirectMusicLoader8,
                               reinterpret_cast<LPVOID *>(&m_pIDMLoader8))))
    {
        return(false);
    }

    //////////////////////////////
    //Create *m_pIDMPerformance8//
    //////////////////////////////
    if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance,NULL,CLSCTX_INPROC,
                               IID_IDirectMusicPerformance8,
                               reinterpret_cast<LPVOID *>(&m_pIDMPerformance8))))
    {
        return(false);
    }

    if(FAILED(m_pIDMPerformance8->InitAudio(NULL,NULL,m_hWnd,DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                            64,DMUS_AUDIOF_ALL,NULL)))
    {
        return(false);
    }

    ////////////////////////
    //Get *m_pIDMSt8Matrix//
    ////////////////////////
    if(!MusicLoad(MAKEINTRESOURCE(IDS_MATRIX),&m_pIDMSt8Matrix,false,"SOUND"))
    {
        return(false);
    }

    if(FAILED(m_pIDMSt8Matrix->Download(m_pIDMPerformance8)))
    {
        return(false);
    }

    if(FAILED(m_pIDMSt8Matrix->SetRepeats(DMUS_SEG_REPEAT_INFINITE)))
    {
        return(false);
    }

    /////////////////////////////////
    //Set the random starting point//
    /////////////////////////////////
    srand(static_cast<unsigned int>(time(0)));

    //////////////////////
    //Create m_CDantares//
    //////////////////////
    if(!MapCreate())
    {
        return(false);
    }

    ///////////////
    //Show m_hWnd//
    ///////////////
    ShowWindow(m_hWnd,SW_SHOWNORMAL);
    ShowWindow(m_hWnd,SW_SHOWDEFAULT);

    if(!IsWindowVisible(m_hWnd))
    {
        return(false);
    }

    /////////////////////////////////////////////
    //Set m_hWnd as the focus                  //
    //Set m_hWnd at the top of the window order//
    /////////////////////////////////////////////
    SetFocus(m_hWnd);
    SetForegroundWindow(m_hWnd);

    ///////////////////
    //Hide the cursor//
    ///////////////////
    ShowCursor(FALSE);

    /////////////////////////////
    //Acquire m_lpDIDe8Keyboard//
    /////////////////////////////
    if(FAILED(m_lpDIDe8Keyboard->Acquire()))
    {
        return(false);
    }

    /////////////////////////
    //Play *m_pIDMSt8Matrix//
    /////////////////////////
    if(FAILED(m_pIDMPerformance8->PlaySegment(m_pIDMSt8Matrix,0,0,NULL)))
    {
        return(false);
    }

    return(true);
}

inline void CWINMAIN::Delete()
{
    //////////////////////
    //Delete m_CDantares//
    //////////////////////
    if(m_iMapID >= 0)
    {
        m_CDantares.DeleteMap(m_iMapID);
        m_iMapID = -1;
    }

    if(m_ppiMap != 0)
    {
        for(unsigned long ulX = 0; ulX < m_ulMapWidth; ulX++)
        {
            if(m_ppiMap[ulX] != 0)
            {
                delete [] m_ppiMap[ulX];
                m_ppiMap[ulX] = 0;
            }
        }

        delete [] m_ppiMap;
        m_ppiMap = 0;
    }

    if(m_ppiMapNoObjects != 0)
    {
        for(unsigned long ulX = 0; ulX < m_ulMapWidth; ulX++)
        {
            if(m_ppiMapNoObjects[ulX] != 0)
            {
                delete [] m_ppiMapNoObjects[ulX];
                m_ppiMapNoObjects[ulX] = 0;
            }
        }

        delete [] m_ppiMapNoObjects;
        m_ppiMapNoObjects = 0;
    }

    //////////////////////////////
    //Delete *m_pIDMPerformance8//
    //////////////////////////////
    if(m_pIDMPerformance8 != NULL)
    {
        m_pIDMPerformance8->Stop(NULL,NULL,0,0);
        m_pIDMPerformance8->CloseDown();
        m_pIDMPerformance8->Release();
        m_pIDMPerformance8 = NULL;
    }

    ///////////////////////////
    //Delete *m_pIDMSt8Matrix//
    ///////////////////////////
    if(m_pIDMSt8Matrix != NULL)
    {
        m_pIDMSt8Matrix->Release();
        m_pIDMSt8Matrix = NULL;
    }

    /////////////////////////
    //Delete *m_pIDMLoader8//
    /////////////////////////
    if(m_pIDMLoader8 != NULL)
    {
        m_pIDMLoader8->Release();
        m_pIDMLoader8 = NULL;

        CoUninitialize();
    }

    ////////////////////////////
    //Delete m_lpDIDe8Keyboard//
    ////////////////////////////
    if(m_lpDIDe8Keyboard != NULL)
    {
        m_lpDIDe8Keyboard->Unacquire();
        m_lpDIDe8Keyboard->Release();
        m_lpDIDe8Keyboard = NULL;
    }

    ///////////////////////////
    //Delete m_lpDirectInput8//
    ///////////////////////////
    if(m_lpDirectInput8 != NULL)
    {
        m_lpDirectInput8->Release();
        m_lpDirectInput8 = NULL;
    }

    ///////////////////
    //Delete textures//
    ///////////////////
    if(m_uiTextureCeiling != 0)
    {
        glDeleteTextures(1,&m_uiTextureCeiling);
        m_uiTextureCeiling = 0;
    }

    if(m_uiTextureCell != 0)
    {
        glDeleteTextures(1,&m_uiTextureCell);
        m_uiTextureCell = 0;
    }

    if(m_uiTextureEnd != 0)
    {
        glDeleteTextures(1,&m_uiTextureEnd);
        m_uiTextureEnd = 0;
    }

    if(m_uiTextureFloor != 0)
    {
        glDeleteTextures(1,&m_uiTextureFloor);
        m_uiTextureFloor = 0;
    }

    if(m_uiTextureIntroduction != 0)
    {
        glDeleteTextures(1,&m_uiTextureIntroduction);
        m_uiTextureIntroduction = 0;
    }

    if(m_uiTextureRobot != 0)
    {
        glDeleteTextures(1,&m_uiTextureRobot);
        m_uiTextureRobot = 0;
    }

    if(m_uiTextureWall != 0)
    {
        glDeleteTextures(1,&m_uiTextureWall);
        m_uiTextureWall = 0;
    }

    //////////////////////////////////////////////
    //Change the display settings back to normal//
    //////////////////////////////////////////////
    ChangeDisplaySettings(NULL,0);

    //////////////////
    //Delete m_hGLRC//
    //////////////////
    if(m_hGLRC != NULL)
    {
        wglMakeCurrent(NULL,NULL);
        wglDeleteContext(m_hGLRC);
        m_hGLRC = NULL;
    }

    ////////////////
    //Delete m_hDC//
    ////////////////
    if(m_hDC != NULL)
    {
        ReleaseDC(m_hWnd,m_hDC);
        m_hDC = NULL;
    }

    /////////////////
    //Delete m_hWnd//
    /////////////////
    if(m_hWnd != NULL)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = NULL;
    }

    //////////////////////
    //Delete m_hInstance//
    //////////////////////
    if(m_hInstance != NULL)
    {
        UnregisterClass("CEkoScape",m_hInstance);
        m_hInstance = NULL;
    }

    ///////////////////
    //Show the cursor//
    ///////////////////
    ShowCursor(TRUE);
}

inline bool CWINMAIN::MapCreate()
{
    using namespace std;

    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_iMapID >= 0)
    {
        m_CDantares.DeleteMap(m_iMapID);
        m_iMapID = -1;
    }

    ////////////////////
    //Create variables//
    ////////////////////
    char          szMap[_MAX_PATH]             = "";
    ifstream      Fin("Files\\Maps\\Maps.TXT");
    unsigned long ulMapMax                     = 0,
                  ulMaps                       = 0;

    if(!Fin.is_open())
    {
        return(false);
    }

    while(Fin.getline(szMap,_MAX_PATH))
    {
        ulMaps++;
    }

    srand(time(0));
    ulMapMax = (rand()%ulMaps)+1;

    Fin.seekg(ios::beg);
    Fin.clear();

    for(unsigned long ulMap = 0; ulMap < ulMapMax; ulMap++)
    {
        Fin.getline(szMap,_MAX_PATH);
    }

    Fin.close();
    Fin.open(szMap,ios::nocreate);

    if(!Fin.is_open())
    {
        return(false);
    }

    if(!MapLoad(szMap))
    {
        return(false);
    }

    if(!m_CDantares.GenerateMap())
    {
        return(false);
    }

    return(true);
}

inline bool CWINMAIN::MapLoad(const char *p_pszFile)
{
    using namespace std;

    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_iMapID >= 0)
    {
        return(false);
    }

    unsigned long ulRobots = 0;

    for(; ulRobots < CWMRS_MAXIMUM; ulRobots++)
    {
        m_CRobots[ulRobots].Set(-1,-1,0);
    }

    ////////////////////
    //Create variables//
    ////////////////////
    char     chType                                     = 0;
    double   dDummy                                     = 0;
    ifstream Fin(p_pszFile,ios::binary | ios::nocreate);

    ulRobots = 0;

    if(!Fin.is_open())
    {
        return(false);
    }

    /////////////////
    //Get m_szTitle//
    /////////////////
    Fin.getline(m_szTitle,CWMCA_MAXIMUM,'"');
    Fin.getline(m_szTitle,CWMCA_MAXIMUM,'"');

    //////////////////
    //Get m_szAuthor//
    //////////////////
    Fin.getline(m_szAuthor,CWMCA_MAXIMUM,'"');
    Fin.getline(m_szAuthor,CWMCA_MAXIMUM,'"');

    /////////////////////
    //Get turning speed//
    /////////////////////
    Fin >> dDummy;

    m_CDantares.SetTurningSpeed(static_cast<float>(dDummy));

    /////////////////////
    //Get walking speed//
    /////////////////////
    Fin >> dDummy;

    m_CDantares.SetWalkingSpeed(static_cast<float>(dDummy));

    ////////////////////
    //Get m_cCellSpace//
    ////////////////////
    while(Fin.get(m_cCellSpace))
    {
        if(m_cCellSpace == '\'')
        {
            break;
        }
    }

    Fin.get(m_cCellSpace);
    Fin.get();

    //////////////////////
    //Get m_cPlayerSpace//
    //////////////////////
    while(Fin.get(m_cPlayerSpace))
    {
        if(m_cPlayerSpace == '\'')
        {
            break;
        }
    }

    Fin.get(m_cPlayerSpace);
    Fin.get();

    /////////////////////
    //Get m_cRobotSpace//
    /////////////////////
    while(Fin.get(m_cRobotSpace))
    {
        if(m_cRobotSpace == '\'')
        {
            break;
        }
    }

    Fin.get(m_cRobotSpace);
    Fin.get();

    //////////////////////
    //Get m_ulRobotDelay//
    //////////////////////
    Fin >> m_ulRobotDelay;

    //////////////////////////////////////
    //Get m_ulMapWidth and m_ulMapHeight//
    //////////////////////////////////////
    Fin >> m_ulMapWidth;

    m_ulMapWidth++;

    Fin >> m_ulMapHeight;

    m_ulMapHeight++;

    if(m_ulMapHeight < 1 || m_ulMapHeight > CWMDM_MAXIMUMHEIGHT ||
       m_ulMapWidth  < 1 || m_ulMapWidth  > CWMDM_MAXIMUMWIDTH)
    {
        return(false);
    }

    /////////////////////
    //Delete **m_ppiMap//
    /////////////////////
    if(m_ppiMap != 0)
    {
        for(unsigned long ulX = 0; ulX < m_ulMapWidth; ulX++)
        {
            if(m_ppiMap[ulX] != 0)
            {
                delete [] m_ppiMap[ulX];
                m_ppiMap[ulX] = 0;
            }
        }

        delete [] m_ppiMap;
        m_ppiMap = 0;
    }

    /////////////////////
    //Create **m_ppiMap//
    /////////////////////
    if((m_ppiMap = new int *[m_ulMapWidth]) == 0)
    {
        return(false);
    }

    unsigned long ulX = 0;

    for(; ulX < m_ulMapWidth; ulX++)
    {
        if((m_ppiMap[ulX] = new int[m_ulMapHeight]) == 0)
        {
            return(false);
        }
    }

    //////////////////////////////
    //Delete **m_ppiMapNoObjects//
    //////////////////////////////
    if(m_ppiMapNoObjects != 0)
    {
        for(unsigned long ulX = 0; ulX < m_ulMapWidth; ulX++)
        {
            if(m_ppiMapNoObjects[ulX] != 0)
            {
                delete [] m_ppiMapNoObjects[ulX];
                m_ppiMapNoObjects[ulX] = 0;
            }
        }

        delete [] m_ppiMapNoObjects;
        m_ppiMapNoObjects = 0;
    }

    //////////////////////////////
    //Create **m_ppiMapNoObjects//
    //////////////////////////////
    if((m_ppiMapNoObjects = new int *[m_ulMapWidth]) == 0)
    {
        return(false);
    }

    for(ulX = 0; ulX < m_ulMapWidth; ulX++)
    {
        if((m_ppiMapNoObjects[ulX] = new int[m_ulMapHeight]) == 0)
        {
            return(false);
        }
    }

    //////////////////////
    //Create m_CDantares//
    //////////////////////
    m_iMapID = m_CDantares.AddMap(static_cast<void *>(m_ppiMap),m_ulMapWidth,m_ulMapHeight);

    m_CDantares.SetCurrentMap(m_iMapID);

    if(m_CDantares.GetCurrentMap() != m_iMapID)
    {
        return(false);
    }

    ///////////////////
    //Load *p_pszFile//
    ///////////////////
    //Flip iY because the map files are flipped
    for(int iY = (static_cast<int>(m_ulMapHeight)-1); iY > -1; iY--)
    {
        for(int iX = 0; iX < static_cast<int>(m_ulMapWidth); iX++)
        {
            //Get chType
            if(!Fin.get(chType))
            {
                for(; iX > -1; iX--)
                {
                    m_ppiMap[iX][iY]          = 'x';
                    m_ppiMapNoObjects[iX][iY] = 'x';
                    m_CDantares.ChangeSquare(iX,iY,'x');
                    m_CDantares.MakeSpaceNonWalkable(iX,iY);
                }
            }

            if(chType == '@')
            {
                m_ulCellsTotal++;
            }

            switch(chType)
            {
                //Cell
                //Empty
                //End
                //LivingSpace
                //WallGhost
                //WhiteFloor
                //WhiteGhost
                case '@':
                case ' ':
                case '$':
                case '*':
                case '%':
                case 'w':
                case '+':
                    {
                        m_ppiMap[iX][iY]          = chType;
                        m_ppiMapNoObjects[iX][iY] = chType;
                        m_CDantares.ChangeSquare(iX,iY,chType);
                        m_CDantares.MakeSpaceWalkable(iX,iY);
                    }break;

                //DeadSpace
                //EndWall
                //Wall
                //White
                case 'x':
                case '&':
                case '#':
                case 'W':
                    {
                        m_ppiMap[iX][iY]          = chType;
                        m_ppiMapNoObjects[iX][iY] = chType;
                        m_CDantares.ChangeSquare(iX,iY,chType);
                        m_CDantares.MakeSpaceNonWalkable(iX,iY);
                    }break;

                //DeadSpaces
                case '\n':
                case '\r':
                    {
                        for(; iX > -1; iX--)
                        {
                            m_ppiMap[iX][iY]          = 'x';
                            m_ppiMapNoObjects[iX][iY] = 'x';
                            m_CDantares.ChangeSquare(iX,iY,'x');
                            m_CDantares.MakeSpaceNonWalkable(iX,iY);
                        }

                        while(Fin.get(chType))
                        {
                            if(chType != '\n' && chType != '\r')
                            {
                                break;
                            }
                        }

                        Fin.putback(chType);
                    }break;

                //PlayerEast
                //PlayerNorth
                //PlayerSouth
                //PlayerWest
                case '>':
                case '^':
                case 'v':
                case '<':
                    {
                        switch(chType)
                        {
                            case '>':
                                {
                                    chType = CWMDF_EAST;
                                }break;

                            case '^':
                                {
                                    chType = CWMDF_NORTH;
                                }break;

                            case 'v':
                                {
                                    chType = CWMDF_SOUTH;
                                }break;

                            case '<':
                                {
                                    chType = CWMDF_WEST;
                                }break;
                        }

                        m_CDantares.SetPlayerPosition(iX,iY,chType);
                        m_ppiMap[iX][iY]          = m_cPlayerSpace;
                        m_ppiMapNoObjects[iX][iY] = m_cPlayerSpace;
                        m_CDantares.ChangeSquare(iX,iY,m_cPlayerSpace);
                        m_CDantares.MakeSpaceWalkable(iX,iY);
                    }break;

                //Robot
                //RobotGhost
                //RobotSnake
                //RobotWorm
                case '!':
                case '|':
                case 'Q':
                case '?':
                    {
                        //Create variables
                        unsigned long ulType = 0;

                        switch(chType)
                        {
                            case '!':
                                {
                                    ulType = CRT_NORMAL;
                                }break;

                            case '|':
                                {
                                    ulType = CRT_GHOST;
                                }break;

                            case 'Q':
                                {
                                    ulType = CRT_SNAKE;
                                }break;

                            case '?':
                                {
                                    ulType = CRT_WORM;
                                }break;
                        }

                        if(ulRobots < CWMRS_MAXIMUM)
                        {
                            m_CRobots[ulRobots].Set(iX,iY,ulType);

                            ulRobots++;
                        }
                        else
                        {
                            chType = ' ';
                        }

                        m_ppiMap[iX][iY]          = chType;
                        m_ppiMapNoObjects[iX][iY] = m_cRobotSpace;
                        m_CDantares.ChangeSquare(iX,iY,chType);
                        m_CDantares.MakeSpaceWalkable(iX,iY);
                    }break;

                default:
                    {
                        iX++;
                    }break;
            }
        }
    }

    m_CDantares.SetCeilingTexture('@',m_uiTextureCeiling);
    m_CDantares.SetCeilingTexture('x',-1);
    m_CDantares.SetCeilingTexture(' ',m_uiTextureCeiling);
    m_CDantares.SetCeilingTexture('$',m_uiTextureEnd);
    m_CDantares.SetCeilingTexture('&',m_uiTextureEnd);
    m_CDantares.SetCeilingTexture('*',-1);
    m_CDantares.SetCeilingTexture('!',m_uiTextureRobot);
    m_CDantares.SetCeilingTexture('|',m_uiTextureRobot);
    m_CDantares.SetCeilingTexture('Q',m_uiTextureRobot);
    m_CDantares.SetCeilingTexture('?',m_uiTextureRobot);
    m_CDantares.SetCeilingTexture('#',m_uiTextureCeiling);
    m_CDantares.SetCeilingTexture('%',m_uiTextureCeiling);
    m_CDantares.SetCeilingTexture('W',0);
    m_CDantares.SetCeilingTexture('w',0);
    m_CDantares.SetCeilingTexture('+',0);

    m_CDantares.SetFloorTexture('@',m_uiTextureFloor);
    m_CDantares.SetFloorTexture('x',-1);
    m_CDantares.SetFloorTexture(' ',m_uiTextureFloor);
    m_CDantares.SetFloorTexture('$',m_uiTextureEnd);
    m_CDantares.SetFloorTexture('&',m_uiTextureEnd);
    m_CDantares.SetFloorTexture('*',-1);
    m_CDantares.SetFloorTexture('!',m_uiTextureRobot);
    m_CDantares.SetFloorTexture('|',m_uiTextureRobot);
    m_CDantares.SetFloorTexture('Q',m_uiTextureRobot);
    m_CDantares.SetFloorTexture('?',m_uiTextureRobot);
    m_CDantares.SetFloorTexture('#',m_uiTextureFloor);
    m_CDantares.SetFloorTexture('%',m_uiTextureFloor);
    m_CDantares.SetFloorTexture('W',0);
    m_CDantares.SetFloorTexture('w',0);
    m_CDantares.SetFloorTexture('+',0);

    m_CDantares.SetWallTexture('@',m_uiTextureCell);
    m_CDantares.SetWallTexture('x',-1);
    m_CDantares.SetWallTexture('$',m_uiTextureEnd);
    m_CDantares.SetWallTexture('&',m_uiTextureEnd);
    m_CDantares.SetWallTexture('*',-1);
    m_CDantares.SetWallTexture('!',m_uiTextureRobot);
    m_CDantares.SetWallTexture('|',m_uiTextureRobot);
    m_CDantares.SetWallTexture('Q',m_uiTextureRobot);
    m_CDantares.SetWallTexture('?',m_uiTextureRobot);
    m_CDantares.SetWallTexture('#',m_uiTextureWall);
    m_CDantares.SetWallTexture('%',m_uiTextureWall);
    m_CDantares.SetWallTexture('W',0);
    m_CDantares.SetWallTexture('+',0);

    return(true);
}

inline WPARAM CWINMAIN::MessageLoop()
{
    ////////////////////
    //Create variables//
    ////////////////////
    MSG Msg;

    while(1)
    {
        ZeroMemory(&Msg,sizeof(MSG));

        if(PeekMessage(&Msg,NULL,0,0,PM_REMOVE))
        {
            if(Msg.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }
        }

        if(m_bBackgroundProcessing)
        {
            //Set the refresh rate
            m_bBackgroundProcessing = false;

            SetTimer(m_hWnd,WMT_BACKGROUNDPROCESSING,CWMBP_REFRESHRATE,NULL);

            //Set OpenGL crap
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            //Clear the screen
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //Get m_uchKey[]
            if(FAILED(m_lpDIDe8Keyboard->GetDeviceState(sizeof(unsigned char)*256,
                                                        static_cast<LPVOID>(m_uchKey))))
            {
                m_lpDIDe8Keyboard->Unacquire();
                m_lpDIDe8Keyboard->Acquire();
            }

            //Background processing
            BackgroundProcessing();

            //Swap the front and back buffers
            SwapBuffers(m_hDC);
        }
    }

    return(Msg.wParam);
}

inline bool CWINMAIN::MusicLoad(const char *p_pszFile,IDirectMusicSegment8 **p_ppIDMSegment8,bool p_bFile,const char *p_pszType)
{
    //////////////////////////////////////
    //Check for invalid member variables//
    //////////////////////////////////////
    if(m_hInstance == NULL || m_pIDMLoader8 == NULL || m_pIDMPerformance8 == NULL)
    {
        return(false);
    }

    ////////////////////////////////
    //Check for invalid parameters//
    ////////////////////////////////
    if(p_ppIDMSegment8 == NULL)
    {
        return(false);
    }

    ////////////////////////////
    //Create **p_ppIDMSegment8//
    ////////////////////////////
    if(p_bFile)
    {
        //Create variables
        char  szCrap[MAX_PATH]  = "",
              szPart[MAX_PATH]  = "";
        WCHAR wszName[MAX_PATH],
              wszPath[MAX_PATH];

        //Get wszName[]
        _splitpath(p_pszFile,NULL,NULL,szPart,NULL);
        strcat(szCrap,szPart);

        _splitpath(p_pszFile,NULL,NULL,NULL,szPart);
        strcat(szCrap,szPart);

        if(MultiByteToWideChar(CP_ACP,0,szCrap,-1,wszName,MAX_PATH) == 0)
        {
            return(false);
        }

        //Get wszPath[]
        strcpy(szCrap,"");

        _splitpath(p_pszFile,szPart,NULL,NULL,NULL);
        strcat(szCrap,szPart);

        _splitpath(p_pszFile,NULL,szPart,NULL,NULL);
        strcat(szCrap,szPart);

        if(MultiByteToWideChar(CP_ACP,0,szCrap,-1,wszPath,MAX_PATH) == 0)
        {
            return(false);
        }

        //Set search directory to wszPath[]
        if(FAILED(m_pIDMLoader8->SetSearchDirectory(GUID_DirectMusicAllTypes,wszPath,FALSE)))
        {
            return(false);
        }

        //Get **p_ppIDMSegment8
        if(FAILED(m_pIDMLoader8->LoadObjectFromFile(CLSID_DirectMusicSegment,
                                                    IID_IDirectMusicSegment8,wszName,
                                                    reinterpret_cast<LPVOID *>(p_ppIDMSegment8))))
        {
            return(false);
        }
    }
    else
    {
        //Create variables
        DMUS_OBJECTDESC DMUS_ObjectDesc;
        HGLOBAL         hGlobal         = NULL;
        HRSRC           hRsrc           = NULL;

        //Get hRsrc
        if((hRsrc = FindResource(m_hInstance,p_pszFile,p_pszType)) == NULL)
        {
            return(false);
        }

        //Get hGlobal
        if((hGlobal = LoadResource(m_hInstance,hRsrc)) == NULL)
        {
            return(false);
        }

        //Set DMUS_ObjectDesc
        ZeroMemory(&DMUS_ObjectDesc,sizeof(DMUS_OBJECTDESC));
        DMUS_ObjectDesc.dwSize      = sizeof(DMUS_OBJECTDESC);
        DMUS_ObjectDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY;
        DMUS_ObjectDesc.guidClass   = CLSID_DirectMusicSegment;
        DMUS_ObjectDesc.llMemLength = SizeofResource(m_hInstance,hRsrc);
        DMUS_ObjectDesc.pbMemData   = static_cast<LPBYTE>(LockResource(hGlobal));

        //Get **p_ppIDMSegment8
        if(FAILED(m_pIDMLoader8->GetObject(&DMUS_ObjectDesc,IID_IDirectMusicSegment8,
                                           reinterpret_cast<LPVOID *>(p_ppIDMSegment8))))
        {
            return(false);
        }
    }

    return(true);
}

inline LRESULT CALLBACK WINMAIN::CWINMAIN::WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
            {
                m_bBackgroundProcessing = false;

                PostQuitMessage(0);

                return(0);
            }break;

        case WM_KEYDOWN:
            {
                switch(wParam)
                {
                    case VK_CANCEL:
                    case VK_ESCAPE:
                        {
                            SendMessage(hwnd,WM_CLOSE,0,0);

                            return(0);
                        }break;

                    case VK_END:
                        {
                            m_pIDMPerformance8->Stop(NULL,NULL,0,0);

                            return(0);
                        }break;

                    case VK_HOME:
                        {
                            m_pIDMPerformance8->PlaySegment(m_pIDMSt8Matrix,0,0,NULL);

                            return(0);
                        }break;
                }
            }break;

        case WM_TIMER:
            {
                switch(wParam)
                {
                    case WMT_BACKGROUNDPROCESSING:
                        {
                            m_bBackgroundProcessing = true;

                            return(0);
                        }break;

                    case WMT_ROBOTDELAY:
                        {
                            unsigned long ulRobot = 0;

                            for(; ulRobot < CWMRS_MAXIMUM; ulRobot++)
                            {
                                if(!m_CRobots[ulRobot].Move())
                                {
                                    break;
                                }
                            }

                            //Originally, Move() changed the square for you, but I didn't find
                            //out until later that when a robot went through another robot that
                            //it killed it...
                            for(ulRobot = 0; ulRobot < CWMRS_MAXIMUM; ulRobot++)
                            {
                                if(!m_CRobots[ulRobot].Draw())
                                {
                                    break;
                                }
                            }

                            SetTimer(hwnd,WMT_ROBOTDELAY,m_ulRobotDelay,NULL);

                            return(0);
                        }break;
                }
            }break;
    }

    return(DefWindowProc(hwnd,uMsg,wParam,lParam));
}

/*****************************
*#ifndef WINMAIN_H_NO_CLASSES*
*****************************/
#endif

/******************
*namespace WINMAIN*
*{                *
******************/
#ifndef WINMAIN_H_NO_NAMESPACES

}

#endif

/******************
*#ifndef WINMAIN_H*
******************/
#endif
