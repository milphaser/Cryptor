//---------------------------------------------------------------------------
// Copyright (c) 1998 by Warren Ward.
//---------------------------------------------------------------------------
// Adapted 2004-2018 by Milen Loukantchevsky.
// Enhanced 2018, 2019 by Milen Loukantchevsky, Oktay Halilov.
//---------------------------------------------------------------------------

#include <System.Hash.hpp>
#include <iomanip>
#include <fstream>
#include <exception>
#include <sstream>
#pragma hdrstop

#include "UnitCryptor.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
Cryptor::Cryptor(bool boolStreaming) :
	// Initialize LFSRs to non-zero values.
	m_LFSR_A(0x13579BDF),
	m_LFSR_B(0x2468ACE0),
	m_LFSR_C(0xFDB97531),

	// Initialize feedback masks to primitive polynomials
//	m_Mask_A(0x80000062),   // (32, 7, 6, 2, 0)
	m_Mask_A(0x80000057),   // (32, 7, 5, 3, 2, 1, 0)
	m_Mask_B(0x40000020),   // (31, 6, 0)
	m_Mask_C(0x10000002),   // (29, 2, 0)

	// Set up LFSR "rotate" masks.
	m_Rot0_A(0x7FFFFFFF),
	m_Rot0_B(0x3FFFFFFF),
	m_Rot0_C(0x0FFFFFFF),
	m_Rot1_A(0x80000000),
	m_Rot1_B(0xC0000000),
	m_Rot1_C(0xF0000000)
{
	this->boolStreaming = boolStreaming;
}
//---------------------------------------------------------------------------
__fastcall Cryptor::~Cryptor(void)
{
}
//---------------------------------------------------------------------------
void __fastcall Cryptor::randomize(unsigned long seed)
{
	std::wstring strKey = std::to_wstring(seed);
	Set_Key(strKey);
}
//---------------------------------------------------------------------------
void __fastcall Cryptor::randomize_weak(unsigned long seed)
{
	std::wstring strKey = std::to_wstring(seed);
	Set_Key(strKey);
}
//---------------------------------------------------------------------------
std::byte __fastcall Cryptor::rand(void)
{
	std::byte Crypto {0};
	unsigned long Out_B = m_LFSR_B & 0x00000001;
	unsigned long Out_C = m_LFSR_C & 0x00000001;

	// Cycle the LFSRs eight times to get eight pseudo-
	// random bits. Assemble these into a single random
	// character (Crypto).
	for(int i = 0; i < 8; i++)
	{
		if(m_LFSR_A & 0x00000001)
		{
			// The least-significant bit of LFSR A is
			// "1". XOR LFSR A with its feedback mask.
		#if defined (FAULTY_RNG)
			m_LFSR_A = (m_LFSR_A ^ (m_Mask_A >> 1)) | m_Rot1_A; 	// ГРЕШНО
		#else
			m_LFSR_A = ((m_LFSR_A ^ m_Mask_A) >> 1) | m_Rot1_A;
		#endif

			// Clock shift register B once.
			if(m_LFSR_B & 0x00000001)
			{
				// The LSB of LFSR B is "1".
				// XOR LFSR B with its feedback mask.
			#if defined (FAULTY_RNG)
				m_LFSR_B = (m_LFSR_B ^ (m_Mask_B >> 1)) | m_Rot1_B; // ГРЕШНО
			#else
				m_LFSR_B = ((m_LFSR_B ^ m_Mask_B) >> 1) | m_Rot1_B;
			#endif
				Out_B = 0x00000001;
			}
			else
			{
				// The LSB of LFSR B is "0". Rotate
				// the LFSR contents once.
				m_LFSR_B = (m_LFSR_B >> 1) & m_Rot0_B;
				Out_B = 0x00000000;
			}
		}
		else
		{
			// The LSB of LFSR A is "0".
			// Rotate the LFSR contents once.
			m_LFSR_A = (m_LFSR_A >> 1) & m_Rot0_A;

			// Clock shift register C once.
			if(m_LFSR_C & 0x00000001)
			{
				// The LSB of LFSR C is "1".
				// XOR LFSR C with its feedback mask.
			#if defined (FAULTY_RNG)
				m_LFSR_C = (m_LFSR_C ^ (m_Mask_C >> 1)) | m_Rot1_C; // ГРЕШНО
			#else
				m_LFSR_C = ((m_LFSR_C ^ m_Mask_C) >> 1) | m_Rot1_C;
            #endif
				Out_C = 0x00000001;
			}
			else
			{
				// The LSB of LFSR C is "0". Rotate
				// the LFSR contents once.
				m_LFSR_C = (m_LFSR_C >> 1) & m_Rot0_C;
				Out_C = 0x00000000;
			}
		}

		// XOR the output from LFSRs B and C and rotate it
		// into the right bit of Crypto.
		Crypto = (Crypto << 1) | static_cast<std::byte>(Out_B ^ Out_C);
	}

	return Crypto;
}
//---------------------------------------------------------------------------
std::byte __fastcall Cryptor::rand_weak(void)
{
	std::byte Crypto {0};
	unsigned long Out_A = 0x00000000;

	// Cycle the LFSR eight times to get eight pseudo-
	// random bits. Assemble these into a single random
	// character (Crypto).
	for(int i = 0; i < 8; i++)
	{
		if(m_LFSR_A & 0x00000001)
		{
			// The least-significant bit of LFSR A is
			// "1". XOR LFSR A with its feedback mask.
		#if defined (FAULTY_RNG)
			m_LFSR_A = (m_LFSR_A ^ (m_Mask_A >> 1)) | m_Rot1_A; 	// ГРЕШНО
		#else
			m_LFSR_A = ((m_LFSR_A ^ m_Mask_A) >> 1) | m_Rot1_A;
		#endif
			Out_A = 0x00000001;
		}
		else
		{
			// The LSB of LFSR A is "0".
			// Rotate the LFSR contents once.
			m_LFSR_A = (m_LFSR_A >> 1) & m_Rot0_A;
			Out_A = 0x00000000;
		}

		// Rotate the output bit into Crypto.
		//
		Crypto = (Crypto << 1) | static_cast<std::byte>(Out_A);
	}

	return Crypto;
}
//---------------------------------------------------------------------------
void __fastcall Cryptor::Set_Key(const std::wstring& Key)
{
	vStreamIn1.clear();
	vStreamIn2.clear();
	vStreamOut.clear();

//	1. Стрингът Key със зададения ключ (парола) се трансформира в 40-символен хеш H.
//	2. Началните стойности на регистрита A, B и C се формират по следния начин:
//	A <- H[ 0.. 7] ^ H[24..31]
//	B <- H[ 8..15] ^ H[32..39]
//	C <- H[16..23] ^ H[32..39]
//	където с H[0] е обозначен първият, а с H[39] - последният символ от хеш H.

	// Determine 40 symbols Key Digest
	m_Key_Digest = to_string(THashSHA1::GetHashString(to_string(Key)));

	unsigned int intH00_07 = std::stoul(m_Key_Digest.substr( 0, 8), nullptr, 16);
	unsigned int intH08_15 = std::stoul(m_Key_Digest.substr( 8, 8), nullptr, 16);
	unsigned int intH16_23 = std::stoul(m_Key_Digest.substr(16, 8), nullptr, 16);
	unsigned int intH24_31 = std::stoul(m_Key_Digest.substr(24, 8), nullptr, 16);
	unsigned int intH32_39 = std::stoul(m_Key_Digest.substr(32, 8), nullptr, 16);

	m_LFSR_A = intH00_07 ^ intH24_31;
	m_LFSR_B = intH08_15 ^ intH32_39;
	m_LFSR_C = intH16_23 ^ intH32_39;
}
//---------------------------------------------------------------------------
std::wstring __fastcall Cryptor::Transform_String(const std::wstring& Src, const std::wstring& Key)
{
	// Reset the shift registers.
	if(!Key.empty())
	{
		Set_Key(Key);
	}

	std::wstring Dst;

	// Transform each character in the string.
	for(auto& x : Src)
	{
		std::byte b_hi = Transform_Char(static_cast<std::byte>(x >> 8));
		std::byte b_lo = Transform_Char(static_cast<std::byte>(x & 0xFF));
		int i = (static_cast<int>(b_hi) << 8) + static_cast<int>(b_lo);
		Dst += static_cast<wchar_t>(i);
	}

	return Dst;
}
//---------------------------------------------------------------------------
std::vector<std::byte> __fastcall Cryptor::Transform_Binary(const std::vector<std::byte>& Src, const std::wstring& Key)
{
	// Reset the shift registers.
	if(!Key.empty())
	{
		Set_Key(Key);
	}

	std::vector<std::byte> Dst;

	// Transform each byte in the vector.
	for(auto& x : Src)
	{
		std::byte b = Transform_Char(x);
		Dst.push_back(b);
	}

	return Dst;
}
//---------------------------------------------------------------------------
std::byte __fastcall Cryptor::Transform_Char(const std::byte& Src)
{
	std::byte Crypto = rand();

	// XOR the resulting character with the
	// input character to encrypt/decrypt it.
	std::byte Out {Src ^ Crypto};

	if(boolStreaming)
	{
		vStreamIn1.push_back(Src);
		vStreamIn2.push_back(Crypto);
		vStreamOut.push_back(Out);
	}

	return Out;
}
//---------------------------------------------------------------------------
std::byte __fastcall Cryptor::Weak_Transform_Char(const std::byte& Src)
{
	std::byte Crypto = rand_weak();

	// XOR the resulting character with the
	// input character to encrypt/decrypt it.
	std::byte Out {Src ^ Crypto};

	if(boolStreaming)
	{
		vStreamIn1.push_back(Src);
		vStreamIn2.push_back(Crypto);
		vStreamOut.push_back(Out);
	}

	return Out;
}
//---------------------------------------------------------------------------
void __fastcall Cryptor::Streaming(std::wstring fname)
{
	if(!boolStreaming)
		return;

	try
	{
		std::ofstream of(fname);

		of << "In1: ";
		for(auto& vs: vStreamIn1)
		{
			of << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(vs) << " ";
		}
		of << std::endl;

		of << "In2: ";
		for(auto& vs: vStreamIn2)
		{
			of << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(vs) << " ";
		}
		of << std::endl;

		of << "Out: ";
		for(auto& vs: vStreamOut)
		{
			of << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << static_cast<int>(vs) << " ";
		}
		of << std::endl;
	}
	catch(std::exception& e)
	{
//		e.what();
	}
}
//---------------------------------------------------------------------------
std::wstring __fastcall to_string(const String& src)
{
	std::wstring strDst;

	strDst = src.w_str();
//	for(auto& it : src)
//	{
//		strDst += it;
//	}
//
	return strDst;
}
//---------------------------------------------------------------------------
String __fastcall to_string(const std::wstring& src)
{
	String strDst;

	strDst = src.c_str();
//	for(auto& it : src)
//	{
//		strDst += it;
//	}
//
	return strDst;
}
//---------------------------------------------------------------------------
std::wstring __fastcall s2r(const std::wstring& src)
{
	std::wstring strDstRaw;		// Destination Raw String

	std::wstringstream ss;
	for(auto& it: src)
	{
		ss << std::setw(4) << std::setfill(L'0') << std::hex << std::uppercase << static_cast<int> (it);
	}

	ss >> strDstRaw;

	return strDstRaw;
}
//---------------------------------------------------------------------------
std::wstring __fastcall r2s(const std::wstring& src)
{
	std::wstring strDstStr;         // Destination Wide String

	std::vector<int> vSrc;
	std::wstring strSrc = src;
	std::wstring strSrcItem;

	while(!strSrc.empty())
	{
		strSrcItem = strSrc.substr(0, 4);
		strSrc = strSrc.c_str() + 4;

		int val = std::stoi(strSrcItem, nullptr, 16);
		vSrc.push_back(val);
	}

	for(auto& it : vSrc)
	{
		strDstStr += it;
	}

	return strDstStr;
}
//---------------------------------------------------------------------------

