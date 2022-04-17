#include "MainWidget.h"
#include "./ui_MainWidget.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QUrlQuery>

static const QString SAME_SITE_STRINGS[] = {QObject::tr("Default"), QObject::tr("None"), QObject::tr("Lax"), QObject::tr("Strict")};

MainWidget::MainWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MainWidget), networkManager(new QNetworkAccessManager(this))
{
	ui->setupUi(this);
	ui->responseHeaderTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->responseCookieTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	connect(networkManager, &QNetworkAccessManager::finished, this, &MainWidget::onRelyFinished);
	connect(ui->requestParameterTableWidget, &QTableWidget::cellChanged, this, &MainWidget::onTableWidgetLastRowChanged);
	connect(ui->requestHeaderTableWidget, &QTableWidget::cellChanged, this, &MainWidget::onTableWidgetLastRowChanged);
	connect(ui->requestBodyEncodedTableWidget, &QTableWidget::cellChanged, this, &MainWidget::onTableWidgetLastRowChanged);

	addCellActionButton(ui->requestParameterTableWidget, 0);
	addCellActionButton(ui->requestHeaderTableWidget, 0);
	addCellActionButton(ui->requestBodyEncodedTableWidget, 0);
}

MainWidget::~MainWidget()
{
	delete ui;
	delete networkManager;
}

void MainWidget::on_sendButton_released()
{
	QUrl url = QUrl(ui->urlLineEdit->text(), QUrl::ParsingMode::StrictMode);

	if (!url.isValid()) {
		QMessageBox::warning(this, tr("Invalid Request URL"), '"' + ui->urlLineEdit->text() + '"' + tr(" is not a valid URL!"));
		return;
	}

	if (url.scheme().isEmpty())
		url.setScheme("http");

	if (url.scheme() != "http" && url.scheme() != "https") {
		QMessageBox::warning(this, tr("Invalid Scheme"), tr("Only http/https are supported"));
		return;
	}

	if (url.toString().mid(url.scheme().size(), 3) != "://")
		url.setUrl(url.scheme() + "://" + url.host() + url.path());

	QNetworkRequest request = QNetworkRequest(url);
	ui->sendButton->setDisabled(true);
	ui->responseBodyPlainTextEdit->clear();
	lastRequestStartTime = QTime::currentTime();

	/* Parameters Tab */
	QUrlQuery queries;
	for (int i = 0; i < ui->requestParameterTableWidget->rowCount() - 1; i++) {
		QString name = ui->requestParameterTableWidget->item(i, 0)->text();
		if (!name.isEmpty()) {
			queries.addQueryItem(name, ui->requestParameterTableWidget->item(i, 1)->text());
		}
	}
	url.setQuery(queries);

	/* Headers Tab */
	for (int i = 0; i < ui->requestHeaderTableWidget->rowCount() - 1; i++) {
		QString name = ui->requestHeaderTableWidget->item(i, 0)->text();
		if (!name.isEmpty()) {
			request.setRawHeader(name.toUtf8(), ui->requestHeaderTableWidget->item(i, 1)->text().toUtf8());
		}
	}

	if (ui->methodComboBox->currentText() == "GET") {
		networkManager->get(request);
	} else if (ui->methodComboBox->currentText() == "POST" || ui->methodComboBox->currentText() == "PUT") {
		/* Body Tab */
		QByteArray body;

		if (ui->requestBodyTypeTabWidget->currentIndex() == ENCODED) {
			QUrlQuery encodedBody;
			for (int i = 0; i < ui->requestBodyEncodedTableWidget->rowCount() - 1; i++) {
				QString name = ui->requestBodyEncodedTableWidget->item(i, 0)->text();
				if (!name.isEmpty()) {
					encodedBody.addQueryItem(name, ui->requestBodyEncodedTableWidget->item(i, 1)->text());
				}
			}
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			body = encodedBody.toString(QUrl::FullyEncoded).toUtf8();
		} else if (ui->requestBodyTypeTabWidget->currentIndex() == TEXT) {
			request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain;charset=UTF-8");
			body = ui->requestBodyPlainTextEdit->toPlainText().toUtf8();
		} else if (ui->requestBodyTypeTabWidget->currentIndex() == BINARY) {
			if (ui->requestBodyBinaryFileNameLabel->text() == "No File Selected") {
				QMessageBox::warning(this, tr("Invalid File"), tr("No File Selected"));
				return;
			} else {
				body = QFile(ui->requestBodyBinaryFileNameLabel->text()).readAll();
			}
		}

		if (ui->methodComboBox->currentText() == "POST")
			networkManager->post(request, body);
		else if ( ui->methodComboBox->currentText() == "PUT")
			networkManager->put(request, body);
	} else if (ui->methodComboBox->currentText() == "DELETE") {
		networkManager->deleteResource(request);
	} else if (ui->methodComboBox->currentText() == "HEAD") {
		networkManager->head(request);
	}
}

void MainWidget::on_methodComboBox_currentTextChanged(const QString &text)
{
	ui->requestBodyTab->setEnabled(text == "POST" || text == "PUT");
}

