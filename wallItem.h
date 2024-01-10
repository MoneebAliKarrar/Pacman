#pragma once
#include <QPen>
class WallItem : public QGraphicsRectItem {
  public:
	WallItem(qreal x, qreal y, qreal width, qreal height)
		: QGraphicsRectItem(x, y, width, height)
	{
		setBrush(QBrush(QColor("#342DCA"))), setPen(QPen(QColor("#342DCA")));
	}
	enum { Type = UserType + 1 };
	int type() const override
	{
		return Type;
	}
};
