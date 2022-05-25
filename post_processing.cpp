#include "post_processing.h"

#include "build_instructions.h"

#include <algorithm>
#include <regex>
#include <array>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::literals;
using namespace std::string_view_literals;
using namespace std::literals::string_literals;

constexpr bool operator ==(const environment_t& a, const environment_t& b)
  { return a.cpus == b.cpus && a.property == b.property; }


static const std::array<std::pair<std::string_view, std::string_view>, 26> unicode_symbols =
{
  {
    { ">", "&gt;" }, // because we use the < character
    { "<", "&lt;" }, // because we use the > character
    { "=", "&equal;" }, // because we use the = character
    { "/", "&divide;" }, // because we use the = character
    { "≥", "<var title=\"greater than or equal\"></var>" },
    { "≤", "<var title=\"less than or equal\"></var>" },
    { "&lt;&lt;", "<var title=\"shift bits left\"></var>" },
    { "&gt;&gt;", "<var title=\"shift bits right\"></var>" },
    { "&gt;", "<var title=\"greater than\"></var>" },
    { "&lt;", "<var title=\"less than\"></var>" },
    { "&equal;", "<var title=\"equal\"></var>" },
    { "∀", "<var title=\"for all\"></var>" },
    { "-", "<var title=\"subtract\"></var>" },
    { "−", "<var title=\"subtract\"></var>" },
    { "+", "<var title=\"add\"></var>" },
    { "×", "<var title=\"multiply\"></var>" },
    { "÷", "<var title=\"divide\"></var>" },
    { "&divide;", "<var title=\"divide\"></var>" },
    { "∨", "<var title=\"binary or\"></var>" },
    { "∧", "<var title=\"binary and\"></var>" },
    { "→", "<var title=\"store into (right)\"></var>" },
    { "←", "<var title=\"store into (left)\"></var>" },
    { "⊕", "<var title=\"binary xor\"></var>" },
    { "~", "<var title=\"binary not\"></var>" },
    { "PC’’","<var title=\"double prime\">PC</var>"},
    { "PC’", "<var title=\"prime\">PC</var>"},
  }
};

static const std::array<std::pair<std::string_view, std::string_view>, 21> typeable_symbols =
{
  {
    { ">", "&gt;" }, // because we use the < character
    { "<", "&lt;" }, // because we use the > character
    { "=", "&equal;" }, // because we use the = character
    { "/", "&divide;" }, // because we use the = character
    { "-&gt;", "<var title=\"store into (right)\"></var>" },
    { "&lt;-", "<var title=\"store into (left)\"></var>" },
    { "&gt;&equal;", "<var title=\"greater than or equal\"></var>" },
    { "&lt;&equal;", "<var title=\"less than or equal\"></var>" },
    { "&equal;", "<var title=\"equality\"></var>" },
    { "&lt;&lt;", "<var title=\"shift bits left\"></var>" },
    { "&gt;&gt;", "<var title=\"shift bits right\"></var>" },
    { "&gt;", "<var title=\"greater than\"></var>" },
    { "&lt;", "<var title=\"less than\"></var>" },
    { "^", "<var title=\"binary xor\"></var>" },
    { "~", "<var title=\"binary not\"></var>" },
    { "*", "<var title=\"multiply\"></var>" },
    { "-", "<var title=\"subtract\"></var>" },
    { "+", "<var title=\"add\"></var>" },
    { "&divide;", "<var title=\"divide\"></var>" },
    { "&", "<var title=\"binary and\"></var>" },
    { "|", "<var title=\"binary or\"></var>" },
  }
};

static const std::array<std::pair<const char*, const char*>, 2> typeable_patterns =
{
  {
    { "abs\\(([^\\)]+)\\)", "<var title=\"absolute value\">\\1</var>" },
    { "sqrt\\(([^\\)]+)\\)", "<var title=\"square root\">\\1</var>" },
  }
};

