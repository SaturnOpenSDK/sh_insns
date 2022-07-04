#pragma once

#include <cstdint>
#include <string>
#include <list>
#include <tuple>

#if __cplusplus < 202002L
template< class T >
constexpr int countr_zero( T x ) noexcept
{
  for(std::size_t i = 0; i < sizeof(T) * 8; ++i)
    if(x & (1 << i))
      return i;
  return sizeof (T) * 8;
}
#else
# include <bit>
#endif

enum isa : uint16_t
{
  SH_NONE   = 0x0000,
  SH1       = 0x0001,
  SH1_DSP   = 0x0002,
  SH2       = 0x0004,
  SH2_DSP   = 0x0008,
  SH2E      = 0x0010,
  SH2A      = 0x0020,
  SH2A_FPU  = 0x0040,
  SH3       = 0x0080,
  SH3_FPU   = 0x0100,
  SH3_DSP   = 0x0200,
  SH4       = 0x0400,
  SH4A      = 0x0800,

  SH_ALL  = SH1 | SH1_DSP | SH2 | SH2_DSP | SH2E | SH2A | SH2A_FPU | SH3 | SH3_FPU | SH3_DSP | SH4 | SH4A,
};
constexpr const int isa_count = 12;

constexpr isa operator |(isa a, isa b)
  { return isa(uint16_t(a) | uint16_t(b)); }

struct isa_property : std::array<std::string, isa_count>
{
  using parent = std::array<std::string, isa_count>;
  isa_property (void) { }

  template <typename... Args>
  isa_property (const isa f, const char* val, const Args&... args)
    : isa_property(args...)
  {
    for(std::size_t pos = 0; pos < parent::size(); ++pos)
      if(f & (1 << pos))
        parent::operator[](pos) = val;
  }

  std::string operator[] (uint16_t i) const
  {
    switch(i)
    {
      case SH1: case SH1_DSP:
      case SH2: case SH2_DSP:
      case SH2E: case SH2A: case SH2A_FPU:
      case SH3: case SH3_FPU: case SH3_DSP:
      case SH4: case SH4A:
        return parent::operator[](countr_zero(i));

      case SH1_DSP | SH2_DSP | SH3_DSP:
        if(!operator[](SH1_DSP).empty())
          return operator[](SH1_DSP);
        if(!operator[](SH2_DSP).empty())
          return operator[](SH2_DSP);
        if(!operator[](SH3_DSP).empty())
          return operator[](SH3_DSP);
        break;

      case SH2A | SH2A_FPU:
        if(!operator[](SH2A).empty())
          return operator[](SH2A);
        if(!operator[](SH2A_FPU).empty())
          return operator[](SH2A_FPU);
        break;
    }
    return "";
  }
};

struct group : public isa_property { using isa_property::isa_property; };
struct issue : public isa_property { using isa_property::isa_property; };
struct latency : public isa_property { using isa_property::isa_property; };


template<typename T, T default_value>
struct property_class
{
  constexpr property_class(const T& other) : value(other) {}
  constexpr property_class(void) : value(default_value) {}
  constexpr operator const T(void) const { return value; }
  constexpr T& operator =(const T& other) { return value = other; }
  T value;
};

enum document
{
  SH1_DOC = 0,
  SH1_2_PROG_DOC,
  SH1_2_DSP_DOC,
  SH2A_2E_DOC,
  SH3_3E_DSP_DOC,
  SHA4_CORE_DOC,
  SH7750_PROG_DOC,
  SH4A_DOC,
};

struct document_details_t
{
  const std::string_view name;
  const std::string_view identifier;
  const std::string_view date;
  const std::string_view location;
  isa instruction_sets;
};

