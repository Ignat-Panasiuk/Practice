#include "shape.h"
#include <QPainterPath>
#include <QPolygon>
#include <cmath>

Shape::Shape(ShapeType type)
    : m_type(type)
    , m_startPoint(0, 0)
    , m_endPoint(0, 0)
    , m_penColor(Qt::black)
    , m_brushColor(Qt::transparent)
    , m_penWidth(2)
    , m_isSelected(false)
{}

Shape::~Shape() {}

ShapeType Shape::type() const { return m_type; }
void Shape::setStartPoint(const QPoint &pt) { m_startPoint = pt; }
QPoint Shape::startPoint() const { return m_startPoint; }
void Shape::setEndPoint(const QPoint &pt) { m_endPoint = pt; }
QPoint Shape::endPoint() const { return m_endPoint; }
void Shape::setPenColor(const QColor &color) { m_penColor = color; }
QColor Shape::penColor() const { return m_penColor; }
void Shape::setBrushColor(const QColor &color) { m_brushColor = color; }
QColor Shape::brushColor() const { return m_brushColor; }
void Shape::setPenWidth(int width) { m_penWidth = width; }
int Shape::penWidth() const { return m_penWidth; }
void Shape::setSelected(bool selected) { m_isSelected = selected; }
bool Shape::isSelected() const { return m_isSelected; }

void Shape::move(int dx, int dy)
{
    m_startPoint.rx() += dx;
    m_startPoint.ry() += dy;
    m_endPoint.rx() += dx;
    m_endPoint.ry() += dy;
}

void Shape::serialize(QDataStream &out) const
{
    out << m_startPoint.x() << m_startPoint.y() << m_endPoint.x() << m_endPoint.y()
    << (quint32)m_penColor.rgba() << (quint32)m_brushColor.rgba() << m_penWidth;
}

void Shape::deserialize(QDataStream &in)
{
    int sx, sy, ex, ey;
    quint32 penRgba, brushRgba;
    in >> sx >> sy >> ex >> ey >> penRgba >> brushRgba >> m_penWidth;
    m_startPoint = QPoint(sx, sy);
    m_endPoint = QPoint(ex, ey);
    m_penColor = QColor::fromRgba(penRgba);
    m_brushColor = QColor::fromRgba(brushRgba);
}

LineShape::LineShape() : Shape(TypeLine) {}
Shape* LineShape::clone() const { return new LineShape(*this); }

void LineShape::draw(QPainter &painter)
{
    QPen pen(m_penColor, m_penWidth);
    if (m_isSelected) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);
    painter.drawLine(m_startPoint, m_endPoint);
}

bool LineShape::contains(const QPoint &point)
{
    double x1 = m_startPoint.x();
    double y1 = m_startPoint.y();
    double x2 = m_endPoint.x();
    double y2 = m_endPoint.y();
    double px = point.x();
    double py = point.y();

    double l2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
    if (l2 == 0) return false;

    double t = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / l2;
    t = std::max(0.0, std::min(1.0, t));

    double projectionX = x1 + t * (x2 - x1);
    double projectionY = y1 + t * (y2 - y1);

    double distance = std::sqrt((px - projectionX) * (px - projectionX) + (py - projectionY) * (py - projectionY));
    return distance <= (m_penWidth + 4);
}

RectShape::RectShape() : Shape(TypeRect) {}
Shape* RectShape::clone() const { return new RectShape(*this); }

void RectShape::draw(QPainter &painter)
{
    QPen pen(m_penColor, m_penWidth);
    if (m_isSelected) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);
    painter.setBrush(m_brushColor);
    painter.drawRect(QRect(m_startPoint, m_endPoint));
}

bool RectShape::contains(const QPoint &point)
{
    return QRect(m_startPoint, m_endPoint).normalized().contains(point);
}

EllipseShape::EllipseShape() : Shape(TypeEllipse) {}
Shape* EllipseShape::clone() const { return new EllipseShape(*this); }

void EllipseShape::draw(QPainter &painter)
{
    QPen pen(m_penColor, m_penWidth);
    if (m_isSelected) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);
    painter.setBrush(m_brushColor);
    painter.drawEllipse(QRect(m_startPoint, m_endPoint));
}

bool EllipseShape::contains(const QPoint &point)
{
    QPainterPath path;
    path.addEllipse(QRect(m_startPoint, m_endPoint));
    return path.contains(point);
}

TriangleShape::TriangleShape() : Shape(TypeTriangle) {}
Shape* TriangleShape::clone() const { return new TriangleShape(*this); }

void TriangleShape::draw(QPainter &painter)
{
    QPen pen(m_penColor, m_penWidth);
    if (m_isSelected) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);
    painter.setBrush(m_brushColor);

    QPolygon polygon;
    polygon << QPoint((m_startPoint.x() + m_endPoint.x()) / 2, m_startPoint.y())
            << QPoint(m_startPoint.x(), m_endPoint.y())
            << QPoint(m_endPoint.x(), m_endPoint.y());

    painter.drawPolygon(polygon);
}

bool TriangleShape::contains(const QPoint &point)
{
    QPolygon polygon;
    polygon << QPoint((m_startPoint.x() + m_endPoint.x()) / 2, m_startPoint.y())
            << QPoint(m_startPoint.x(), m_endPoint.y())
            << QPoint(m_endPoint.x(), m_endPoint.y());
    return polygon.containsPoint(point, Qt::OddEvenFill);
}

CurveShape::CurveShape() : Shape(TypeCurve) {}
Shape* CurveShape::clone() const { return new CurveShape(*this); }

void CurveShape::addPoint(const QPoint &pt)
{
    m_points.append(pt);
}

void CurveShape::move(int dx, int dy)
{
    Shape::move(dx, dy);
    for (QPoint &pt : m_points) {
        pt.rx() += dx;
        pt.ry() += dy;
    }
}

void CurveShape::draw(QPainter &painter)
{
    QPen pen(m_penColor, m_penWidth);
    if (m_isSelected) {
        pen.setStyle(Qt::DashLine);
    }
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    for (int i = 1; i < m_points.size(); ++i) {
        painter.drawLine(m_points[i - 1], m_points[i]);
    }
}

bool CurveShape::contains(const QPoint &point)
{
    for (const QPoint &pt : m_points) {
        double dist = std::sqrt((pt.x() - point.x()) * (pt.x() - point.x()) + (pt.y() - point.y()) * (pt.y() - point.y()));
        if (dist <= (m_penWidth + 5)) {
            return true;
        }
    }
    return false;
}

void CurveShape::serialize(QDataStream &out) const
{
    Shape::serialize(out);
    out << (int)m_points.size();
    for (const QPoint &pt : m_points) {
        out << pt.x() << pt.y();
    }
}

void CurveShape::deserialize(QDataStream &in)
{
    Shape::deserialize(in);
    int size;
    in >> size;
    m_points.clear();
    for (int i = 0; i < size; ++i) {
        int px, py;
        in >> px >> py;
        m_points.append(QPoint(px, py));
    }
}