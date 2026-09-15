/*
    birdman.c
    Birdman for OS/2 Presentation Manager - version 1.0

    Original: (C) 1998 Mike, Vienna
    OS2World port: (C) 2026, OS2World
    License: BSD 3-Clause
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

#define BIRD_SIZE 128L   /* rendered bird size in pixels (sprite sheet frames are 64x64) */

static const char bldlevel[] =
    "@#Mike, Vienna:1.1#@##1## 14 Sep 2026 00:00:00      "
    "ARCAOS:::0::::@@Birdman for OS/2\r\n\x1a";

/* ===== Language Table ===== */

int current_lang = LANG_EN;

const char *lang_strings[LANG_COUNT][STR_COUNT] = {
    /* English */
    { "~Game", "~New Game\tCtrl+N", "~Pause Game\tCtrl+P", "~Quit Game\tCtrl+Q",
      "E~xit\tCtrl+X", "~Options", "~Language", "~Save settings on exit",
      "~Background Run\tCtrl+B", "~Frame Controls\tCtrl+F", "~Help", "~About..." },
    /* Spanish */
    { "~Juego", "~Nuevo Juego\tCtrl+N", "~Pausar Juego\tCtrl+P", "~Salir Juego\tCtrl+Q",
      "~Salir\tCtrl+X", "~Opciones", "~Idioma", "~Guardar al salir",
      "~Fondo Activo\tCtrl+B", "~Controles Marco\tCtrl+F", "~Ayuda", "~Acerca de..." },
    /* Dutch */
    { "~Spel", "~Nieuw Spel\tCtrl+N", "~Pauze Spel\tCtrl+P", "~Stop Spel\tCtrl+Q",
      "~Afsluiten\tCtrl+X", "~Opties", "~Taal", "~Instellingen opslaan",
      "~Achtergrond Actief\tCtrl+B", "~Kader Besturing\tCtrl+F", "~Help", "~Over..." },
    /* German */
    { "~Spiel", "~Neues Spiel\tCtrl+N", "~Pause\tCtrl+P", "~Spiel Beenden\tCtrl+Q",
      "~Beenden\tCtrl+X", "~Optionen", "~Sprache", "~Einstellungen speichern",
      "~Hintergrundlauf\tCtrl+B", "~Rahmen Steuerung\tCtrl+F", "~Hilfe", "~Ueber..." },
    /* French */
    { "~Jeu", "~Nouveau Jeu\tCtrl+N", "~Pause Jeu\tCtrl+P", "~Quitter Jeu\tCtrl+Q",
      "~Quitter\tCtrl+X", "~Options", "~Langue", "~Sauver au depart",
      "~Arriere-plan Actif\tCtrl+B", "~Controles Cadre\tCtrl+F", "~Aide", "~A propos..." },
    /* Italian */
    { "~Gioco", "~Nuovo Gioco\tCtrl+N", "~Pausa Gioco\tCtrl+P", "~Esci Gioco\tCtrl+Q",
      "~Esci\tCtrl+X", "~Opzioni", "~Lingua", "~Salva all'uscita",
      "~Sfondo Attivo\tCtrl+B", "~Controlli Cornice\tCtrl+F", "~Aiuto", "~Informazioni..." }
};

/* ===== Global Data ===== */

typedef struct {
    INT  iState;
    LONG x;
    LONG y;
} BIRDSTATE, *PBIRDSTATE;

PBIRDSTATE pBirdState   = NULL;
HAB        hab          = NULLHANDLE;
HPS        hpsMemory    = NULLHANDLE;
HPS        hpsWindow    = NULLHANDLE;
HDC        hdcMemory    = NULLHANDLE;
HWND       hwndSaver    = NULLHANDLE;
HWND       hwndFrame    = NULLHANDLE;
HWND       hwndTitleBar = NULLHANDLE;
HWND       hwndSysMenu  = NULLHANDLE;
HWND       hwndMinMax   = NULLHANDLE;
HWND       hwndMenuBar  = NULLHANDLE;
HPOINTER   hPointer     = NULLHANDLE;
HBITMAP    hBmp         = NULLHANDLE;

PSZ  pszClassName = "WC_BIRDMAN";

LONG screenSizeX  = 960L;
LONG screenSizeY  = 720L;
LONG killed       = 0L;
LONG survived     = 0L;

BOOL bGame        = FALSE;
BOOL bPaused      = FALSE;
BOOL bBackgrnd    = FALSE;
BOOL bFrameHidden = FALSE;
BOOL bSaveOnExit  = FALSE;
BOOL bFocusPaused = FALSE;

