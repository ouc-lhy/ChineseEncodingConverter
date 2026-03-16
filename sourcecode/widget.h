#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QStringList>
#include <QListWidgetItem>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButtonGetFile_clicked();
    void on_pushButtonDir_clicked();
    void on_pushButtonSave_clicked();
    void on_listWidgetInput_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_radioButtonSave_toggled(bool checked);
    void on_radioButtonSaveAs_toggled(bool checked);
    void on_checkBox_toggled(bool checked);
    void on_lineEdit_2_textEdited(const QString &text);
    void on_lineEdit_3_textEdited(const QString &text);
    void on_comboBoxOrigin_currentIndexChanged(int index);
    void on_listWidgetInput_customContextMenuRequested(const QPoint &pos);
    void deleteSelectedSourceFile();
    void on_pushButtonClear_clicked();
    void on_listWidgetOutput_itemChanged(QListWidgetItem *item);

private:
    Ui::Widget *ui;
    QStringList m_fileList;
    QString m_outputDir;



    QString getOriginCodecName() const;
    QString getTargetCodecName() const;
    bool convertFile(const QString &sourcePath, const QString &destPath,
                     const QString &fromCodecName, const QString &toCodecName,
                     QString &errorString);

    void updateOutputListDisplay();
    QString generateDestFileName(const QString &sourcePath) const;
    void previewFile(const QString &filePath, bool isSource);
    void initStyle();
private:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // WIDGET_H