static constexpr const std::array<std::pair<std::string_view, std::string_view>, 30> long_accronyms =
{
  {
    { "NaN", "<abbr title=\"Not a Number\">NaN</abbr>" },
    { "ALU", "<abbr title=\"Arithmetic Logic Unit\">ALU</abbr>" },
    { "ASID", "<abbr title=\"Address Space Identifier\">ASID</abbr>" },
    { "CPU", "<abbr title=\"Central Processing Unit\">CPU</abbr>" },
    { "UTLB", "<abbr title=\"Unified Translation Lookaside Buffer\">UTLB</abbr>" },
    { "ITLB", "<abbr title=\"Instruction Translation Lookaside Buffer\">ITLB</abbr>" },
    { "LRU", "<abbr title=\"Least Recently Used\">LRU</abbr>" },
    { "LSB", "<abbr title=\"Least Significant Bit\">LSB</abbr>" },
    { "MSB", "<abbr title=\"Most Significant Bit\">MSB</abbr>" },
    { "PMB", "<abbr title=\"Privileged space Mapping Buffer\">PMB</abbr>" },
    { "RISC", "<abbr title=\"Reduced Instruction Set Computer\">RISC</abbr>" },
    { "UBC", "<abbr title=\"User Break Controller\">UBC</abbr>" },
    { "GBR", "<abbr title=\"Global Base Register\">GBR</abbr>" },
    { "VBR", "<abbr title=\"Vector Base Register\">VBR</abbr>" },
    { "DSR", "<abbr title=\"DSP Status Register\">DSR</abbr>" },
    { "I0-I3", "<abbr title=\"Interrupt mask flag bits\">I0-I3</abbr>" },
    { "I3-I0", "<abbr title=\"Interrupt mask flag bits\">I3-I0</abbr>" },
    { "MACH", "<abbr title=\"Multiply and ACcumulate High (word)\">MACH</abbr>" },
    { "MACL", "<abbr title=\"Multiply and ACcumulate Low (word)\">MACL</abbr>" },
    { "FPUL", "<abbr title=\"Floating-Point Communication Register\">FPUL</abbr>" },
    { "FPSCR", "<abbr title=\"Floating-Point Status/Control Register\">FPSCR</abbr>" },
    { "INTEVT", "<abbr title=\"Interrupt Event Register\">INTEVT</abbr>" },
    { "EXPMASK", "<abbr title=\"Non-Support Detection Exception Register\">EXPMASK</abbr>" },
    { "EXPEVT", "<abbr title=\"Exception Event Register\">EXPEVT</abbr>" },
    { "PTEH", "<abbr title=\"Page Table Entry High Register\">PTEH</abbr>" },
    { "PTEL", "<abbr title=\"Page Table Entry Low Register\">PTEL</abbr>" },
    { "MMUCR", "<abbr title=\"MMU (Memory Management Unit) Control Register\">MMUCR</abbr>" },
    { "PTEA", "<abbr title=\"Page Table Entry Assistance Register\">PTEA</abbr>" },
    { "PASCR", "<abbr title=\"Physical Address Space Control Register\">PASCR</abbr>" },
    { "IRMCR", "<abbr title=\"Instruction Re-Fetch Inhibit Control Register\">IRMCR</abbr>" },
  }
};

static constexpr std::array<std::pair<const char*, const char*>, 30> short_accronyms =
{
  {
    // { "([^\\<\\>[:alnum:]\\S])XXXXXX([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"YYYYY\">XXXXXX</abbr>\\2" },

    { "([^\\<\\>[:alnum:]\\S])FV(1?[[:digit:]])([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Single-precision Floating-Point Vector Register \\2\">FV\\2</abbr>\\3" },
    { "([^\\<\\>[:alnum:]\\S])FR(1?[[:digit:]])([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Single-precision Floating-Point Register \\2\">FR\\2</abbr>\\3" },
    { "([^\\<\\>[:alnum:]\\S])XF(1?[[:digit:]])([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Single-precision Floating-Point Extended Register \\2\">XF\\2</abbr>\\3" },
    { "([^\\<\\>[:alnum:]\\S])DR(1?[[:digit:]])([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Double-precision Floating-Point Register Pair or Single-precision Floating-point Register Pair \\2\">DR\\2</abbr>\\3" },
    { "([^\\<\\>[:alnum:]\\S])XD(1?[[:digit:]])([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Single-precision Floating-Point Extended Register Pair \\2\">XD\\2</abbr>\\3" },

    { "([^\\<\\>[:alnum:]\\S])PR([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Procedure Register\">PR</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])DSP([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Digital Signal Processor\">DSP</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])MMU([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Memory Management Unit\">MMU</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])TTB([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Translation Table Base Register\">TTB</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])TEA([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"TLB (Translation Lookaside Buffer) Exception Address register\">TEA</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])TRA([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"TRAPA (Trap Always instruction) Exception Register\">TRA</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])FPU([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Floating Point Unit\">FPU</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])MAC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Multiply and Accumulate\">MAC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])TLB([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Translation Lookaside Buffer\">TLB</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])PC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Program Counter\">PC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I0([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit 0\">I0</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I1([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit 1\">I1</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I2([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit 2\">I2</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I3([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit 3\">I3</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])SR([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Status Register\">SR</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])CS([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Condition Select bit flags (2 bits)\">CS</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])DC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"DSP (Digital Signal Processor) Condition bit flag\">DC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])GT([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Signed Greater Than bit flag\">GT</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])Z([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Zero value bit flag\">Z</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])N([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Negative value flag\">N</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])V([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Overflow bit flag\">V</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])S([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Saturation bit flag (for multiply-accumulate)\">S</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])T([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Test condition bit flag\">T</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])M([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Divide-step M flag\">M</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])Q([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Divide-step Q flag\">Q</abbr>\\2" },
  }
};

template<typename T>
void replace_symbols(std::string& data, const T& symbols)
{
  if(!data.empty())
    for(const auto& spair : symbols)
    {
      auto pos = std::begin(data);
      while(pos = std::search(pos, std::end(data), std::begin(spair.first), std::end(spair.first)),
            pos != std::end(data))
      {
         pos = data.erase(pos, pos + spair.first.size());
         pos = data.insert(pos, std::begin(spair.second), std::end(spair.second));
         pos = std::next(pos, spair.second.size());
      }
    }
}

template<typename T>
void replace_patterns(std::string& data, const T& patterns)
{
  try
  {
    if(!data.empty())
      for(const auto& spair : patterns)
        data = std::regex_replace(data,
                                  std::regex(spair.first, std::regex_constants::extended),
                                  spair.second,
                                  std::regex_constants::format_sed);
  }
  catch(const std::regex_error& err)
  {
    std::cerr << err.what() << std::endl;
  }
}

void fix_format(std::string& format, std::size_t fixed_width)
{
  std::string::iterator pos;
  do
  {
    pos = std::find(std::begin(format), std::end(format), '\t');
    if(pos != std::end(format))
    {
      std::size_t tabpos = std::distance(std::begin(format), pos);
      format.erase(tabpos, 1);
      if(tabpos > fixed_width)
        tabpos -= format.rfind('\n', tabpos) + 1;
      format.insert(pos, fixed_width - tabpos, ' ');
    }
  } while (pos != std::end(format));
}

