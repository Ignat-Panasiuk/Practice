#include "paintcanvas.h"
#include <QMouseEvent>
#include <QPainter>
#include <QFile>
#include <QMessageBox>

PaintCanvas::PaintCanvas(QWidget *parent)
    : QWidget(parent)
    , m_currentTool(TypeLine)
    , m_currentPenColor(Qt::black)
    , m_currentBrushColor(Qt::transparent)
    , m_currentPenWidth(2)
    , m_temporaryShape(nullptr)
    , m_selectedShape(nullptr)
    , m_copiedShape(nullptr)
    , m_isDrawing(false)
    , m_isMoving(false)
    , m_gridSnap(false)
    , m_zoomFactor(1.0)
    , m_isModified(false)
{
    setAttribute(Qt::WA_StaticContents);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::white);
    setPalette(pal);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);
}

PaintCanvas::~PaintCanvas()
{
    clearScene();
    clearHistory();
    delete m_copiedShape;
}

void PaintCanvas::saveState()
{
    m_isModified = true;
    QVector<Shape*> state;
    for (Shape *shape : m_shapes) {
        state.append(shape->clone());
    }
    m_undoStack.append(state);

    for (QVector<Shape*> rState : m_redoStack) {
        qDeleteAll(rState);
    }
    m_redoStack.clear();
}

void PaintCanvas::clearHistory()
{
    for (QVector<Shape*> state : m_undoStack) qDeleteAll(state);
    for (QVector<Shape*> state : m_redoStack) qDeleteAll(state);
    m_undoStack.clear();
    m_redoStack.clear();
}

void PaintCanvas::undo()
{
    if (m_undoStack.isEmpty()) return;

    m_isModified = true;
    QVector<Shape*> state;
    for (Shape *shape : m_shapes) {
        state.append(shape->clone());
    }
    m_redoStack.append(state);

    qDeleteAll(m_shapes);
    m_shapes = m_undoStack.takeLast();
    m_selectedShape = nullptr;
    update();
}

void PaintCanvas::redo()
{
    if (m_redoStack.isEmpty()) return;

    m_isModified = true;
    QVector<Shape*> state;
    for (Shape *shape : m_shapes) {
        state.append(shape->clone());
    }
    m_undoStack.append(state);

    qDeleteAll(m_shapes);
    m_shapes = m_redoStack.takeLast();
    m_selectedShape = nullptr;
    update();
}

void PaintCanvas::copy()
{
    if (!m_selectedShape) return;
    delete m_copiedShape;
    m_copiedShape = m_selectedShape->clone();
}

void PaintCanvas::paste()
{
    if (!m_copiedShape) return;
    saveState();
    Shape* pasted = m_copiedShape->clone();
    pasted->move(20, 20);
    pasted->setSelected(false);
    m_shapes.append(pasted);
    update();
}

void PaintCanvas::bringToFront()
{
    if (!m_selectedShape) return;
    saveState();
    m_shapes.removeOne(m_selectedShape);
    m_shapes.append(m_selectedShape);
    update();
}

void PaintCanvas::sendToBack()
{
    if (!m_selectedShape) return;
    saveState();
    m_shapes.removeOne(m_selectedShape);
    m_shapes.prepend(m_selectedShape);
    update();
}

void PaintCanvas::setGridSnap(bool enabled) { m_gridSnap = enabled; update(); }
void PaintCanvas::setZoom(double factor) { m_zoomFactor = factor; update(); }

QPoint PaintCanvas::applyGridSnap(const QPoint &pt) const
{
    if (!m_gridSnap) return pt;
    int gridX = qRound(pt.x() / 20.0) * 20;
    int gridY = qRound(pt.y() / 20.0) * 20;
    return QPoint(gridX, gridY);
}

void PaintCanvas::setCurrentTool(ShapeType tool) { m_currentTool = tool; }

void PaintCanvas::setPenColor(const QColor &color)
{
    m_currentPenColor = color;
    if (m_selectedShape) {
        saveState();
        m_selectedShape->setPenColor(color);
        update();
    }
}

void PaintCanvas::setBrushColor(const QColor &color)
{
    m_currentBrushColor = color;
    if (m_selectedShape) {
        saveState();
        m_selectedShape->setBrushColor(color);
        update();
    }
}

void PaintCanvas::setPenWidth(int width)
{
    m_currentPenWidth = width;
    if (m_selectedShape) {
        saveState();
        m_selectedShape->setPenWidth(width);
        update();
    }
}

void PaintCanvas::clearScene()
{
    saveState();
    qDeleteAll(m_shapes);
    m_shapes.clear();
    m_temporaryShape = nullptr;
    m_selectedShape = nullptr;
    update();
}

void PaintCanvas::deleteSelected()
{
    if (m_selectedShape) {
        saveState();
        m_shapes.removeOne(m_selectedShape);
        delete m_selectedShape;
        m_selectedShape = nullptr;
        update();
    }
}

