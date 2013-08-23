
#include "mythuiguidegrid.h"

// ANSI C headers
#include <cmath>

// C++ headers
#include <algorithm>
using namespace std;

// Qt headers
#include <QFile>
#include <QDomElement>

// myth
#include "mythfontproperties.h"
#include "mythuihelper.h"
#include "x11colors.h"
#include "mythverbose.h"
#include "mythimage.h"
#include "mythuitype.h"
#include "mythmainwindow.h"
#if 1
#include "mythdb.h"
#endif

#define LOC QString("MythUIGuideGrid: ")
#define LOC_ERR QString("MythUIGuideGrid, Error: ")

MythUIGuideGrid::MythUIGuideGrid(MythUIType *parent, const QString &name)
               : MythUIType(parent, name)
{
    // themeable setting defaults
    m_channelCount = 5;
    m_timeCount = 4;
    m_verticalLayout = false;

    m_font = new MythFontProperties();
    m_justification = Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap;
    m_multilineText = true;
    m_cutdown = true;

    m_selType = "box";
    m_selLineColor = QColor();
    m_selFillColor = QColor();
    m_drawSelLine = false;
    m_drawSelFill = false;

    for (uint x = 0; x < RECSTATUSSIZE; x++)
        m_recImages[x] = NULL;

    for (uint x = 0; x < ARROWIMAGESIZE; x++)
        m_arrowImages[x] = NULL;

    m_fillType = Solid;

    m_rowCount = 0;
    m_progPastCol = 0;
#if 0
    m_drawCategoryColors = GetMythDB()->GetNumSetting("EPGShowCategoryColors", 1);
#else
	m_drawCategoryColors = 1;
#endif
    m_drawCategoryText = true;
    m_categoryAlpha = 255;

    QMap<QString, QString> catColors;
    parseDefaultCategoryColors(catColors);
    SetCategoryColors(catColors);

}

void MythUIGuideGrid::Finalize(void)
{
    m_rowCount = m_channelCount;

    allData = new QList<UIGTCon*>[m_rowCount];

    MythUIType::Finalize();
}

MythUIGuideGrid::~MythUIGuideGrid()
{
    for (int i = 0; i < m_rowCount; i++)
        ResetRow(i);

    delete [] allData;

    delete m_font;
    m_font = NULL;

    for (uint x = 0; x < RECSTATUSSIZE; x++)
    {
        if (m_recImages[x])
            m_recImages[x]->DownRef();
    }

    for (uint x = 0; x < ARROWIMAGESIZE; x++)
    {
        if (m_arrowImages[x])
            m_arrowImages[x]->DownRef();
    }
}

