#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>

#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <xinput.h>
#include <unistd.h>

#include "iwindows_xinput_wrapper.h"
#include "joypad.h"

#include <xmlwindow.h>

QT_BEGIN_NAMESPACE
namespace Ui { class RoboUI; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QTcpSocket *_pSocket0;
    QTcpSocket *_pSocket1;
    JoyPad *jPad;
    QElapsedTimer *stopwatch;
    QTimer *poller;
    IWindows_XInput_Wrapper * xWrapper;
    QButtonGroup *armControls;
    XmlWindow *secondaryWindow;
    QTimer *windowPoller;


private slots:
    void xChanged();
    void yChanged();
    void xyChanged();

    void setVelocityX();
    void setVelocityY();
    void resetX();
    void resetY();

    void stand();
    void trot();

    void up();
    void down();

    void setArmLRP();
    void setGripAngleP();
    void setArmHeightP();
    void setArmExtensionP();
    void setGripP();

    void setArmLRN();
    void setGripAngleN();
    void setArmHeightN();
    void setArmExtensionN();
    void setGripN();

    void sensorSearch();
    void updateView();

    void keyPressEvent(QKeyEvent *event);

    void GetButtons(short uID, QList<XboxOneButtons> PressedButtons);
    void GetLeftThumbstick(short, double x, double y);
    void GetRightThumbstick(short uID, double, double y);

    void updateTime();

    void swapWindows();
    void pendingWindow();

private:
    Ui::RoboUI *ui;
    void initJoyPad();
    void initTimer();
    void initVelocitySliders();
    void initHeight();
    void initArm();
    void initMovement();
    void initSearchBar();
    void initViews();
    void initXInputWrapper();
    void initStopwatch();
    void initWindowSwap();

    void connectTCP0();
    void writeTCP0(std::string);
    void connectTCP1();
    void readTCP1();
    void writeTCP1(std::string);
};
#endif // MAINWINDOW_H
