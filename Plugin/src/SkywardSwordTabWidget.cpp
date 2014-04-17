#include "include/SkywardSwordTabWidget.hpp"
#include "ui_SkywardSwordTabWidget.h"

SkywardSwordTabWidget::SkywardSwordTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
}

SkywardSwordTabWidget::~SkywardSwordTabWidget()
{
}

QTabBar* SkywardSwordTabWidget::tabBar() const
{
    return QTabWidget::tabBar();
}
