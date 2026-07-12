#ifndef SHAPE_H
#define SHAPE_H

#include <QPoint>
#include <QColor>
#include <QPainter>
#include <QDataStream>
#include <QVector>

enum ShapeType {
    TypeLine = 1,
    TypeRect = 2,
    TypeEllipse = 3,
    TypeTriangle = 4,
    TypeCurve = 5
};

class Shape
{
protected:
    ShapeType m_type;
    QPoint m_startPoint;
    QPoint m_endPoint;
    QColor m_penColor;
    QColor m_brushColor;
    int m_penWidth;
    bool m_isSelected;

public:
    Shape(ShapeType type);
    virtual ~Shape();

    ShapeType type() const;

    void setStartPoint(const QPoint &pt);
    QPoint startPoint() const;

    void setEndPoint(const QPoint &pt);
    QPoint endPoint() const;

    void setPenColor(const QColor &color);
    QColor penColor() const;

    void setBrushColor(const QColor &color);
    QColor brushColor() const;

    void setPenWidth(int width);
    int penWidth() const;

    void setSelected(bool selected);
    bool isSelected() const;

    virtual void move(int dx, int dy);

    virtual void draw(QPainter &painter) = 0;
    virtual bool contains(const QPoint &point) = 0;

    virtual Shape* clone() const = 0;

    virtual void serialize(QDataStream &out) const;
    virtual void deserialize(QDataStream &in);
};

class LineShape : public Shape
{
public:
    LineShape();
    void draw(QPainter &painter) override;
    bool contains(const QPoint &point) override;
    Shape* clone() const override;
};

class RectShape : public Shape
{
public:
    RectShape();
    void draw(QPainter &painter) override;
    bool contains(const QPoint &point) override;
    Shape* clone() const override;
};

class EllipseShape : public Shape
{
public:
    EllipseShape();
    void draw(QPainter &painter) override;
    bool contains(const QPoint &point) override;
    Shape* clone() const override;
};

class TriangleShape : public Shape
{
public:
    TriangleShape();
    void draw(QPainter &painter) override;
    bool contains(const QPoint &point) override;
    Shape* clone() const override;
};

class CurveShape : public Shape
{
private:
    QVector<QPoint> m_points;

public:
    CurveShape();
    void addPoint(const QPoint &pt);
    void move(int dx, int dy) override;
    void draw(QPainter &painter) override;
    bool contains(const QPoint &point) override;
    Shape* clone() const override;
    void serialize(QDataStream &out) const override;
    void deserialize(QDataStream &in) override;
};

#endif