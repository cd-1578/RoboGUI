#include "mainwindow.h"
#include "./ui_mainwindow.h"

//---------------------------------- CONSTRUCTOR AND DESTRUCTOR ------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RoboUI)
{
    ui->setupUi(this);

    QString windowTitle("RoboUI");

    initJoyPad();
    initVelocitySliders();
    initMovement();
    initArm();
    initHeight();
    connectTCP0();
    connectTCP1();
    initSearchBar();
    initViews();
    initXInputWrapper();
    initStopwatch();
    initWindowSwap();

    this->setWindowTitle(windowTitle);
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}

//---------------------------------- INITIALIZATION ------------------------------------

/**
 * @brief MainWindow::initJoyPad
 * @return JoyPad* jPad
 */
void MainWindow::initJoyPad()
{
    jPad = new JoyPad(this->ui->centralwidget);
    jPad->setGeometry(385, 345, 200, 200);
    jPad->show();

    connect(jPad, &JoyPad::xChanged, this, &MainWindow::xChanged);
    connect(jPad, &JoyPad::yChanged, this, &MainWindow::yChanged);
    connect(jPad, &JoyPad::xyChanged, this, &MainWindow::xyChanged);
}

/**
 * @brief MainWindow::initVelocitySliders
 */
void MainWindow::initVelocitySliders()
{
    this->ui->vxSlider->setRange(-100, 100);
    this->ui->vySlider->setRange(-100, 100);

    this->ui->vxLCD->setMode(QLCDNumber::Dec);
    this->ui->vyLCD->setMode(QLCDNumber::Dec);

    connect(this->ui->vxSlider, &QSlider::sliderMoved, this, &MainWindow::setVelocityX);
    connect(this->ui->vySlider, &QSlider::sliderMoved, this, &MainWindow::setVelocityY);

    connect(this->ui->resetVelocityX, &QPushButton::clicked, this, &MainWindow::resetX);
    connect(this->ui->resetVelocityY, &QPushButton::clicked, this, &MainWindow::resetY);
}

/**
 * @brief MainWindow::initHeight
 */
void MainWindow::initHeight()
{
    connect(this->ui->up, &QPushButton::clicked, this, &MainWindow::up);
    connect(this->ui->down, &QPushButton::clicked, this, &MainWindow::down);
}

/**
 * @brief MainWindow::initArm
 */
void MainWindow::initArm()
{
    armControls = new QButtonGroup(this);

    armControls->addButton(this->ui->rotPos, 0);
    armControls->addButton(this->ui->rotNeg, 1);
    armControls->addButton(this->ui->reaPos, 2);
    armControls->addButton(this->ui->reaNeg, 3);
    armControls->addButton(this->ui->heiPos, 4);
    armControls->addButton(this->ui->heiNeg, 5);
    armControls->addButton(this->ui->angPos, 6);
    armControls->addButton(this->ui->angNeg, 7);
    armControls->addButton(this->ui->gripPos, 8);
    armControls->addButton(this->ui->gripNeg, 9);

    foreach(QAbstractButton *button, armControls->buttons())
    {
        button->setDisabled(true);
        ((QPushButton*)button)->setFlat(true);
        button->setText("");
    }

    connect(this->ui->rotPos, &QPushButton::clicked, this, &MainWindow::setArmLRP);
    connect(this->ui->rotNeg, &QPushButton::clicked, this, &MainWindow::setArmLRN);

    connect(this->ui->reaPos, &QPushButton::clicked, this, &MainWindow::setArmExtensionP);
    connect(this->ui->reaNeg, &QPushButton::clicked, this, &MainWindow::setArmExtensionN);

    connect(this->ui->heiPos, &QPushButton::clicked, this, &MainWindow::setArmHeightP);
    connect(this->ui->heiNeg, &QPushButton::clicked, this, &MainWindow::setArmHeightN);

    connect(this->ui->angPos, &QPushButton::clicked, this, &MainWindow::setGripAngleP);
    connect(this->ui->angNeg, &QPushButton::clicked, this, &MainWindow::setGripAngleN);

    connect(this->ui->gripPos, &QPushButton::clicked, this, &MainWindow::setGripP);
    connect(this->ui->gripNeg, &QPushButton::clicked, this, &MainWindow::setGripN);
}

