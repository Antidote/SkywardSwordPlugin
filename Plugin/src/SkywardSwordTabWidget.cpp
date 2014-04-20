#include "include/SkywardSwordTabWidget.hpp"

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