/* ===== Settings ===== */

#define SETTINGS_FILE "BIRDMAN.CFG"

typedef struct {
    int saveonexit;
    int detaillevel;
    int current_lang;
} SETTINGS;

static void load_settings( void ) {
    SETTINGS s;
    FILE *f;
    s.saveonexit  = 0;
    s.detaillevel = 0;
    s.current_lang = LANG_EN;
    f = fopen( SETTINGS_FILE, "rb" );
    if ( f ) {
        fread( &s, sizeof(s), 1, f );
        fclose( f );
    }
    if ( s.saveonexit < 0 || s.saveonexit > 1 ) s.saveonexit = 0;
    if ( s.current_lang < 0 || s.current_lang >= LANG_COUNT ) s.current_lang = LANG_EN;
    bSaveOnExit  = (BOOL)s.saveonexit;
    current_lang = s.current_lang;
}

static void save_settings( void ) {
    SETTINGS s;
    FILE *f;
    s.saveonexit  = (int)bSaveOnExit;
    s.detaillevel = 0;
    s.current_lang = current_lang;
    f = fopen( SETTINGS_FILE, "wb" );
    if ( f ) {
        fwrite( &s, sizeof(s), 1, f );
        fclose( f );
    }
}

/* ===== Language ===== */

static void set_language( HWND hMenu, int lang ) {
    MENUITEM mi;
    HWND hSub;
    int i;

    if ( lang < 0 || lang >= LANG_COUNT ) lang = LANG_EN;
    current_lang = lang;

    /* Top-level menu labels */
    WinSendMsg( hMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_GAME),
                MPFROMP(tr(STR_MENU_GAME)) );
    WinSendMsg( hMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_OPT),
                MPFROMP(tr(STR_MENU_OPTIONS)) );
    WinSendMsg( hMenu, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_HELP),
                MPFROMP(tr(STR_MENU_HELP)) );

    /* Game submenu items */
    memset( &mi, 0, sizeof(mi) );
    WinSendMsg( hMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_GAME, TRUE), MPFROMP(&mi) );
    hSub = mi.hwndSubMenu;
    if ( hSub ) {
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_NEW),
                    MPFROMP(tr(STR_MENU_NEW)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_PAUSE),
                    MPFROMP(tr(STR_MENU_PAUSE)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_QUIT),
                    MPFROMP(tr(STR_MENU_QUIT)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_EXIT),
                    MPFROMP(tr(STR_MENU_EXIT)) );
    }

    /* Options submenu items */
    memset( &mi, 0, sizeof(mi) );
    WinSendMsg( hMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_OPT, TRUE), MPFROMP(&mi) );
    hSub = mi.hwndSubMenu;
    if ( hSub ) {
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SUBMENU_LANG),
                    MPFROMP(tr(STR_MENU_LANGUAGE)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_SAVEONEXIT),
                    MPFROMP(tr(STR_MENU_SAVEONEXIT)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_BACKGRND),
                    MPFROMP(tr(STR_MENU_BACKGRND)) );
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_FRAME),
                    MPFROMP(tr(STR_MENU_FRAME)) );
    }

    /* Help submenu items */
    memset( &mi, 0, sizeof(mi) );
    WinSendMsg( hMenu, MM_QUERYITEM, MPFROM2SHORT(IDM_SUBMENU_HELP, TRUE), MPFROMP(&mi) );
    hSub = mi.hwndSubMenu;
    if ( hSub ) {
        WinSendMsg( hSub, MM_SETITEMTEXT, MPFROMSHORT(IDM_ABOUT),
                    MPFROMP(tr(STR_MENU_ABOUT)) );
    }

    /* Checkmark selected language */
    for ( i = IDM_LANG_EN; i <= IDM_LANG_IT; i++ )
        WinCheckMenuItem( hMenu, i, (i - IDM_LANG_EN) == lang );
}

/* ===== About Dialog ===== */

