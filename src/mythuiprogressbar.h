#ifndef MYTHUI_PROGRESSBAR_H_
#define MYTHUI_PROGRESSBAR_H_

#include "mythuitype.h"
#include "mythuiimage.h"
#include "mythuishape.h"

#include "mythverbose.h"

/** \class MythUIProgressBar
 *
 * \brief Progress bar widget.
 *
 * \ingroup MythUI_Widgets
 */
class MPUBLIC MythUIProgressBar : public MythUIType
{
  public:
    MythUIProgressBar(MythUIType *parent, const QString &name);
   ~MythUIProgressBar() {
       VERBOSE(VB_IMPORTANT, "~MythUIProgressBar()");
   }

    void Reset(void);

    enum LayoutType { LayoutVertical, LayoutHorizontal };
    enum EffectType { EffectReveal, EffectSlide, EffectAnimate };

    void SetStart(int);
    void SetUsed(int);
    void SetTotal(int);
    int  GetUsed(void) { return m_current; }

  protected:
    virtual bool ParseElement(
        const QString &filename, QDomElement &element, bool showWarnings);
    virtual void CopyFrom(MythUIType *base);
    virtual void CreateCopy(MythUIType *parent);
    virtual void Finalize(void);

    LayoutType m_layout;
    EffectType m_effect;

    int m_total;
    int m_start;
    int m_current;

    void CalculatePosition(void);
};

#endif