void no_rogue_angle_brackets(std::string& data)
{
  data = std::regex_replace(data, std::regex("<([idefghjklmnopqrtuwxyz[:space:]=<<][^m])", std::regex_constants::extended), "&lt;\\1", std::regex_constants::format_sed);
  data = std::regex_replace(data, std::regex("([^nrepb\"[:space:]/])>", std::regex_constants::extended), "\\1&gt;", std::regex_constants::format_sed);
/*
  std::size_t pos = std::string::npos;
  while(pos = data.find('<'), pos != std::string::npos)
    data.replace(pos, 1, "&lt;");
  while(pos = data.find('>'), pos != std::string::npos)
    data.replace(pos, 1, "&gt;");
    */
}

void replace_string(std::string& haystack,
                    const std::string& needle,
                    const std::string& replacement)
{

  for(auto pos = std::begin(haystack);
      pos = std::search(pos, std::end(haystack), std::begin(needle), std::end(needle)), pos != std::end(haystack);)
  {
    pos = haystack.erase(pos, std::next(pos, needle.size()));
    pos = haystack.insert(pos, std::begin(replacement), std::end(replacement));
    pos = std::next(pos, replacement.size());
  }
}

bool trim_endlines(std::string& val)
{
  auto is_endline = [](unsigned char c) -> bool { return c == '\n'; };
  if(!val.empty())
  {
    auto pos = std::find_if_not(std::begin(val), std::end(val), is_endline); // forward search
    val.erase(std::begin(val), pos);
  }
  if(!val.empty())
  {
    auto pos = std::find_if_not(std::rbegin(val), std::rend(val), is_endline); // backwards search
    val.erase(pos.base(), std::end(val));
  }
  return !val.empty();
}

void format_assembly(std::string& data)
{
  constexpr std::string_view noreformat = "#NOREFORMAT";
  if(!data.compare(0, noreformat.size(), noreformat))
  {
    data.erase(0, noreformat.size());
  }
  else
  {
    data = std::regex_replace(data, std::regex("H'([[:xdigit:]]+)", std::regex_constants::extended), "0x\\1", std::regex_constants::format_sed);
    data = std::regex_replace(data, std::regex("R([[:digit:]]{1,2})", std::regex_constants::extended), "r\\1", std::regex_constants::format_sed);
    replace_string(data, "After execution: ", "After execution:  ");

    enum captures : uint8_t
    {
      address = 0,
      label,
      mnemonic,
      directive,
      operand,
      comment,
      spacing,
    };

    typedef std::array<std::string, 7> asm_parts_t;

    asm_parts_t regexes =
    {
      "^[[:space:]]*([[:xdigit:]]{4,8})[[:space:]]*" ,
      "^[[:space:]]*([[:alpha:]_][[:alnum:]_]*:)",
      "^[[:space:]]*([[:alpha:]][[:alnum:]/\\.]+)",
      "^[[:space:]]*(\\.[[:alpha:]][[:alnum:]\\.]+)",
      "^[[:space:]]*([-+,#_@”“\\(\\)[:alnum:]]+)",
      "^[[:space:]]*;(.*)$",
      "^[[:space:]\\.]*$",
    };

    std::smatch matches;
    std::list<asm_parts_t> lines;

    auto pos = std::cbegin(data);
    while (pos != std::cend(data))
    {
      asm_parts_t line;
      auto line_pos = pos;
      auto  eol = std::find(pos, std::cend(data), '\n');
      if(line_pos != eol)
      {
        for(uint8_t part = address; part <= spacing; ++part)
        {
          std::regex_search(line_pos, eol, matches, std::regex(regexes[part], std::regex_constants::extended), std::regex_constants::format_sed);
          if(!matches.empty())
          {
            line[part] = matches[1];
            line_pos = std::next(line_pos, matches[0].str().size());
          }
        }
      }
      if(!line[comment].empty())
        line[comment].insert(0, "! ");
      if(std::end(line) != std::find_if(std::begin(line), std::end(line), [](const std::string& str) -> bool { return !str.empty(); })) // if line is not empty
        lines.push_back(line);
      pos = std::next(eol);
    }

    data.clear(); // wipe existing string

    std::array<std::size_t, 7> indent = { { 0 } };

    for(const auto& line : lines)
    {
      for(uint8_t part = address; part <= spacing; ++part)
        if(!line[part].empty())
          indent[part] = std::max(indent[part], line[part].size() + 1);
    }

    indent[mnemonic] = indent[directive] = std::max(indent[mnemonic], indent[directive]);


    for(const auto& line : lines)
    {
      for(uint8_t part = address; part <= spacing; ++part)
      {
        if(!line[part].empty())
          data.append(line[part])
              .append(indent[part] - line[part].size(), ' ');
        else if(part != directive)
            data.append(indent[part], ' ');
      }
      data.push_back('\n');
    }
  }
}

void format_exceptions(std::string& data)
{
  data.insert(0, "  <var>").append("</var>");
  for(auto pos = data.find('\n'); pos != std::string::npos; pos = data.find('\n', pos + "</var>\n  <var>"sv.size()))
    data.replace(pos, 1, "</var>\n  <var>");

}