bool MythUIGuideGrid::ParseElement(
    const QString &filename, QDomElement &element, bool showWarnings)
{
    if (element.tagName() == "layout")
    {
        QString layout = getFirstText(element).toLower();
        m_verticalLayout = (layout == "vertical");
    }
    else if (element.tagName() == "channels")
    {
        m_channelCount = getFirstText(element).toInt();
        m_channelCount = max(m_channelCount, 1);
        m_channelCount = min(m_channelCount, 12);
    }
    else if (element.tagName() == "timeslots")
    {
        m_timeCount = getFirstText(element).toInt();
        m_timeCount = max(m_timeCount, 1);
        m_timeCount = min(m_timeCount, 8);
    }
    else if (element.tagName() == "solidcolor")
    {
        QString color = getFirstText(element);
        m_solidColor = QColor(color);
    }
    else if (element.tagName() == "selector")
    {
        m_selType = element.attribute("type");
        QString lineColor = element.attribute("linecolor", "");
        QString fillColor = element.attribute("fillcolor", "");

        if (!lineColor.isEmpty())
        {
            m_selLineColor = QColor(lineColor);
            m_drawSelLine = true;
        }
        else
        {
            m_drawSelLine = false;
        }

        if (!fillColor.isEmpty())
        {
            m_selFillColor = QColor(fillColor);
            m_drawSelFill = true;
        }
        else
        {
            m_drawSelFill = false;
        }
   }
    else if (element.tagName() == "recordingcolor")
    {
        QString color = getFirstText(element);
        m_recordingColor = QColor(color);
    }
    else if (element.tagName() == "conflictingcolor")
    {
        QString color = getFirstText(element);
        m_conflictingColor = QColor(color);
    }
    else if (element.tagName() == "categoryalpha")
    {
        m_categoryAlpha = getFirstText(element).toInt();
        m_categoryAlpha = max(m_categoryAlpha, 1);
        m_categoryAlpha = min(m_categoryAlpha, 255);
    }
    else if (element.tagName() == "showcategories")
    {
        m_drawCategoryText = parseBool(element);
    }
    else if (element.tagName() == "cutdown")
    {
        m_cutdown = parseBool(element);
    }
    else if (element.tagName() == "multiline")
    {
        m_multilineText = parseBool(element);
    }
    else if (element.tagName() == "textoffset")
    {
        m_textOffset =  parsePoint(element);
    }
    else if (element.tagName() == "font")
    {
        QString fontname = getFirstText(element);
        MythFontProperties *font = GetFont(fontname);
        if (!font)
            font = GetGlobalFontMap()->GetFont(fontname);

        if (font)
        {
            MythFontProperties fontcopy = *font;
            int screenHeight = GetMythMainWindow()->GetUIScreenRect().height();
            fontcopy.Rescale(screenHeight);
            int fontStretch = GetMythUI()->GetFontStretch();
            fontcopy.AdjustStretch(fontStretch);
            *m_font = fontcopy;
        }
        else
            VERBOSE(VB_IMPORTANT, LOC_ERR + "Unknown font: " + fontname);
    }
    else if (element.tagName() == "recordstatus")
    {
        int inttype = 0;
        QString typ = element.attribute("type");
        QString img = element.attribute("image");

        if (typ == "SingleRecord")
            inttype = 1;
        else if (typ == "TimeslotRecord")
            inttype = 2;
        else if (typ == "ChannelRecord")
            inttype = 3;
        else if (typ == "AllRecord")
            inttype = 4;
        else if (typ == "WeekslotRecord")
            inttype = 5;
        else if (typ == "FindOneRecord")
            inttype = 6;
        else if (typ == "OverrideRecord")
            inttype = 7;

        LoadImage(inttype, img);
    }
    else if (element.tagName() == "arrow")
    {
        QString dir = element.attribute("direction");
        QString image = element.attribute("image");

        if (dir == "left")
            SetArrow(0, image);
        else if (dir == "right")
            SetArrow(1, image);
        else if (dir == "up")
            SetArrow(2, image);
        else if (dir == "down")
            SetArrow(3, image);
    }
    else
    {
        return MythUIType::ParseElement(filename, element, showWarnings);
    }

    return true;
}

void MythUIGuideGrid::CopyFrom(MythUIType *base)
{
    MythUIGuideGrid *gg = dynamic_cast<MythUIGuideGrid *>(base);
    if (!gg)
    {
        VERBOSE(VB_IMPORTANT, LOC_ERR + "bad parsing");
        return;
    }

    m_channelCount = gg->m_channelCount;
    m_timeCount = gg->m_timeCount;
    m_verticalLayout = gg->m_verticalLayout;
    m_categoryAlpha = gg->m_categoryAlpha;
    m_textOffset = gg->m_textOffset;
    m_justification = gg->m_justification;
    m_multilineText = gg->m_multilineText;
    *m_font = *gg->m_font;
    m_solidColor = gg->m_solidColor;

    m_selType = gg->m_selType;
    m_selLineColor = gg->m_selLineColor;
    m_selFillColor = gg->m_selFillColor;
    m_drawSelLine = gg->m_drawSelLine;
    m_drawSelFill = gg->m_drawSelFill;

    m_recordingColor = gg->m_recordingColor;
    m_conflictingColor = gg->m_conflictingColor;

    m_fillType = gg->m_fillType;
    m_cutdown = gg->m_cutdown;
    m_drawCategoryColors = gg->m_drawCategoryColors;
    m_drawCategoryText = gg->m_drawCategoryText;

    MythUIType::CopyFrom(base);
}

void MythUIGuideGrid::CreateCopy(MythUIType *parent)
{
    MythUIGuideGrid *gg = new MythUIGuideGrid(parent, objectName());
    gg->CopyFrom(this);
}

