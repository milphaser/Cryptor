//---------------------------------------------------------------------------

#ifndef UnitFormSliderH
#define UnitFormSliderH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
const int FRM_CNT_WIDTH_VISIBLE_YES	= 1024;
const int FRM_CNT_WIDTH_VISIBLE_NOT	= 12;
//---------------------------------------------------------------------------
class TformSlider : public TForm
{
__published:	// IDE-managed Components
	TPanel *panelSlider;
	TButton *btnShowHide;
	TGroupBox *gboxSlider;
	TStatusBar *stbarStatus;
	TImage *imageEye;
	TLabeledEdit *leditKey;
	TCheckBox *cboxCheckOnlyMode;
	TCheckBox *cboxProcessFirstRow;
	TCheckBox *cboxPasswordStrength;
	TProgressBar *pbarProgress;
	TCheckBox *cboxProcessFirstCol;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnShowHideClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall imageEyeMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall imageEyeMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall gboxSliderClick(TObject *Sender);
private:	// User declarations
	bool boolVisible;
	bool boolCloseEnable;

public:		// User declarations
	__fastcall TformSlider(TComponent* Owner);
	bool __fastcall IsVisible(void)			{ return boolVisible;}
	void __fastcall SetCloseEnable(void)    { boolCloseEnable = true;}
	void __fastcall SetStatus(String msg);
};
//---------------------------------------------------------------------------
extern PACKAGE TformSlider *formSlider;
//---------------------------------------------------------------------------
#endif
