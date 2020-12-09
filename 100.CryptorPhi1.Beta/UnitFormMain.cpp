////////////////////////////////////////////////////////////////////////////////
//	ПРОЕКТ: 073.CryptoChecker
//
//	Развитие на 072.CryptoChecker
//	1. Запазена е функционалността за криптиране/декриптиране на текстови файлове.
//	2. Добавена е функционалност за криптиране на XLSX файлове,
//	като криптираният файл е CSV, както и обратно – криптираният CSV файл
//	се декриптира и представя отново в XLSX.
// 	Криптират се всички колони на контактния файл.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ФОРМАТ НА КРИПТИРАНИЯ ФАЙЛ - TXT
//
//	Header Row SHA-512[Key]
//	SHA1[RAW[ENCRYPT[Row 1]]] + RAW[ENCRYPT[Row 1]]
//	SHA1[RAW[ENCRYPT[Row 2]]] + RAW[ENCRYPT[Row 2]]
//	...
//	SHA1[RAW[ENCRYPT[Row N]]] + RAW[ENCRYPT[Row N]]
//	Tail Row SHA1[N]
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// СХЕМА НА КРИПТИРАНЕ - TXT
//
//  SHA-512[Key]
//  Header Row
//
//  Еднократно установяване на паролата в началото на криптирането
//
//  Encrypt Line by Line
//	  ENCRYPT[Row i]
//	  RAW[ENCRYPT[Row i]]
//	  SHA1[RAW[ENCRYPT[Row i]]]
//	  SHA1[RAW[ENCRYPT[Row i]]] + RAW[ENCRYPT[Row i]]
//
//  SHA1[N]
//  Tail Row
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// СХЕМА НА ДЕКРИПТИРАНЕ - TXT
//
//  SHA-512[Key]
//  Header Row Processing (Валидност на паролата за декриптиране)
//
//  Еднократно установяване на паролата в началото на декриптирането
//
//  Decrypt Line by Line
//	  Regular Row Processing
//	    Row i[0..39]
//	    Row i[40..eol]
//
//	    SHA1[Row i[40..eol]]
//	    Horizontal Data Integrity Check
//	    Row i[0..39] == SHA1[Row i[40..eol]]
//
//	    STR[Row i[40..eol]]
//	    DECRYPT[STR[Row i[40..eol]]]
//
//	Tail Row Processing
//	  Vertical Data Integrity Check
//	  Row i[0..39] == SHA1[N]
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ФОРМАТ НА КРИПТИРАНИЯ ФАЙЛ - CSV
//
//	Header Row SHA-512[Key]
//	SHA1[R1] + <;> + R1
//	SHA1[R2] + <;> + R2
//	...
//	SHA1[RN] + <;> + RN
//	Tail Row SHA1[N]
////////////////////////////////////////////////////////////////////////////////
//	Ri = RAW[ENCRYPT[Row i/Col 1]] + <;> + RAW[ENCRYPT[Row i/Col 2]] + <;> ...
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// СХЕМА НА КРИПТИРАНЕ - XLSX
//
//
//  SHA-512[Key]
//  Header Row
//
//  Encrypt Line by Line
//    Установяване на паролата в началото на криптирането на реда
//    Encrypt Col by Col
//	    ENCRYPT[Row i/Col j]
//	    RAW[ENCRYPT[Row i/Col j]]
// 	    R += RAW[ENCRYPT[Row i/Col i]] + <;>
//    SHA1[R]
//	  SHA1[R] + <;> + R
//
//  SHA1[N]
//  Tail Row
////////////////////////////////////////////////////////////////////////////////
//  Ri = RAW[ENCRYPT[Row i/Col 1]] + <;> + RAW[ENCRYPT[Row i/Col 2]] + <;> ...
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// СХЕМА НА ДЕКРИПТИРАНЕ - CSV
//
//  SHA-512[Key]
//  Header Row Processing (Валидност на паролата за декриптиране)
//
//  Decrypt Line by Line
//	  Regular Row Processing
//	    Horizontal Data Integrity Check
//      Установяване на паролата в началото на декриптирането на реда
//		  Decrypt Col by Col
//		  STR[Row i/Col j]
//		  DECRYPT[STR[Row i/Col j]]
//
//  Tail Row Processing
//    Vertical Data Integrity Check
//	  Row i[0..39] == SHA1[N]
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// ФОРМАТ НА КРИПТИРАНИЯ ФАЙЛ - WA_/WM_/BI_
//
//  #64            #64
//  SHA-512[Key] + SHA-512[ENCRYPT[Data]] + ENCRYPT[Data]
////////////////////////////////////////////////////////////////////////////////

#include <vcl.h>
#include <System.SysUtils.hpp>
#include <System.Hash.hpp>
#include <System.IOUtils.hpp>

#include <fstream>
#include <iomanip>
#include <string>

#include <locale>
#include <codecvt>
#pragma hdrstop

