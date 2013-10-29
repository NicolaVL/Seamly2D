#include "vtoolheight.h"

const QString VToolHeight::ToolType = QStringLiteral("height");

VToolHeight::VToolHeight(VDomDocument *doc, VContainer *data, const qint64 &id, const QString &typeLine,
                         const qint64 &basePointId, const qint64 &p1LineId, const qint64 &p2LineId,
                         Tool::Sources typeCreation, QGraphicsItem * parent)
    :VToolLinePoint(doc, data, id, typeLine, QString(), basePointId, 0, parent),
      dialogHeight(QSharedPointer<DialogHeight>()), p1LineId(p1LineId), p2LineId(p2LineId){
    ignoreFullUpdate = true;
    if(typeCreation == Tool::FromGui){
        AddToFile();
    }
}

void VToolHeight::setDialog(){
    Q_ASSERT(!dialogHeight.isNull());
    VPointF p = VAbstractTool::data.GetPoint(id);
    dialogHeight->setTypeLine(typeLine);
    dialogHeight->setBasePointId(basePointId, id);
    dialogHeight->setP1LineId(p1LineId, id);
    dialogHeight->setP2LineId(p2LineId, id);
    dialogHeight->setPointName(p.name());
}

void VToolHeight::Create(QSharedPointer<DialogHeight> &dialog, VMainGraphicsScene *scene, VDomDocument *doc,
                         VContainer *data){
    disconnect(doc, &VDomDocument::FullUpdateFromFile, dialog.data(), &DialogHeight::UpdateList);
    QString pointName = dialog->getPointName();
    QString typeLine = dialog->getTypeLine();
    qint64 basePointId = dialog->getBasePointId();
    qint64 p1LineId = dialog->getP1LineId();
    qint64 p2LineId = dialog->getP2LineId();
    Create(0, pointName, typeLine, basePointId, p1LineId, p2LineId, 5, 10, scene, doc, data,
           Document::FullParse, Tool::FromGui);
}

void VToolHeight::Create(const qint64 _id, const QString &pointName, const QString &typeLine,
                         const qint64 &basePointId, const qint64 &p1LineId, const qint64 &p2LineId,
                         const qreal &mx, const qreal &my, VMainGraphicsScene *scene, VDomDocument *doc,
                         VContainer *data, const Document::Documents &parse, Tool::Sources typeCreation){
    VPointF basePoint = data->GetPoint(basePointId);
    VPointF p1Line = data->GetPoint(p1LineId);
    VPointF p2Line = data->GetPoint(p2LineId);

    QPointF pHeight = FindPoint(QLineF(p1Line.toQPointF(), p2Line.toQPointF()), basePoint.toQPointF());
    qint64 id = _id;
    if(typeCreation == Tool::FromGui){
        id = data->AddPoint(VPointF(pHeight.x(), pHeight.y(), pointName, mx, my));
        data->AddLine(basePointId, id);
        data->AddLine(p1LineId, id);
        data->AddLine(p2LineId, id);
    } else {
        data->UpdatePoint(id, VPointF(pHeight.x(), pHeight.y(), pointName, mx, my));
        data->AddLine(basePointId, id);
        data->AddLine(p1LineId, id);
        data->AddLine(p2LineId, id);
        if(parse != Document::FullParse){
            doc->UpdateToolData(id, data);
        }
    }
    VDrawTool::AddRecord(id, Tool::Height, doc);
    if(parse == Document::FullParse){
        VToolHeight *point = new VToolHeight(doc, data, id, typeLine, basePointId, p1LineId, p2LineId,
                                             typeCreation);
        scene->addItem(point);
        connect(point, &VToolPoint::ChoosedTool, scene, &VMainGraphicsScene::ChoosedItem);
        connect(point, &VToolPoint::RemoveTool, scene, &VMainGraphicsScene::RemoveTool);
        connect(scene, &VMainGraphicsScene::NewFactor, point, &VToolPoint::SetFactor);
        doc->AddTool(id, point);
        doc->IncrementReferens(basePointId);
        doc->IncrementReferens(p1LineId);
        doc->IncrementReferens(p2LineId);
    }
}

QPointF VToolHeight::FindPoint(const QLineF &line, const QPointF &point){
    qreal a = 0, b = 0, c = 0;
    LineCoefficients(line, &a, &b, &c);
    qreal x = point.x() + a;
    qreal y = b + point.y();
    QLineF l (point, QPointF(x, y));
    QPointF p;
    QLineF::IntersectType intersect = line.intersect(l, &p);
    if(intersect == QLineF::UnboundedIntersection || intersect == QLineF::BoundedIntersection){
        return p;
    } else {
        return QPointF();
    }
}

void VToolHeight::FullUpdateFromFile(){
    QDomElement domElement = doc->elementById(QString().setNum(id));
    if(domElement.isElement()){
        typeLine = domElement.attribute(AttrTypeLine, "");
        basePointId = domElement.attribute(AttrBasePoint, "").toLongLong();
        p1LineId = domElement.attribute(AttrP1Line, "").toLongLong();
        p2LineId = domElement.attribute(AttrP2Line, "").toLongLong();
    }
    RefreshGeometry();

}

void VToolHeight::FullUpdateFromGui(int result){
    if(result == QDialog::Accepted){
        QDomElement domElement = doc->elementById(QString().setNum(id));
        if(domElement.isElement()){
            domElement.setAttribute(AttrName, dialogHeight->getPointName());
            domElement.setAttribute(AttrTypeLine, dialogHeight->getTypeLine());
            domElement.setAttribute(AttrBasePoint, QString().setNum(dialogHeight->getBasePointId()));
            domElement.setAttribute(AttrP1Line, QString().setNum(dialogHeight->getP1LineId()));
            domElement.setAttribute(AttrP2Line, QString().setNum(dialogHeight->getP2LineId()));
            emit FullUpdateTree();
        }
    }
    dialogHeight.clear();
}

void VToolHeight::contextMenuEvent(QGraphicsSceneContextMenuEvent *event){
    ContextMenu(dialogHeight, this, event);
}

void VToolHeight::AddToFile(){
    VPointF point = VAbstractTool::data.GetPoint(id);
    QDomElement domElement = doc->createElement(TagName);

    AddAttribute(domElement, AttrId, id);
    AddAttribute(domElement, AttrType, ToolType);
    AddAttribute(domElement, AttrName, point.name());
    AddAttribute(domElement, AttrMx, toMM(point.mx()));
    AddAttribute(domElement, AttrMy, toMM(point.my()));

    AddAttribute(domElement, AttrTypeLine, typeLine);
    AddAttribute(domElement, AttrBasePoint, basePointId);
    AddAttribute(domElement, AttrP1Line, p1LineId);
    AddAttribute(domElement, AttrP2Line, p2LineId);

    AddToCalculation(domElement);

}