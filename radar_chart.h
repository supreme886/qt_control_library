#ifndef KKRADARCHARTLABEL_H
#define KKRADARCHARTLABEL_H

#include <QWidget>

class QPolygonF;


/**
 * @brief The KKRadarChartLabel class
 *
 *  基于 QWidget 实现的一个雷达图控件
 */
class KKRadarChartLabel : public QWidget
{
    Q_OBJECT
public:
    explicit KKRadarChartLabel(QWidget *parent = nullptr);
    ~KKRadarChartLabel();

    ///> 设置雷达图的数据纬度，默认为5，并且大于2；
    void setLatitude(int val);
    int  getLatitude() { return mLatitude;}

    void setTextListForLatitude(const QStringList &valList, const QVector<qreal>&);

//    QVector<int> getValListForLatitude();

    void setLatitudeValueList(const QVector<qreal>&);

    void setValueForCenter(const QString &);

    QString  getValueForCenter();
    bool getIsSelectCenter();
    void setIsSelectCenter(bool);

    ///> 获取选中的数据纬度
    QString getLatitudeSelect();
    int  getLatitudeSelectIndex();
    void setLatitudeSelectIndex(int);

    ///> 设置是否有内部网格线
    void setHasGrid(bool);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

private:
    void drawPolygonLine(int i, qreal r = 0);
    void drawRadiationLine(QPainter &);
    void drawLatitudeValueList(QPainter &);
    QPointF getRadarChartPoint(qreal angle, qreal r);

signals:
    void latitudeClicked(QString);
    void centerRectCliked(bool);

private:
    int mLatitude{5};
    int mDiameter{0};
    int mLatitudeSelect{-1};
    int mLatitudeHover{-1};
    bool mHasGrid;
    bool mMousePressed{false};
    bool mCenterHover{false};
    bool mCenterClicked{true};
    QString mCenterVal{""};
    QPolygonF mPointList;
    QPolygonF mCenterPointList;
    QVector<QPushButton> mBtnList;
    QVector<qreal> mLatitudeValueList;
    QStringList mTextListForLatitude;
    QList<QRectF> mLatitudeTextRect;
};

#endif // KKRADARCHARTLABEL_H