#include "UnitFormMain.h"
#include "UnitCryptor.h"
#include "UnitFormSlider.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TformMain *formMain;
//---------------------------------------------------------------------------
__fastcall TformMain::TformMain(TComponent* Owner)
	: TForm(Owner)
{
	boolFirstShow = true;
	boolCloseEnable = false;

    odlgSrcFile->Filter = "Files to Encrypt (*.xlsx;*.wav;*.wma;*.txt;*.bin)|*.xlsx;*.wav;*.wma;*.txt;*.bin|Files to Decrypt (*.csv;*.wa_;*.wm_;*.txt;*.bi_)|*.csv;*.wa_;*.wm_;*.txt;*.bi_";
}
//---------------------------------------------------------------------------
void __fastcall TformMain::FormShow(TObject *Sender)
{
	Left = Screen->WorkAreaWidth - Width;
	Top = Screen->WorkAreaHeight - Height;

	SetWorkingMode();

	if(boolFirstShow)
	{
		formSlider->Show();
		boolFirstShow = false;
	}

	formSlider->Show();
}
//---------------------------------------------------------------------------
void __fastcall TformMain::FormHide(TObject *Sender)
{
	formSlider->Hide();
}
//---------------------------------------------------------------------------
void __fastcall TformMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(!boolCloseEnable)
	{
		// Do not Close
		Action = caNone;

		Hide();
		WindowState = wsMinimized;

		return;
	}

	tiTray->PopupMenu = nullptr;
	Hide();

	formSlider->SetCloseEnable();
	formSlider->Close();
}
//---------------------------------------------------------------------------
void __fastcall TformMain::tiTrayDblClick(TObject *Sender)
{
	if(tiTray->PopupMenu != nullptr)
	{
		miOpenClick(Sender);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::miOpenClick(TObject *Sender)
{
	Show();
	WindowState = wsNormal;

	BringToFront();
	FormStyle = fsStayOnTop;
}
//---------------------------------------------------------------------------
void __fastcall TformMain::miCloseClick(TObject *Sender)
{
	boolCloseEnable = true;
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TformMain::miSrcClick(TObject *Sender)
{
	SetWorkingMode();
}
//---------------------------------------------------------------------------
void __fastcall TformMain::btnJoystickClick(TObject *Sender)
{
	if(wmWorkingMode == WorkingMode::Source)
	{
		Source();
	}
	else
	{
		if(!PasswordStrength(formSlider->leditKey->Text))
		{
			String str = "Password is weak, cannot proceed";
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}

		btnJoystick->Enabled = false;
		TCursor cr = Screen->Cursor;        // запомняне на текущия курсор
		Screen->Cursor = crHourGlass;
		formSlider->pbarProgress->Position = formSlider->pbarProgress->Min;
		formSlider->pbarProgress->Step = 1;

		if(wmWorkingMode == WorkingMode::Encrypt_TXT)
		{
			Encrypt_TXT();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Decrypt_TXT)
		{
			Decrypt_TXT();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Encrypt_XLS)
		{
			Encrypt_XLS();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Decrypt_CSV)
		{
			if(!formSlider->cboxCheckOnlyMode->Checked)
			{
				Decrypt_CSV();
			}
			else
			{
				Check_CSV();
            }
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Encrypt_WAV)
		{
			Encrypt_BIN();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Decrypt_WA_)
		{
			Decrypt_BIN();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Encrypt_WMA)
		{
			Encrypt_BIN();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Decrypt_WM_)
		{
			Decrypt_BIN();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Encrypt_BIN)
		{
			Encrypt_BIN();
			SetWorkingMode();
		}
		else if(wmWorkingMode == WorkingMode::Decrypt_BI_)
		{
			Decrypt_BIN();
			SetWorkingMode();
		}

		formSlider->leditKey->Text = "";    // изчистване на паролата след криптиране/декриптиране

		Sleep(1000);
		formSlider->pbarProgress->Position = formSlider->pbarProgress->Min;

		Screen->Cursor = cr;                // възстановяване на курсора
		btnJoystick->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Source(void)
{
	if(odlgSrcFile->Execute())
	{
		String str;

		if(FileExists(odlgSrcFile->FileName))
		{
			str = odlgSrcFile->FileName + " selected";
			SetWorkingMode(odlgSrcFile->FileName);
		}
		else
		{
			str = odlgSrcFile->FileName + " does not exist";
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			odlgSrcFile->FileName = "";
			SetWorkingMode();
		}

		formSlider->SetStatus(str);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Encrypt_TXT(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	std::wifstream fsIn(to_string(odlgSrcFile->FileName));
	if(!fsIn)
	{
		String str = "Cannot open the input file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	std::wofstream fsOut(to_string(strDstFileName));
	if(!fsOut)
	{
		String str = "Cannot open the output file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	formSlider->SetStatus("Encryption started. Please wait...");

	int intLinesNumber = wifstream_lines_number(to_string(odlgSrcFile->FileName));
	formSlider->pbarProgress->Max = intLinesNumber;

	try
	{
//		fsIn.exceptions(std::ios::badbit | std::ios::failbit);
//		fsOut.exceptions(std::ios::badbit | std::ios::failbit);

		// SHA-512[Key]
		String usDigest = THashSHA2::GetHashString(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		std::wstring strDigest = to_string(usDigest);
		// Header Row
		fsOut << strDigest << std::endl;	// L'\n';

		// Еднократно установяване на паролата в началото на криптирането
		Cryptor Cryptor;
		std::wstring strKey = to_string(formSlider->leditKey->Text);
		Cryptor.Set_Key(strKey);

		// Encrypt Line by Line
		int intN = 0;                       // брой редове
		std::wstring strLineIn;             // буфер на текущия ред
		while(std::getline(fsIn, strLineIn))
		{
			formSlider->pbarProgress->StepIt();
			Application->ProcessMessages();

			// ENCRYPT[Row i]
			std::wstring strTxt = Cryptor.Transform_String(strLineIn);
			// RAW[ENCRYPT[Row i]]
			strTxt = s2r(strTxt);

			// SHA1[RAW[ENCRYPT[Row i]]]
			usDigest = THashSHA1::GetHashString(to_string(strTxt));
			strDigest = to_string(usDigest);

			// SHA1[RAW[ENCRYPT[Row i]]] + RAW[ENCRYPT[Row i]]
			std::wstring strLineOut = strDigest + strTxt;
			fsOut << strLineOut << std::endl;	// L'\n';

			intN++;
		}

		// SHA1[N]
		usDigest = THashSHA1::GetHashString(to_string(std::to_wstring(intN)));
		strDigest = to_string(usDigest);
		// Tail Row
		fsOut << strDigest << std::endl;	// L'\n';

		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(Exception& e)
	{
		String str = e.Message;
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Decrypt_TXT(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	std::wifstream fsIn(to_string(odlgSrcFile->FileName));
	if(!fsIn)
	{
		String str = "Cannot open the input file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	std::wofstream fsOut(to_string(strDstFileName));
	if(!fsOut)
	{
		String str = "Cannot open the output file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	int intLinesNumber = wifstream_lines_number(to_string(odlgSrcFile->FileName));
	if(intLinesNumber < 2)
	{
		String str = "Distorted input file - Too little rows in";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}
	intLinesNumber -= 2;    // Header and Tail rows compensation
	formSlider->pbarProgress->Max = intLinesNumber;

	formSlider->SetStatus("Decryption started. Please wait...");

	try
	{
//		fsIn.exceptions(std::ios::badbit | std::ios::failbit);
//		fsOut.exceptions(std::ios::badbit | std::ios::failbit);

		// SHA-512[Key]
		String usDigest = THashSHA2::GetHashString(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		std::wstring strDigest = to_string(usDigest);

		// Header Row Processing (Валидност на паролата за декриптиране)
		std::wstring strLineIn;
		std::getline(fsIn, strLineIn);
		if(strDigest != strLineIn)
		{
			String str = "Invalid Password";
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}

		// Еднократно установяване на паролата в началото на декриптирането
		Cryptor Cryptor;
		std::wstring strKey = to_string(formSlider->leditKey->Text);
		Cryptor.Set_Key(strKey);

		// Decrypt Line by Line
		int intN = 0;                       // брой редове
		while(std::getline(fsIn, strLineIn))
		{
			formSlider->pbarProgress->StepIt();
			Application->ProcessMessages();

			if(intN < intLinesNumber)
			{   // Regular Row Processing
				intN++;

				// Row i[0..39]
				strDigest = strLineIn.substr(0, 40);
				// Row i[40..eol]
				std::wstring strTxt = strLineIn.c_str() + 40;

				// SHA1[Row i[40..eol]]
				String usDigest = THashSHA1::GetHashString(to_string(strTxt));
				// Horizontal Data Integrity Check
				// Row i[0..39] == SHA1[Row i[40..eol]]
				if(strDigest != to_string(usDigest))
				{
					String str = "Horizontal Data Integrity Error in Row #" + IntToStr(intN);
					formSlider->SetStatus(str);
					MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
					return;
				}

				// STR[Row i[40..eol]]
				strTxt = r2s(strTxt);
				// DECRYPT[STR[Row i[40..eol]]]
				std::wstring strLineOut = Cryptor.Transform_String(strTxt);

				fsOut << strLineOut << std::endl;	// L'\n';
			}
			else
			{   // Tail Row Processing
				usDigest = THashSHA1::GetHashString(to_string(std::to_wstring(intN)));
				// Vertical Data Integrity Check
				// Row i[0..39] == SHA1[N]
				if(strLineIn != to_string(usDigest))
				{
					String str = "Vertical Data Integrity Error";
					formSlider->SetStatus(str);
					MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
					return;
				}
			}
		}

		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(Exception& e)
	{
		String str = e.Message;
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);

		try
		{
			if(FileExists(strDstFileName))
			{
				DeleteFile(strDstFileName);
			}
		}
		catch(...)
		{
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Encrypt_XLS(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	std::wofstream fsOut(to_string(strDstFileName));
	if(!fsOut)
	{
		String str = "Cannot open the output file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	fsOut.imbue(loc);

	Variant varExcelApplication;
	Variant varExcelVersion;
	Variant varWorkbooks;
	Variant varWorkbook;
	Variant varWorksheet;

	try
	{
//		fsOut.exceptions(std::ios::badbit | std::ios::failbit);

		varExcelApplication = Variant::CreateObject("excel.application");
		Sleep(100);

//		varExcelApplication.OlePropertySet("Visible", false);
		varExcelVersion = varExcelApplication.OlePropertyGet("Version");
		String stringExcelVersion = varExcelVersion.VOleStr;
		String us = stringExcelVersion.SubString(1, stringExcelVersion.Pos(TCHAR_DECIMAL_SEPARATOR) - 1);
		int intExcelVersion = us.ToInt();

		varWorkbooks = varExcelApplication.OlePropertyGet("Workbooks");
		varWorkbooks.OleProcedure("Open", WideString(odlgSrcFile->FileName));
		varWorkbook  = varExcelApplication.OlePropertyGet("ActiveWorkbook");
		varWorksheet = varExcelApplication.OlePropertyGet("Worksheets").OlePropertyGet("Item", 1);
//		varWorksheet = varWorkbook.OlePropertyGet("ActiveSheet");

		int intLinesNumber = xsl_get_raw_num(varWorksheet);
		formSlider->pbarProgress->Max = intLinesNumber;

		/* КРИПТИРАНЕ */
		formSlider->SetStatus("Encryption started. Please wait...");
		int intXLSColNum = xsl_get_col_num(varWorksheet);

		// SHA-512[Key]
		String usDigest = THashSHA2::GetHashString(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		std::wstring strDigest = to_string(usDigest);
		// Header Row
		fsOut << strDigest << std::endl;	// L'\n';

		int intN = 0;                       // брой редове
		bool boolEoF = false;
		for(int i = 1; !boolEoF; i++)
		{
			formSlider->pbarProgress->StepIt();
			Application->ProcessMessages();

			// Encrypt Line by Line
			// Еднократно установяване на паролата в началото на криптирането
			// на текущия ред
			Cryptor Cryptor;
			std::wstring strKey = to_string(formSlider->leditKey->Text);
			Cryptor.Set_Key(strKey);

			std::wstring strCellIn;             // буфер на текущата клетка
			std::wstring strR;					// R, буфер на криптирания ред

			bool boolProcessRow = (i != 1) || ((i == 1) && formSlider->cboxProcessFirstRow->Checked);

			for(int j = 1; j <= intXLSColNum; j++)
			{
				String str = varWorksheet.OlePropertyGet("Cells", i, j);
				if(j == 1)
				{
					if(str.IsEmpty())
					{
						boolEoF = true;
						break;
					}
					else
					{
						intN = i;
					}
				}

				// Encrypt Cell by Cell
				strCellIn = to_string(str);
				std::wstring strTxt = strCellIn;

				bool boolProcessCol = (j != 1) || ((j == 1) && formSlider->cboxProcessFirstCol->Checked);
				bool boolProcessCell = boolProcessRow && boolProcessCol;

				if(boolProcessCell)
				{
					// ENCRYPT[Row i/Col i]
					strTxt = Cryptor.Transform_String(strCellIn);
					// RAW[ENCRYPT[Row i/Col i]]
					strTxt = s2r(strTxt);
				}

				// R += RAW[ENCRYPT[Row i/Col i]] + ;
				strR += strTxt;
				if(j != intXLSColNum)
				{
					strR += L";";
				}
			}

			if(!boolEoF)
			{
				// SHA1[R]
				usDigest = THashSHA1::GetHashString(to_string(strR));
				strDigest = to_string(usDigest);

				// SHA1[R] + ; + R
				std::wstring strLineOut = strDigest + L";" + strR;
				fsOut << strLineOut << std::endl;	// L'\n';
			}
		}

		// SHA1[N]
		usDigest = THashSHA1::GetHashString(to_string(std::to_wstring(intN)));
		strDigest = to_string(usDigest);
		// Tail Row
		fsOut << strDigest << std::endl;	// L'\n';

		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(Exception &e)
	{
		String str = e.Message;
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
	}

	try
	{
		varWorkbook.OleProcedure("Close");
//		varExcelApplication.OlePropertySet("Visible", true);
		varExcelApplication.OleProcedure("Quit");
	}
	catch(...)
	{
		// Excel is already closed
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Decrypt_CSV(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	std::wifstream fsIn(to_string(odlgSrcFile->FileName));
	if(!fsIn)
	{
		String str = "Cannot open the input file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	fsIn.imbue(loc);

	Variant varExcelApplication;
	Variant varExcelVersion;
	Variant varWorkbooks;
	Variant varWorkbook;
	Variant varWorksheet;

	try
	{
//		fsIn.exceptions(std::ios::badbit | std::ios::failbit);

		varExcelApplication = Variant::CreateObject("excel.application");
		Sleep(100);

//		varExcelApplication.OlePropertySet("Visible", false);
		varExcelVersion = varExcelApplication.OlePropertyGet("Version");
		String stringExcelVersion = varExcelVersion.VOleStr;
		String us = stringExcelVersion.SubString(1, stringExcelVersion.Pos(TCHAR_DECIMAL_SEPARATOR) - 1);
		int intExcelVersion = us.ToInt();

		varWorkbooks = varExcelApplication.OlePropertyGet("Workbooks");
		varWorkbooks.OleProcedure("Open", WideString(strDstXLSFileNameTemplate));
		varWorkbook  = varExcelApplication.OlePropertyGet("ActiveWorkbook");
		varWorksheet = varExcelApplication.OlePropertyGet("Worksheets").OlePropertyGet("Item", 1);
//		varWorksheet = varWorkbook.OlePropertyGet("ActiveSheet");

		int intLinesNumber = wifstream_lines_number(to_string(odlgSrcFile->FileName));
		if(intLinesNumber < 2)
		{
			String str = "Distorted input file - Too little rows in";
			throw Exception(str);
		}
		intLinesNumber -= 2;    // Header and Tail rows compensation
		formSlider->pbarProgress->Max = intLinesNumber;

		formSlider->SetStatus("Decryption started. Please wait...");

		// SHA-512[Key]
		String usDigest = THashSHA2::GetHashString(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		std::wstring strDigest = to_string(usDigest);

		// Header Row Processing (Валидност на паролата за декриптиране)
		std::wstring strLineIn;
		std::getline(fsIn, strLineIn);
		if(strDigest != strLineIn)
		{
			String str = "Invalid Password";
			throw Exception(str);
		}

		// Decrypt Line by Line
		int intN = 0;                       // брой редове
		while(std::getline(fsIn, strLineIn))
		{
			formSlider->pbarProgress->StepIt();
			Application->ProcessMessages();

			if(intN < intLinesNumber)
			{   // Regular Row Processing
				intN++;

				// Horizontal Data Integrity Check
				std::string::size_type inx = strLineIn.find(';');
				if(inx == std::string::npos)
				{
					String str = "Horizontal Data Integrity Error in Row #" + IntToStr(intN);
					throw Exception(str);
				}
				strDigest = strLineIn.substr(0, inx);
				std::wstring strR = strLineIn.c_str() + inx + 1;
				String usDigest = THashSHA1::GetHashString(to_string(strR));
				if(strDigest != to_string(usDigest))
				{
					String str = "Horizontal Data Integrity Error in Row #" + IntToStr(intN);
					throw Exception(str);
				}

				// Еднократно установяване на паролата в началото на декриптирането
				// на текущия ред
				Cryptor Cryptor;
				std::wstring strKey = to_string(formSlider->leditKey->Text);
				Cryptor.Set_Key(strKey);

				bool boolProcessRow = (intN != 1) || ((intN == 1) && formSlider->cboxProcessFirstRow->Checked);

				for(int j = 1; !strR.empty(); j++)
				{
					// Decrypt Col by Col
					std::wstring strTxt;
					inx = strR.find(';');
					if(inx != std::string::npos)
					{
						strTxt = strR.substr(0, inx);
						strR = strR.c_str() + inx + 1;
					}
					else
					{
						strTxt = strR;
						strR.clear();
					}

					std::wstring strCell = strTxt;

					bool boolProcessCol = (j != 1) || ((j == 1) && formSlider->cboxProcessFirstCol->Checked);
					bool boolProcessCell = boolProcessRow && boolProcessCol;

					if(boolProcessCell)
					{
						// STR[Row i/Col j]
						strTxt = r2s(strTxt);
						// DECRYPT[STR[Row i/Col j]]
						strCell = Cryptor.Transform_String(strTxt);
					}
					String usCell = to_string(strCell);
					varWorksheet.OlePropertyGet("Cells", intN, j).OlePropertySet("Value", WideString(usCell));
				}
			}
			else
			{   // Tail Row Processing
				usDigest = THashSHA1::GetHashString(to_string(std::to_wstring(intN)));
				// Vertical Data Integrity Check
				// Row i[0..39] == SHA1[N]
				if(strLineIn != to_string(usDigest))
				{
					String str = "Vertical Data Integrity Error";
					throw Exception(str);
				}
			}
		}

		varWorkbook.OleProcedure("SaveAs", WideString(strDstFileName));
		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(Exception &e)
	{
		String str = e.Message;
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
	}

	try
	{
		varWorkbook.OleProcedure("Close");
//		varExcelApplication.OlePropertySet("Visible", true);
		varExcelApplication.OleProcedure("Quit");
	}
	catch(...)
	{
		// Excel is already closed
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Check_CSV(void)
{
	std::wifstream fsIn(to_string(odlgSrcFile->FileName));
	if(!fsIn)
	{
		String str = "Cannot open the input file";
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
		return;
	}

	std::locale loc(std::locale::classic(), new std::codecvt_utf8<wchar_t>);
	fsIn.imbue(loc);

	try
	{
//		fsIn.exceptions(std::ios::badbit | std::ios::failbit);

		int intLinesNumber = wifstream_lines_number(to_string(odlgSrcFile->FileName));
		if(intLinesNumber < 2)
		{
			String str = "Distorted input file - Too little rows in";
			throw Exception(str);
		}
		intLinesNumber -= 2;    // Header and Tail rows compensation

		formSlider->SetStatus("Check only mode started. Please wait...");

		// SHA-512[Key]
		String usDigest = THashSHA2::GetHashString(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		std::wstring strDigest = to_string(usDigest);

		// Header Row Processing (Валидност на паролата за декриптиране)
		std::wstring strLineIn;
		std::getline(fsIn, strLineIn);
		if(strDigest != strLineIn)
		{
			String str = "Invalid Password";
			throw Exception(str);
		}

		// Decrypt Line by Line
		int intN = 0;                       // брой редове
		while(std::getline(fsIn, strLineIn))
		{
			if(intN < intLinesNumber)
			{   // Regular Row Processing
				intN++;

				// Horizontal Data Integrity Check
				std::string::size_type inx = strLineIn.find(';');
				if(inx == std::string::npos)
				{
					String str = "Horizontal Data Integrity Error in Row #" + IntToStr(intN);
					throw Exception(str);
				}
				strDigest = strLineIn.substr(0, inx);
				std::wstring strR = strLineIn.c_str() + inx + 1;
				String usDigest = THashSHA1::GetHashString(to_string(strR));
				if(strDigest != to_string(usDigest))
				{
					String str = "Horizontal Data Integrity Error in Row #" + IntToStr(intN);
					throw Exception(str);
				}
			}
			else
			{   // Tail Row Processing
				usDigest = THashSHA1::GetHashString(to_string(std::to_wstring(intN)));
				// Vertical Data Integrity Check
				// Row i[0..39] == SHA1[N]
				if(strLineIn != to_string(usDigest))
				{
					String str = "Vertical Data Integrity Error";
					throw Exception(str);
				}
			}
		}

		formSlider->SetStatus(odlgSrcFile->FileName + " integrity is verified");
	}
	catch(Exception &e)
	{
		String str = e.Message;
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Encrypt_BIN(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	try
	{
		std::ifstream fsIn(to_string(odlgSrcFile->FileName), std::ios::binary);
		fsIn.exceptions(std::ios::badbit | std::ios::failbit);

		std::ofstream fsOut(to_string(strDstFileName), std::ios::binary);
		fsOut.exceptions(std::ios::badbit | std::ios::failbit);

		fsIn.seekg(0, ios::end);
		std::streampos size = fsIn.tellg();
		// Проверка на размера на входния файл fsIn
		if(size < 1)
		{
			std::string errorMessage = std::string("Invalid input file size");
			throw std::runtime_error(errorMessage);
		}

		std::vector<std::byte> vData(size);

		fsIn.seekg (0, ios::beg);
		fsIn.read(reinterpret_cast<char *>(vData.data()), size * sizeof(std::vector<std::byte>::value_type));

		// SHA-512[Key]
		System::DynamicArray<System::Byte> arrDigestKey = THashSHA2::GetHashBytes(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);
		// Header DigestKey
		std::vector<std::byte> vDigestKey;
		for(auto& x: arrDigestKey)
		{
			vDigestKey.push_back(static_cast<std::byte>(x));
		}
		fsOut.write(reinterpret_cast<char *>(vDigestKey.data()), vDigestKey.size() * sizeof(std::vector<std::byte>::value_type));

		// Binary Data
		Cryptor Cryptor;
		std::wstring strKey = to_string(formSlider->leditKey->Text);
		Cryptor.Set_Key(strKey);

		std::vector<std::byte> vDataEncrypt = Cryptor.Transform_Binary(vData);

		// SHA-512[DataEncrypt]
		std::unique_ptr<TMemoryStream> msDataEncrypt(new TMemoryStream);
		msDataEncrypt->Write(vDataEncrypt.data(), vDataEncrypt.size() * sizeof(std::vector<std::byte>::value_type));
		// Задължително връщане показалеца на потока в самото начало,
		// преди изчисляването на хеша на този поток.
		// В противен случай връща хеша на празен поток.
		msDataEncrypt->Seek(0, soFromBeginning);
		System::DynamicArray<System::Byte> arrDigestDataEncrypt = THashSHA2::GetHashBytes(msDataEncrypt.get(), THashSHA2::TSHA2Version::SHA512);
		// Header DigestDataEncrypt
		std::vector<std::byte> vDigestDataEncrypt;
		for(auto& x: arrDigestDataEncrypt)
		{
			vDigestDataEncrypt.push_back(static_cast<std::byte>(x));
		}
		fsOut.write(reinterpret_cast<char *>(vDigestDataEncrypt.data()), vDigestDataEncrypt.size() * sizeof(std::vector<std::byte>::value_type));

		fsOut.write(reinterpret_cast<char *>(vDataEncrypt.data()), vDataEncrypt.size() * sizeof(std::vector<std::byte>::value_type));

		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(std::exception& e)
	{
		String str = e.what();
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::Decrypt_BIN(void)
{
	String strDstFilePath = ExtractFilePath(strDstFileName);
	if(!TDirectory::Exists(strDstFilePath))
	{
		try
		{
			TDirectory::CreateDirectory(strDstFilePath);
		}
		catch(Exception& e)
		{
			String str = e.Message;
			formSlider->SetStatus(str);
			MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);
			return;
		}
	}

	try
	{
		std::ifstream fsIn(to_string(odlgSrcFile->FileName), std::ios::binary);
		fsIn.exceptions(std::ios::badbit | std::ios::failbit);

		std::ofstream fsOut(to_string(strDstFileName), std::ios::binary);
		fsOut.exceptions(std::ios::badbit | std::ios::failbit);

		fsIn.seekg(0, ios::end);
		std::streampos size = fsIn.tellg();
		// Проверка на размера на входния файл fsIn
		if(size <= 2*64) // HeaderKey(#64) + HeaderData(#64) + Data
		{
			std::string errorMessage = std::string("Invalid input file size");
			throw std::runtime_error(errorMessage);
		}

		std::vector<std::byte> vHeaderKey(64), vHeaderDataEncrypt(64), vDataEncrypt(static_cast<int>(size) - 2*64);

		fsIn.seekg (0, ios::beg);
		fsIn.read(reinterpret_cast<char *>(vHeaderKey.data()), 64);
		fsIn.read(reinterpret_cast<char *>(vHeaderDataEncrypt.data()), 64);
		fsIn.read(reinterpret_cast<char *>(vDataEncrypt.data()), static_cast<int>(size) - 2*64);

		// SHA-512[Key]
		System::DynamicArray<System::Byte> arrDigestKey = THashSHA2::GetHashBytes(formSlider->leditKey->Text, THashSHA2::TSHA2Version::SHA512);

		// Header DigestKey Processing (Валидност на паролата за декриптиране)
		std::vector<std::byte> vDigestKey;
		for(auto& x: arrDigestKey)
		{
			vDigestKey.push_back(static_cast<std::byte>(x));
		}

		if(vDigestKey != vHeaderKey)
		{
			std::string errorMessage = std::string("Invalid Password");
			throw std::runtime_error(errorMessage);
		}

		// SHA-512[DataEncrypt]
		std::unique_ptr<TMemoryStream> msDataEncrypt(new TMemoryStream);
		msDataEncrypt->Write(vDataEncrypt.data(), vDataEncrypt.size() * sizeof(std::vector<std::byte>::value_type));
		// Задължително връщане показалеца на потока в самото начало,
		// преди изчисляването на хеша на този поток.
		// В противен случай връща хеша на празен поток.
		msDataEncrypt->Seek(0, soFromBeginning);
		System::DynamicArray<System::Byte> arrDigestDataEncrypt = THashSHA2::GetHashBytes(msDataEncrypt.get(), THashSHA2::TSHA2Version::SHA512);
		// Header DigestDataEncrypt
		std::vector<std::byte> vDigestDataEncrypt;
		for(auto& x: arrDigestDataEncrypt)
		{
			vDigestDataEncrypt.push_back(static_cast<std::byte>(x));
		}

		if(vDigestDataEncrypt != vHeaderDataEncrypt)
		{
			std::string errorMessage = std::string("Data Integrity Error");
			throw std::runtime_error(errorMessage);
		}

		// Binary Data Processing
		Cryptor Cryptor;
		std::wstring strKey = to_string(formSlider->leditKey->Text);
		Cryptor.Set_Key(strKey);

		std::vector<std::byte> vDataDecrypt = Cryptor.Transform_Binary(vDataEncrypt);
		fsOut.write(reinterpret_cast<char *>(vDataDecrypt.data()), vDataDecrypt.size() * sizeof(std::vector<std::byte>::value_type));

		formSlider->SetStatus(strDstFileName + " created");
	}
	catch(std::exception& e)
	{
		String str = e.what();
		formSlider->SetStatus(str);
		MessageDlg(str, mtError, TMsgDlgButtons() << mbOK, 0);

		try
		{
			if(FileExists(strDstFileName))
			{
				DeleteFile(strDstFileName);
			}
		}
		catch(...)
		{
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TformMain::SetWorkingMode(String src_file)
{
	formSlider->cboxCheckOnlyMode->Checked = false;
	formSlider->cboxCheckOnlyMode->Enabled = false;

	formSlider->cboxProcessFirstRow->Checked = false;
	formSlider->cboxProcessFirstRow->Enabled = false;

	formSlider->cboxProcessFirstCol->Checked = false;
	formSlider->cboxProcessFirstCol->Enabled = false;

	if(src_file.IsEmpty())
	{   // Source Mode
		strDstFileName = "";
		wmWorkingMode = WorkingMode::Source;
		btnJoystick->Caption = STR_SOURCE;
	}
	else
	{   // Encrypt/Decrypt Mode
		String strSrcFilePath = ExtractFilePath(odlgSrcFile->FileName);
		String strSrcFileName = ExtractFileName(odlgSrcFile->FileName);
		String strSrcFileExt  = ExtractFileExt(odlgSrcFile->FileName);

		if(strSrcFileExt.IsEmpty() || (AnsiCompareText(strSrcFileExt, STR_FILE_EXT_TXT) == 0))
		{   // .TXT or .
			int inx;

			if((inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED)) > 0)
			{   // Decrypt Mode - DEC TXT
				strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_DEC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_DECRYPTED + STR_FILE_EXT_TXT;
				wmWorkingMode = WorkingMode::Decrypt_TXT;
				btnJoystick->Caption = STR_DECRYPT_TXT;
			}
			else if((inx = strSrcFileName.Pos(STR_FILE_SUFFICS_DECRYPTED)) > 0)
			{   // Encrypt Mode - ENC TXT
				strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_TXT;
				wmWorkingMode = WorkingMode::Encrypt_TXT;
				btnJoystick->Caption = STR_ENCRYPT_TXT;
			}
			else
			{   // Encrypt Mode - ENC TXT
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
				if(inx > 0)
				{
					strDstFileName = strSrcFilePath  + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_TXT;
				}
				else
				{
					strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_TXT;
				}
				wmWorkingMode = WorkingMode::Encrypt_TXT;
				btnJoystick->Caption = STR_ENCRYPT_TXT;
			}
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_XLS) == 0)
		{   // .XLSX
			// Encrypt Mode - ENC XLS
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_CSV;

			wmWorkingMode = WorkingMode::Encrypt_XLS;
			btnJoystick->Caption = STR_ENCRYPT_XLS;

			formSlider->cboxCheckOnlyMode->Enabled = true;
			formSlider->cboxProcessFirstRow->Enabled = true;
			formSlider->cboxProcessFirstCol->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_CSV) == 0)
		{   // .CSV
			// Decrypt Mode - DEC CSV
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_DEC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_DECRYPTED + STR_FILE_EXT_XLS;
			strDstXLSFileNameTemplate = strSrcFilePath + STR_DST_XLS_FILE_TEMPLATE;

			wmWorkingMode = WorkingMode::Decrypt_CSV;
			btnJoystick->Caption = STR_DECRYPT_CSV;

			formSlider->cboxCheckOnlyMode->Enabled = true;
			formSlider->cboxProcessFirstRow->Enabled = true;
			formSlider->cboxProcessFirstCol->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_WAV) == 0)
		{   // .WAV
			// Encrypt Mode - ENC WAV
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_WA_;

			wmWorkingMode = WorkingMode::Encrypt_WAV;
			btnJoystick->Caption = STR_ENCRYPT_WAV;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_WA_) == 0)
		{   // .WA_
			// Decrypt Mode - DEC WA_
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_DEC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_DECRYPTED + STR_FILE_EXT_WAV;

			wmWorkingMode = WorkingMode::Decrypt_WA_;
			btnJoystick->Caption = STR_DECRYPT_WA_;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_WMA) == 0)
		{   // .WMA
			// Encrypt Mode - ENC WMA
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_WM_;

			wmWorkingMode = WorkingMode::Encrypt_WMA;
			btnJoystick->Caption = STR_ENCRYPT_WMA;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_WM_) == 0)
		{   // .WM_
			// Decrypt Mode - DEC WM_
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_DEC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_DECRYPTED + STR_FILE_EXT_WMA;

			wmWorkingMode = WorkingMode::Decrypt_WM_;
			btnJoystick->Caption = STR_DECRYPT_WM_;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_BIN) == 0)
		{   // .BIN
			// Encrypt Mode - ENC BIN
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_ENC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_ENCRYPTED + STR_FILE_EXT_BI_;

			wmWorkingMode = WorkingMode::Encrypt_BIN;
			btnJoystick->Caption = STR_ENCRYPT_BIN;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
		else if(AnsiCompareText(strSrcFileExt, STR_FILE_EXT_BI_) == 0)
		{   // .BI_
			// Decrypt Mode - DEC BI_
			int inx = strSrcFileName.Pos(STR_FILE_SUFFICS_ENCRYPTED);

			if(inx == 0)
			{
				inx = strSrcFileName.LastDelimiter(".");    // Extension to be removed
			}
			strDstFileName = strSrcFilePath + STR_DST_FILE_PATH_DEC + strSrcFileName.SubString(1, inx - 1) + STR_FILE_SUFFICS_DECRYPTED + STR_FILE_EXT_BIN;

			wmWorkingMode = WorkingMode::Decrypt_BI_;
			btnJoystick->Caption = STR_DECRYPT_BI_;

			formSlider->cboxCheckOnlyMode->Enabled = true;
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TformMain::PasswordStrength(String pass)
{
	bool boolResult = true;

	if(pass.IsEmpty())
	{
		boolResult = false;
	}
	else
	{
		if(formSlider->cboxPasswordStrength->Checked)
		{
			if(pass.Length() < PASSWORD_MIN_LEN)
			{
				boolResult = false;
			}
		/* TODO : Допълнителен контрол на силата на паролата по вида на символите (опция) */
		// ...

		}
	}

	return boolResult;
}
//---------------------------------------------------------------------------
int __fastcall TformMain::wifstream_lines_number(std::wstring src_file)
{
	int intN = 0;
	std::wstring strLine;

	try
	{
		std::wifstream fs(src_file);
//		fs.exceptions(std::ios::badbit | std::ios::failbit);

		if(fs)
		{
			while(std::getline(fs, strLine))
			{
				intN++;
			}
		}
	}
	catch(...)
	{
		intN = 0;
	}

	return intN;
}
//---------------------------------------------------------------------------
int __fastcall TformMain::xsl_get_col_num(Variant& ws)
{
	int intN = 0;

	try
	{
		for(int j = 1; true; j++)
		{
			String str = ws.OlePropertyGet("Cells", 1, j);
			if(str.IsEmpty())
			{
				break;
			}
			else
			{
                intN++;
            }
		}
	}
	catch(...)
	{
		intN = 0;
	}

	return intN;
}
//---------------------------------------------------------------------------
int __fastcall TformMain::xsl_get_raw_num(Variant& ws)
{
	int intN = 0;

	try
	{
		for(int i = 1; true; i++)
		{
			String str = ws.OlePropertyGet("Cells", i, 1);
			if(str.IsEmpty())
			{
				break;
			}
			else
			{
				intN++;
			}
		}
	}
	catch(...)
	{
		intN = 0;
	}

	return intN;
}
//---------------------------------------------------------------------------