QColor MythUIGuideGrid::calcColor(const QColor &color, int alphaMod)
{
    QColor newColor(color);
    newColor.setAlpha((int)(color.alpha() * (alphaMod / 255.0)));
    return newColor;
}

QColor MythUIGuideGrid::calcColor(const QColor &color, int alpha, int alphaMod)
{
    QColor newColor(color);
    newColor.setAlpha((int)(alpha * (alphaMod / 255.0)));
    return newColor;
}

void MythUIGuideGrid::DrawSelf(MythPainter *p, int xoffset, int yoffset,
                               int alphaMod, QRect clipRect)
{
    for (int i = 0; i < m_rowCount; i++)
    {
        QList<UIGTCon*>::iterator it = allData[i].begin();
        for (; it != allData[i].end(); ++it)
        {
            UIGTCon *data = *it;
            if (data->recStat == 0)
                drawBackground(p, data, alphaMod);
            else if (data->recStat == 1)
                drawBox(p, data, m_recordingColor, alphaMod);
            else
                drawBox(p, data, m_conflictingColor, alphaMod);
        }
    }

    drawCurrent(p, &selectedItem, alphaMod);

    for (int i = 0; i < m_rowCount; i++)
    {
        QList<UIGTCon*>::iterator it = allData[i].begin();
        for (; it != allData[i].end(); ++it)
        {
            UIGTCon *data = *it;
            drawText(p, data, alphaMod);

            if (data->recType != 0 || data->arrow != 0)
                drawRecType(p, data, alphaMod);
        }
    }
}

void MythUIGuideGrid::drawCurrent(MythPainter *p, UIGTCon *data, int alphaMod)
{
    int breakin = 2;
    QRect area = data->drawArea;
    area.translate(m_Area.x(), m_Area.y());
    area.adjust(breakin, breakin, -breakin, -breakin);
    int status = data->recStat;

    if (m_selType == "roundbox")
    {
        QColor fillColor = calcColor(m_selFillColor, alphaMod);
        QColor lineColor = calcColor(m_selLineColor, alphaMod);

        if (status == 1)
            lineColor = m_recordingColor;
        else if (status == 2)
            lineColor = m_conflictingColor;

        p->DrawRoundRect(area, 10, m_drawSelFill, fillColor,
                         m_drawSelLine, 2, lineColor);
    }
    else if (m_selType == "highlight")
    {
        QColor fillColor;
        QColor lineColor = calcColor(m_selFillColor, alphaMod);

        if (m_drawCategoryColors && data->categoryColor.isValid())
            fillColor = calcColor(data->categoryColor, m_categoryAlpha, alphaMod);
        else
            fillColor = calcColor(m_solidColor, m_categoryAlpha, alphaMod);

        if (status == 1)
            lineColor = m_recordingColor;
        else if (status == 2)
            lineColor = m_conflictingColor;

        p->DrawRect(area, true, fillColor.lighter(), m_drawSelLine, 2, lineColor);
    }
    else
    {
        // default to "box" selection type
        QColor fillColor = calcColor(m_selFillColor, alphaMod);
        QColor lineColor = calcColor(m_selLineColor, alphaMod);

        if (status == 1)
            lineColor = m_recordingColor;
        else if (status == 2)
            lineColor = m_conflictingColor;

        p->DrawRect(area, m_drawSelFill, fillColor, m_drawSelLine, 2, lineColor);
    }
}

void MythUIGuideGrid::drawRecType(MythPainter *p, UIGTCon *data, int alphaMod)
{
    int breakin = 1;
    QRect area = data->drawArea;
    area.translate(m_Area.x(), m_Area.y());
    area.adjust(breakin, breakin, -breakin, -breakin);

    // draw arrows
    if (data->arrow != 0)
    {
        if (data->arrow == 1 || data->arrow == 3)
        {
            if (m_verticalLayout)
            {
                p->DrawImage(area.left() + (area.width() / 2) - (m_arrowImages[2]->width() / 2),
                             area.top() , m_arrowImages[2], alphaMod);
            }
            else
            {
                p->DrawImage(area.left(), area.top() + (area.height() / 2) -
                            (m_arrowImages[0]->height() / 2), m_arrowImages[0], alphaMod);
            }
        }
        if (data->arrow == 2 || data->arrow == 3)
        {
            if (m_verticalLayout)
            {
                p->DrawImage(area.left() + (area.width() / 2) - (m_arrowImages[3]->width() / 2),
                            area.top() + area.height() - m_arrowImages[3]->height(), m_arrowImages[3], alphaMod);
            }
            else
            {
                p->DrawImage(area.right() - m_arrowImages[1]->width(),
                            area.top() + (area.height() / 2) -
                            (m_arrowImages[1]->height() / 2), m_arrowImages[1], alphaMod);
            }
        }
    }

    // draw recording status
    if (data->recType != 0 && m_recImages[data->recType])
    {
        p->DrawImage(area.right() - m_recImages[data->recType]->width(),
                     area.bottom() - m_recImages[data->recType]->height(),
                     m_recImages[data->recType], alphaMod);
    }
}

