/*-----------------------------------------------------------------------------
 * Copyright (c) 2026, UChicago Argonne, LLC
 * See LICENSE file.
 *---------------------------------------------------------------------------*/

#ifndef CustomPeakDialog_H_
#define CustomPeakDialog_H_

//---------------------------------------------------------------------------

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

//---------------------------------------------------------------------------


class CustomPeakDialog: public QDialog
{

   Q_OBJECT

public:

    CustomPeakDialog(QWidget* parent = 0);

    ~CustomPeakDialog();

    QString get_name() { return _le_name->text(); }

    double get_energy() { return _le_peak_energy_center->text().toDouble(); }

public slots:
	void on_accepted();

	void on_rejected();

private:    

    QLineEdit* _le_name;

    QLineEdit* _le_peak_energy_center;

	QPushButton* _btn_accept;

	QPushButton* _btn_cancel;

};

//---------------------------------------------------------------------------


#endif /* _H_ */