/**
 * @brief MainWindow::initSearchBar
 */
void MainWindow::initSearchBar()
{
    connect(this->ui->searchBar, &QLineEdit::returnPressed, this, &MainWindow::sensorSearch);
}

/**
 * @brief MainWindow::initMovement
 */
void MainWindow::initMovement()
{
    connect(this->ui->trot, &QRadioButton::clicked, this, &MainWindow::trot);
    connect(this->ui->stand, &QRadioButton::clicked, this, &MainWindow::stand);
}

/**
 * @brief MainWindow::initViews
 */
void MainWindow::initViews()
{
    connect(this->ui->front, &QRadioButton::clicked, this, &MainWindow::updateView);
    connect(this->ui->back, &QRadioButton::clicked, this, &MainWindow::updateView);
    connect(this->ui->top, &QRadioButton::clicked, this, &MainWindow::updateView);
    connect(this->ui->gripper, &QRadioButton::clicked, this, &MainWindow::updateView);
    connect(this->ui->side, &QRadioButton::clicked, this, &MainWindow::updateView);
}

/**
 * @brief MainWindow::initXInputWrapper
 */
void MainWindow::initXInputWrapper()
{
    xWrapper = new IWindows_XInput_Wrapper;
    xWrapper->Setup();

    connect(xWrapper, &IWindows_XInput_Wrapper::ButtonPressed, this, &MainWindow::GetButtons);
    connect(xWrapper, &IWindows_XInput_Wrapper::LeftThumbStick, this, &MainWindow::GetLeftThumbstick);
    connect(xWrapper, &IWindows_XInput_Wrapper::RightThumbStick, this, &MainWindow::GetRightThumbstick);

    xWrapper->Start();
}

/**
 * @brief MainWindow::initStopwatch
 */
void MainWindow::initStopwatch()
{
    stopwatch = new QElapsedTimer;

    poller = new QTimer;
    poller->setInterval(100);

    connect(poller, &QTimer::timeout, this, &MainWindow::updateTime);

    stopwatch->start();
    poller->start();

}

/**
 * @brief MainWindow::initWindowSwap
 */
void MainWindow::initWindowSwap()
{
    secondaryWindow = new  XmlWindow();
    windowPoller = new QTimer;

    connect(this->ui->windowSwap, &QPushButton::clicked, this, &MainWindow::swapWindows);
    connect(windowPoller, &QTimer::timeout, this, &MainWindow::pendingWindow);

    windowPoller->start(10);
}
// ---------------------------------- SWAP WINDOWS ----------------------------------

/**
 * @brief MainWindow::swapWindows
 */
void MainWindow::swapWindows()
{
    this->hide();
    secondaryWindow->show();
}

void MainWindow::pendingWindow()
{
    if (!secondaryWindow->isVisible())
        this->show();
}

// ---------------------------------- TIME ----------------------------------

/**
 * @brief MainWindow::updateTime
 */
void MainWindow::updateTime()
{
    this->ui->simTimeLCD->display((float)stopwatch->elapsed() / (float)1000);
}

// ---------------------------------- COMMUNICATION ------------------------------------

/**
 * @brief MainWindow::connectTCP
 */
void MainWindow::connectTCP0()
{
    _pSocket0 = new QTcpSocket(this);
    _pSocket0->connectToHost("127.0.0.1", 9000);
}

/**
 * @brief MainWindow::sendData
 */
void MainWindow::writeTCP0(std::string msg)
{
    _pSocket0->write(msg.c_str(), qstrlen(msg.c_str()));
}

/**
 * @brief MainWindow::connectTCP
 */
void MainWindow::connectTCP1()
{
    _pSocket1 = new QTcpSocket(this);
    connect(_pSocket1, &QTcpSocket::readyRead, this, &MainWindow::readTCP1);
    _pSocket1->connectToHost("127.0.0.1", 8080);
}

