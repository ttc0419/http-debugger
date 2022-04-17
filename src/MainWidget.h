#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTableWidget>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
	Q_OBJECT

public:
	MainWidget(QWidget *parent = nullptr);
	~MainWidget();

private slots:
	void on_sendButton_released();
	void on_methodComboBox_currentTextChanged(const QString &text);
	void on_requestBodyBinarySelectPushButton_released();

	void onRelyFinished(QNetworkReply *reply);
	void onTableWidgetLastRowChanged(int row, int column);
	void onDeleteButtonReleased();

private:
	enum ResponseTabIndex: int {
		BODY_TAB,
		HEADERS_TAB,
		COOKIES_TAB
	};

	enum RequestBodyType: int {
		ENCODED,
		TEXT,
		BINARY
	};

	Ui::MainWidget *ui;
	QNetworkAccessManager *networkManager;
	QTime lastRequestStartTime;

	void addCellActionButton(QTableWidget *tableWidget, int row);
};
#endif // MAIN_WIDGET_H
