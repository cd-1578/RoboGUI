#include "xmlwindow.h"
#include "ui_xmlwindow.h"

XmlWindow::XmlWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::XmlWindow)
{
    ui->setupUi(this);
    connect(this->ui->pushButton, &QPushButton::clicked, this, &XmlWindow::swapWindows);
    connect(this->ui->textEdit, &QTextEdit::textChanged, this, &XmlWindow::processFile);
}

XmlWindow::~XmlWindow()
{
    delete ui;
}

void XmlWindow::swapWindows()
{
    this->hide();
}

void XmlWindow::processFile()
{
    QString path = this->ui->textEdit->toPlainText();
    path.remove(0,8);

    xml = new QFile(path);

    xml->open((QIODevice::ReadWrite | QIODevice::Text));

    if (xml->isOpen())
        this->ui->label->setText("File Status: Open");
    else
        this->ui->label->setText("File Status: Failed to Open");

    createMujocoXML();
}

void XmlWindow::createMujocoXML()
{
    const QString result = "<mujoco model=\"a1 scene\">\n"
                     "\t<include file=\"a1_arm.xml\"/>\n"
                     "\t<statistic center=\"0 0 0.1\" extent=\"0.8\"/>\n\n"
                     "\t<visual>\n"
                     "\t\t<headlight diffuse=\"0.6 0.6 0.6\" ambient=\"0.3 0.3 0.3\" specular=\"0 0 0;\"/>\n"
                     "\t\t<rgba haze=\"0.15 0.25 0.35 1\"/>\n"
                     "\t\t<global azimuth=\"120\" elevation=\"-20\"/>\n"
                     "\t</visual>\n\n"
                     "\t<asset>\n"
                     "\t\t<texture type=\"skybox\" builtin=\"gradient\" rgb1=\"0.3 0.5 0.7\" rgb2=\"0 0 0\" width=\"512\" height=\"3072\"/>\n"
                     "\t\t<texture type=\"2d\" name=\"groundplane\" builtin=\"checker\" mark=\"edge\" rgb1=\"0.2 0.3 0.4\" rgb2=\"0.1 0.2 0.3\"\n"
                     "\t\t\ttmarkrgb=\"0.8 0.8 0.8\" width=\"300\" height=\"300\"/>\n"
                     "\t\t<material name=\"groundplane\" texture=\"groundplane\" texuniform=\"true\" texrepeat=\"5 5\" reflectance=\"0.2\"/>\n"
                     "\t</asset>\n\n"
                     "\t<worldbody>\n"
                     "\t\t<light pos=\"0 0 1.5\" dir=\"0 0 -1\" directional=\"true\"/>\n"
                     "\t\t<geom name=\"floor\" size=\"0 0 0.05\" type=\"plane\" material=\"groundplane\"/>\n";

    const QString close = "\t</worldbody>\n"
                          "</mujoco>\n";

    const QByteArray cuboid = "<type>cuboid</type>";
    int count = 0;

    while (!xml->atEnd())
    {
        QByteArray line = xml->readLine();

        if (line.contains(cuboid))
            count++;
    }

    this->xml->reset(); // go to top of file
    QTextStream outfile(this->xml);
    outfile << result; // overwrite with mujoco file structure

    for (int i = 0; i < count/2; ++i)
    {
        outfile << "\t\t<body pos = \"2 2 0\">\n"
                << "\t\t\t<geom type=\"box\" size=\".03 .03 .3\" rgba=\"0 .9 0 1\" mass=\"1\"/>\n"
                << "\t\t</body>\n";
    }

    outfile << close;
}