void PaintCanvas::selectShapeAt(const QPoint &pos)
{
    if (m_selectedShape) {
        m_selectedShape->setSelected(false);
        m_selectedShape = nullptr;
    }

    for (int i = m_shapes.size() - 1; i >= 0; --i) {
        if (m_shapes[i]->contains(pos)) {
            m_selectedShape = m_shapes[i];
            m_selectedShape->setSelected(true);
            break;
        }
    }
    update();
}

void PaintCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.scale(m_zoomFactor, m_zoomFactor);

    if (m_gridSnap) {
        QPen gridPen(QColor(220, 220, 220), 1, Qt::DotLine);
        painter.setPen(gridPen);
        for (int x = 0; x < width() / m_zoomFactor; x += 20) {
            painter.drawLine(x, 0, x, height() / m_zoomFactor);
        }
        for (int y = 0; y < height() / m_zoomFactor; y += 20) {
            painter.drawLine(0, y, width() / m_zoomFactor, y);
        }
    }

    for (Shape *shape : m_shapes) {
        shape->draw(painter);
    }

    if (m_temporaryShape) {
        m_temporaryShape->draw(painter);
    }
}

void PaintCanvas::mousePressEvent(QMouseEvent *event)
{
    QPoint logicalPos(event->pos().x() / m_zoomFactor, event->pos().y() / m_zoomFactor);

    if (event->button() == Qt::LeftButton) {
        selectShapeAt(logicalPos);

        if (m_selectedShape) {
            saveState();
            m_isMoving = true;
            m_lastMousePos = logicalPos;
        } else {
            m_isDrawing = true;
            QPoint snappedPos = applyGridSnap(logicalPos);
            switch (m_currentTool) {
            case TypeLine:
                m_temporaryShape = new LineShape();
                break;
            case TypeRect:
                m_temporaryShape = new RectShape();
                break;
            case TypeEllipse:
                m_temporaryShape = new EllipseShape();
                break;
            case TypeTriangle:
                m_temporaryShape = new TriangleShape();
                break;
            case TypeCurve:
                m_temporaryShape = new CurveShape();
                static_cast<CurveShape*>(m_temporaryShape)->addPoint(snappedPos);
                break;
            }
            if (m_temporaryShape) {
                m_temporaryShape->setStartPoint(snappedPos);
                m_temporaryShape->setEndPoint(snappedPos);
                m_temporaryShape->setPenColor(m_currentPenColor);
                m_temporaryShape->setBrushColor(m_currentBrushColor);
                m_temporaryShape->setPenWidth(m_currentPenWidth);
            }
        }
    } else if (event->button() == Qt::RightButton) {
        if (m_selectedShape) {
            m_selectedShape->setSelected(false);
            m_selectedShape = nullptr;
            update();
        }
    }
}

void PaintCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QPoint logicalPos(event->pos().x() / m_zoomFactor, event->pos().y() / m_zoomFactor);

    if (m_isDrawing && m_temporaryShape) {
        QPoint snappedPos = applyGridSnap(logicalPos);
        m_temporaryShape->setEndPoint(snappedPos);
        if (m_currentTool == TypeCurve) {
            static_cast<CurveShape*>(m_temporaryShape)->addPoint(snappedPos);
        }
        update();
    } else if (m_isMoving && m_selectedShape) {
        QPoint snappedPos = applyGridSnap(logicalPos);
        QPoint snappedLast = applyGridSnap(m_lastMousePos);
        int dx = snappedPos.x() - snappedLast.x();
        int dy = snappedPos.y() - snappedLast.y();
        if (dx != 0 || dy != 0) {
            m_selectedShape->move(dx, dy);
            m_lastMousePos = snappedPos;
            update();
        }
    }
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (event->button() == Qt::LeftButton) {
        if (m_isDrawing && m_temporaryShape) {
            saveState();
            m_shapes.append(m_temporaryShape);
            m_temporaryShape = nullptr;
            m_isDrawing = false;
            update();
        }
        m_isMoving = false;
    }
}

bool PaintCanvas::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_15);
    out << (int)m_shapes.size();
    for (Shape *shape : m_shapes) {
        out << (int)shape->type();
        shape->serialize(out);
    }
    file.close();
    m_isModified = false;
    return true;
}

bool PaintCanvas::loadFromFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    clearScene();
    clearHistory();

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_15);
    int size = 0;
    in >> size;

    for (int i = 0; i < size; ++i) {
        int typeVal = 0;
        in >> typeVal;
        ShapeType type = (ShapeType)typeVal;
        Shape *shape = nullptr;
        switch (type) {
        case TypeLine: shape = new LineShape(); break;
        case TypeRect: shape = new RectShape(); break;
        case TypeEllipse: shape = new EllipseShape(); break;
        case TypeTriangle: shape = new TriangleShape(); break;
        case TypeCurve: shape = new CurveShape(); break;
        }
        if (shape) {
            shape->deserialize(in);
            m_shapes.append(shape);
        }
    }
    file.close();
    m_isModified = false;
    update();
    return true;
}