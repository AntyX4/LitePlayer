#pragma once

#include <QSlider>
#include <QMouseEvent>
#include <QStyleOptionSlider>

class MySlider : public QSlider
{
    Q_OBJECT

public:
    MySlider(QWidget* parent);
    ~MySlider();

protected:
    void mousePressEvent(QMouseEvent* event);

signals:
    void onClick(int value);
};
