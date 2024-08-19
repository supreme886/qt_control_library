#include "KKradarchartlabel.h"

#include <KKcorelib.h>
#include <KKcolormanager.h>

#include <QPainter>
#include <QPoint>
#include <QPalette>
#include <QFontMetricsF>
#include <QPushButton>
#include <QPolygonF>
#include <QtMath>

#define PI 3.1415926

KKRadarChartLabel::KKRadarChartLabel(QWidget *parent)
    : QWidget{parent},
      mHasGrid(true)
{
    setMinimumSize(180, 180);
    setMouseTracking(true);
}

KKRadarChartLabel::~KKRadarChartLabel()
{

}

void KKRadarChartLabel::setLatitude(int val)
{
    if (val <= 2)
        return;

    mLatitude = val;
    update();
}

void KKRadarChartLabel::setTextListForLatitude(const QStringList &textList, const QVector<qreal> &valList)
{
    if (valList.size() != mLatitude || textList.size() != mLatitude)
        return;
    mTextListForLatitude = textList;
    mLatitudeValueList = valList;
    update();
}

void KKRadarChartLabel::setLatitudeValueList(const QVector<qreal> &list)
{
    if (list.size() != mLatitude)
        return;
    mLatitudeValueList = list;
    update();
}

void KKRadarChartLabel::setValueForCenter(const QString &str)
{
    mCenterVal = str;
    update();
}

QString KKRadarChartLabel::getValueForCenter()
{
    return mCenterVal;
}

bool KKRadarChartLabel::getIsSelectCenter()
{
    return mCenterClicked;
}

void KKRadarChartLabel::setIsSelectCenter(bool val)
{
    mCenterClicked = val;
    mLatitudeSelect = -1;
    update();
}

QString KKRadarChartLabel::getLatitudeSelect()
{
    if (mLatitudeSelect >= 0 && mLatitudeSelect < mTextListForLatitude.size())
        return mTextListForLatitude.at(mLatitudeSelect);
    else
        return "";
}

int KKRadarChartLabel::getLatitudeSelectIndex()
{
    return mLatitudeSelect;
}

void KKRadarChartLabel::setLatitudeSelectIndex(int i)
{
    if (i < -1 || i >= mLatitude)
        return;

    if (i != mLatitudeSelect) {
        mLatitudeSelect = i;
        mCenterClicked = false;
        update();
    }
}

void KKRadarChartLabel::setHasGrid(bool val)
{
    mHasGrid = val;
    update();
}

void KKRadarChartLabel::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    painter.setPen(QPen(Qt::blue, Qt::SolidLine));
    painter.setRenderHint(QPainter::Antialiasing);

    drawRadiationLine(painter);

    painter.save();

    for (int i = 0; i <= 4; i++) {
        QPolygonF polygonF;
        for (int j = 0; j < mLatitude; j++) {
            int index = i*mLatitude + j;
            if (index > mPointList.size())
                continue;
            polygonF.append(mPointList.at(index));
        }

        if (0 == i) {
            painter.setPen(QPen(gKKColorManager->getColorByColorId("ZX007"), 1, Qt::SolidLine));
            if (!mHasGrid)
                break;
        } else {
            painter.setPen(QPen(gKKColorManager->getColorByColorId("ZX009"), 1, Qt::DashLine));
        }

        painter.drawPolygon(polygonF);
    }

    painter.restore();

    drawLatitudeValueList(painter);

    QFont mFont = font();
    mFont.setBold(true);
    mFont.setPixelSize(16);
    QFontMetricsF metrics(mFont);
    qreal pixelsWid = metrics.horizontalAdvance(mCenterVal);
    qreal pixelsHigh = metrics.height();

    QPoint point = rect().center();
    point.setX(point.x() + 5);

    painter.setFont(mFont);
    painter.setPen(gKKColorManager->getColorByColorId("CG013"));

    painter.drawText(QRectF(point.x() - pixelsWid/2 - 5, point.y() - pixelsHigh/2, pixelsWid, pixelsHigh), mCenterVal);
}

void KKRadarChartLabel::resizeEvent(QResizeEvent *)
{
    mDiameter = height() <= width() ? height() : width();
    mDiameter /= 2;  ///> 直径/2 = 半径
    mDiameter -= 50; ///> 预留边界给文字

    mPointList.clear();

    int step = mDiameter/5;

    for (int i = 0; i <= 4; i++) {
//        if (0 == i) {
//            drawPolygonLine(i, mDiameter-i*step);
//        } else {
//            drawPolygonLine(i, mDiameter-i*step);
//        }
        drawPolygonLine(i, mDiameter-i*step);
    }
}

