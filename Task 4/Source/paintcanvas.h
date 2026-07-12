#ifndef PAINTCANVAS_H
#define PAINTCANVAS_H

#include <QWidget>
#include <QVector>
#include "shape.h"

class PaintCanvas : public QWidget
{
    Q_OBJECT

private:
    QVector<Shape*> m_shapes;
    ShapeType m_currentTool;
    QColor m_currentPenColor;
    QColor m_currentBrushColor;
    int m_currentPenWidth;

    Shape* m_temporaryShape;
    Shape* m_selectedShape;
    Shape* m_copiedShape;

    bool m_isDrawing;
    bool m_isMoving;
    QPoint m_lastMousePos;

    bool m_gridSnap;
    double m_zoomFactor;
    bool m_isModified;

    QVector<QVector<Shape*>> m_undoStack;
    QVector<QVector<Shape*>> m_redoStack;

    void selectShapeAt(const QPoint &pos);
    QPoint applyGridSnap(const QPoint &pt) const;
    void saveState();
    void clearHistory();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public:
    explicit PaintCanvas(QWidget *parent = nullptr);
    ~PaintCanvas();

    void setCurrentTool(ShapeType tool);
    void setPenColor(const QColor &color);
    void setBrushColor(const QColor &color);
    void setPenWidth(int width);

    QColor penColor() const { return m_currentPenColor; }
    QColor brushColor() const { return m_currentBrushColor; }

    bool isModified() const { return m_isModified; }
    void setModified(bool modified) { m_isModified = modified; }
    bool hasSelected() const { return m_selectedShape != nullptr; }

    void clearScene();
    void deleteSelected();

    void undo();
    void redo();
    void copy();
    void paste();
    void bringToFront();
    void sendToBack();
    void setGridSnap(bool enabled);
    void setZoom(double factor);

    bool saveToFile(const QString &fileName);
    bool loadFromFile(const QString &fileName);
};

#endif