/**
 * @brief MainWindow::sendData
 */
void MainWindow::writeTCP1(std::string msg)
{
    _pSocket1->write(msg.c_str(), qstrlen(msg.c_str()));
}

/**
 * @brief MainWindow::readTcpData
 */
void MainWindow::readTCP1()
{
    QByteArray data = _pSocket1->readAll();
    this->ui->textEdit->setText(data);
}

// ---------------------------------- XBOX CONTROLLER SLOT ----------------------------------

/**
 * @brief MainWindow::GetButtons
 * @param uID
 * @param PressedButtons
 */
void MainWindow::GetButtons(short uID, QList<XboxOneButtons> PressedButtons)
{
    Q_UNUSED(uID);

    std::string data;

    if (!this->ui->stand->isChecked())
    {
        if (PressedButtons.contains(XboxOneButtons::X1_up))
        {
            data = "X2.00000";
            writeTCP0(data);
            this->ui->vxLCD->display(2.00);
            this->ui->vxSlider->setValue(99);
        }
        else if (PressedButtons.contains(XboxOneButtons::X1_down))
        {
            data = "X-2.0000";
            this->ui->vxLCD->display(-2.00);
            this->ui->vxSlider->setValue(-99);
            writeTCP0(data);
        }
        else if (PressedButtons.contains(XboxOneButtons::X1_left))
        {
            data = "Y1.0000";
            writeTCP0(data);
            this->ui->vyLCD->display(-1.00);
            this->ui->vySlider->setValue(-99);
        }
        else if(PressedButtons.contains(XboxOneButtons::X1_right))
        {
            data = "Y-1.00000";
            writeTCP0(data);
            this->ui->vyLCD->display(1.00);
            this->ui->vySlider->setValue(99);
        }
        else if (PressedButtons.contains(XboxOneButtons::X1_a))
        {
            data = "Y0.00000";
            this->ui->vyLCD->display(0);
            this->ui->vySlider->setValue(0);
            writeTCP0(data);
        }
        else if (PressedButtons.contains(XboxOneButtons::X1_x))
        {
            data = "X0.00000";
            this->ui->vxLCD->display(0);
            this->ui->vxSlider->setValue(0);
            writeTCP0(data);
        }
    }
    else
    {
        if (PressedButtons.contains(XboxOneButtons::X1_up))
           up();
        else if (PressedButtons.contains(XboxOneButtons::X1_down))
            down();

        if (PressedButtons.contains(XboxOneButtons::X1_x))
            setGripP();
        else if (PressedButtons.contains(XboxOneButtons::X1_a))
            setGripN();
    }
}

/**
 * @brief MainWindow::GetLeftThumbstick
 * @param uID
 * @param x
 * @param y
 */
void MainWindow::GetLeftThumbstick(short uID, double x, double y)
{
    Q_UNUSED(uID);
    Q_UNUSED(x);

    if (!this->ui->stand->isChecked())
    {
        if (y != 0)
        {
            // combine y position into a string
            std::string data = "P";
            data.append(std::to_string(y));

            // send the current position
            writeTCP0(data);
            this->ui->thetaLCD->display(y);
        }
        else if (y == 0 && this->ui->thetaLCD->value() != (double)0 && !jPad->knopPressed)
        {
            std::string data = "P0.00000";
            writeTCP0(data);
            this->ui->thetaLCD->display(0);
        }
    }
    else
    {
        if (x >= .15)
            setArmLRP();
        else if (x <= -.15)
            setArmLRN();

        if (y >= .15)
            setArmExtensionP();
        else if (y <= -.15)
            setArmExtensionN();
    }
}

/**
 * @brief MainWindow::GetRightThumbstick
 * @param uID
 * @param x
 * @param y
 */
