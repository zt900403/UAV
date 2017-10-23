#include "networkdialog.h"
#include "ui_networkdialog.h"
#include <QListWidgetItem>

NetworkDialog::NetworkDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetworkDialog)
{
    ui->setupUi(this);
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->ipLineEdit->setValidator(ipValidator);

    QRegExp portRegex("^([0-9]{1,5})$");
    QRegExpValidator *portValidator = new QRegExpValidator(portRegex, this);
    ui->portLineEdit->setValidator(portValidator);

    initWidgets();

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(sendRequest()));
    connect(&m_tcpSocket, SIGNAL(disconnected()), this, SLOT(connectionClosedByServer()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(updateUAVWidgets()));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
}

NetworkDialog::~NetworkDialog()
{
    delete ui;
}

void NetworkDialog::sendRequest()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << quint16(0) << QString("P0");
    out.device()->seek(0);
    out << quint16(block.size() - sizeof(quint16));
    m_tcpSocket.write(block);
}

void NetworkDialog::connectionClosedByServer()
{
    if (m_nextBlockSize != 0xFFFF) {

    }
    closeConnection();
}

void NetworkDialog::error()
{
    closeConnection();
}

void NetworkDialog::updateUAVWidgets()
{
    QDataStream in(&m_tcpSocket);
    in.setVersion(QDataStream::Qt_4_8);

    forever {
        if (m_nextBlockSize == 0) {
            if (m_tcpSocket.bytesAvailable() < sizeof(quint16))
                break;
            in >> m_nextBlockSize;
        }

        if (m_nextBlockSize== 0xFFFF) {
            closeConnection();
            break;
        }

        if (m_tcpSocket.bytesAvailable() < m_nextBlockSize)
            break;

        QString responseType;
        in >> responseType >> m_uavs >> m_weapons;

        QVectorIterator<UAV> i(m_uavs);
        while(i.hasNext()) {
            UAV u = i.next();
            QString name = u.name();
            QPixmap pixmap = u.pixmap();
            QString desc = u.description();

            QListWidgetItem *item= new QListWidgetItem(name);
            QVariant i;
            QVariant d;
            i.setValue(pixmap);
            d.setValue(desc);
            item->setData(MyImgRole, i);
            item->setData(MyDescRole, d);
            ui->uavslistWidget->addItem(item);
        }
        m_nextBlockSize = 0;
    }
}


void NetworkDialog::initWidgets()
{
    ui->ipLineEdit->setText("127.0.0.1");
    ui->portLineEdit->setText("10666");
    ui->uavslistWidget->clear();
    ui->uavImageTextLabel->setDisabled(true);
    ui->okBtn->setDisabled(true);
}

void NetworkDialog::closeConnection()
{
    m_tcpSocket.close();
    ui->ipLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
}

void NetworkDialog::on_connectToServerBtn_clicked()
{
    m_tcpSocket.connectToHost(ui->ipLineEdit->text(), ui->portLineEdit->text().toInt());
    ui->ipLineEdit->setDisabled(true);
    ui->portLineEdit->setDisabled(true);
    m_nextBlockSize = 0;
}