MRESULT EXPENTRY AboutDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {
    switch ( msg ) {
        case WM_COMMAND:
            switch ( COMMANDMSG(&msg)->cmd ) {
                case DID_OK:
                case DID_CANCEL:
                    WinDismissDlg( hwnd, TRUE );
                    return (MRESULT)0L;
            }
    }
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

/* ===== Game Helpers ===== */

static void update_title( void ) {
    CHAR pc[ 128 ];
    if ( killed + survived == 0L ) {
        WinSetWindowText( hwndFrame, "Birdman" );
    } else {
        sprintf( pc, "Birdman: %ld killed, %ld survived ... %ld%%",
                 killed, survived,
                 100L * killed / ( killed + survived ) );
        WinSetWindowText( hwndFrame, pc );
    }
}

static void start_game( HWND hwnd ) {
    INT iLoop, birdCount;

    if ( bGame ) {
        WinStopTimer( hab, hwnd, IDT_MOVE );
    }
    killed       = 0L;
    survived     = 0L;
    bPaused      = FALSE;
    bFocusPaused = FALSE;
    bGame        = TRUE;

    if ( pBirdState ) {
        birdCount = (INT)( screenSizeX / BIRD_SIZE + 2L );
        for ( iLoop = 0; iLoop < birdCount; iLoop++ ) {
            pBirdState[ iLoop ].iState = rand() % 4;
            pBirdState[ iLoop ].x = (LONG)( rand() % (INT)( screenSizeX + 2L * BIRD_SIZE ) ) - BIRD_SIZE;
            pBirdState[ iLoop ].y = (LONG)( rand() / (float)RAND_MAX * ( screenSizeY - BIRD_SIZE ) );
        }
    }

    /* Clear the screen before birds start drawing */
    WinInvalidateRect( hwnd, NULL, TRUE );
    WinUpdateWindow( hwnd );

    WinStartTimer( hab, hwnd, IDT_MOVE, 30L );
    update_title();
    WinCheckMenuItem( hwndMenuBar, IDM_PAUSE, FALSE );
}

static void pause_game( HWND hwnd, BOOL bFocus ) {
    WinStopTimer( hab, hwnd, IDT_MOVE );
    bPaused = TRUE;
    if ( bFocus ) bFocusPaused = TRUE;
    WinCheckMenuItem( hwndMenuBar, IDM_PAUSE, TRUE );
}

static void resume_game( HWND hwnd, BOOL bFocus ) {
    WinStartTimer( hab, hwnd, IDT_MOVE, 30L );
    bPaused = FALSE;
    if ( bFocus ) bFocusPaused = FALSE;
    WinCheckMenuItem( hwndMenuBar, IDM_PAUSE, FALSE );
}

static void quit_game( HWND hwnd ) {
    if ( !bGame ) return;
    WinStopTimer( hab, hwnd, IDT_MOVE );
    bGame        = FALSE;
    bPaused      = FALSE;
    bFocusPaused = FALSE;
    killed       = 0L;
    survived     = 0L;
    WinInvalidateRect( hwnd, NULL, TRUE );
    WinUpdateWindow( hwnd );
    WinSetWindowText( hwndFrame, "Birdman" );
    WinCheckMenuItem( hwndMenuBar, IDM_PAUSE, FALSE );
}

/* ===== Window Procedure ===== */

MRESULT EXPENTRY SaverWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 ) {

    MRESULT mResult = (MRESULT)0L;

    switch ( msg ) {

        case WM_CREATE: {
            SIZEL        sizel = { 0L, 0L };
            DEVOPENSTRUC devopenstruc = { NULL, "DISPLAY", NULL, NULL, NULL, NULL, NULL, NULL, NULL };
            INT          iLoop, birdCount;

            hpsWindow = WinGetPS( hwnd );
            if ( (hdcMemory = DevOpenDC( hab, OD_MEMORY, "*", 9L,
                                         (PDEVOPENDATA)(PVOID)&devopenstruc,
                                         NULLHANDLE )) == DEV_ERROR ) {
                DosBeep( 3333L, 100L );
                break;
            }
            if ( (hpsMemory = GpiCreatePS( hab, hdcMemory, &sizel,
                                           GPIT_MICRO | GPIF_DEFAULT |
                                           GPIA_ASSOC | PU_PELS )) == GPI_ERROR ) {
                DosBeep( 3333L, 100L );
                break;
            }
            if ( (hBmp = GpiLoadBitmap( hpsMemory, NULLHANDLE, IDR_BMP_BIRD, 0L, 0L )) == NULLHANDLE ) {
                DosBeep( 3333L, 100L );
                break;
            }

            birdCount = (INT)( screenSizeX / BIRD_SIZE + 2L );
            if ( (pBirdState = (PBIRDSTATE)calloc( (ULONG)birdCount, sizeof(BIRDSTATE) )) == NULL ) {
                DosBeep( 3333L, 100L );
                break;
            }
            GpiSetBitmap( hpsMemory, hBmp );
            hPointer = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, IDR_PTR_GUN );
            for ( iLoop = 0; iLoop < birdCount; iLoop++ ) {
                pBirdState[ iLoop ].iState = 6;
                pBirdState[ iLoop ].x = ( iLoop - 1 ) * BIRD_SIZE;
                pBirdState[ iLoop ].y = 0L;
            }
            /* Timer not started here; started on IDM_NEW */
            break;
        }

        case WM_PAINT: {
            HPS   hPresentationSpace;
            RECTL rectlUpdate;
            hPresentationSpace = WinBeginPaint( hwnd, NULLHANDLE, &rectlUpdate );
            WinFillRect( hPresentationSpace, &rectlUpdate, CLR_BLACK );
            WinEndPaint( hPresentationSpace );
            break;
        }

        case WM_TIMER: {
            if ( SHORT1FROMMP(mp1) == IDT_MOVE ) {
                INT     iLoop, birdCount;
                POINTL  aptlBitBlt[ 4 ];  /* 4 points for scaled blit */

                if ( pBirdState == NULL ) break;
                birdCount = (INT)( screenSizeX / BIRD_SIZE + 2L );
                for ( iLoop = 0; iLoop < birdCount; iLoop++ ) {
                    INT iState = pBirdState[ iLoop ].iState;
                    INT iBlit  = 1;

                    /* destination: BIRD_SIZE x BIRD_SIZE */
                    aptlBitBlt[ 0 ].x = pBirdState[ iLoop ].x;
                    aptlBitBlt[ 0 ].y = pBirdState[ iLoop ].y;
                    aptlBitBlt[ 1 ].x = aptlBitBlt[ 0 ].x + BIRD_SIZE;
                    aptlBitBlt[ 1 ].y = aptlBitBlt[ 0 ].y + BIRD_SIZE;
                    /* source: 64x64 frame in sprite sheet */
                    aptlBitBlt[ 2 ].x = 64L * iState;
                    aptlBitBlt[ 2 ].y = 0L;
                    aptlBitBlt[ 3 ].x = aptlBitBlt[ 2 ].x + 64L;
                    aptlBitBlt[ 3 ].y = 64L;

                    if ( iState < 3 ) {
                        iState++;
                    } else if ( iState == 3 ) {
                        iState = 0; aptlBitBlt[ 2 ].x = 64L * 1L; aptlBitBlt[ 3 ].x = 64L * 2L;
                    } else if ( iState == 4 ) {
                        iState = 5; aptlBitBlt[ 2 ].x = 64L * 3L; aptlBitBlt[ 3 ].x = 64L * 4L;
                    } else if ( iState == 5 ) {
                        iState = 6; aptlBitBlt[ 2 ].x = 64L * 4L; aptlBitBlt[ 3 ].x = 64L * 5L;
                    } else {
                        iBlit = 0;
                    }

                    if ( iBlit ) {
                        GpiBitBlt( hpsWindow, hpsMemory, 4L, aptlBitBlt,
                                   ROP_SRCCOPY, BBO_IGNORE );
                    }

                    if ( pBirdState[ iLoop ].x <= -BIRD_SIZE ) {
                        if ( iState < 4 ) {
                            survived++;
                            update_title();
                        }
                        iState = rand() % 4;
                        pBirdState[ iLoop ].x = screenSizeX + BIRD_SIZE;
                        pBirdState[ iLoop ].y = (LONG)( rand() / (float)RAND_MAX *
                                                         ( screenSizeY - BIRD_SIZE ) );
                    } else {
                        pBirdState[ iLoop ].x -= 4;
                        pBirdState[ iLoop ].y += ( rand() > (RAND_MAX / 2) ) ? 1L : -1L;
                        if ( pBirdState[ iLoop ].y < 0L ) pBirdState[ iLoop ].y = 0L;
                        if ( pBirdState[ iLoop ].y > screenSizeY - BIRD_SIZE )
                            pBirdState[ iLoop ].y = screenSizeY - BIRD_SIZE;
                    }
                    pBirdState[ iLoop ].iState = iState;
                }
            } else {
                mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            }
            break;
        }

        case WM_MOUSEMOVE: {
            if ( hPointer ) {
                WinSetPointer( HWND_DESKTOP, hPointer );
                mResult = MPFROMSHORT( FALSE );
            } else {
                mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            }
            break;
        }

        case WM_BUTTON1DOWN: {
            if ( pBirdState && bGame && !bPaused ) {
                LONG x, y;
                INT  iLoop, birdCount;

                x = SHORT1FROMMP( mp1 );
                y = SHORT2FROMMP( mp1 );
                birdCount = (INT)( screenSizeX / BIRD_SIZE + 2L );

                for ( iLoop = 0; iLoop < birdCount; iLoop++ ) {
                    if (   ( pBirdState[ iLoop ].iState < 4 )
                        && ( ( pBirdState[ iLoop ].x + 22L ) < x )
                        && ( ( pBirdState[ iLoop ].y + 22L ) < y )
                        && ( ( pBirdState[ iLoop ].x + 110L ) > x )
                        && ( ( pBirdState[ iLoop ].y + 110L ) > y ) ) {
                        pBirdState[ iLoop ].iState = 4;
                        killed++;
                        update_title();
                        break;
                    }
                }
            }
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_BUTTON2DBLCLK: {
            if ( pBirdState && bGame && !bPaused ) {
                INT iLoop, birdCount;
                birdCount = (INT)( screenSizeX / BIRD_SIZE + 2L );
                for ( iLoop = 0; iLoop < birdCount; iLoop++ ) {
                    if ( pBirdState[ iLoop ].iState < 4 ) {
                        pBirdState[ iLoop ].iState = 4;
                        killed++;
                    }
                }
                update_title();
            }
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_SETFOCUS: {
            if ( SHORT1FROMMP(mp2) ) {
                /* Gaining focus */
                if ( bFocusPaused && bGame ) {
                    resume_game( hwnd, TRUE );
                }
            } else {
                /* Losing focus */
                if ( !bBackgrnd && bGame && !bPaused ) {
                    pause_game( hwnd, TRUE );
                }
            }
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        case WM_COMMAND: {
            USHORT usCmd = SHORT1FROMMP( mp1 );

            switch ( usCmd ) {

                case IDM_NEW:
                    srand( (unsigned)WinGetCurrentTime(hab) );
                    start_game( hwnd );
                    break;

                case IDM_PAUSE:
                    if ( !bGame ) break;
                    if ( !bPaused ) {
                        pause_game( hwnd, FALSE );
                    } else {
                        resume_game( hwnd, FALSE );
                    }
                    break;

                case IDM_QUIT:
                    quit_game( hwnd );
                    break;

                case IDM_EXIT:
                    WinPostMsg( hwnd, WM_CLOSE, MPFROMLONG(0L), MPFROMLONG(0L) );
                    break;

                case IDM_SAVEONEXIT:
                    bSaveOnExit = !bSaveOnExit;
                    WinCheckMenuItem( hwndMenuBar, IDM_SAVEONEXIT, bSaveOnExit );
                    break;

                case IDM_BACKGRND:
                    bBackgrnd = !bBackgrnd;
                    WinCheckMenuItem( hwndMenuBar, IDM_BACKGRND, bBackgrnd );
                    break;

                case IDM_FRAME: {
                    if ( !bFrameHidden ) {
                        WinSetParent( hwndTitleBar, HWND_OBJECT, FALSE );
                        WinSetParent( hwndSysMenu,  HWND_OBJECT, FALSE );
                        WinSetParent( hwndMinMax,   HWND_OBJECT, FALSE );
                        WinSetParent( hwndMenuBar,  HWND_OBJECT, FALSE );
                        bFrameHidden = TRUE;
                    } else {
                        WinSetParent( hwndTitleBar, hwndFrame, FALSE );
                        WinSetParent( hwndSysMenu,  hwndFrame, FALSE );
                        WinSetParent( hwndMinMax,   hwndFrame, FALSE );
                        WinSetParent( hwndMenuBar,  hwndFrame, FALSE );
                        bFrameHidden = FALSE;
                    }
                    WinSendMsg( hwndFrame, WM_UPDATEFRAME,
                                MPFROMLONG(FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX | FCF_MENU),
                                NULL );
                    WinInvalidateRect( hwndFrame, NULL, TRUE );
                    WinUpdateWindow( hwndFrame );
                    /* Note: when frame is hidden hwndMenuBar is HWND_OBJECT child;
                       WinCheckMenuItem still finds it via the stored handle */
                    WinCheckMenuItem( hwndMenuBar, IDM_FRAME, bFrameHidden );
                    break;
                }

                case IDM_LANG_EN:
                case IDM_LANG_ES:
                case IDM_LANG_NL:
                case IDM_LANG_DE:
                case IDM_LANG_FR:
                case IDM_LANG_IT:
                    set_language( hwndMenuBar, usCmd - IDM_LANG_EN );
                    break;

                case IDM_ABOUT:
                    WinDlgBox( HWND_DESKTOP, hwndFrame, AboutDlgProc,
                               NULLHANDLE, IDD_ABOUT, NULL );
                    break;

                default:
                    mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
                    break;
            }
            break;
        }

        case WM_DESTROY: {
            if ( bGame ) {
                WinStopTimer( hab, hwnd, IDT_MOVE );
            }
            free( pBirdState );
            pBirdState = NULL;
            if ( hPointer != NULLHANDLE ) {
                WinDestroyPointer( hPointer );
                hPointer = NULLHANDLE;
            }
            GpiSetBitmap( hpsMemory, NULLHANDLE );
            GpiDeleteBitmap( hBmp );
            hBmp = NULLHANDLE;
            GpiDestroyPS( hpsMemory );
            hpsMemory = NULLHANDLE;
            DevCloseDC( hdcMemory );
            hdcMemory = NULLHANDLE;
            WinReleasePS( hpsWindow );
            hpsWindow = NULLHANDLE;
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;
        }

        default:
            mResult = WinDefWindowProc( hwnd, msg, mp1, mp2 );
            break;

    } /* endswitch */

    return mResult;
}

/* ===== Main ===== */

INT main( INT argc, CHAR *argv[], CHAR *envp[] ) {
    HMQ   hmq;
    QMSG  qmsg;
    ULONG flFrameFlags;
    RECTL rectl;
    LONG  cxScreen, cyScreen, winW, winH, x, y;

    (VOID)argc; (VOID)argv; (VOID)envp;
    (VOID)bldlevel;

    hab = WinInitialize( 0 );
    hmq = WinCreateMsgQueue( hab, 0 );

    srand( (unsigned)WinGetCurrentTime(hab) );

    load_settings();

    cxScreen = WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
    cyScreen = WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

    WinRegisterClass( hab, pszClassName, SaverWindowProc, 0L, 0L );

    flFrameFlags = FCF_TASKLIST | FCF_SYSMENU | FCF_TITLEBAR |
                   FCF_MINBUTTON | FCF_MENU | FCF_ACCELTABLE | FCF_DLGBORDER;

    hwndFrame = WinCreateStdWindow( HWND_DESKTOP,
                                    0L,
                                    &flFrameFlags,
                                    pszClassName,
                                    "Birdman",
                                    0L,
                                    NULLHANDLE,
                                    ID_RESOURCE,
                                    &hwndSaver );

    if ( hwndFrame == NULLHANDLE ) {
        WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                       "Failed to create main window.",
                       "Error", 0, MB_OK | MB_MOVEABLE );
        WinDestroyMsgQueue( hmq );
        WinTerminate( hab );
        return 1;
    }

    /* Capture frame control handles */
    hwndTitleBar = WinWindowFromID( hwndFrame, FID_TITLEBAR );
    hwndSysMenu  = WinWindowFromID( hwndFrame, FID_SYSMENU );
    hwndMinMax   = WinWindowFromID( hwndFrame, FID_MINMAX );
    hwndMenuBar  = WinWindowFromID( hwndFrame, FID_MENU );

    /* Apply persisted language */
    set_language( hwndMenuBar, current_lang );

    /* Apply persisted settings */
    WinCheckMenuItem( hwndMenuBar, IDM_SAVEONEXIT, bSaveOnExit );

    /* Size window to 640x480 client area, centered */
    rectl.xLeft   = 0L;
    rectl.xRight  = screenSizeX;
    rectl.yBottom = 0L;
    rectl.yTop    = screenSizeY;
    WinCalcFrameRect( hwndFrame, &rectl, FALSE );
    winW = rectl.xRight  - rectl.xLeft;
    winH = rectl.yTop    - rectl.yBottom;
    x    = ( cxScreen - winW ) / 2L;
    y    = ( cyScreen - winH ) / 2L;
    if ( x < 0L ) x = 0L;
    if ( y < 0L ) y = 0L;

    WinSetWindowPos( hwndFrame, HWND_TOP, x, y, winW, winH,
                     SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW );

    /* Message loop */
    while ( WinGetMsg( hab, &qmsg, 0, 0, 0 ) ) {
        WinDispatchMsg( hab, &qmsg );
    }

    if ( bSaveOnExit ) save_settings();

    WinDestroyWindow( hwndSaver );
    WinDestroyWindow( hwndFrame );
    WinDestroyMsgQueue( hmq );
    WinTerminate( hab );
    return 0;
}
