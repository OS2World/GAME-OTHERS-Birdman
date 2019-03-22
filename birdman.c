/*
    birdman.c
    birdman saver module C source file version 0.01

    (C) 1998 Mike, Vienna
*/

#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "birdman.h"
// ===== prototypes

BOOL EXPENTRY StartScreenSaver( HAB _hab, HWND hwndParent );
VOID EXPENTRY StopScreenSaver( VOID );
MRESULT EXPENTRY SaverWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
VOID Update( HWND hwnd );

INT  _CRT_init( VOID );
VOID _CRT_term( VOID );

// ===== global data

typedef struct {
    INT    iState;
    LONG   x;
    LONG   y;
} BIRDSTATE , *PBIRDSTATE;
PBIRDSTATE pBirdState = NULL;

HMODULE  hModuleDLL = NULLHANDLE;             // saver module dll handle
HAB      hab        = NULLHANDLE;             // anchor block handle
HPS      hpsMemory  = NULLHANDLE;
HPS      hpsWindow  = NULLHANDLE;
HDC      hdcMemory  = NULLHANDLE;
HWND     hwndSaver  = NULLHANDLE;
HPOINTER hPointer   = NULLHANDLE;
HBITMAP  hBmp       = NULLHANDLE;
PSZ      pszClassName    = "WC_SCREEN_SAVER";
LONG     screenSizeX, screenSizeY;
LONG     killed = 0;
LONG     survived = 0;

//============================================================================================================
BOOL EXPENTRY StartScreenSaver( HAB _hab, HWND hwndParent ) {
//============================================================================================================

    RECTL rectl;

    hab = _hab;
    srand( WinGetCurrentTime( hab ));

    WinQueryWindowRect( hwndParent, &rectl );
    screenSizeX = rectl.xRight;
    screenSizeY = rectl.yTop - 23;

    WinRegisterClass( hab, pszClassName, SaverWindowProc, 0L, 0L );
    hwndSaver = WinCreateWindow( hwndParent     // Parent-window handle
                               , pszClassName   // Registered-class name
                               , NULL           // Window text
                               , WS_VISIBLE     // Window style
                               , 0L             // x-coordinate of window position
                               , 23L             // y-coordinate of window position
                               , screenSizeX    // Width of window, in window coordinates
                               , screenSizeY    // Height of window, in window coordinates
                               , HWND_DESKTOP   // Owner-window handle
                               , HWND_TOP       // Sibling-window handle
                               , 17+4           // Window identifier
                               , NULL           // Pointer to control data
                               , NULL );        // Presentation parameters
    return (BOOL)( hwndSaver != NULLHANDLE );
}

//============================================================================================================
VOID EXPENTRY StopScreenSaver( VOID ) {
//============================================================================================================

    if ( hwndSaver != NULLHANDLE ) {
        WinDestroyWindow( hwndSaver );
        hwndSaver = NULLHANDLE;
    } /* endif */
}

//============================================================================================================
ULONG _DLL_InitTerm( HMODULE hModule, ULONG flag ) {
//============================================================================================================

    ULONG ulReturn = 1L;

    switch( flag ) {

        case 0 :  // initializing DLL
            hModuleDLL = hModule;
            if ( _CRT_init() == -1 ) {
                ulReturn = 0L;
            } /* endif */
            break;

        case 1 :  // terminating DLL
            _CRT_term();
            break;

        default : // return error
            ulReturn = 0L;
            break;

    } /* endswitch */

    return ulReturn;
}

//    SaverWindowProc
//
//    This is the window procedure of the screen-size window that is
//    created when the saver starts.

