/*
    birdman.h
    Birdman for OS/2 - resource IDs and language definitions

    Original: (C) 1998 Mike, Vienna
    OS2World port: (C) 2026, OS2World
    License: BSD 3-Clause
*/

/* Resource IDs */
#define IDR_PTR_GUN  110
#define IDR_BMP_BIRD 111
#define IDT_MOVE     ( TID_USERMAX - 77 )

/* Frame resource ID (menu + accelerator) */
#define ID_RESOURCE        200

/* Menu IDs - Game menu (100-199) */
#define IDM_NEW            101
#define IDM_PAUSE          102
#define IDM_QUIT           103
#define IDM_EXIT           104

/* Menu IDs - Options menu (200-299) */
#define IDM_SAVEONEXIT     201
#define IDM_BACKGRND       202
#define IDM_FRAME          203

/* Menu IDs - Language submenu (300-399) */
#define IDM_LANG_EN        300
#define IDM_LANG_ES        301
#define IDM_LANG_NL        302
#define IDM_LANG_DE        303
#define IDM_LANG_FR        304
#define IDM_LANG_IT        305

/* Menu IDs - Help menu (900-999) */
#define IDM_ABOUT          999

/* Submenu cascade IDs (1000-1099) */
#define IDM_SUBMENU_GAME   1001
#define IDM_SUBMENU_OPT    1002
#define IDM_SUBMENU_LANG   1003
#define IDM_SUBMENU_HELP   1004

/* Dialog IDs */
#define IDD_ABOUT          100

/* Language defines */
#define LANG_EN    0
#define LANG_ES    1
#define LANG_NL    2
#define LANG_DE    3
#define LANG_FR    4
#define LANG_IT    5
#define LANG_COUNT 6

/* String indices */
enum {
    STR_MENU_GAME = 0,
    STR_MENU_NEW,
    STR_MENU_PAUSE,
    STR_MENU_QUIT,
    STR_MENU_EXIT,
    STR_MENU_OPTIONS,
    STR_MENU_LANGUAGE,
    STR_MENU_SAVEONEXIT,
    STR_MENU_BACKGRND,
    STR_MENU_FRAME,
    STR_MENU_HELP,
    STR_MENU_ABOUT,
    STR_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][STR_COUNT];
#define tr(id) ((char*)lang_strings[current_lang][(id)])
