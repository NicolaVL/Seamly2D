#ifndef VMODELINGNORMAL_H
#define VMODELINGNORMAL_H

#include <QSharedPointer>
#include "vmodelinglinepoint.h"
#include "dialogs/dialognormal.h"

class VModelingNormal : public VModelingLinePoint
{
    Q_OBJECT
public:
                                 VModelingNormal(VDomDocument *doc, VContainer *data, const qint64 &id,
                                             const QString &typeLine, const QString &formula,
                                             const qreal &angle, const qint64 &firstPointId,
                                             const qint64 &secondPointId, Tool::Enum typeCreation,
                                             QGraphicsItem * parent = 0);
    virtual void setDialog();
    static VModelingNormal* Create(QSharedPointer<DialogNormal> &dialog, VDomDocument *doc, VContainer *data);
    static VModelingNormal* Create(const qint64 _id, const QString &formula, const qint64 &firstPointId,
                                   const qint64 &secondPointId, const QString typeLine,
                                   const QString pointName, const qreal angle, const qreal &mx,
                                   const qreal &my, VDomDocument *doc, VContainer *data, Document::Enum parse,
                                   Tool::Enum typeCreation);
    static QPointF               FindPoint(const QPointF &firstPoint, const QPointF &secondPoint,
                                           const qreal &length, const qreal &angle = 0);
public slots:
    virtual void                 FullUpdateFromFile();
    virtual void                 FullUpdateFromGui(int result);
protected:
    virtual void                 contextMenuEvent ( QGraphicsSceneContextMenuEvent * event );
    virtual void                 AddToFile();
private:
    qint64                       secondPointId;
    QSharedPointer<DialogNormal> dialogNormal;
};

#endif // VMODELINGNORMAL_H
