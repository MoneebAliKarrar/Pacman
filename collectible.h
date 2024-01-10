#pragma once
#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QPen>
class CollectibleItem : public QGraphicsEllipseItem {
  public:
	enum CollectibleType { Simple, Special };
	CollectibleItem(qreal x, qreal y, qreal width, qreal height,
					CollectibleType itemType)
		: QGraphicsEllipseItem(x, y, width, height), itemType(itemType)
	{
		if (itemType == Simple) {
			setBrush(QBrush(QColor("#FFFF00")));
			setPen(QPen(QColor("#FFFF00")));
		}
		else if (itemType == Special) {
			setBrush(QBrush(QColor("#FF0000")));
			setPen(QPen(QColor("#FF0000")));
		}
	}

	enum { Type = UserType + 2 };
	int type() const override
	{
		return Type;
	}

	CollectibleType getCollectibleType() const
	{
		return itemType;
	}

  private:
	CollectibleType itemType;
};