void MainWidget::onRelyFinished(QNetworkReply *reply)
{
	QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	if (!statusCode.isValid()) {
		QMessageBox::warning(this, tr("Invalid Response Received"), tr("Please check you request parameters and make sure the server is running!"));
	} else {
		/* Statistic Labels */
		ui->statusLabel->setText(tr("Status: ") + statusCode.toString() + ' ' + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
		ui->timeLabel->setText(tr("Time: ") + QString::number(lastRequestStartTime.msecsTo(QTime::currentTime())) + "ms");

		/* Response Body Tab */
		ui->responseTabWidget->setTabText(BODY_TAB, tr("Body") + " (" + this->locale().formattedDataSize(reply->bytesAvailable()) + ')');
		ui->responseBodyPlainTextEdit->appendPlainText(QString(reply->readAll()));

		/* Reponse Headers Tab */
		QList<QNetworkReply::RawHeaderPair> headers = reply->rawHeaderPairs();
		ui->responseHeaderTableWidget->clearContents();
		ui->responseHeaderTableWidget->setRowCount(headers.size());
		ui->responseTabWidget->setTabText(HEADERS_TAB, tr("Headers") + " (" + QString::number(headers.size()) + ')');

		for (int i = 0; i < headers.size(); i++) {
			ui->responseHeaderTableWidget->setItem(i, 0, new QTableWidgetItem(headers[i].first));
			ui->responseHeaderTableWidget->setItem(i, 1, new QTableWidgetItem(headers[i].second));
		}

		ui->responseHeaderTableWidget->resizeColumnsToContents();

		/* Response Cookies Tab */
		QList<QNetworkCookie> cookies = networkManager->cookieJar()->cookiesForUrl(reply->url());
		ui->responseCookieTableWidget->setRowCount(cookies.size());
		ui->responseTabWidget->setTabText(COOKIES_TAB, "Cookies (" + QString::number(cookies.size()) + ')');

		for (int i = 0; i < cookies.size(); i++) {
			ui->responseCookieTableWidget->setItem(i, 0, new QTableWidgetItem(QString(cookies[i].name())));
			ui->responseCookieTableWidget->setItem(i, 1, new QTableWidgetItem(QString(cookies[i].value())));
			ui->responseCookieTableWidget->setItem(i, 2, new QTableWidgetItem(cookies[i].domain()));
			ui->responseCookieTableWidget->setItem(i, 3, new QTableWidgetItem(cookies[i].path()));
			ui->responseCookieTableWidget->setItem(i, 4, new QTableWidgetItem(cookies[i].expirationDate().isNull() ? tr("N/A") : cookies[i].expirationDate().toString("yyyy-MM-dd HH:mm:ss")));
			ui->responseCookieTableWidget->setItem(i, 5, new QTableWidgetItem(cookies[i].isSessionCookie() ? tr("Yes") : tr("No")));
			ui->responseCookieTableWidget->setItem(i, 6, new QTableWidgetItem(cookies[i].isHttpOnly() ? tr("Yes") : tr("No")));
			ui->responseCookieTableWidget->setItem(i, 7, new QTableWidgetItem(cookies[i].isSecure() ? tr("Yes") : tr("No")));
			ui->responseCookieTableWidget->setItem(i, 8, new QTableWidgetItem(SAME_SITE_STRINGS[static_cast<int>(cookies[i].sameSitePolicy())]));
		}

		ui->responseCookieTableWidget->resizeColumnsToContents();
	}

	ui->sendButton->setDisabled(false);
	reply->deleteLater();
}

void MainWidget::onTableWidgetLastRowChanged(int row, int column)
{
	QTableWidget *tableWidget = qobject_cast<QTableWidget *>(sender());

	if (tableWidget->rowCount() > row + 1 || (tableWidget->itemAt(row, 0)->text().isEmpty() && tableWidget->itemAt(row, 1)->text().isEmpty()))
		return;

	tableWidget->setRowCount(row + 2);
	addCellActionButton(tableWidget, row + 1);
}

void MainWidget::addCellActionButton(QTableWidget *tableWidget, int row)
{
	QWidget *widget = new QWidget();
	QPushButton *deletePushButton = new QPushButton("Delete");
	QHBoxLayout *layout = new QHBoxLayout(widget);

	layout->addWidget(deletePushButton);
	layout->setAlignment(Qt::AlignCenter);
	layout->setContentsMargins(0, 0, 0, 0);
	widget->setLayout(layout);
	tableWidget->setCellWidget(row, 2, widget);

	connect(deletePushButton, &QPushButton::released, this, &MainWidget::onDeleteButtonReleased);
}

void MainWidget::onDeleteButtonReleased()
{
	QWidget *cell = qobject_cast<QWidget *>(sender()->parent());
	QTableWidget *tableWidget = qobject_cast<QTableWidget *>(cell->parent()->parent());
	tableWidget->removeRow(tableWidget->indexAt(cell->pos()).row());
}

void MainWidget::on_requestBodyBinarySelectPushButton_released()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Choose a file to upload"));
	if (fileName.isNull())
		return;

	ui->requestBodyBinaryFileNameLabel->setText(fileName);

	QMimeDatabase mimeDb;
	for (int i = 0; i < ui->requestHeaderTableWidget->rowCount() - 1; i++) {
		if (ui->requestHeaderTableWidget->item(i, 0)->text() == "Content-Type") {
			ui->requestHeaderTableWidget->setItem(i, 1, new QTableWidgetItem(mimeDb.mimeTypeForFile(QFileInfo(ui->requestBodyBinaryFileNameLabel->text())).name()));
			addCellActionButton(ui->requestHeaderTableWidget, i + 1);
			ui->requestHeaderTableWidget->resizeColumnsToContents();
			return;
		}
	}

	int rowCount = ui->requestHeaderTableWidget->rowCount();
	ui->requestHeaderTableWidget->setRowCount(rowCount + 1);
	ui->requestHeaderTableWidget->setItem(rowCount - 1, 0, new QTableWidgetItem("Content-Type"));
	ui->requestHeaderTableWidget->setItem(rowCount - 1, 1, new QTableWidgetItem(mimeDb.mimeTypeForFile(QFileInfo(ui->requestBodyBinaryFileNameLabel->text())).name()));
	addCellActionButton(ui->requestHeaderTableWidget, rowCount);
	ui->requestHeaderTableWidget->resizeColumnsToContents();
}
