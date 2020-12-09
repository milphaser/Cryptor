//---------------------------------------------------------------------------
// Copyright (c) 1998 by Warren Ward.
//---------------------------------------------------------------------------
// Adapted 2004-2018 by Milen Loukantchevshy.
// Enhanced 2018, 2019 by Milen Loukantchevshy, Oktay Halilov.
//---------------------------------------------------------------------------

#ifndef UnitCryptorH
#define UnitCryptorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <cstddef>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
std::wstring __fastcall to_string(const String& src);
String __fastcall to_string(const std::wstring& src);

std::wstring __fastcall s2r(const std::wstring& src);
std::wstring __fastcall r2s(const std::wstring& src);
//---------------------------------------------------------------------------
class Cryptor
{
protected:
	std::wstring m_Key_Digest;  // SHA-1 Hash of the Key

	bool boolStreaming;
	std::vector<std::byte> vStreamIn1;
	std::vector<std::byte> vStreamIn2;
	std::vector<std::byte> vStreamOut;

    // LFSR Galoa's Configuration
	unsigned long m_LFSR_A;
	unsigned long m_LFSR_B;
	unsigned long m_LFSR_C;
	const unsigned long m_Mask_A;
	const unsigned long m_Mask_B;
	const unsigned long m_Mask_C;
	const unsigned long m_Rot0_A;
	const unsigned long m_Rot0_B;
	const unsigned long m_Rot0_C;
	const unsigned long m_Rot1_A;
	const unsigned long m_Rot1_B;
	const unsigned long m_Rot1_C;

public:
	Cryptor(bool boolStreaming = false);
	~Cryptor(void);

	// RNG
	void __fastcall randomize(unsigned long seed);
	std::byte __fastcall rand(void);

	void __fastcall randomize_weak(unsigned long seed);
	std::byte __fastcall rand_weak(void);

	// Cryptor
	void __fastcall Set_Key(const std::wstring& Key);
	std::wstring __fastcall Transform_String(const std::wstring& Src, const std::wstring& Key = L"");
	std::vector<std::byte> __fastcall Transform_Binary(const std::vector<std::byte>& Src, const std::wstring& Key = L"");
	std::byte __fastcall Transform_Char(const std::byte& Src);
	std::byte __fastcall Weak_Transform_Char(const std::byte& Src);

	void __fastcall Streaming(std::wstring fname);
};
//---------------------------------------------------------------------------
#endif
