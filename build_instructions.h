#pragma once

#include <cstdint>
#include <string_view>
#include <vector>
#include <array>
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
  SH_NONE = 0x0000,
  SH1     = 0x0001,
  SH2     = 0x0002,
  SH2E    = 0x0004,
  SH2A    = 0x0008,
  SH3     = 0x0010,
  SH3E    = 0x0020,
  SH4     = 0x0040,
  SH4A    = 0x0080,
  SH_DSP  = 0x0100,

  SH_ANY  = SH1 | SH2 | SH3 | SH2A | SH4 | SH4A,
};

struct isa_property : std::array<std::string_view, 9>
{
  using parent = std::array<std::string_view, 9>;
  isa_property (void) { }

  template <typename... Args>
  isa_property (const isa f, const char* val, const Args&... args)
    : isa_property(args...)
  {
    for(std::size_t pos = 0; pos < parent::size(); ++pos)
      if(f & (1 << pos))
        parent::operator[](pos) = val;
  }

  std::string_view operator[] (isa i) const
    { return parent::operator[](countr_zero(i)); }
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

using string_class = property_class<const char*, nullptr>;

struct dc_bit       : std::string_view {};
struct t_bit        : std::string_view {};
struct format       : std::string_view {};
struct abstract     : std::string_view {};
struct code         : std::string_view {};
struct description  : std::string_view {};
struct note         : std::string_view {};
struct operation    : std::string_view {};
struct example      : std::string_view {};
struct exceptions   : std::string_view {};
using privilege = property_class<bool, false>;
constexpr privilege privileged = { true };

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

  bool is_isa (isa i) const { return std::get<isa>(details) & i; }
  bool privileged() const { return std::get<privilege>(details); }

  std::tuple<format, abstract, code, description, note,
             operation, example, exceptions, group, issue,
             latency, privilege, t_bit, dc_bit, isa> details =
  {
    format(), abstract(), code(), description(), note(),
    operation(), example(), exceptions(), group(), issue(),
    latency(), privilege(), t_bit(), dc_bit(), SH_NONE,
  };
};

struct insns : public std::vector<insn>
{
  template <typename... Args>
  insns (const char* title, const Args&... args)
    : std::vector<insn>({ args... })
  { section_title = title; }

  const char* section_title;
};

// ----------------------------------------------------------------------------

std::vector<insns> build_insn_blocks(void);
