#include "myththemebase.h"
#include "mythuiimage.h"
#include "mythmainwindow.h"
#include "mythscreentype.h"
#include "xmlparsebase.h"
#include "mythfontproperties.h"
#include "mythfontmanager.h"

#include "mythdirs.h"
#if 1
#include "oldsettings.h"
#endif
#include "mythuihelper.h"

#include "mythverbose.h"

class MythThemeBasePrivate
{
  public:
    MythScreenStack *background;
    MythScreenType *backgroundscreen;

    MythUIImage *backimg; // just for now
};


MythThemeBase::MythThemeBase()
{
    d = new MythThemeBasePrivate();

    Init();
}

MythThemeBase::~MythThemeBase()
{
    GetGlobalFontManager()->ReleaseFonts("UI");
    GetGlobalFontManager()->ReleaseFonts("Shared");
    delete d;
}

void MythThemeBase::Reload(void)
{
	VERBOSE(VB_IMPORTANT, QString("MythThemeBase::Reload\n"));
    MythMainWindow *mainWindow = GetMythMainWindow();
    QRect uiSize = mainWindow->GetUIScreenRect();

    GetGlobalFontMap()->Clear();
    XMLParseBase::ClearGlobalObjectStore();
    GetGlobalFontManager()->ReleaseFonts("UI");
    GetGlobalFontManager()->LoadFonts(GetMythUI()->GetThemeDir(), "UI");
    XMLParseBase::LoadBaseTheme();

    d->background->PopScreen(false, true);

    d->backgroundscreen = new MythScreenType(d->background, "backgroundscreen");

    if (!XMLParseBase::CopyWindowFromBase("backgroundwindow",
                                          d->backgroundscreen))
    {
	   VERBOSE(VB_IMPORTANT, QString("set base background from file qtlook.txt\n"));
        QString backgroundname = GetMythUI()->qtconfig()->GetSetting("BackgroundPixmap");
        backgroundname = GetMythUI()->GetThemeDir() + backgroundname;

        d->backimg = new MythUIImage(backgroundname, d->backgroundscreen,"backimg");
        d->backimg->SetPosition(mainWindow->NormPoint(QPoint(0, 0)));
        d->backimg->SetSize(uiSize.width(), uiSize.height());
        d->backimg->Load();
    }
	else
	{
    	VERBOSE(VB_GENERAL, "\n-------------do not load base background\n");
	}
    d->background->AddScreen(d->backgroundscreen, false);
}

void MythThemeBase::Init(void)
{
	VERBOSE(VB_IMPORTANT, QString("MythThemeBase::Init\n"));
    MythMainWindow *mainWindow = GetMythMainWindow();
    QRect uiSize = mainWindow->GetUIScreenRect();

    d->background = new MythScreenStack(mainWindow, "background");
    d->background->DisableEffects();
	
	VERBOSE(VB_GENERAL, "\n-------------MythThemeBase::Init 1\n");
	
    GetGlobalFontManager()->LoadFonts(GetFontsDir(), "Shared");
    GetGlobalFontManager()->LoadFonts(GetMythUI()->GetThemeDir(), "UI");
	VERBOSE(VB_GENERAL, "\n-------------MythThemeBase::Init 1-1\n");
    XMLParseBase::LoadBaseTheme();
    d->backgroundscreen = new MythScreenType(d->background, "backgroundscreen");

	VERBOSE(VB_GENERAL, "\n-------------MythThemeBase::Init 2\n");
	
    if (!XMLParseBase::CopyWindowFromBase("backgroundwindow", 
                                          d->backgroundscreen))
    {
    		VERBOSE(VB_IMPORTANT, QString("set base background from file qtlook.txt\n"));
        QString backgroundname = GetMythUI()->qtconfig()->GetSetting("BackgroundPixmap");
        backgroundname = GetMythUI()->GetThemeDir() + backgroundname;

        d->backimg = new MythUIImage(backgroundname, d->backgroundscreen,
                                     "backimg");
        d->backimg->SetPosition(mainWindow->NormPoint(QPoint(0, 0)));
        d->backimg->SetSize(uiSize.width(), uiSize.height());
        d->backimg->Load();
    }
	else
	{
		VERBOSE(VB_GENERAL, "\n-------------do not load base background\n");
	}

    d->background->AddScreen(d->backgroundscreen, false);

    new MythScreenStack(mainWindow, "main stack", true);

    new MythScreenStack(mainWindow, "popup stack");
}