void MainWindow::GetRightThumbstick(short uID, double x, double y)
{
    Q_UNUSED(uID);
    Q_UNUSED(y);

    if (!this->ui->stand->isChecked())
    {
        if (x != 0)
        {
            // combine x position into a string
            std::string data = "R";
            data.append(std::to_string(x));

            // send the current position
            writeTCP0(data);
            this->ui->omegaLCD->display(x);
        }
        else if (x == 0 && this->ui->omegaLCD->value() != (double)0 && !jPad->knopPressed)
        {
            std::string data = "R0.00000";
            writeTCP0(data);
            this->ui->omegaLCD->display(0);
        }
    }
    else
    {
        if (x >= .15)
            setGripAngleP();
        else if (x <= -.15)
            setGripAngleN();

        if (y >= .15)
            setArmHeightP();
        else if (y <= -.15)
            setArmHeightN();
    }
}

// ---------------------------------- KEYPRESS SLOT ------------------------------------

/**
 * @brief MainWindow::keyPressEvent
 * @param k
 */
void MainWindow::keyPressEvent( QKeyEvent *k )
{
    std::string data = "";

    switch ( k->key() )
    {
    case Qt::Key_W:
        data = "X2.00000";
        writeTCP0(data);
        this->ui->vxLCD->display(2.00);
        this->ui->vxSlider->setValue(99);
        break;

    case Qt::Key_S:
        data = "X-2.0000";
        this->ui->vxLCD->display(-2.00);
        this->ui->vxSlider->setValue(-99);
        writeTCP0(data);
        break;

    case Qt::Key_A:
        data = "Y1.0000";
        writeTCP0(data);
        this->ui->vyLCD->display(-1.00);
        this->ui->vySlider->setValue(-99);
        break;

    case Qt::Key_D:
        data = "Y-1.00000";
        writeTCP0(data);
        this->ui->vyLCD->display(1.00);
        this->ui->vySlider->setValue(99);
        break;

    case Qt::Key_Q:
        data = "D0.00000";
        writeTCP0(data);
        break;

    case Qt::Key_E:
        data = "U0.00000";
        writeTCP0(data);
        break;

    case Qt::Key_V:
        data = "X0.00000";
        this->ui->vxLCD->display(0);
        this->ui->vxSlider->setValue(0);
        writeTCP0(data);
        break;

    case Qt::Key_B:
        data = "Y0.00000";
        this->ui->vyLCD->display(0);
        this->ui->vySlider->setValue(0);
        writeTCP0(data);
        break;

    case Qt::Key_R:
        stand();
        break;

    case Qt::Key_T:
        trot();
        break;

    default:
        break;
    }
}

// ---------------------------------- SEARCH SLOT ------------------------------------

/**
 * @brief MainWindow::sensorSearch
 */
void MainWindow::sensorSearch()
{
    std::string data = "I";
    data.append(this->ui->searchBar->text().toStdString());
    writeTCP0(data);
}

// ---------------------------------- VIEW SLOT ------------------------------------

/**
 * @brief MainWindow::updateView
 */
void MainWindow::updateView()
{
    std::string data = "V";

    if (this->ui->front->isChecked())
        data.append("1.00000");
    else if (this->ui->back->isChecked())
        data.append("2.00000");
    else if (this->ui->top->isChecked())
        data.append("3.00000");
    else if (this->ui->side->isChecked())
        data.append("4.00000");
    else
        data.append("5.00000");

    writeTCP0(data);
}

// ---------------------------------- JOYPAD SLOTS ------------------------------------

/**
 * @brief MainWindow::xChanged
 */
void MainWindow::xChanged()
{
    if (this->ui->thetaLock->isChecked())
    {
        // combine x position into a string
        std::string data = "";
        data.push_back('R');
        data.append(std::to_string(jPad->x()));

        // send the current position
        writeTCP0(data);
        this->ui->omegaLCD->display(jPad->x());
    }
}

/**
 * @brief MainWindow::yChanged
 */
void MainWindow::yChanged()
{
    if (this->ui->omegaLock->isChecked())
    {
        // combine y position into a string
        std::string data = "";
        data.push_back('P');
        data.append(std::to_string(jPad->y()));

        // send the current position
        writeTCP0(data);
        this->ui->thetaLCD->display(jPad->y());
    }
}

