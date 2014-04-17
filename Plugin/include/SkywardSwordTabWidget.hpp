#ifndef SKYWARDSWORDTABWIDGET_HPP
#define SKYWARDSWORDTABWIDGET_HPP

#include <QTabWidget>

class SkywardSwordTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit SkywardSwordTabWidget(QWidget *parent = 0);
    ~SkywardSwordTabWidget();

    QTabBar* tabBar() const;
private:
};

#endif // SKYWARDSWORDTABWIDGET_HPP