void KKRadarChartLabel::mouseMoveEvent(QMouseEvent *e)
{
    bool val = false;
    for (int i = 0; i < mLatitudeTextRect.size(); i++) {
        if (mLatitudeTextRect.at(i).contains(e->pos())) {
            mLatitudeHover = i;
            setCursor(Qt::PointingHandCursor);
            val = true;
            break;
        }
    }

    if (mCenterPointList.containsPoint(QPointF(e->pos()), Qt::OddEvenFill)) {
        mCenterHover = true;
        setCursor(Qt::PointingHandCursor);
        val = true;
    }

    if (!val) {
        setCursor(Qt::ArrowCursor);
        mLatitudeHover = -1;
        mCenterHover = false;
    }

    update();
}

void KKRadarChartLabel::mousePressEvent(QMouseEvent *e)
{
    if (mLatitudeHover != -1 || mCenterHover) {
        if (Qt::LeftButton == e->button()) {
            mMousePressed = true;
        }
    }
}

void KKRadarChartLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if (mLatitudeHover != -1) {
        if (Qt::LeftButton == e->button() && mMousePressed) {
            mLatitudeSelect = mLatitudeHover;
            mMousePressed = false;
            mCenterClicked = false;
            update();
            if (mLatitudeSelect >= 0 && mLatitudeSelect < mTextListForLatitude.size())
                emit latitudeClicked(mTextListForLatitude.at(mLatitudeSelect));
        }
    }

    if (mCenterHover) {
        if (Qt::LeftButton == e->button() && mMousePressed) {
//            mCenterClicked = !mCenterClicked;
            mCenterClicked = true;
            mMousePressed = false;
            mLatitudeSelect = -1;
            update();
            emit centerRectCliked(mCenterClicked);
        }
    }
}

/**
 *     绘制嵌套的五个多边形 以及 评分段数字(2\4\6\8)
 *
 * @brief KKRadarChartLabel::drawPolygonLine
 * @param painter 画笔
 * @param r 多边形顶点所在圆的半径
 */
void KKRadarChartLabel::drawPolygonLine(int num, qreal r)
{
    qreal diameter; ///获取widget在当前大小下，所能得到的最大的圆直径
    if (0 == r)
        diameter = height() <= width() ? height() : width();
    else
        diameter = r;

    QPointF centerPoint = rect().center(); ///获取widget当前中心点;
    centerPoint.setY(centerPoint.y()+5);

    ///>设置最外层多边形起点顶点
    QPointF mstartingPoint;
    mstartingPoint.setX(centerPoint.x());
    mstartingPoint.setY(centerPoint.y()-diameter);

    qreal angle = 360/mLatitude;

    QPolygonF pointList;
    pointList.push_back(mstartingPoint);
    mPointList.push_back(mstartingPoint);

    for (int i = 1; i < mLatitude; i++) {
        QPointF point(0, 0);
        qreal tarAngle = angle*i;
        point = getRadarChartPoint(tarAngle, diameter);
        pointList.push_back(point);
        mPointList.push_back(point);
    }

//    painter.drawPolygon(pointList);

//    if (num && mHasGrid) { ///> 显示内部分割的评分
//        QString str = QString::number((5-num)*2);

//        QFontMetricsF metrics(font());
//        qreal pixelsWide = metrics.horizontalAdvance(str);
//        qreal pixelsHigh = metrics.height();

//        QPointF point = pointList.at(mLatitude-1);
//        QRectF txtRect;
//        txtRect.setX(point.x()-pixelsWide/1.5);
//        txtRect.setY(point.y()-pixelsHigh);
//        txtRect.setWidth(pixelsWide);
//        txtRect.setHeight(pixelsHigh);

//        painter.drawText(txtRect, str);
//    }
}

/**
 *     绘制原点连接多边形顶点的 放射线段 以及各个纬度的描述文字
 *
 * @brief KKRadarChartLabel::drawRadiationLine
 * @param painter 画笔
 */
