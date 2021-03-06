#include "treeinterface.h"
#include "ui_treeinterface.h"
#include <QDebug>
#include "TreeUnitTests.h"



TreeInterface::TreeInterface(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TreeInterface)
{
    QVBoxLayout *layoutPrincipal = new QVBoxLayout;
    QGridLayout *layoutName = new QGridLayout();
    QGridLayout *layoutPath = new QGridLayout();
    QGridLayout *layoutBackupPath = new QGridLayout();
    QGridLayout *layoutButtons = new QGridLayout();
    ui->setupUi(this);
    ui->labelTitle->setText(QObject::tr("Create a genealogic tree"));
    ui->labelName->setText(QObject::tr("Name"));
    ui->labelPath->setText(QObject::tr("Path"));
    ui->checkBoxBackupPath->setText(QObject::tr("BackupPath"));
    ui->pushButtonBrowsePath->setText(QObject::tr("Browse"));
    ui->pushButtonBrowseBackupPath->setText(QObject::tr("Browse"));
	if (TreeUnitTests().getTestInterface()) {
		ui->lineEditName->setText("Test");
		ui->lineEditPath->setText(QDir::currentPath() + "/Tests Results");
	}
    connect(ui->checkBoxBackupPath, SIGNAL(clicked()), this, SLOT(enableBackupPath()));
    connect(ui->pushButtonBrowsePath, SIGNAL(clicked()), this, SLOT(browsePath()));
    connect(ui->pushButtonBrowseBackupPath, SIGNAL(clicked()), this, SLOT(browseBackupPath()));
    connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(createTree()));
	connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));

    layoutPrincipal->setSpacing(20);
    layoutPrincipal->addWidget(ui->labelTitle);
    layoutName->addWidget(ui->labelName,1,1);
    layoutName->addWidget(ui->lineEditName,1,2);
    layoutPrincipal->addLayout(layoutName);
    layoutPath->addWidget(ui->labelPath,1,1);
    layoutPath->addWidget(ui->lineEditPath,1,2);
    layoutPath->addWidget(ui->pushButtonBrowsePath,1,3);
    layoutPrincipal->addLayout(layoutPath);
    layoutBackupPath->addWidget(ui->checkBoxBackupPath,1,1);
    layoutBackupPath->addWidget(ui->lineEditBackupPath,1,2);
    layoutBackupPath->addWidget(ui->pushButtonBrowseBackupPath,1,3);
    layoutPrincipal->addLayout(layoutBackupPath);
    layoutButtons->addWidget(ui->pushButtonOk,1,1);
    layoutButtons->addWidget(ui->pushButtonCancel,1,2);
    layoutPrincipal->addLayout(layoutButtons,2);

    this->setLayout(layoutPrincipal);
}

std::shared_ptr<Tree> TreeInterface::getCurrentTree() {
	return currentTree;
}
TreeInterface::~TreeInterface()
{
    delete ui;
}

void TreeInterface::createTree(){
	treeName = ui->lineEditName->text();
	updateDate = QDateTime::currentDateTime();
	path = ui->lineEditPath->text();
	backupPath = ui->lineEditBackupPath->text();
    QString stringError;

    if (ui->lineEditName->text() == ""){
        stringError += tr("Please fill the name field\n");
    }
    if(ui->lineEditPath->text() == "" )
    {
        stringError += tr("Please fill the path field\n");
    }
    if(ui->lineEditBackupPath->text() == "" && ui->checkBoxBackupPath->isChecked())
    {
        stringError += tr("Please fill the backup path field or untick the backup path checkbox\n");
    }


	if (TreeUnitTests().getTestInterface()) {

		QFile file(path.path() + "/" + treeName + ".tree");
		file.remove();
		QFile backupFile(backupPath.path() + "/" + treeName + ".tree");
		backupFile.remove();

	}

	QFileInfo check_file(path.path() + "/" + treeName + ".tree");
	// check if file exists and if yes: Is it really a file and no directory?
	if (check_file.exists() && check_file.isFile()) {
		stringError += tr("A Tree with the same name already exists in this folder, please change the name of your Tree\n");
	}
	if (backupPath.path() != "") {
		QFileInfo check_Backupfile(backupPath.path() + "/" + treeName + ".tree");
		// check if file exists and if yes: Is it really a file and no directory?
		if (check_Backupfile.exists() && check_Backupfile.isFile()) {
			stringError += tr("A Tree with the same name already exists in the backup folder, please change the name of your Tree\n");
		}
	}


    if(stringError != ""){
        QMessageBox msgBox;
        msgBox.setText(tr("Error"));
        msgBox.setInformativeText(stringError);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
    else{

        currentTree = std::make_shared<Tree>(treeName, updateDate, path, backupPath);

        emit signalReturnTree();


        this->hide();
    }

}

void TreeInterface::enableBackupPath(){
    if(ui->checkBoxBackupPath->isChecked()){
        ui->lineEditBackupPath->setEnabled(true);
        ui->pushButtonBrowseBackupPath->setEnabled(true);
		if (TreeUnitTests().getTestInterface()) {
			ui->lineEditBackupPath->setText(QDir::currentPath() + "/Tests Results/Backup");
		}
    }
    else {
        ui->pushButtonBrowseBackupPath->setEnabled(false);
        ui->lineEditBackupPath->setEnabled(false);
		if (TreeUnitTests().getTestInterface()) {
			ui->lineEditBackupPath->setText("");
		}
    }
}

void TreeInterface::browsePath() {
	browse(true);
}

void TreeInterface::browseBackupPath() {
	browse(false);
}
void TreeInterface::browse(bool pathType){

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    if (dialog.exec())
      QStringList fileNames = dialog.selectedFiles();
    if(pathType){
        path= dialog.directory().path();
		ui->lineEditPath->setText(path.path()+"/");
    }
    else{
        backupPath= dialog.directory().path();
		ui->lineEditBackupPath->setText(backupPath.path()+"/");
    }



}
