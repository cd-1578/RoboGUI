#ifndef CONVERTERWINDOW_H
#define CONVERTERWINDOW_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class RoboUI; }
QT_END_NAMESPACE

class ConverterWindow : public QMainWindow
{
    Q_OBJECT
public:
    ConverterWindow(QWidget *parent = nullptr);
    ~ConverterWindow();

private slots:
    void swapWindows();

};

#endif // CONVERTERWINDOW_H
