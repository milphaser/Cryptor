//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitFormSlider.h"
#include "UnitFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformSlider *formSlider;
//---------------------------------------------------------------------------
__fastcall TformSlider::TformSlider(TComponent* Owner)
	: TForm(Owner)
{
	boolVisible = false;
	boolCloseEnable = false;

	cboxCheckOnlyMode->Checked = false;
	cboxCheckOnlyMode->Enabled = false;

	cboxProcessFirstRow->Checked = false;
	cboxProcessFirstRow->Enabled = false;

	cboxProcessFirstCol->Checked = false;
	cboxProcessFirstCol->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::FormShow(TObject *Sender)
{
	if(boolVisible)
	{
		Width = FRM_CNT_WIDTH_VISIBLE_YES;
	}
	else
	{
		Width = FRM_CNT_WIDTH_VISIBLE_NOT;
	}
	Left = Screen->WorkAreaWidth - formMain->Width - Width;
	Top = Screen->WorkAreaHeight - Height;
//	Top = Screen->WorkAreaHeight - formMain->ClientHeight/2 - Height/2;
//	Top = Screen->WorkAreaHeight -
//		  (GetSystemMetrics(SM_CXFRAME) + formMain->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::btnShowHideClick(TObject *Sender)
{
	if(boolVisible)
	{
		boolVisible = false;
		Width = FRM_CNT_WIDTH_VISIBLE_NOT;
	}
	else
	{
		boolVisible = true;
		Width = FRM_CNT_WIDTH_VISIBLE_YES;
	}
	Left = Screen->WorkAreaWidth - formMain->Width - Width;
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::gboxSliderClick(TObject *Sender)
{
	if(boolVisible)
	{
		btnShowHideClick(this);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(!boolCloseEnable)
	{
		formMain->Close();

		// Do not Close at this stage
		Action = caNone;
		return;
	}
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::imageEyeMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	leditKey->PasswordChar = '\0';  // Show password
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::imageEyeMouseUp(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	leditKey->PasswordChar = '*';   // Hide password
	btnShowHide->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TformSlider::SetStatus(String msg)
{
	formSlider->stbarStatus->SimpleText = msg;

	if(!boolVisible)
	{
		btnShowHideClick(this);
	}
}
//---------------------------------------------------------------------------