void KKRadarChartLabel::drawRadiationLine(QPainter &painter)
{
    if (mPointList.isEmpty())
        return;

    painter.setPen(gKKColorManager->getColorByColorId("ZX012"));

    qreal angle = 360/mLatitude;
    mLatitudeTextRect.clear();

    QPoint point = rect().center();
    point.setY(point.y() + 5);

    ///> 绘制 放射线段
    for (int i = 0; i < mLatitude; i++) {

        if (i < mTextListForLatitude.size()) {

            QString num = QString::number(mLatitudeValueList.at(i), 'f', 2);
            num = num == "0.00" ? "--" : num;
            QString str = mTextListForLatitude.at(i);

            QFontMetricsF metrics(font());
            qreal pixelsWid = metrics.horizontalAdvance(mTextListForLatitude.at(i));
            qreal pixelsWid1 = metrics.horizontalAdvance(QString::number(mLatitudeValueList.at(i), 'f', 2));
            qreal pixelsHigh = metrics.height();

            pixelsWid = pixelsWid < pixelsWid1 ? pixelsWid1 : pixelsWid;

            QPointF point;
            if (0 == i || 1 == i || 4 == i)
                point = getRadarChartPoint(angle*i, mDiameter+35);
            else
                point = getRadarChartPoint(angle*i, mDiameter+25);
            QRectF txtRectf(point.x()-pixelsWid/2, point.y() - pixelsHigh/2, pixelsWid, pixelsHigh);
            QRectF numRectf(point.x()-pixelsWid1/2, point.y() + pixelsHigh - pixelsHigh/2, pixelsWid1, pixelsHigh);

            painter.save();
            if (i == mLatitudeHover || i == mLatitudeSelect) {
                QColor hoverColor = gKKColorManager->getColorByColorId("CG014");
                painter.setPen(hoverColor);
                painter.drawText(txtRectf, Qt::AlignCenter, str);
                painter.drawText(numRectf, Qt::AlignCenter, num);
            }else{
                QColor color = gKKColorManager->getColorByColorId("ZX001");
                painter.setPen(color);
                painter.drawText(numRectf, Qt::AlignCenter, num);

                color = gKKColorManager->getColorByColorId("ZX002");
                color.setAlphaF(0.7);
                painter.setPen(color);
                painter.drawText(txtRectf, Qt::AlignCenter, str);
            }
            painter.restore();

            txtRectf.setHeight(txtRectf.height()*2);
            mLatitudeTextRect.append(txtRectf);
        }

        if (i >= mPointList.size())
            continue;

        if (mHasGrid) {
            painter.setPen(gKKColorManager->getColorByColorId("ZX009"));
            painter.drawLine(point, mPointList.at(i));
        }
    }
}

/**
 *    根据设置的多个纬度的值，计算多边形的顶点，并填充多边形区域
 *
 * @brief KKRadarChartLabel::drawLatitudeValueList
 * @param painter
 */
void KKRadarChartLabel::drawLatitudeValueList(QPainter &painter)
{
    if (!mLatitudeValueList.size() || mLatitudeValueList.size() < mLatitude)
        return;

    QPolygonF pointList;
    QPointF startPoint;
    startPoint.setX(rect().center().x());
    startPoint.setY(rect().center().y() + 5 - mDiameter*(mLatitudeValueList.at(0)/10.0));
    if (0 != mLatitudeValueList.at(0))
        pointList.append(startPoint);

    qreal angle = 360/mLatitude;

    for (int i = 1; i < mLatitude; i++) {
        if (0 == mLatitudeValueList.at(i)) ///> 为0时跳过该顶点的绘制
            continue;

        QPointF point(0, 0);
        qreal tarAngle = angle*i;
        qreal scale = mLatitudeValueList.at(i)/10.0;
        point = getRadarChartPoint(tarAngle, mDiameter*scale);
        pointList.append(point);
    }

    if (mCenterHover || mCenterClicked)
        painter.setPen(QPen(gKKColorManager->getColorByColorId("CG013"), 2.5));
    else
        painter.setPen(QPen(gKKColorManager->getColorByColorId("CG013"), 1.5));

    painter.setBrush(gKKColorManager->getColorByColorId("CG013"));
    painter.save();

    QColor color =gKKColorManager->getColorByColorId("CG013");
    color.setAlphaF(0.08);
    painter.setBrush(color);
    painter.drawPolygon(pointList);
    painter.restore();

    painter.setPen(gKKColorManager->getColorByColorId("ZX031"));
    for (int i = 0; i < pointList.size(); i++) {
        painter.drawEllipse(pointList.at(i), 2.5, 2.5);
    }

    mCenterPointList.clear();
    mCenterPointList = pointList;
}

/**
 *     根据传入的指定值计算多边形的其中一个顶点
 *
 * @brief KKRadarChartLabel::getRadarChartPoint
 * @param angle 以起点和一个r长线条与原点围成的角度
 * @param r  多边形顶点所在圆的半径
 * @return   多边形的其中一个顶点
 */
QPointF KKRadarChartLabel::getRadarChartPoint(qreal angle, qreal r)
{
    QPointF point(0, 0);
    if (angle < 0 || angle > 360)
        return point;

    QPointF centerPoint = rect().center(); ///获取widget当前中心点;
    centerPoint.setY(centerPoint.y()+5);

    ///> 根据以 centerPoint 为二维坐标原点的坐标轴中的，不同象限来计算顶点的位置
    if (angle >= 0 && angle <= 90) {
        point.setX(centerPoint.x() + sin(angle*PI/180)*r);
        point.setY(centerPoint.y() - cos(angle*PI/180)*r);
    } else if (angle > 90 && angle <= 180) {
        point.setX(centerPoint.x() + sin((180-angle)*PI/180)*r);
        point.setY(centerPoint.y() + cos((180-angle)*PI/180)*r);
    } else if (angle > 180 && angle <= 270) {
        point.setX(centerPoint.x() - sin((angle-180)*PI/180)*r);
        point.setY(centerPoint.y() + cos((angle-180)*PI/180)*r);
    } else {
        point.setX(centerPoint.x() - sin((360-angle)*PI/180)*r);
        point.setY(centerPoint.y() - cos((360-angle)*PI/180)*r);
    }

    return point;
}