void MythUIGuideGrid::drawBox(MythPainter *p, UIGTCon *data, const QColor &color, int alphaMod)
{
    int breakin = 1;
    QRect area = data->drawArea;
    area.translate(m_Area.x(), m_Area.y());
    area.adjust(breakin, breakin, -breakin, -breakin);

    QColor fillColor = calcColor(color, m_categoryAlpha, alphaMod);
    p->DrawRect(area, true, fillColor, false, 0, QColor());
}

void MythUIGuideGrid::drawBackground(MythPainter *p, UIGTCon *data, int alphaMod)
{
    QColor overColor;
    QRect overArea;

    int breakin = 1;
    QRect area = data->drawArea;
    area.translate(m_Area.x(), m_Area.y());
    QColor fillColor;

    if (m_drawCategoryColors && data->categoryColor.isValid())
        fillColor = calcColor(data->categoryColor, m_categoryAlpha, alphaMod);
    else
        fillColor = calcColor(m_solidColor, m_categoryAlpha, alphaMod);

    if (m_verticalLayout)
    {
        if (m_progPastCol && area.top() < m_progPastCol)
        {
            if (area.bottom() < m_progPastCol)
            {
                fillColor = fillColor.dark();
                area.adjust(breakin, breakin, -breakin, -breakin);
            }
            else
            {
                overColor = fillColor.dark();
                int first = m_progPastCol - area.top();
                int second = area.height() - first;
                overArea = area;
                overArea.setHeight(first);
                area.translate(0, first);
                area.setHeight(second);

                area.adjust(0, -breakin, -breakin, -breakin);
                overArea.adjust(0, breakin, -breakin, -breakin);
            }
        }
        else
            area.adjust(breakin, breakin, -breakin, -breakin);
    }
    else
    {
        if (m_progPastCol && area.left() < m_progPastCol)
        {
            if (area.right() < m_progPastCol)
            {
                fillColor = fillColor.dark();
                area.adjust(breakin, breakin, -breakin, -breakin);
            }
            else
            {
                overColor = fillColor.dark();
                int first = m_progPastCol - area.left();
                int second = area.width() - first;
                overArea = area;
                overArea.setWidth(first);
                area.translate(first, 0);
                area.setWidth(second);

                area.adjust(0, breakin, -breakin, -breakin);
                overArea.adjust(breakin, breakin, 0, -breakin);
            }
        }
        else
            area.adjust(breakin, breakin, -breakin, -breakin);
    }

    if (area.width() <= 1)
        area.setWidth(2);
    if (area.height() <= 1)
        area.setHeight(2);

    p->DrawRect(area, true, fillColor, false, 0, QColor());
    if (overArea.width() > 0)
        p->DrawRect(overArea, true, overColor, false, 0, QColor());
}

void MythUIGuideGrid::drawText(MythPainter *p, UIGTCon *data, int alphaMod)
{
    QString msg = data->title;

    if (m_drawCategoryText && !data->category.isEmpty())
        msg += QString(" (%1)").arg(data->category);

    QRect area = data->drawArea;
    area.translate(m_Area.x(), m_Area.y());
    area.adjust(m_textOffset.x(), m_textOffset.y(),
                -m_textOffset.x(), -m_textOffset.y());

    if (m_verticalLayout)
    {
        if (data->arrow == 1 || data->arrow == 3)
            area.setTop(area.top() + m_arrowImages[2]->height());
        if (data->arrow == 2 || data->arrow == 3)
            area.setBottom(area.bottom() - m_arrowImages[3]->height());
    }
    else
    {
        if (data->arrow == 1 || data->arrow == 3)
            area.setLeft(area.left() + m_arrowImages[0]->width());
        if (data->arrow == 2 || data->arrow == 3)
            area.setRight(area.right() - m_arrowImages[1]->width());
    }

    if (area.width() <= 0 || area.height() <= 0)
        return;

    p->DrawText(area, msg, m_justification, *m_font, alphaMod, area);
}