void format_code(std::string& data)
{
  constexpr auto operand_type = [](char c) -> char { return c == '1' ? '0' : c; };
  std::string result;
  uint8_t count = 0, spaces = 0;
  char current = '\0';

  for(auto pos = std::begin(data); pos != std::end(data); pos = std::next(pos))
  {
    result.push_back(*pos);
    ++count;

    if(*pos == ' ' && std::next(pos) != std::end(data))
    {
      ++spaces;
      continue;
    }
    current = *pos;

    if(std::next(pos) == std::end(data) ||
       operand_type(current) != operand_type(*std::next(pos)))
    {
      auto total = std::count(std::begin(data), std::end(data), current); // find the total number of occurances in the whole string
      std::string tag = "<var title=\"";
      switch(operand_type(current))
      {
        case '0': tag.append("Opcode Identifier"); break;
        case '*': tag.append("Ignored"); break;
        case 'm': tag.append("Source Register"); break;
        case 'n': tag.append("Destination Register"); break;
        case 'i': tag.append("Unsigned Immediate Data"); break;
        case 's': tag.append("Signed Immediate Data"); break;
        case 'd': tag.append("Displacement"); break;

        case 'A': tag.append("A"); break;
        case 'D': tag.append("D"); break;
        case 'e': tag.append("Multiplier Source Register 1 (A1, X0, X1, Y0)"); break;
        case 'f': tag.append("Multiplier Source Register 2 (A1, X0, Y0, Y1)"); break;
        case 'g': tag.append("Multiplier Destination Register (A0, A1, M0, M1)"); break;
        case 'x': tag.append("ALU Source Register 1 (A0, A1, X0, X1)"); break;
        case 'y': tag.append("ALU Source Register 2 (M0, M1, Y0, Y1)"); break;
        case 'u': tag.append("ALU Destination Register (A0, A1, X0, Y0)"); break;
        case 'z': tag.append("ALU Destination Register (A0, A1, M0, M1, X0, X1, Y0, Y1)"); break;

        default: throw("unknown operand type: '"s + current + "'");
      }

      switch(current)
      {
        case 'm':
        case 'n':
          tag.append(" (R0 - R")
           .append(std::to_string((1 << total) - 1))
           .push_back(')');
          break;
        case 'i':
        case 's':
        case 'd':
          tag.append(" (")
           .append(std::to_string(total))
           .append(" bits)");
          break;
        case 'e':
        case 'f':
        case 'g':
        case 'x':
        case 'y':
        case 'u':
          if(total != 2)
            throw("Expected two bits for '"s + current + "' register type. Found: " + std::to_string(total));
          break;

        case 'z':
          if(total != 4)
            throw("Expected four bits for 'z' register type. Found: "s + std::to_string(total));
          break;
      }
      tag.append("\">");

      result.insert(std::prev(std::end(result), count - spaces), std::begin(tag), std::end(tag));
      result.append("</var>");
      count = 0;
      spaces = 0;
    }
  }

  data.assign(result);
}

