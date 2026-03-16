#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextCodec>
#include <QMimeData>
#include <QDir>
#include <QBrush>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_outputDir.clear();
    this->setWindowTitle("中文编码转换器");
    this->setAcceptDrops(true);

    ui->listWidgetInput->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->radioButtonSave->setChecked(true);


    ui->checkBox->setChecked(false);
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);
    ui->checkBox->setEnabled(false);


    ui->pushButtonDir->setEnabled(false);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit->setText(tr("(原地保存)"));
    ui->lineEdit->setStyleSheet("color: gray;");

    initStyle();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initStyle()
{
    QString styleSheet = R"(

        QWidget {
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
            font-size: 14px;
            color: #333333;
            background-color: #F5F6FA;
        }

        QLabel {
            color: #2C3E50;
            font-weight: bold;
        }

        QPushButton {
            background-color: #3498DB;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 15px;
            font-weight: bold;
            min-height: 30px;
        }
        QPushButton:hover {
            background-color: #2980B9;
        }
        QPushButton:pressed {
            background-color: #1ABC9C;
            padding-top: 9px;
            padding-bottom: 7px;
        }
        QPushButton:disabled {
            background-color: #BDC3C7;
            color: #7F8C8D;
        }

        QPushButton#pushButtonClear, QPushButton#deleteAction {
            background-color: #E74C3C;
        }
        QPushButton#pushButtonClear:hover {
            background-color: #C0392B;
        }

        QRadioButton {
            spacing: 8px;
            color: #2C3E50;
        }
        QRadioButton::indicator {
            width: 18px;
            height: 18px;
            border-radius: 9px;
            border: 2px solid #BDC3C7;
            background: white;
        }
        QRadioButton::indicator:checked {
            background-color: #3498DB;
            border-color: #3498DB;
        }

        QCheckBox {
            spacing: 8px;
            color: #2C3E50;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #BDC3C7;
            background: white;
        }
        QCheckBox::indicator:checked {
            background-color: #3498DB;
            border-color: #3498DB;
            image: url(:/icons/check.png);
        }

        QListWidget {
            background-color: white;
            border: 1px solid #DCE4EC;
            border-radius: 6px;
            padding: 0px;
            outline: none;
        }

        QListWidget::item {
            height: 40px;
            padding-left: 15px;
            border-bottom: 1px solid #E0E0E0;
        }

        QListWidget::item:hover {
            background-color: #F5F7FA;
        }


        QListWidget::item:selected {
            background-color: #3498DB;
            color: white;
            border-bottom: 1px solid #2980B9;
        }

        QListWidget::item:last {
            border-bottom: none;
        }

        QListWidget#listWidgetOutput::item:selected {
            background-color: transparent;
            color: #333333;
            border-bottom: 1px solid #EEEEEE;
        }

        QLineEdit {
            background-color: white;
            border: 1px solid #DCE4EC;
            border-radius: 4px;
            padding: 0px 0px;
            selection-background-color: #3498DB;
        }

        QLineEdit:focus {
            border: 1px solid #3498DB;
        }

        QLineEdit:disabled {
            background-color: #ECF0F1;
            color: #7F8C8D;
            border: 1px solid #BDC3C7;
        }

        QTextEdit {
            background-color: #FFFFFF;
            border: 1px solid #DCE4EC;
            border-radius: 6px;
            padding: 10px;
            font-family: "Consolas", "Courier New", monospace;
            font-size: 13px;
        }

        QComboBox {
            background-color: white;
            border: 1px solid #DCE4EC;
            border-radius: 4px;
            padding: 5px 10px;
            min-width: 100px;
        }

        QComboBox:hover {
            border: 1px solid #3498DB;
        }



    )";

    this->setStyleSheet(styleSheet);
}

//生成目标文件名
QString Widget::generateDestFileName(const QString &sourcePath) const
{
    QFileInfo fi(sourcePath);
    QString baseName = fi.baseName();
    QString suffix = fi.suffix();

    if (!ui->checkBox->isChecked()) {
        QString prefix = ui->lineEdit_2->text();
        QString suffixText = ui->lineEdit_3->text();
        return prefix + baseName + suffixText + (suffix.isEmpty() ? "" : "." + suffix);
    } else {
        return fi.fileName();
    }
}