/**
 * @brief MainWindow::xyChanged
 */
void MainWindow::xyChanged()
{
    if (this->ui->unlock->isChecked())
    {
        // combine xy position into a string
        std::string data = "";
        data.push_back('C');
        data.append(std::to_string(jPad->x()));
        data.push_back(',');
        data.append(std::to_string(jPad->y()));

        // send the current position
        writeTCP0(data);
        this->ui->omegaLCD->display(jPad->x());
        this->ui->thetaLCD->display(jPad->y());
    }
}

// ---------------------------------- VELOCITY SLIDER SLOTS ------------------------------------

/**
 * @brief MainWindow::setVelocityX
 */
void MainWindow::setVelocityX()
{
    std::string data = "X";
    double vx = (double)this->ui->vxSlider->value() / (double)48.5;
    data.append(std::to_string(vx));
    writeTCP0(data);
    this->ui->vxLCD->display(vx);
}

/**
 * @brief MainWindow::setVelocityY
 */
void MainWindow::setVelocityY()
{
    std::string data = "Y";
    double vy = (double)-1 * (double)(this->ui->vySlider->value() / (double)97);
    data.append(std::to_string(vy));
    writeTCP0(data);
    this->ui->vyLCD->display(-1 * vy);
}

/**
 * @brief MainWindow::resetX
 */
void MainWindow::resetX()
{
    std::string data = "X0.00000";
    writeTCP0(data);
    this->ui->vxSlider->setValue(0);
    this->ui->vxLCD->display(0.00);
}

/**
 * @brief MainWindow::resetY
 */
void MainWindow::resetY()
{
    std::string data = "Y0.00000";
    writeTCP0(data);
    this->ui->vySlider->setValue(0);
    this->ui->vyLCD->display(0.00);
}

// ---------------------------------- HEIGHT SLOTS ------------------------------------

/**
 * @brief MainWindow::up
 */
void MainWindow::up()
{
    std::string data = "U0.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::down
 */
void MainWindow::down()
{
    std::string data = "D0.00000";
    writeTCP0(data);
}

// ---------------------------------- MOVEMENT SLOTS ------------------------------------

/**
 * @brief MainWindow::stand
 */
void MainWindow::stand()
{
    int i = 0;

    foreach(QAbstractButton *button, armControls->buttons())
    {
        button->setDisabled(false);
        ((QPushButton*)button)->setFlat(false);

        if (i % 2 == 0)
            button->setText("+");
        else
            button->setText("-");
        i++;
    }

    std::string data = "S0.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::trot
 */
void MainWindow::trot()
{
    foreach(QAbstractButton *button, armControls->buttons())
    {
        button->setDisabled(true);
        ((QPushButton*)button)->setFlat(true);
        button->setText("");
    }

    std::string data = "T1.00000";
    writeTCP0(data);
}

// ---------------------------------- ARM SLOTS ------------------------------------

/**
 * @brief MainWindow::setArmLRP
 */
void MainWindow::setArmLRP()
{
    std::string data = "00.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setArmLRN
 */
void MainWindow::setArmLRN()
{
    std::string data = "10.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setArmExtensionP
 */
void MainWindow::setArmExtensionP()
{
    std::string data = "20.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setArmExtensionN
 */
void MainWindow::setArmExtensionN()
{
    std::string data = "30.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setArmHeightP
 */
void MainWindow::setArmHeightP()
{
    std::string data = "40.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setArmHeightN
 */
void MainWindow::setArmHeightN()
{
    std::string data = "50.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setGripAngleP
 */
void MainWindow::setGripAngleP()
{
    std::string data = "60.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setGripAngleN
 */
void MainWindow::setGripAngleN()
{
    std::string data = "70.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setGripP
 */
void MainWindow::setGripP()
{
    std::string data = "80.00000";
    writeTCP0(data);
}

/**
 * @brief MainWindow::setGripN
 */
void MainWindow::setGripN()
{
    std::string data = "90.00000";
    writeTCP0(data);
}
