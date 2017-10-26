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
    ui->okBtn->setEnabled(true);
    setIp(ui->ipLineEdit->text());
    setPort(ui->portLineEdit->text().toInt());

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0) << QString("P0");
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    m_tcpSocket.write(block);
}

void NetworkDialog::connectionClosedByServer()
{
    if (m_nextBlockSize != 0xFFFFFFFF) {

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
            if (m_tcpSocket.bytesAvailable() < sizeof(qint64))
                break;
            in >> m_nextBlockSize;
        }
        if (m_nextBlockSize == 0xFFFFFFFF) {
            closeConnection();
            break;
        }

        if (m_tcpSocket.bytesAvailable() < m_nextBlockSize)
            break;

        QString responseType;
        in >> responseType;
        if (responseType == "R0") {
            in >> m_uavs >> m_weapons;
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
        }
        if (responseType == "R1") {
            quint8 id;
            in >> id;
            setId(id);
            accept();
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
    ui->connectToServerBtn->setEnabled(true);
}

void NetworkDialog::sendCloseSign()
{
    QDataStream out(&m_tcpSocket);
    out << qint64(0xFFFFFFFF);
    m_tcpSocket.flush();
}

int NetworkDialog::index() const
{
    return m_index;
}

void NetworkDialog::setIndex(int index)
{
    m_index = index;
}

int NetworkDialog::id() const
{
    return m_id;
}

void NetworkDialog::setId(int id)
{
    m_id = id;
}

QPair<int, QString> NetworkDialog::selected() const
{
    return m_selected;
}

void NetworkDialog::setSelected(const QPair<int, QString> &selected)
{
    m_selected = selected;
}

int NetworkDialog::port() const
{
    return m_port;
}

void NetworkDialog::setPort(int port)
{
    m_port = port;
}

QString NetworkDialog::ip() const
{
    return m_ip;
}

void NetworkDialog::setIp(const QString &ip)
{
    m_ip = ip;
}

QVector<Weapon> NetworkDialog::weapons() const
{
    return m_weapons;
}

void NetworkDialog::setWeapons(const QVector<Weapon> &weapons)
{
    m_weapons = weapons;
}

QVector<UAV> NetworkDialog::uavs() const
{
    return m_uavs;
}

void NetworkDialog::setUavs(const QVector<UAV> &uavs)
{
    m_uavs = uavs;
}

void NetworkDialog::on_connectToServerBtn_clicked()
{
    m_tcpSocket.connectToHost(ui->ipLineEdit->text(), ui->portLineEdit->text().toInt());
    ui->ipLineEdit->setDisabled(true);
    ui->portLineEdit->setDisabled(true);
    ui->connectToServerBtn->setDisabled(true);
    ui->uavslistWidget->clear();
    m_nextBlockSize = 0;
}


void NetworkDialog::on_uavslistWidget_itemClicked(QListWidgetItem *item)
{
    QVariant i = item->data(MyImgRole);
    QVariant d = item->data(MyDescRole);
    QPixmap p = i.value<QPixmap>().scaled(150, 150);
    ui->uavImageTextLabel->setPixmap(p);
}

void NetworkDialog::on_okBtn_clicked()
{
    int row = ui->uavslistWidget->currentRow();
    if (row != -1) {
        QString name = ui->uavslistWidget->currentItem()->data(Qt::DisplayRole).toString();
        QPair<int, QString> pair(row, name);
        setSelected(pair);
        setIndex(row);

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << qint64(0)
            << QString("P1")
            << quint8(row)
            << name;
        out.device()->seek(0);
        out << qint64(block.size() - sizeof(qint64));
        m_tcpSocket.write(block);
    }
}