//更新右侧列表显示
void Widget::updateOutputListDisplay()
{

    int currentRow = ui->listWidgetOutput->currentRow();

    ui->listWidgetOutput->clear();

    if (m_fileList.isEmpty()) {
        return;
    }

    if (ui->radioButtonSave->isChecked()) {
        //原地保存
        for (const QString &srcFile : m_fileList) {
            QListWidgetItem *item = new QListWidgetItem(srcFile);
            item->setForeground(QBrush(Qt::black));
            item->setToolTip(tr("原地保存 (覆盖):\n%1").arg(srcFile));
            ui->listWidgetOutput->addItem(item);
        }
    } else {
        //另存为
        if (m_outputDir.isEmpty()) {
            QListWidgetItem *item = new QListWidgetItem(tr("请先选择输出目录..."));
            item->setForeground(QBrush(Qt::gray));
            ui->listWidgetOutput->addItem(item);
        } else {
            for (const QString &srcFile : m_fileList) {
                QString newFileName = generateDestFileName(srcFile);
                QString fullPath = QDir(m_outputDir).filePath(newFileName);

                QListWidgetItem *item = new QListWidgetItem(fullPath);
                item->setForeground(QBrush(Qt::black));
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                item->setToolTip(tr("目标路径:\n%1\n(双击可修改文件名)").arg(fullPath));
                ui->listWidgetOutput->addItem(item);
            }
        }
    }

    // 恢复选中
    if (currentRow >= 0 && currentRow < ui->listWidgetOutput->count()) {
        ui->listWidgetOutput->setCurrentRow(currentRow);
    }
}

//拖拽与文件添加
void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
    else event->ignore();
}

void Widget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    bool changed = false;
    for (const QUrl &url : urls) {
        QString filePath = url.toLocalFile();
        if (QFileInfo(filePath).isFile() && !m_fileList.contains(filePath)) {
            m_fileList.append(filePath);
            ui->listWidgetInput->addItem(filePath);
            changed = true;
        }
    }
    if (changed) {
        ui->listWidgetInput->setCurrentRow(ui->listWidgetInput->count() - 1);
        updateOutputListDisplay();
    }
    event->acceptProposedAction();
}

void Widget::on_pushButtonGetFile_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("选择源文件"), "", tr("All Files (*)"));
    for (const QString &f : files) {
        if (!m_fileList.contains(f)) {
            m_fileList.append(f);
            ui->listWidgetInput->addItem(f);
        }
    }
    if (!files.isEmpty()) {
        ui->listWidgetInput->setCurrentRow(ui->listWidgetInput->count() - 1);
        updateOutputListDisplay();
    }
}

void Widget::on_pushButtonDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择输出目录"));
    if (!dir.isEmpty()) {
        m_outputDir = dir;
        ui->lineEdit->setText(dir);
        ui->lineEdit->setStyleSheet("");
        updateOutputListDisplay();
    }
}

//状态联动逻辑
void Widget::on_radioButtonSave_toggled(bool checked)
{
    if (checked) {
        ui->pushButtonDir->setEnabled(false);
        ui->lineEdit->setEnabled(false);
        ui->lineEdit->setText(tr("(原地保存)"));
        ui->lineEdit->setStyleSheet("color: gray;");
        ui->checkBox->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
        updateOutputListDisplay();
    }
}

void Widget::on_radioButtonSaveAs_toggled(bool checked)
{
    if (checked) {
        ui->pushButtonDir->setEnabled(true);
        ui->lineEdit->setEnabled(true);
        if (m_outputDir.isEmpty()) {
            ui->lineEdit->setText(tr("请选择目录..."));
            ui->lineEdit->setStyleSheet("color: gray;");
        } else {
            ui->lineEdit->setText(m_outputDir);
            ui->lineEdit->setStyleSheet("");
        }

        ui->checkBox->setEnabled(true);
        bool renameEnabled = ui->checkBox->isChecked();
        ui->lineEdit_2->setEnabled(renameEnabled);
        ui->lineEdit_3->setEnabled(renameEnabled);
        ui->checkBox->setChecked(checked);
        updateOutputListDisplay();
    }
}