void MythUIGuideGrid::SetProgramInfo(int row, int col, const QRect &area,
                                 const QString &title, const QString &genre,
                                 int arrow, int recType, int recStat,
                                 bool selected)
{
    (void)col;
    UIGTCon *data = new UIGTCon(area, title, genre, arrow, recType, recStat);
    allData[row].append(data);

    if (m_drawCategoryColors)
    {
        data->categoryColor = categoryColors[data->category.toLower()];
        if (!data->categoryColor.isValid())
            data->categoryColor = categoryColors["none"];
    }

    if (selected)
        selectedItem = *data;
}

bool MythUIGuideGrid::parseDefaultCategoryColors(QMap<QString, QString> &catColors)
{
    QFile f;
    QStringList searchpath = GetMythUI()->GetThemeSearchPath();
    for (QStringList::const_iterator ii = searchpath.begin();
        ii != searchpath.end(); ++ii)
    {
        f.setFileName(*ii + "categories.xml");
        if (f.open(QIODevice::ReadOnly))
            break;
    }
    if (f.handle() == -1)
    {
        VERBOSE(VB_IMPORTANT, LOC_ERR + QString("Unable to open '%1'")
                .arg(f.fileName()));
        return false;
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;

    if (!doc.setContent(&f, false, &errorMsg, &errorLine, &errorColumn))
    {
        VERBOSE(VB_IMPORTANT, LOC_ERR +
                QString("Parsing colors: %1 at line: %2 column: %3")
                .arg(f.fileName()).arg(errorLine).arg(errorColumn) +
                QString("\n\t\t\t%1").arg(errorMsg));
        f.close();
        return false;
    }

    f.close();

    QDomElement element = doc.documentElement();
    for (QDomNode child = element.firstChild(); !child.isNull();
         child = child.nextSibling())
    {
        QDomElement info = child.toElement();
        if (!info.isNull() && info.tagName() == "catcolor")
        {
            QString cat = info.attribute("category");
            QString col = info.attribute("color");

            catColors[cat.toLower()] = col;
        }
    }

    return true;
}

void MythUIGuideGrid::SetCategoryColors(const QMap<QString, QString> &catC)
{
    for (QMap<QString, QString>::const_iterator it = catC.begin();
         it != catC.end(); ++it)
    {
        categoryColors[it.key()] = createColor(*it);
    }
}

void MythUIGuideGrid::LoadImage(int recType, const QString &file)
{
    QString themeDir = GetMythUI()->GetThemeDir();
    QString filename = themeDir + file;

    QPixmap *pix = GetMythUI()->LoadScalePixmap(filename);

    if (pix)
    {
        m_recImages[recType] = GetPainter()->GetFormatImage();
        m_recImages[recType]->Assign(*pix);
        delete pix;
    }
}

void MythUIGuideGrid::SetArrow(int direction, const QString &file)
{
    QString themeDir = GetMythUI()->GetThemeDir();
    QString filename = themeDir + file;

    QPixmap *pix = GetMythUI()->LoadScalePixmap(filename);

    if (pix)
    {
        m_arrowImages[direction] = GetPainter()->GetFormatImage();
        m_arrowImages[direction]->Assign(*pix);
        delete pix;
    }
}

void MythUIGuideGrid::ResetData(void)
{
    for (int i = 0; i < m_rowCount; i++)
        ResetRow(i);
}

void MythUIGuideGrid::ResetRow(int row)
{
    while (!allData[row].empty())
    {
        delete allData[row].back();
        allData[row].pop_back();
    }
}

void MythUIGuideGrid::SetProgPast(int ppast)
{
    if (m_verticalLayout)
        m_progPastCol = m_Area.y() + (m_Area.height() * ppast / 100);
    else
        m_progPastCol = m_Area.x() + (m_Area.width() * ppast / 100);

    SetRedraw();
}