//============================================================================================================
MRESULT EXPENTRY SaverWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
//============================================================================================================

    MRESULT mResult;

    switch ( msg ) {

        case WM_CREATE : {

            INT          iLoop;
            SIZEL        sizel = { 0L, 0L };      // use same page size as device
            DEVOPENSTRUC devopenstruc = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };

            hpsWindow = WinGetPS( hwnd );
            if (( hdcMemory = DevOpenDC( hab
                                       , OD_MEMORY
                                       , "*"
                                       , 9L
                                       , (PDEVOPENDATA)(PVOID)&devopenstruc
                                       , NULLHANDLE )) == DEV_ERROR ) {
                DosBeep( 3333L, 100L );
            } else if (( hpsMemory = GpiCreatePS( hab
                                                , hdcMemory
                                                , &sizel
                                                ,   GPIT_MICRO
                                                  | GPIF_DEFAULT
                                                  | GPIA_ASSOC
                                                  | PU_PELS )) == GPI_ERROR ) {
                DosBeep( 3333L, 100L );
            } else if (( hBmp = GpiLoadBitmap( hpsMemory, hModuleDLL, IDR_BMP_BIRD, 0L, 0L )) == NULLHANDLE ) {
                DosBeep( 3333L, 100L );
            } else if (( pBirdState = (PBIRDSTATE)calloc( (ULONG)( screenSizeX / 64L + 2L ), sizeof( BIRDSTATE ))) == NULL ) {
                DosBeep( 3333L, 100L );
            } else {
                GpiSetBitmap( hpsMemory, hBmp );
                hPointer = WinLoadPointer( HWND_DESKTOP, hModuleDLL, IDR_PTR_GUN );
                for ( iLoop = 0; iLoop < ( screenSizeX / 64L + 2L ); iLoop++ ) {
                    pBirdState[ iLoop ].iState = 6;
                    pBirdState[ iLoop ].x = ( iLoop - 1 ) * 64L;
                    pBirdState[ iLoop ].y = 0L;
                } /* endfor */
                WinStartTimer( hab, hwnd, IDT_MOVE, 30L );
            } /* endif */
            mResult = MRFROMSHORT( FALSE );
            break;
        }

        case WM_PAINT : {

            HPS   hPresentationSpace;
            RECTL rectlUpdate;

            hPresentationSpace = WinBeginPaint( hwnd, NULLHANDLE, &rectlUpdate );
            WinFillRect( hPresentationSpace, &rectlUpdate, CLR_BLACK );
            WinEndPaint( hPresentationSpace );
            mResult = MRFROMLONG( 0L );
            break;
        }

        case WM_TIMER : {

            if ( SHORT1FROMMP( mp1 ) == IDT_MOVE ) {

                INT     iLoop;
                POINTL  aptlBitBlt[ 3 ];

                if ( pBirdState == NULL ) break;
                for ( iLoop = 0; iLoop < ( screenSizeX / 64L + 2L ); iLoop++ ) {
                    INT iState = pBirdState[ iLoop ].iState;
                    INT iBlit = 1;
                    aptlBitBlt[ 0 ].x = pBirdState[ iLoop ].x;
                    aptlBitBlt[ 0 ].y = pBirdState[ iLoop ].y;
                    aptlBitBlt[ 1 ].x = aptlBitBlt[ 0 ].x + 64L;
                    aptlBitBlt[ 1 ].y = aptlBitBlt[ 0 ].y + 64L;
                    aptlBitBlt[ 2 ].x = 64L * iState;
                    aptlBitBlt[ 2 ].y = 0L;
                    if ( iState < 3 ) {
                        iState++;
                    } else if ( iState == 3 ) {
                        iState = 0; aptlBitBlt[ 2 ].x = 64L * 1L;
                    } else if ( iState == 4 ) {
                        iState = 5; aptlBitBlt[ 2 ].x = 64L * 3L;
                    } else if ( iState == 5 ) {
                        iState = 6; aptlBitBlt[ 2 ].x = 64L * 4L;
                    } else {
                        iBlit = 0;
                    } /* endif */
                    if ( iBlit ) {
                        GpiBitBlt( hpsWindow     //  Target presentation-space handle
                                 , hpsMemory     //  Source presentation-space handle
                                 , 3L            //  Point count
                                 , aptlBitBlt    //  Point array
                                 , ROP_SRCCOPY   //  Mixing function required
                                 , BBO_IGNORE ); //  Options
                    } /* endif */
                    if ( pBirdState[ iLoop ].x <= -64L ) {
                        if ( iState < 4 ) {
                            survived++;
                            Update( hwnd );
                        } /* endif */
                        iState = rand() % 4;
                        pBirdState[ iLoop ].x = screenSizeX + 64L;
                        pBirdState[ iLoop ].y = (LONG)( rand() / (float)RAND_MAX * ( screenSizeY - 64L ));
                    } else {
                        pBirdState[ iLoop ].x -= 4;
                        pBirdState[ iLoop ].y += ( rand() > ( RAND_MAX / 2 )) ? 1L : -1L;
                        pBirdState[ iLoop ].y = min( max( pBirdState[ iLoop ].y, 0L ), screenSizeY - 64L );
                    } /* endif */
                    pBirdState[ iLoop ].iState = iState;
                } /* endfor */
                mResult = MRFROMLONG( 0L );
            } else {
                mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            } /* endif */
            break;
        }

        case WM_MOUSEMOVE : {

            if ( hPointer ) {
                WinSetPointer( HWND_DESKTOP, hPointer );
                mResult = MPFROMSHORT( FALSE );
            } else {
                mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            } /* endif */
            break;
        }

        case WM_BUTTON1DOWN : {

            if ( pBirdState ) {

                LONG x, y;
                INT  iLoop;

                x = SHORT1FROMMP( mp1 );
                y = SHORT2FROMMP( mp1 );

                for ( iLoop = 0; iLoop < ( screenSizeX / 64L + 2L ); iLoop++ ) {
                    if (    (  pBirdState[ iLoop ].iState < 4 )
                         && (( pBirdState[ iLoop ].x + 11L ) < x )
                         && (( pBirdState[ iLoop ].y + 11L ) < y )
                         && (( pBirdState[ iLoop ].x + 55L ) > x )
                         && (( pBirdState[ iLoop ].y + 55L ) > y )) {
                        pBirdState[ iLoop ].iState = 4;
                        killed++;
                        Update( hwnd );
                        break;
                    } /* endif */
                } /* endfor */
            } /* endif */
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_BUTTON2DBLCLK : {

            if ( pBirdState ) {

                INT  iLoop;

                for ( iLoop = 0; iLoop < ( screenSizeX / 64L + 2L ); iLoop++ ) {
                    if ( pBirdState[ iLoop ].iState < 4 ) {
                        pBirdState[ iLoop ].iState = 4;
                        killed++;
                    } /* endif */
                } /* endfor */
                Update( hwnd );
            } /* endif */
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_DESTROY : {

            WinStopTimer( hab, hwnd, IDT_MOVE );
            free( pBirdState );
            pBirdState = NULL;
            WinDestroyPointer( hPointer );
            hPointer = NULLHANDLE;
            GpiSetBitmap( hpsMemory, NULLHANDLE );
            GpiDeleteBitmap( hBmp );
            hBmp = NULLHANDLE;
            GpiDestroyPS( hpsMemory );
            hpsMemory = NULLHANDLE;
            DevCloseDC( hdcMemory );
            hdcMemory = NULLHANDLE;
            GpiDestroyPS( hpsWindow );
            hpsWindow = NULLHANDLE;
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_COMMAND : {

            USHORT usCommand = SHORT1FROMMP( mp1 );
            switch ( usCommand ) {
                case ID_MENU_EXIT :
                case ID_KEY_EXIT : {
                    WinPostMsg( hwnd, WM_CLOSE, MPFROMLONG( 0L ), MPFROMLONG( 0L ));
                    break;
                }
                default : {
                    break;
                }
            }
            mResult = MRFROMLONG( 0L );
            break;
        }

        default : {

            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }
    } /* endswitch */

    return mResult;
}

VOID Update( HWND hwnd ) {
    CHAR pc[ 128 ];
    sprintf( pc, "Birdman: %d killed, %d survived ... %d%%", killed, survived, 100 * killed / ( killed + survived ) );
    WinSetWindowText( WinWindowFromID( WinQueryWindow( hwnd, QW_PARENT ), FID_TITLEBAR ), pc );
}

#ifdef _EXE_

INT main( INT argc, CHAR *argv[], CHAR *envp[] ) {

    HMQ   hmq;
    QMSG  qmsg;
    ULONG flFrameFlags    = FCF_TITLEBAR | FCF_SYSMENU | FCF_DLGBORDER |
                            FCF_MINBUTTON | FCF_TASKLIST | FCF_MENU |
                            FCF_ACCELTABLE;
    HWND  hwndFrame;
    RECTL rectl;

    hab = WinInitialize( 0 );
    hmq = WinCreateMsgQueue( hab, 0 );

    srand( WinGetCurrentTime( hab ));
    screenSizeX = 640L;    // screen size x
    screenSizeY = 480L;    // screen size y

    WinRegisterClass( hab, pszClassName, SaverWindowProc, 0L, 0L );
    hwndFrame = WinCreateStdWindow( HWND_DESKTOP
                                  , 0
                                  , &flFrameFlags
                                  , pszClassName
                                  , "Birdman"
                                  , 0
                                  , NULLHANDLE
                                  , ID_RESOURCE
                                  , &hwndSaver );
    rectl.xLeft   = 0L;
    rectl.xRight  = screenSizeX; 
    rectl.yBottom = 0L;
    rectl.yTop    = screenSizeY;
    WinCalcFrameRect( hwndFrame, &rectl, FALSE );
    WinSetWindowPos( hwndFrame
                   , NULLHANDLE
                   , 10, 10, rectl.xRight - rectl.xLeft, rectl.yTop - rectl.yBottom
                   , SWP_SIZE | SWP_MOVE | SWP_SHOW | SWP_ACTIVATE );
    while ( WinGetMsg( hab, &qmsg, 0, 0, 0 )) {
        WinDispatchMsg( hab, &qmsg );
    } /* endwhile */

    WinDestroyWindow( hwndSaver );
    WinDestroyWindow( hwndFrame );
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );
    return 0;
}

#endif