//重命名控件的信号处理
void Widget::on_checkBox_toggled(bool checked)
{
    ui->lineEdit_2->setEnabled(!checked);
    ui->lineEdit_3->setEnabled(!checked);
    updateOutputListDisplay();
}

void Widget::on_lineEdit_2_textEdited(const QString &)
{
    if (ui->radioButtonSaveAs->isChecked() && !ui->checkBox->isChecked()) {
        updateOutputListDisplay();
    }
}

void Widget::on_lineEdit_3_textEdited(const QString &)
{
    if (ui->radioButtonSaveAs->isChecked() && !ui->checkBox->isChecked()) {
        updateOutputListDisplay();
    }
}

//查看原文件
void Widget::on_listWidgetInput_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    if (current) previewFile(current->text(), true);
}

void Widget::previewFile(const QString &filePath, bool isSource)
{
    QFileInfo fi(filePath);
    if (!fi.exists()) {
        if (isSource) {
            QMessageBox::warning(this, tr("错误"), tr("源文件不存在"));
            return;
        } else {
            ui->textEditPreview->setPlainText(tr("[文件尚未生成]\n路径: %1\n请先执行保存操作。").arg(filePath));
            ui->textEditPreview->setStyleSheet("color: gray; font-style: italic;");
            return;
        }
    }

    ui->textEditPreview->setStyleSheet("");

    QString codecName =getOriginCodecName();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->textEditPreview->setPlainText(tr("无法打开: %1").arg(file.errorString()));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QString content;
    if (codecName == "Unicode") {
        QTextCodec *codec = QTextCodec::codecForName("UTF-16");
        if (codec) content = codec->toUnicode(data);
        else {
            if (data.size() % 2 != 0) data.chop(1);
            content = QString::fromUtf16(reinterpret_cast<const char16_t*>(data.constData()), data.size()/2);
        }
    } else {
        QTextCodec *codec = QTextCodec::codecForName(codecName.toUtf8());
        if (codec) content = codec->toUnicode(data);
        else content = "[不支持的编码]";
    }

    ui->textEditPreview->setPlainText(content);
}


//保存逻辑，允许用户编辑
void Widget::on_pushButtonSave_clicked()
{
    if (m_fileList.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("没有文件"));
        return;
    }

    if (ui->radioButtonSaveAs->isChecked() && m_outputDir.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请选择输出目录"));
        return;
    }

    if (ui->radioButtonSave->isChecked()) {
        int ret = QMessageBox::warning(this, tr("高风险"),
                                       tr("将覆盖 %1 个原文件！确定？").arg(m_fileList.count()),
                                       QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }

    QString fromC = getOriginCodecName();
    QString toC = getTargetCodecName();
    int success = 0, fail = 0;
    QString lastErr;


    for (int i = 0; i < m_fileList.count(); ++i) {
        QString srcPath = m_fileList.at(i);
        QString destPath;

        if (ui->radioButtonSave->isChecked()) {
            destPath = srcPath;
        } else {
            if (i < ui->listWidgetOutput->count()) {
                QString editedText = ui->listWidgetOutput->item(i)->text();
                QFileInfo fi(editedText);
                if (fi.isAbsolute()) {
                    destPath = editedText;
                } else {
                    destPath = QDir(m_outputDir).filePath(editedText);
                }
            } else {
                destPath = QDir(m_outputDir).filePath(generateDestFileName(srcPath));
            }
        }

        QString err;
        if (convertFile(srcPath, destPath, fromC, toC, err)) {
            success++;
            if (i < ui->listWidgetOutput->count()) {
                auto *item = ui->listWidgetOutput->item(i);
                item->setText(destPath + " [OK]");
                item->setForeground(QBrush(Qt::darkGreen));
            }
        } else {
            fail++;
            lastErr = err;
            if (i < ui->listWidgetOutput->count()) {
                auto *item = ui->listWidgetOutput->item(i);
                item->setText(destPath + " [FAIL]");
                item->setForeground(QBrush(Qt::red));
            }
        }
    }

    QString msg = tr("完成！成功:%1 失败:%2").arg(success).arg(fail);
    if (fail > 0) msg += "\n最后错误:" + lastErr;

    if (fail > 0) QMessageBox::warning(this, tr("部分失败"), msg);
    else {
        QMessageBox::information(this, tr("成功"), msg);
        if (ui->radioButtonSaveAs->isChecked() && success > 0) {
            ui->listWidgetOutput->setCurrentRow(0);
        }
    }
}

