/*---------------------------------------------------------------------------*/

#ifndef PREFERENCESEXPORT_H
#define PREFERENCESEXPORT_H

/*---------------------------------------------------------------------------*/

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QButtonGroup>
#include <QRadioButton>

#include <QLabel>

class PreferencesExport
: public QWidget
{

   Q_OBJECT

public:

   enum XMLExportOptions {
      noXMLExport,
      singleFileXMLExport,
      sepereateFileXMLExport
   };

   /**
    * Constructor
    *
    * @param parent - the Qt parent widget
    */
   PreferencesExport(QWidget* parent = 0);

   /**
    * Default destructor
    */
   ~PreferencesExport();

   bool getEnableZoomToRegionOnExport();

   void setEnabledZoomToRegionOnExport(bool);

   QString getExportToDirInDataset();

   void setExportToDirInDataset(QString directory);

   bool getPrintNameOnExportedImage();

   void setPrintNameOnExportedImage(bool);

   bool getPrintPyPxOnExportedImage();

   void setPrintPyPxOnExportedImage(bool);

   bool getPrintWidthHeightOnExportedImage();

   void setPrintWidthHeightOnExportedImage(bool);

   int getSelectedXmlExportOption();

   void setSelectedXmlExportOption(int);

private:

   /**
    * @brief m_zoomToRegionOnExport
    */
   QCheckBox* m_zoomToRegionOnExport;

   /**
    * @brief m_exportToDirInDataset
    */
   QLineEdit* m_exportToDirInDataset;

   QCheckBox* m_printNameOnExportedImage;

   QCheckBox* m_printPredictedXYOnExportedImage;

   QCheckBox* m_printWidthHeightOnExportedImage;

   QButtonGroup* m_xmlExportOptions;

   QRadioButton* m_noXmlExportRadioButton;

   QRadioButton* m_singleXmlExportButton;

   QRadioButton* m_seperateXmlExportButton;

};


/*---------------------------------------------------------------------------*/

#endif // PREFERENCESEXPORT_H

/*---------------------------------------------------------------------------*/
