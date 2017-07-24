/*---------------------------------------------------------------------------*/

#include <preferences/PreferencesExport.h>

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

/*---------------------------------------------------------------------------*/

PreferencesExport::PreferencesExport(QWidget* parent) : QWidget(parent)
{
   QFormLayout* mainLayout = new QFormLayout();

   m_zoomToRegionOnExport = new QCheckBox();
   m_exportToDirInDataset = new QLineEdit();

   m_printNameOnExportedImage = new QCheckBox();
   m_printPredictedXYOnExportedImage = new QCheckBox();
   m_printWidthHeightOnExportedImage = new QCheckBox();

   // Add export options for xml files.
   QVBoxLayout* xmlExportOptionsVBox = new QVBoxLayout();
   m_noXmlExportRadioButton = new QRadioButton("No Export");
   m_singleXmlExportButton = new QRadioButton("Export selected regions to a single xml file");
   m_seperateXmlExportButton = new QRadioButton("Export selected regions to seperate xml files");
   xmlExportOptionsVBox->addWidget(m_noXmlExportRadioButton);
   xmlExportOptionsVBox->addWidget(m_singleXmlExportButton);
   xmlExportOptionsVBox->addWidget(m_seperateXmlExportButton);

   m_xmlExportOptions = new QButtonGroup();
   m_xmlExportOptions->addButton(m_noXmlExportRadioButton, noXMLExport);
   m_xmlExportOptions->addButton(m_singleXmlExportButton, singleFileXMLExport);
   m_xmlExportOptions->addButton(m_seperateXmlExportButton, sepereateFileXMLExport);

   QLabel* lblZoomToRegionOnExport = new QLabel("Zoom to region on export");
   QLabel* lblExportToDirInDataset = new QLabel("Use directory in dataset");
   QLabel* lblExportRegionNameOnImage = new QLabel("Print name on exported image");
   QLabel* lblExportPyPxOnImage = new QLabel("Print Py/Px on exported image");
   QLabel* lblExportWidthHeightOnImage = new QLabel("Print width/height on exported image");
   QLabel* lblExportXmlOptions = new QLabel("XML Options");

   mainLayout->addRow(lblZoomToRegionOnExport, m_zoomToRegionOnExport);
   mainLayout->addRow(lblExportToDirInDataset, m_exportToDirInDataset);
   mainLayout->addRow(lblExportRegionNameOnImage, m_printNameOnExportedImage);
   mainLayout->addRow(lblExportPyPxOnImage, m_printPredictedXYOnExportedImage);
   mainLayout->addRow(lblExportWidthHeightOnImage, m_printWidthHeightOnExportedImage);
   mainLayout->addRow(lblExportXmlOptions, xmlExportOptionsVBox);

   setLayout(mainLayout);
}

/*---------------------------------------------------------------------------*/

PreferencesExport::~PreferencesExport()
{

}

/*---------------------------------------------------------------------------*/

QString PreferencesExport::getExportToDirInDataset()
{
   return m_exportToDirInDataset->text();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setExportToDirInDataset(QString directory)
{
   m_exportToDirInDataset->setText(directory);
}

/*---------------------------------------------------------------------------*/

bool PreferencesExport::getEnableZoomToRegionOnExport()
{
   return m_zoomToRegionOnExport->isChecked();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setEnabledZoomToRegionOnExport(bool enableZoomToRegion)
{
   m_zoomToRegionOnExport->setChecked(enableZoomToRegion);
}

/*---------------------------------------------------------------------------*/

bool PreferencesExport::getPrintNameOnExportedImage()
{
   return m_printNameOnExportedImage->isChecked();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setPrintNameOnExportedImage(bool printName)
{
   m_printNameOnExportedImage->setChecked(printName);
}

/*---------------------------------------------------------------------------*/

bool PreferencesExport::getPrintPyPxOnExportedImage()
{
   return m_printPredictedXYOnExportedImage->isChecked();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setPrintPyPxOnExportedImage(bool printPyPx)
{
   m_printPredictedXYOnExportedImage->setChecked(printPyPx);
}

/*---------------------------------------------------------------------------*/

bool PreferencesExport::getPrintWidthHeightOnExportedImage()
{
   return m_printWidthHeightOnExportedImage->isChecked();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setPrintWidthHeightOnExportedImage(bool printWidthHeight)
{
   m_printWidthHeightOnExportedImage->setChecked(printWidthHeight);
}

/*---------------------------------------------------------------------------*/

int PreferencesExport::getSelectedXmlExportOption()
{
   return m_xmlExportOptions->checkedId();
}

/*---------------------------------------------------------------------------*/

void PreferencesExport::setSelectedXmlExportOption(int selectedOption)
{
   if (selectedOption == noXMLExport) m_noXmlExportRadioButton->setChecked(true);
   else if (selectedOption == sepereateFileXMLExport) m_seperateXmlExportButton->setChecked(true);
   else if (selectedOption == singleFileXMLExport) m_singleXmlExportButton->setChecked(true);
}

/*---------------------------------------------------------------------------*/