constexpr std::array<document_details_t, 10> documents =
{
  {
    { // SH1_DOC
      "SH7032/7034 Series\nSuperH™ RISC Engine\nHardware Manual",
      "ADE-602-063A",
      "1995",
      "https://archive.org/details/bitsavers_hitachisupperHRISCEngineHardwareManual_29586120",
      SH1,
    },
    { // SH1_2_PROG_DOC
      "SuperH RISC Engine\nSH-1/SH-2\nProgramming Manual\n3rd Edition",
      "ADE-602-063B",
      "1996/09/03",
      "https://antime.kapsi.fi/sega/files/h12p0.pdf", // "https://archive.org/details/bitsavers_hitachisupmmingManual3rdEdition_11452188",
      SH1 | SH2,
    },
    { // SH1_2_DSP_DOC
      "Hitachi SuperH™ RISC Engine\nSH-1/SH-2/SH-DSP\nProgramming Manual",
      "ADE-602-063C\nRev. 4.0",
      "1999/13/05",
      "https://retrocdn.net/images/3/35/Hitachi_SuperH_Programming_Manual.pdf",
      SH1 | SH2 | SH1_DSP,
    },
    { // SH2A_2E_DOC
      "SH-2A, SH2A-FPU\nSoftware Manual\nUser’s Manual\nRenesas 32-Bit RISC\nMicrocomputer\nSuperH™ RISC Engine",
      "Rev. 3.00",
      "2005/07/08",
      "https://www.renesas.com/us/en/document/mah/sh-2a-sh2a-fpu-software-manual?language=en",
      SH2E | SH2A | SH2A_FPU,
    },
    { // SH3_3E_DSP_DOC
      "SH-3/SH-3E/SH3-DSP\nSoftware Manual",
      "Rev. 4.00",
      "2006/05/15",
      "https://www.renesas.com/us/en/document/mas/sh-3sh-3esh3-dsp-software-manual?language=en",
      SH3 | SH3_FPU | SH3_DSP,
    },
    { // SHA4_CORE_DOC
      "SH-4 CPU Core Architecture",
      "ADCS 7182230F",
      "2002/09/12",
      "https://www.st.com/resource/en/user_manual/cd00147165-sh-4-32-bit-cpu-core-architecture-stmicroelectronics.pdf",
      SH4,
    },
    { // SH7750_PROG_DOC
      "SuperH™ (SH) 32-Bit RISC MCU/MPU Series\nSH7750\nHigh-Performance RISC Engine\nProgramming Manual",
      "ADE-602-156A\nRev. 2.0",
      "1999/03/04",
      "https://archive.org/details/manuallib-id-2595799",
      SH4,
    },
    { // SH4A_DOC
      "SH-4A\nExtended Functions\nSoftware Manual",
      "Rev.2.00",
      "2013/01/18",
      "https://www.renesas.com/us/en/document/mat/sh-4a-extended-functions-software-manual?language=en",
      SH4A,
    },
  }
};

struct citation_t
{
  document source;
  int page;
  isa instruction_sets = SH_NONE;
};

struct environment_t
{
  isa instruction_sets;
  std::string property;
};

struct environments     : std::list<environment_t> {};
struct citations        : std::list<citation_t> {};

struct format           : std::string {};
struct abstract         : std::string {};
struct brief            : std::string {};
struct name             : std::string {};
struct restriction      : std::string {};
struct classification   : std::string {};
struct mnemonic         : std::string {};
struct mnemonic_origin  : std::string {};
struct opcode           : std::string {};
struct flags            : std::string {};
struct description      : std::string {};
struct note             : std::string {};
struct operation        : std::string {};
struct example          : std::string {};
struct exceptions       : std::string {};

/*
 TODO:
 add: BRK instruction
 add: FIPR,FTRV brief

  mnemonic { "FMOV.D" }, // ??
*/

struct insn
{
  insn(void) {}

  template <typename T, typename... Args>
  insn(const T& a, const Args&... args)
    : insn(args...)
  {
    std::get<T>(details) = a;
  }

  template <typename... Args>
  insn (const char* str_format, const Args&... args)
      : insn(args...)
  {
    std::get<format>(details) = format { str_format };
  }

  template <typename T>
  const T& data(void) const { return std::get<T>(details); }

  template <typename T>
  T& data(void) { return std::get<T>(details); }

  bool for_isa (isa i) const { return std::get<isa>(details) & i; }

  std::tuple<format, abstract, name, classification, brief, restriction, mnemonic, mnemonic_origin,
             citations, opcode, description, note, operation, example, exceptions,
             group, issue, latency, environments, flags, isa> details =
  {
    format(), abstract(), name(), classification(), brief(), restriction(), mnemonic(), mnemonic_origin(),
    citations(), opcode(), description(), note(), operation(), example(), exceptions(),
    group(), issue(), latency(), environments(), flags(), SH_NONE,
  };
};

struct insns : public std::list<insn>
{
  template <typename... Args>
  insns (const char* title, const Args&... args)
    : std::list<insn>({ args... })
  { section_title = title; }

  const char* section_title;
};

// ----------------------------------------------------------------------------

void build_insn_blocks(std::list<insns>& insn_blocks);