void post_processing(std::list<insns>& insn_blocks)
{
  struct instruction_info_t
  {
    std::string mnemonic_regex;
    std::string name;
    std::string classification;
    std::list<environment_t> environments;
    std::list<citation_t> citations;
  };

  std::list<instruction_info_t> name_data =
  {
    { "STS", "_S_tore _System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 231 }, { SH7750_PROG_DOC, 373 }, { SH4A_DOC, 425 } } },
    { "STS", "_S_tore from FPU _System Register", "System Control Instruction", {}, { { SH7750_PROG_DOC, 375 }, { SH4A_DOC, 453 } } },
    { "FMOV", "_Floating-point _M_o_ve", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 271 }, { SH4A_DOC, 497 } } },
    { "FMOV", "_Floating-point _M_o_ve Extension", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 275 }, { SH4A_DOC, 501 } } },
    { "LDC", "_Loa_d to _Control Register", "System Control Instruction", {}, { { SH4A_DOC, 337 } } },
    { "LDC", "_Loa_d to _Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 165 }, { SH7750_PROG_DOC, 298 }, { SH4A_DOC, 449 } } },
    { "LDS", "_Loa_d to _System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 172 }, { SH7750_PROG_DOC, 304 }, { SH4A_DOC, 342 } } },
    { "LDS", "_Loa_d to FPU _System register", "System Control Instruction", {}, { { SH7750_PROG_DOC, 302 }, { SH4A_DOC, 450 } } },
    { "BRAF", "_B_r_anch _Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 133 }, { SH7750_PROG_DOC, 213 }, { SH4A_DOC, 307 } } },
    { "BRA", "_B_r_anch", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 131 }, { SH7750_PROG_DOC, 211 }, { SH4A_DOC, 305 } } },
    { "BSRF", "_Branch to _Sub_routine _Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 137 }, { SH7750_PROG_DOC, 216 }, { SH4A_DOC, 445 } } },
    { "BSR", "_Branch to _Sub_routine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 135 }, { SH7750_PROG_DOC, 214 }, { SH4A_DOC, 443 } } },
    { "JMP", "_Ju_m_p", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 162 }, { SH7750_PROG_DOC, 295 }, { SH4A_DOC, 336 } } },
    { "JSR", "_Jump to _Sub_routine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 163 }, { SH7750_PROG_DOC, 296 }, { SH4A_DOC, 447 } } },
    { "RTE", "_Re_turn from _Exception", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" }, { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 212 }, { SH7750_PROG_DOC, 349 }, { SH4A_DOC, 401 } } },
    { "RTS", "_Re_turn from _Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 214 }, { SH7750_PROG_DOC, 351 }, { SH4A_DOC, 403 } } },
    { "STC", "_S_tore _Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 228 }, { SH7750_PROG_DOC, 368 }, { SH4A_DOC, 420 }, { SH4A_DOC, 452 } } },
    { "SLEEP", "_S_l_e_e_p", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 227 }, { SH7750_PROG_DOC, 367 }, { SH4A_DOC, 419 } } },
    { "LDTLB", "_Loa_d PTEH/PTEL to _T_L_B", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH7750_PROG_DOC, 306 }, { SH4A_DOC, 344 } } },
// no environment instructions below
    { "MOV", "_M_o_ve Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 183 }, { SH7750_PROG_DOC, 315 }, { SH4A_DOC, 353 } } },
    { "MOV", "_M_o_ve Constant Value", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 189 }, { SH7750_PROG_DOC, 320 }, { SH4A_DOC, 359 } } },
    { "MOV", "_M_o_ve Global Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 191 }, { SH7750_PROG_DOC, 323 }, { SH4A_DOC, 362 } } },
    { "MOV", "_M_o_ve Structure Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 194 }, { SH7750_PROG_DOC, 326 }, { SH4A_DOC, 365 } } },
    { "ADDC", "_A_d_d with _Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 124 }, { SH4A_DOC, 296 } } },
    { "ADDV", "_A_d_d with `_V Flag` Overflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 125 }, { SH4A_DOC, 297 } } },
    { "ADD", "_A_d_d binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 123 }, { SH4A_DOC, 294 } } },
    { "AND", "Logical _A_n_d", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 126 }, { SH7750_PROG_DOC, 205 }, { SH4A_DOC, 299 } } },
    { "BF/S", "_Branch if _False with Delay _Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 129 }, { SH7750_PROG_DOC, 209 }, { SH4A_DOC, 303 } } },
    { "BF", "_Branch if _False", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 128 }, { SH7750_PROG_DOC, 207 }, { SH4A_DOC, 301 } } },
    { "BT/S", "_Branch if _True with Delay _Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 140 }, { SH7750_PROG_DOC, 220 }, { SH4A_DOC, 310 } } },
    { "BT", "_Branch if _True", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 139 }, { SH7750_PROG_DOC, 218 }, { SH4A_DOC, 308 } } },
    { "CLRMAC", "_C_lea_r _M_A_C Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 142 }, { SH7750_PROG_DOC, 222 }, { SH4A_DOC, 312 } } },
    { "CLRS", "_C_lea_r _S Bit", "System Control Instruction", {}, {  { SH7750_PROG_DOC, 223 }, { SH4A_DOC, 313 } } },
    { "CLRT", "_C_lea_r _T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 143 }, { SH7750_PROG_DOC, 224 }, { SH4A_DOC, 314 } } },
    { "CMP/EQ", "_Co_m_pare If _E_qual To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/GE", "_Co_m_pare If Signed _Greater Than or _Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/GT", "_Co_m_pare If Signed _Greater _Than", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/HI", "_Co_m_pare If _H_igher (Unsigned Greater Than)", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/HS", "_Co_m_pare If _Higher or _Same (Unsigned Greater Than or Equal To)", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/PL", "_Co_m_pare If _P_lus (Signed Greater Than Zero)", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/PZ", "_Co_m_pare If _Positive or _Zero (Signed Greater Than or Equal To Zero)", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "CMP/STR", "_Co_m_pare If _S_t_rings Equal", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH7750_PROG_DOC, 225 }, { SH4A_DOC, 315 } } },
    { "DIV0S", "_D_i_vide `Step _0` as _Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 148 }, { SH7750_PROG_DOC, 229 }, { SH4A_DOC, 319 } } },
    { "DIV0U", "_D_i_vide `Step _0` as _Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 149 }, { SH7750_PROG_DOC, 230 }, { SH4A_DOC, 320 } } },
    { "DIV1", "_D_i_vide _1 Step", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 150 }, { SH7750_PROG_DOC, 231 }, { SH4A_DOC, 321 } } },
    { "DMULS\\.L", "_Double-length _M_u_ltiply as _Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 155 }, { SH7750_PROG_DOC, 236 }, { SH4A_DOC, 326 } } },
    { "DMULU\\.L", "_Double-length _M_u_ltiply as _Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 157 }, { SH7750_PROG_DOC, 238 }, { SH4A_DOC, 328 } } },
    { "DT", "_Decrement and _Test", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 159 }, { SH7750_PROG_DOC, 240 }, { SH4A_DOC, 330 } } },
    { "EXTS", "_E_x_tend as _Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 160 }, { SH7750_PROG_DOC, 241 }, { SH4A_DOC, 331 } } },
    { "EXTU", "_E_x_tend as _Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 161 }, { SH7750_PROG_DOC, 243 }, { SH4A_DOC, 333 } } },
    { "ICBI", "_Instruction _Cache _Block _Invalidate", "Data Transfer Instruction", {}, { { SH4A_DOC, 335 } } },
    { "LDRE", "_Loa_d Effective Address to _R_E Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 168 } } },
    { "LDRS", "_Loa_d Effective Address to _R_S Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 170 } } },
    { "MAC\\.L", "_Multiply _and A_ccumulate", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 177 }, { SH7750_PROG_DOC, 308 }, { SH4A_DOC, 346 } } },
    { "MAC\\.W", "_Multiply _and A_ccumulate", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 180 }, { SH7750_PROG_DOC, 312 }, { SH4A_DOC, 350 } } },
    { "MOVA", "_M_o_ve Effective _Address", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 197 }, { SH7750_PROG_DOC, 329 }, { SH4A_DOC, 369 } } },
    { "MOVT", "_M_o_ve _T Bit", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 198 }, { SH7750_PROG_DOC, 331 }, { SH4A_DOC, 376 } } },
    { "MUL\\.L", "_M_u_ltiply", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 199 }, { SH7750_PROG_DOC, 332 }, { SH4A_DOC, 379 } } },
    { "MULS\\.W", "_M_u_ltiply as _Signed [_Word]", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 200 }, { SH7750_PROG_DOC, 333 }, { SH4A_DOC, 380 } } },
    { "MULU\\.W", "_M_u_ltiply as _Unsigned [_Word]", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 201 }, { SH7750_PROG_DOC, 334 }, { SH4A_DOC, 381 } } },
    { "MOVCA\\.L", "_M_o_ve with _Cache Block _Allocation", "Data Transfer Instruction", {}, { { SH7750_PROG_DOC, 330 }, { SH4A_DOC, 371 } } },
    { "MOVCO", "_M_o_ve _C_onditional", "Data Transfer Instruction", {}, { { SH4A_DOC, 372 } } },
    { "MOVLI", "_M_o_ve _L_inked", "Data Transfer Instruction", {}, { { SH4A_DOC, 374 } } },
    { "MOVUA", "_M_o_ve _Un_aligned", "Data Transfer Instruction", {}, { { SH4A_DOC, 377 } } },
    { "NEGC", "_N_e_gate with _Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 203 }, { SH7750_PROG_DOC, 336 }, { SH4A_DOC, 383 } } },
    { "NEG", "_N_e_gate", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 202 }, { SH7750_PROG_DOC, 335 }, { SH4A_DOC, 382 } } },
    { "NOP", "_No _O_peration", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 204 }, { SH7750_PROG_DOC, 337 }, { SH4A_DOC, 384 } } },
    { "NOT", "Logical _N_o_t Complement", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 205 }, { SH7750_PROG_DOC, 338 }, { SH4A_DOC, 385 } } },
    { "OR", "Logical _O_r", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 206 }, { SH7750_PROG_DOC, 342 }, { SH4A_DOC, 389 } } },
    { "PREFI", "_P_r_e_fetch _Instruction Cache Block", "Data Transfer Instruction", {}, { { SH4A_DOC, 394 } } },
    { "PREF", "_P_r_e_fetch Data to Cache", "Data Transfer Instruction", {}, { { SH7750_PROG_DOC, 344 }, { SH4A_DOC, 391 } } },
    { "ROTCL", "_R_o_tate with _Carry _Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 208 }, { SH7750_PROG_DOC, 345 }, { SH4A_DOC, 397 } } },
    { "ROTCR", "_R_o_tate with _Carry _Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 209 }, { SH7750_PROG_DOC, 346 }, { SH4A_DOC, 398 } } },
    { "ROTL", "_R_o_tate _Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 210 }, { SH7750_PROG_DOC, 347 }, { SH4A_DOC, 399 } } },
    { "ROTR", "_R_o_tate _Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 211 }, { SH7750_PROG_DOC, 348 }, { SH4A_DOC, 400 } } },
    { "SETRC", "_S_e_t Repeat Count to _R_C", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 216 } } },
    { "SETS", "_S_e_t _S Bit", "System Control Instruction", {}, { { SH7750_PROG_DOC, 353 }, { SH4A_DOC, 405 } } },
    { "SETT", "_S_e_t _T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 218 }, { SH7750_PROG_DOC, 354 }, { SH4A_DOC, 406 } } },
    { "SHAD", "_S_hift _Arithmetic _Dynamically", "Shift Instruction", {}, { { SH7750_PROG_DOC, 355 }, { SH4A_DOC, 407 } } },
    { "SHAL", "_S_hift _Arithmetic _Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 219 }, { SH7750_PROG_DOC, 357 }, { SH4A_DOC, 409 } } },
    { "SHAR", "_S_hift _Arithmetic _Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 220 }, { SH7750_PROG_DOC, 358 }, { SH4A_DOC, 410 } } },
    { "SHLD", "_S_hift _Logical _Dynamically", "Shift Instruction", {}, { { SH7750_PROG_DOC, 359 }, { SH4A_DOC, 411 } } },
    { "SHLL([281][6]?)", "_S_hift _Logical _Left $ Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 222 }, { SH7750_PROG_DOC, 362 }, { SH4A_DOC, 414 } } },
    { "SHLL", "_S_hift _Logical _Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 221 }, { SH7750_PROG_DOC, 361 }, { SH4A_DOC, 413 } } },
    { "SHLR([281][6]?)", "_S_hift _Logical _Right $ Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 225 }, { SH7750_PROG_DOC, 365 }, { SH4A_DOC, 417 } } },
    { "SHLR", "_S_hift _Logical _Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 224 }, { SH7750_PROG_DOC, 364 }, { SH4A_DOC, 416 } } },
    { "SUBC", "_S_u_btract with _Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 237 }, { SH7750_PROG_DOC, 378 }, { SH4A_DOC, 428 } } },
    { "SUBV", "_S_u_btract with `_V Flag` Underflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 238 }, { SH7750_PROG_DOC, 379 }, { SH4A_DOC, 429 } } },
    { "SUB", "_S_u_btract Binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 236 }, { SH7750_PROG_DOC, 377 }, { SH4A_DOC, 427 } } },
    { "SWAP", "_S_w_a_p Register Halves", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 239 }, { SH7750_PROG_DOC, 381 }, { SH4A_DOC, 431 } } },
    { "SYNCO", "_S_y_n_chronize Data _Operation", "Data Transfer Instruction", {}, { { SH4A_DOC, 433 } } },
    { "TAS", "_Test _and _Set", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 241 }, { SH7750_PROG_DOC, 383 }, { SH4A_DOC, 434 } } },
    { "TRAPA", "_T_r_a_p _Always", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 242 }, { SH7750_PROG_DOC, 385 }, { SH4A_DOC, 436 } } },
    { "TST", "Logical _Te_s_t", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 243 }, { SH7750_PROG_DOC, 386 }, { SH4A_DOC, 438 } } },
    { "XOR", "Logical E_xclusive _O_r", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 245 }, { SH7750_PROG_DOC, 388 }, { SH4A_DOC, 440 } } },
    { "XTRCT", "E_x_t_ra_c_t", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 247 }, { SH7750_PROG_DOC, 390 }, { SH4A_DOC, 442 } } },
    { "MOVS", "_M_o_ve _Single Data between Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 255 } } },
    { "MOVX", "_M_o_ve between _X Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 257 } } },
    { "MOVY", "_M_o_ve between _Y Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 258 } } },
    { "NOPX", "_No Access _O_peration for _X Memory", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 260 } } },
    { "PABS", "_Parallel _A_b_solute", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 278 } } },
    { "DC[TF] PADD", "_Parallel _A_d_dition with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 282 } } },
    { "PADDC", "_Parallel _A_d_dition with _Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 291 } } },
    { "PADD PMULS", "_Parallel _A_d_dition & _M_u_ltiply _Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 286 } } },
    { "DC[TF] PAND", "_Parallel Logical _A_n_d", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 294 } } },
    { "DC[TF] PCLR", "_Parallel _C_lea_r", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 298 } } },
    { "PCMP", "_Parallel _Co_m_pare Two Data", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 301 } } },
    { "DC[TF] PCOPY", "_Parallel _C_o_p_y with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 303 } } },
    { "DC[TF] PDEC", "_Parallel _D_e_crement by 1", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 307 } } },
    { "DC[TF] PDMSB", "_Parallel _Detect _Most _Significant _Bit with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 312 } } },
    { "DC[TF] PINC", "_Parallel _I_n_crement by 1 with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 317 } } },
    { "DC[TF] PLDS", "_Parallel _Loa_d _System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 322 } } },
    { "PMULS", "_Parallel _M_u_ltiply _Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 326 } } },
    { "DC[TF] PNEG", "_Parallel _N_e_gate", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 329 } } },
    { "DC[TF] POR", "_Parallel Logical _O_r", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 334 } } },
    { "PRND", "_Parallel _Rou_n_ding", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 338 } } },
    { "DC[TF] PSHA", "_Parallel _S_hift _Arithmetically with Condition", "DSP Arithmetic Shift Instruction", {}, { { SH1_2_DSP_DOC, 342 } } },
    { "DC[TF] PSHL", "_Parallel _S_hift _Logically with Condition", "DSP Logical Shift Instruction", {}, { { SH1_2_DSP_DOC, 350 } } },
    { "DC[TF] PSTS", "_Parallel _S_tore _System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 357 } } },
    { "DC[TF] PSUB", "_Parallel _S_u_btract with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 362 } } },
    { "PSUB PMULS", "_Parallel _S_u_btraction & _Parallel _M_u_ltiply _Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 367 } } },
    { "PSUBC", "_Parallel _S_u_btraction with _Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 372 } } },
    { "DC[TF] PXOR", "_Parallel Logical E_xclusive _O_r", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 375 } } },
    { "OCBI", "_Operand _Cache _Block _Invalidate", "Data Transfer Instruction", {}, { { SH7750_PROG_DOC, 339 }, { SH4A_DOC, 386 } } },
    { "OCBP", "_Operand _Cache _Block _Purge", "Data Transfer Instruction", {}, { { SH7750_PROG_DOC, 340 }, { SH4A_DOC, 387 } } },
    { "OCBWB", "_Operand _Cache _Block _Write _Back", "Data Transfer Instruction", {}, { { SH7750_PROG_DOC, 341 }, { SH4A_DOC, 388 } } },
    { "FABS", "_Floating-point _A_b_solute Value", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 244 }, { SH4A_DOC, 467 } } },
    { "FADD", "_Floating-point _A_d_d", "Floating-Point Instruction", {}, {  { SH7750_PROG_DOC, 245 },{ SH4A_DOC, 468 } } },
    { "FCMP", "_Floating-point _Co_m_pare", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 247 }, { SH4A_DOC, 471 } } },
    { "FCNVDS", "_Floating-point _Co_n_vert _Double to _Single Precision", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 250 }, { SH4A_DOC, 475 } } },
    { "FCNVSD", "_Floating-point _Co_n_vert _Single to _Double Precision", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 252 }, { SH4A_DOC, 478 } } },
    { "FDIV", "_Floating-point _D_i_vide", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 254 }, { SH4A_DOC, 480 } } },
    { "FIPR", "_Floating-point _Inner _P_roduct", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 258 }, { SH4A_DOC, 484 } } },
    { "FLDI0", "_Floating-point _Loa_d _Immediate _0.0", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 260 }, { SH4A_DOC, 486 } } },
    { "FLDI1", "_Floating-point _Loa_d _Immediate _1.0", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 261 }, { SH4A_DOC, 487 } } },
    { "FLDS", "_Floating-point Load to System register", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 262 }, { SH4A_DOC, 488 } } },
    { "FLOAT", "_F_l_o_a_ting-point Convert from Integer", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 263 }, { SH4A_DOC, 489 } } },
    { "FMAC", "_Floating-point _Multiply _and A_ccumulate", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 265 }, { SH4A_DOC, 491 } } },
    { "FMUL", "_Floating-point _M_u_ltiply", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 278 }, { SH4A_DOC, 504 } } },
    { "FNEG", "_Floating-point _N_e_gate Value", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 280 }, { SH4A_DOC, 507 } } },
    { "FPCHG", "_Floating-point _Pr-bit _C_han_ge", "Floating-Point Instruction", {}, { { SH4A_DOC, 508 } } },
    { "FRCHG", "_Floating-point F_R-bit _C_han_ge", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 281 }, { SH4A_DOC, 509 } } },
    { "FSCA", "_Floating-point _Sine And _Cosine _Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 510 } } },
    { "FSCHG", "_Floating-point _Sz-bit _C_han_ge", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 282 }, { SH4A_DOC, 512 } } },
    { "FSQRT", "_Floating-point _S_quare _Roo_t", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 283 }, { SH4A_DOC, 513 } } },
    { "FSRRA", "Floating-point _Squa_re _Reciprocal _Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 516 } } },
    { "FSTS", "_Floating-point _S_tore _System Register", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 286 }, { SH4A_DOC, 518 } } },
    { "FSUB", "_Floating-point _S_u_btract", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 287 }, { SH4A_DOC, 519 } } },
    { "FTRC", "_Floating-point _T_runcate and _Convert to integer", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 289 }, { SH4A_DOC, 522 } } },
    { "FTRV", "_Floating-point _T_ransform _Vector", "Floating-Point Instruction", {}, { { SH7750_PROG_DOC, 292 }, { SH4A_DOC, 525 } } },
  };

  for(instruction_info_t& info : name_data)
  {
    std::transform(std::begin(info.mnemonic_regex), std::end(info.mnemonic_regex), std::begin(info.mnemonic_regex),
                   [](char c){ return std::tolower(c); }); // convert to lowercase
    info.mnemonic_regex.insert(0, 1, '^'); // add regex string start
    info.mnemonic_regex.append("[\\S]*"); // add "not whitespace" matching to end
  }


  for(insns& block : insn_blocks)
  {
    for(insn& instruction : block)
    {
      auto& fmt = instruction.data<format>();
      auto& n = instruction.data<name>();
      auto& e = instruction.data<environments>();
      for(const instruction_info_t& info : name_data)
      {
        try
        {
          std::smatch match;
          if(std::regex_search(fmt, match, std::regex(info.mnemonic_regex, std::regex_constants::extended)) &&
             (n.empty() || n == info.name) &&
             e == environments { info.environments })
          {
            if(n.empty())
            {
              n = { info.name };
              if(match.size() >= 1)
              {
                std::string replacement;
                for(char c : match[1].str())
                {
                  replacement.push_back('_');
                  replacement.push_back(c);
                }
                replace_string(n, "$"s, replacement);
              }
            }

            instruction.data<mnemonic>() = { match.str() };
            instruction.data<citations>() = { info.citations };
            instruction.data<classification>() = { info.classification };
            break;
          }
        }
        catch(const std::regex_error& err)
        {
          std::cerr << err.what() << std::endl;
        }
      }
    }
  }


  auto fix_images = [](std::string& data, const std::string& title)
    { replace_string(data, "<img src=", "<img alt=\""s + title + "\" class=\"image_filter\" src="s); };

  for(insns& block : insn_blocks)
  {
    for(insn& instruction : block)
    {
      std::string clean_name = instruction.data<name>();
      std::remove_if(std::begin(clean_name), std::end(clean_name), [](char c) -> bool { return c == '_'; });

      fix_format(instruction.data<format>(), 10); // replace tabs with spaces
      fix_images(instruction.data<note>(), clean_name);
      fix_images(instruction.data<description>(), clean_name);

      replace_symbols(instruction.data<abstract>(), typeable_symbols);
      replace_patterns(instruction.data<abstract>(), typeable_patterns);
      replace_symbols(instruction.data<brief>(), unicode_symbols);
      replace_symbols(instruction.data<flags>(), typeable_symbols);

      replace_symbols(instruction.data<description>(), long_accronyms);
      replace_patterns(instruction.data<description>(), short_accronyms);

      replace_symbols(instruction.data<note>(), long_accronyms);
      replace_patterns(instruction.data<note>(), short_accronyms);

      format_code(instruction.data<code>());
      format_assembly(instruction.data<example>());
      format_exceptions(instruction.data<exceptions>());

      no_rogue_angle_brackets(instruction.data<brief>());
      no_rogue_angle_brackets(instruction.data<operation>());

      trim_endlines(instruction.data<brief>());
      trim_endlines(instruction.data<description>());
      trim_endlines(instruction.data<note>());
      trim_endlines(instruction.data<operation>());
      trim_endlines(instruction.data<example>());
      trim_endlines(instruction.data<exceptions>());

      instruction
          .data<name>()
          .assign(std::regex_replace(instruction.data<name>(),
                                     std::regex("_([[:alnum:]])", std::regex_constants::extended),
                                     "<em>\\1</em>", std::regex_constants::format_sed));

    }
  }
}
