#ifndef XMLWINDOW_H
#define XMLWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QStringList>

namespace Ui {
class XmlWindow;
}

class XmlWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit XmlWindow(QWidget *parent = nullptr);
    ~XmlWindow();

private:
    Ui::XmlWindow *ui;
    QFile *xml;

private slots:
    void swapWindows();
    void processFile();
    void createMujocoXML();
};

#endif // XMLWINDOW_H