//辅助函数
QString Widget::getOriginCodecName() const { return ui->comboBoxOrigin->currentText(); }
QString Widget::getTargetCodecName() const { return ui->comboBoxTarget->currentText(); }

bool Widget::convertFile(const QString &src, const QString &dst, const QString &fromC, const QString &toC, QString &err)
{
    QFile fSrc(src);
    if (!fSrc.open(QIODevice::ReadOnly)) { err = tr("无法打开源文件"); return false; }
    QByteArray raw = fSrc.readAll(); fSrc.close();

    QString uText;
    if (fromC == "Unicode") {
        QTextCodec *c = QTextCodec::codecForName("UTF-16");
        if(c) uText = c->toUnicode(raw);
        else { if(raw.size()%2!=0)raw.chop(1); uText=QString::fromUtf16(reinterpret_cast<const char16_t*>(raw.constData()), raw.size()/2); }
    } else {
        QTextCodec *c = QTextCodec::codecForName(fromC.toUtf8());
        if(!c) { err="Bad Source Codec"; return false; }
        uText = c->toUnicode(raw);
    }

    QByteArray outData;
    if (toC == "Unicode") {
        QTextCodec *c = QTextCodec::codecForName("UTF-16");
        if(c) outData = c->fromUnicode(uText);
        else outData = QByteArray(reinterpret_cast<const char*>(uText.utf16()), uText.length()*2);
    } else {
        QTextCodec *c = QTextCodec::codecForName(toC.toUtf8());
        if(!c) { err="Bad Target Codec"; return false; }
        outData = c->fromUnicode(uText);
    }

    QFile fDst(dst);
    if (!fDst.open(QIODevice::WriteOnly)) { err = tr("无法写入目标文件"); return false; }
    if (fDst.write(outData) != outData.size()) { err = tr("写入不完整"); return false; }
    fDst.close();
    return true;
}

void Widget::on_comboBoxOrigin_currentIndexChanged(int index)
{
    QListWidgetItem *currentItem = ui->listWidgetInput->currentItem();
    if (!currentItem) {
        return;
    }
    previewFile(currentItem->text(), true);
}

//右键菜单触发槽函数
void Widget::on_listWidgetInput_customContextMenuRequested(const QPoint &pos)
{
    if (ui->listWidgetInput->count() == 0) return;
    QMenu contextMenu(this);
    QAction *deleteAction = contextMenu.addAction(tr("删除"));
    if (ui->listWidgetInput->currentRow() == -1) {
        deleteAction->setEnabled(false);
    }

    QAction *selectedAction = contextMenu.exec(ui->listWidgetInput->viewport()->mapToGlobal(pos));

    if (selectedAction == deleteAction) {
        deleteSelectedSourceFile();
    }
}

//执行删除逻辑
void Widget::deleteSelectedSourceFile()
{
    int currentRow = ui->listWidgetInput->currentRow();
    if (currentRow < 0) return;

    QListWidgetItem *item = ui->listWidgetInput->takeItem(currentRow);
    if (!item) return;

    if (currentRow < m_fileList.count()) {
        m_fileList.removeAt(currentRow);
    }

    delete item;
    updateOutputListDisplay();
    if(ui->listWidgetInput->currentRow()==-1)ui->textEditPreview->clear();
}

void Widget::on_pushButtonClear_clicked()
{
    m_fileList.clear();
    ui->listWidgetInput->clear();
    updateOutputListDisplay();
    ui->textEditPreview->clear();
}

void Widget::on_listWidgetOutput_itemChanged(QListWidgetItem *item)
{
    int row = ui->listWidgetOutput->row(item);
    if (row >= 0 && row < m_fileList.count()) {
        QString srcFile = m_fileList.at(row);
        QString currentText = item->text();
        QFileInfo fi(currentText);
        QString displayPath = fi.isAbsolute() ? currentText : QDir(m_outputDir).filePath(fi.fileName());

        item->setToolTip(tr("目标路径:\n%1\n(双击可修改文件名)").arg(displayPath));
    }
}
