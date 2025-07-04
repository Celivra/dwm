#include <X11/XF86keysym.h>

/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 20;       /* vert inner gap between windows */
static const unsigned int gappoh    = 20;       /* horiz outer gap between windows and screen edge */ 
static const unsigned int gappov    = 20;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
static const char *fonts[]          = { "SauceCodePro Nerd Font:size=13",
										"Noto Sans CJK TC:size=13",
										"JoyPixels:pixelsize=13"};
static const char dmenufont[]       = "SauceCodePro Nerd Font Mono:size=14";
static const char col_gray1[]       = "#222222";//狀態條色
static const char col_gray2[]       = "#444444";//外邊框顏色
static const char col_gray3[]       = "#bbbbbb";//當前非活動的title顏色
static const char col_gray4[]       = "#ffffff";//活動的title顏色
static const char col_cyan[]        = "#37474F";//title底色
static const char col_border[]        = "#80EEFF";//窗口邊框色
static const unsigned int baralpha = 0xd0;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_border  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_border  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
//static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
//static const char *tags[] = { "\uf120", /*"\uf7ae"*/"󰹖", "\uf121", "\uf04b", "\ue62e", "\uf251", "\ue727", "\uf537", "\uf684" };
static const char *tags[] = { "壹", "貳", "叁", "肆", "伍", "陸", "柒", "捌", "玖" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class             instance     title       tags mask     isfloating   monitor */
	{ "Gimp",              NULL,       NULL,       0,            1,           -1 },
	{ "Android Emulator",  NULL,       NULL,       0,            1,           -1 },
	{ "Emulator", 		   NULL,       NULL,       0,            1,           -1 },
	{ "quemu-system-i386", NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  		   NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "Tile",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
//static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *dmenucmd[] = {"rofi", "-show", "drun", NULL};
static const char *termcmd[]  = { "alacritty", NULL };
static const char *browsercmd[]  = { "google-chrome-stable", NULL };
static const char *torbrowsercmd[]  = { "/home/xiaozhai/Documents/tor-browser/Browser/start-tor-browser", NULL };
static const char *backgroundcmd[] = {"nitrogen", NULL};

static const char *upvol[]   = { "amixer","-c","0","set", "Master", "5%+",  NULL };
static const char *downvol[] = { "amixer","-c","0","set", "Master", "5%-",  NULL };
static const char *blueupvol[]   = { "/home/xiaozhai/Documents/script/bluetoothUp.sh",  NULL };
static const char *bluedownvol[] = { "/home/xiaozhai/Documents/script/bluetoothDown.sh",  NULL };
static const char *mutevol[] = { "amixer","sset", "Master", "mute",  NULL };
static const char *unmutevol[] = { "amixer","sset", "Master", "unmute",  NULL };
static const char *unmutespeak[] = { "amixer","sset", "Speaker", "unmute",  NULL };
static const char *touchpadup[] = { "xinput","set-prop", "11", "156","1",  NULL };
static const char *touchpaddown[] = { "xinput","set-prop", "11", "156","0",  NULL };
/*
static const char *lightup[] = { "light", "-A", "10",  NULL };
static const char *lightdown[] = { "light", "-U", "10",  NULL };
*/
static const char *lightup[] = { "/home/xiaozhai/Documents/script/lightup.sh",  NULL };
static const char *lightdown[] = { "/home/xiaozhai/Documents/script/lightdown.sh",  NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "alacritty", "-t", scratchpadname, "-g", "80x24", NULL };
static const char *screenshotcmd[] = { "flameshot", "gui", NULL };

static const char *manbo[] = { "/home/xiaozhai/Documents/script/playMusic/manbo.sh ",  NULL };
static const char *mbhaha[] = { "/home/xiaozhai/Documents/script/playMusic/mbha.sh ",  NULL };


static Key keys[] = {
	/* modifier            key                      function        argument */
	{ MODKEY,              XK_s,                    spawn,          {.v = dmenucmd } },
	{ MODKEY,              XK_Return,               spawn,          {.v = termcmd } },
	{ MODKEY,              XK_c,                    spawn,          {.v = browsercmd } },
	{ MODKEY,              XK_t,                    spawn,          {.v = torbrowsercmd } },
	{ MODKEY,    	       XK_g,                    spawn,          {.v = backgroundcmd} },
	{ 0,                   XF86XK_AudioLowerVolume, spawn,          {.v = downvol } },
	{ 0,                   XF86XK_AudioRaiseVolume, spawn,          {.v = upvol   } },
	{ 0|ShiftMask,         XF86XK_AudioLowerVolume, spawn,          {.v = bluedownvol } },
	{ 0|ShiftMask,         XF86XK_AudioRaiseVolume, spawn,          {.v = blueupvol   } },
	{ 0,                   XF86XK_AudioMute,        spawn,          {.v = mutevol } },
	{ 0|MODKEY,            XF86XK_AudioMute,        spawn,          {.v = unmutevol } },
	{ 0|ShiftMask,         XF86XK_AudioMute,        spawn,          {.v = unmutespeak} },
	{ 0,		           XF86XK_MonBrightnessUp,	spawn,          {.v = lightup} },
	{ 0,		           XF86XK_MonBrightnessDown,spawn,          {.v = lightdown} },
	{ MODKEY,              XK_bracketleft,          spawn,          {.v = downvol } },
	{ MODKEY,              XK_backslash,            spawn,          {.v = mutevol } },
	{ MODKEY,              XK_bracketright,         spawn,          {.v = upvol   } },
	{ 0,                   XK_Print,                spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,    XK_e,                    rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_u,                    rotatestack,    {.i = -1 } },
	{ MODKEY,              XK_e,                    focusstack,     {.i = +1 } },
	{ MODKEY,              XK_u,                    focusstack,     {.i = -1 } },
	{ MODKEY,              XK_n,                    viewtoleft,     {0} },
	{ MODKEY,              XK_i,                    viewtoright,    {0} },
	{ MODKEY|ShiftMask,    XK_n,                    tagtoleft,      {0} },
	{ MODKEY|ShiftMask,    XK_i,                    tagtoright,     {0} },
	{ MODKEY|ShiftMask,    XK_h,                    incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_l,                    incnmaster,     {.i = -1 } },
	{ MODKEY,              XK_h,                    setmfact,       {.f = -0.04} },
	{ MODKEY,              XK_l,                    setmfact,       {.f = +0.04} },
	{ MODKEY,              XK_k,                    hidewin,        {0} },
	{ MODKEY|ShiftMask,    XK_k,                    restorewin,     {0} },
	{ MODKEY,              XK_o,                    hideotherwins,  {0}},
	{ MODKEY|ShiftMask,    XK_o,                    restoreotherwins, {0}},
	{ MODKEY|ShiftMask,    XK_Return,               zoom,           {0} },
	{ MODKEY,              XK_Tab,                  view,           {0} },
	{ MODKEY|ShiftMask,    XK_q,                    killclient,     {0} },
	{ MODKEY,              XK_t,                    setlayout,      {.v = &layouts[0]} },
	{ MODKEY,              XK_m,                    setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,    XK_f,                    fullscreen,     {0} },
	{ MODKEY,              XK_space,                setlayout,      {0} },
	{ MODKEY|ShiftMask,    XK_space,                togglefloating, {0} },
	{ MODKEY,              XK_0,                    view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_0,                    tag,            {.ui = ~0 } },
	{ MODKEY,              XK_comma,                focusmon,       {.i = -1 } },
	{ MODKEY,              XK_period,               focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,               tagmon,         {.i = +1 } },
	TAGKEYS(               XK_1,                      0)
	TAGKEYS(               XK_2,                      1)
	TAGKEYS(               XK_3,                      2)
	TAGKEYS(               XK_4,                      3)
	TAGKEYS(               XK_5,                      4)
	TAGKEYS(               XK_6,                      5)
	TAGKEYS(               XK_7,                      6)
	TAGKEYS(               XK_8,                      7)
	TAGKEYS(               XK_9,                      8)
	{ MODKEY|ControlMask,  XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	//{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkStatusText,        0,              Button2,        NULL,           {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
