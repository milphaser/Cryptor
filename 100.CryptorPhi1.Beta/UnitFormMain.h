//---------------------------------------------------------------------------

#ifndef UnitFormMainH
#define UnitFormMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <System.ImageList.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Menus.hpp>
//---------------------------------------------------------------------------
const int PASSWORD_MIN_LEN      = 10;

const String STR_SOURCE 		= "SRC";
const String STR_ENCRYPT_TXT    = "ENC TXT";
const String STR_DECRYPT_TXT    = "DEC TXT";
const String STR_ENCRYPT_XLS    = "ENC XLS";
const String STR_DECRYPT_CSV    = "DEC CSV";
const String STR_ENCRYPT_WAV    = "ENC WAV";
const String STR_DECRYPT_WA_    = "DEC WA_";
const String STR_ENCRYPT_WMA    = "ENC WMA";
const String STR_DECRYPT_WM_    = "DEC WM_";
const String STR_ENCRYPT_BIN    = "ENC BIN";
const String STR_DECRYPT_BI_    = "DEC BI_";

const String STR_FILE_SUFFICS_ENCRYPTED = "_encrypted";
const String STR_FILE_SUFFICS_DECRYPTED = "_decrypted";

const String STR_DST_FILE_PATH_ENC = "ENC\\";
const String STR_DST_FILE_PATH_DEC = "DEC\\";

const String STR_FILE_EXT_TXT = ".txt";
const String STR_FILE_EXT_XLS = ".xlsx";
const String STR_FILE_EXT_CSV = ".csv";
const String STR_FILE_EXT_WAV = ".wav";
const String STR_FILE_EXT_WA_ = ".wa_";
const String STR_FILE_EXT_WMA = ".wma";
const String STR_FILE_EXT_WM_ = ".wm_";
const String STR_FILE_EXT_BIN = ".bin";
const String STR_FILE_EXT_BI_ = ".bi_";

const String STR_DST_XLS_FILE_TEMPLATE = "temp.xltx";

const TCHAR TCHAR_DECIMAL_SEPARATOR = '.';
//---------------------------------------------------------------------------
enum class WorkingMode : int
{
	Source,
	Encrypt_TXT,
	Decrypt_TXT,
	Encrypt_XLS,
	Decrypt_CSV,
	Encrypt_WAV,
	Decrypt_WA_,
	Encrypt_WMA,
	Decrypt_WM_,
	Encrypt_BIN,
	Decrypt_BI_
};
//---------------------------------------------------------------------------
class TformMain : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *odlgSrcFile;
	TTrayIcon *tiTray;
	TImageList *ilTray;
	TPopupMenu *pupTray;
	TMenuItem *miOpen;
	TMenuItem *miClose;
	TButton *btnJoystick;
	TMenuItem *miSrc;
	TMenuItem *N1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall miOpenClick(TObject *Sender);
	void __fastcall miCloseClick(TObject *Sender);
	void __fastcall tiTrayDblClick(TObject *Sender);
	void __fastcall btnJoystickClick(TObject *Sender);
	void __fastcall miSrcClick(TObject *Sender);

private:	// User declarations
	bool boolFirstShow;
	bool boolCloseEnable;

	WorkingMode wmWorkingMode;
	String strDstFileName;
	String strDstXLSFileNameTemplate;

	void __fastcall Source(void);

	void __fastcall Encrypt_TXT(void);
	void __fastcall Decrypt_TXT(void);

	void __fastcall Encrypt_XLS(void);
	void __fastcall Decrypt_CSV(void);
	void __fastcall Check_CSV(void);

	void __fastcall Encrypt_BIN(void);
	void __fastcall Decrypt_BIN(void);

	void __fastcall SetWorkingMode(String src_file = "");
	bool __fastcall PasswordStrength(String pass);

	int __fastcall wifstream_lines_number(std::wstring src_file);
	int __fastcall xsl_get_col_num(Variant& ws);
	int __fastcall xsl_get_raw_num(Variant& ws);

public:		// User declarations
	__fastcall TformMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TformMain *formMain;
//---------------------------------------------------------------------------
#endif
