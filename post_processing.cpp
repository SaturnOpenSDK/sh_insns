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

static constexpr const std::array<std::pair<std::string_view, std::string_view>, 20> long_accronyms =
{
  {
    { "ALU", "<abbr title=\"Arithmetic Logic Unit\">ALU</abbr>" },
    { "ASID", "<abbr title=\"Address Space Identifier\">ASID</abbr>" },
    { "CPU", "<abbr title=\"Central Processing Unit\">CPU</abbr>" },
    { "FPU", "<abbr title=\"Floating Point Unit\">FPU</abbr>" },
    { "UTLB", "<abbr title=\"Unified Translation Lookaside Buffer\">UTLB</abbr>" },
    { "ITLB", "<abbr title=\"Instruction Translation Lookaside Buffer\">ITLB</abbr>" },
    { "LRU", "<abbr title=\"Least Recently Used\">LRU</abbr>" },
    { "LSB", "<abbr title=\"Least Significant Bit\">LSB</abbr>" },
    { "MMU", "<abbr title=\"Memory Management Unit\">MMU</abbr>" },
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
  }
};

static constexpr std::array<std::pair<const char*, const char*>, 16> short_accronyms =
{
  {

    { "([^\\<\\>[:alnum:]\\S])MAC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Multiply and ACcumulate\">MAC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])TLB([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Translation Lookaside Buffer\">TLB</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])PC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Program Counter\">PC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I0([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit\">I0</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I1([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit\">I1</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I2([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit\">I2</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])I3([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Interrupt mask bit\">I3</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])SR([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Status Register\">SR</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])CS([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Condition Select bit flags\">CS</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])DC([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"DSP Condition bit flag\">DC</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])GT([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Signed Greater Than bit flag\">GT</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])Z([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Zero value bit flag\">Z</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])N([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Negative value flag\">N</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])V([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"oVerflow bit flag\">V</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])S([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Sum bit flag\">S</abbr>\\2" },
    { "([^\\<\\>[:alnum:]\\S])T([^\\<\\>[:alnum:]\\S])", "\\1<abbr title=\"Test bit flag\">T</abbr>\\2" },
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

void no_angle_brackets(std::string& data)
{
  std::size_t pos = std::string::npos;
  while(pos = data.find('<'), pos != std::string::npos)
    data.replace(pos, 1, "&lt;");
  while(pos = data.find('>'), pos != std::string::npos)
    data.replace(pos, 1, "&gt;");
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
      "^[[:space:]]*([-,#_@”“\\(\\)[:alnum:]]+)",
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

  no_angle_brackets(data); // final fix
}

std::list<insns> post_processing(std::list<insns>&& insn_blocks)
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
    { "STS", "Store System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 231 }, { SH4A_DOC, 425 } } },
    { "STS", "Store from FPU System Register", "System Control Instruction", {}, { { SH4A_DOC, 453 } } },
    { "FMOV", "Floating-point Move", "Floating-Point Instruction", {}, { { SH4A_DOC, 497 } } },
    { "FMOV", "Floating-point Move Extension", "Floating-Point Instruction", {}, { { SH4A_DOC, 501 } } },
    { "LDC", "Load to Control Register", "System Control Instruction", {}, { { SH4A_DOC, 337 } } },
    { "LDC", "Load to Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 165 }, { SH4A_DOC, 449 } } },
    { "LDS", "Load to System Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" } }, { { SH1_2_DSP_DOC, 172 }, { SH4A_DOC, 342 } } },
    { "LDS", "Load to FPU System register", "System Control Instruction", {}, { { SH4A_DOC, 450 } } },
    { "BRAF", "Branch Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 133 }, { SH4A_DOC, 307 } } },
    { "BRA", "Branch", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 131 }, { SH4A_DOC, 305 } } },
    { "BSRF", "Branch to Subroutine Far", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 137 }, { SH4A_DOC, 445 } } },
    { "BSR", "Branch to Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 135 }, { SH4A_DOC, 443 } } },
    { "JMP", "Jump", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 162 }, { SH4A_DOC, 336 } } },
    { "JSR", "Jump to Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 163 }, { SH4A_DOC, 447 } } },
    { "RTE", "Return from Exception", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" }, { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 212 }, { SH4A_DOC, 401 } } },
    { "RTS", "Return from Subroutine", "Branch Instruction", { { SH_ALL, "Delayed Branch" } }, { { SH1_2_DSP_DOC, 214 }, { SH4A_DOC, 403 } } },
    { "STC", "Store Control Register", "System Control Instruction", { { SH1 | SH2 | SH2A | SH_DSP, "Interrupt Disabled" }, { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 228 }, { SH4A_DOC, 420 }, { SH4A_DOC, 452 } } },
    { "SLEEP", "Sleep", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH1_2_DSP_DOC, 227 }, { SH4A_DOC, 419 } } },
    { "LDTLB", "Load PTEH/PTEL to TLB", "System Control Instruction", { { SH4A, "Privileged" } }, { { SH4A_DOC, 344 } } },
// no environment instructions below
    { "MOV", "Move Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 183 }, { SH4A_DOC, 353 } } },
    { "MOV", "Move Constant Value", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 189 }, { SH4A_DOC, 359 } } },
    { "MOV", "Move Global Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 191 }, { SH4A_DOC, 362 } } },
    { "MOV", "Move Structure Data", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 194 }, { SH4A_DOC, 365 } } },
    { "ADDC", "Add with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 124 }, { SH4A_DOC, 296 } } },
    { "ADDV", "ADD with `V Flag` Overflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 125 }, { SH4A_DOC, 297 } } },
    { "ADD", "Add binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 123 }, { SH4A_DOC, 294 } } },
    { "AND", "AND Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 126 }, { SH4A_DOC, 299 } } },
    { "BF/S", "Branch if False with Delay Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 129 }, { SH4A_DOC, 303 } } },
    { "BF", "Branch if False", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 128 }, { SH4A_DOC, 301 } } },
    { "BT/S", "Branch if True with Delay Slot", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 140 }, { SH4A_DOC, 310 } } },
    { "BT", "Branch if True", "Branch Instruction", {}, { { SH1_2_DSP_DOC, 139 }, { SH4A_DOC, 308 } } },
    { "CLRMAC", "Clear MAC Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 142 }, { SH4A_DOC, 312 } } },
    { "CLRS", "Clear S Bit", "System Control Instruction", {}, { { SH4A_DOC, 313 } } },
    { "CLRT", "Clear T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 143 }, { SH4A_DOC, 314 } } },
    { "CMP/EQ", "Compare If Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/GE", "Compare If Signed Greater Than or Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/GT", "Compare If Signed Greater Than", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/HI", "Compare If Unsigned Greater Than", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/HS", "Compare If Unsigned Greater Than or Equal To", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/PL", "Compare If Signed Greater Than Zero", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/PZ", "Compare If Signed Greater Than or Equal To Zero", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "CMP/STR", "Compare If Strings Equal", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 144 }, { SH4A_DOC, 315 } } },
    { "DIV0S", "Divide `Step 0` as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 148 }, { SH4A_DOC, 319 } } },
    { "DIV0U", "Divide `Step 0` as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 149 }, { SH4A_DOC, 320 } } },
    { "DIV1", "Divide 1 Step", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 150 }, { SH4A_DOC, 321 } } },
    { "DMULS\\.L", "Double-length Multiply as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 155 }, { SH4A_DOC, 326 } } },
    { "DMULU\\.L", "Double-length Multiply as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 157 }, { SH4A_DOC, 328 } } },
    { "DT", "Decrement and Test", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 159 }, { SH4A_DOC, 330 } } },
    { "EXTS", "Extend as Signed", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 160 }, { SH4A_DOC, 331 } } },
    { "EXTU", "Extend as Unsigned", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 161 }, { SH4A_DOC, 333 } } },
    { "ICBI", "Instruction Cache Block Invalidate", "Data Transfer Instruction", {}, { { SH4A_DOC, 335 } } },
    { "LDRE", "Load Effective Address to RE Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 168 } } },
    { "LDRS", "Load Effective Address to RS Register", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 170 } } },
    { "MAC\\.L", "Multiply and Accumulate Long", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 177 }, { SH4A_DOC, 346 } } },
    { "MAC\\.W", "Multiply and Accumulate Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 180 }, { SH4A_DOC, 350 } } },
    { "MOVA", "Move Effective Address", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 197 }, { SH4A_DOC, 369 } } },
    { "MOVT", "Move T Bit", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 198 }, { SH4A_DOC, 376 } } },
    { "MUL\\.L", "Multiply Long", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 199 }, { SH4A_DOC, 379 } } },
    { "MULS\\.W", "Multiply as Signed Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 200 }, { SH4A_DOC, 380 } } },
    { "MULU\\.W", "Multiply as Unsigned Word", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 201 }, { SH4A_DOC, 381 } } },
    { "MOVCA\\.L", "Move with Cache Block Allocation", "Data Transfer Instruction", {}, { { SH4A_DOC, 371 } } },
    { "MOVCO", "Move Conditional", "Data Transfer Instruction", {}, { { SH4A_DOC, 372 } } },
    { "MOVLI", "Move Linked", "Data Transfer Instruction", {}, { { SH4A_DOC, 374 } } },
    { "MOVUA", "Move Unaligned", "Data Transfer Instruction", {}, { { SH4A_DOC, 377 } } },
    { "NEGC", "Negate with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 203 }, { SH4A_DOC, 383 } } },
    { "NEG", "Negate", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 202 }, { SH4A_DOC, 382 } } },
    { "NOP", "No Operation", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 204 }, { SH4A_DOC, 384 } } },
    { "NOT", "NOT-Logical Complement", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 205 }, { SH4A_DOC, 385 } } },
    { "OR", "OR Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 206 }, { SH4A_DOC, 389 } } },
    { "PREFI", "Prefetch Instruction Cache Block", "Data Transfer Instruction", {}, { { SH4A_DOC, 394 } } },
    { "PREF", "Prefetch Data to Cache", "Data Transfer Instruction", {}, { { SH4A_DOC, 391 } } },
    { "ROTCL", "Rotate with Carry Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 208 }, { SH4A_DOC, 397 } } },
    { "ROTCR", "Rotate with Carry Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 209 }, { SH4A_DOC, 398 } } },
    { "ROTL", "Rotate Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 210 }, { SH4A_DOC, 399 } } },
    { "ROTR", "Rotate Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 211 }, { SH4A_DOC, 400 } } },
    { "SETRC", "Set Repeat Count to RC", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 216 } } },
    { "SETS", "Set S Bit", "System Control Instruction", {}, { { SH4A_DOC, 405 } } },
    { "SETT", "Set T Bit", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 218 }, { SH4A_DOC, 406 } } },
    { "SHAD", "Shift Arithmetic Dynamically", "Shift Instruction", {}, { { SH4A_DOC, 407 } } },
    { "SHAL", "Shift Arithmetic Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 219 }, { SH4A_DOC, 409 } } },
    { "SHAR", "Shift Arithmetic Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 220 }, { SH4A_DOC, 410 } } },
    { "SHLD", "Shift Logical Dynamically", "Shift Instruction", {}, { { SH4A_DOC, 411 } } },
    { "SHLL([281][6]?)", "Shift Logical Left \\1 Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 222 }, { SH4A_DOC, 414 } } },
    { "SHLL", "Shift Logical Left", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 221 }, { SH4A_DOC, 413 } } },
    { "SHLR([281][6]?)", "Shift Logical Right \\1 Bits", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 225 }, { SH4A_DOC, 417 } } },
    { "SHLR", "Shift Logical Right", "Shift Instruction", {}, { { SH1_2_DSP_DOC, 224 }, { SH4A_DOC, 416 } } },
    { "SUBC", "Subtract with Carry", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 237 }, { SH4A_DOC, 428 } } },
    { "SUBV", "Subtract with `V Flag` Underflow Check", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 238 }, { SH4A_DOC, 429 } } },
    { "SUB", "Subtract Binary", "Arithmetic Instruction", {}, { { SH1_2_DSP_DOC, 236 }, { SH4A_DOC, 427 } } },
    { "SWAP", "Swap Register Halves", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 239 }, { SH4A_DOC, 431 } } },
    { "SYNCO", "Synchronize Data Operation", "Data Transfer Instruction", {}, { { SH4A_DOC, 433 } } },
    { "TAS", "Test and Set", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 241 }, { SH4A_DOC, 434 } } },
    { "TRAPA", "Trap Always", "System Control Instruction", {}, { { SH1_2_DSP_DOC, 242 }, { SH4A_DOC, 436 } } },
    { "TST", "Test Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 243 }, { SH4A_DOC, 438 } } },
    { "XOR", "Exclusive OR Logical", "Logical Instruction", {}, { { SH1_2_DSP_DOC, 245 }, { SH4A_DOC, 440 } } },
    { "XTRCT", "Extract", "Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 247 }, { SH4A_DOC, 442 } } },
    { "MOVS", "Move Single Data between Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 255 } } },
    { "MOVX", "Move between X Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 257 } } },
    { "MOVY", "Move between Y Memory and DSP Register", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 258 } } },
    { "NOPX", "No Access Operation for X Memory", "DSP Data Transfer Instruction", {}, { { SH1_2_DSP_DOC, 260 } } },
    { "PABS", "Absolute", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 278 } } },
    { "DC[TF] PADD", "Addition with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 282 } } },
    { "PADDC", "Addition with Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 291 } } },
    { "PADD PMULS", "Addition & Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 286 } } },
    { "DC[TF] PAND", "Logical AND", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 294 } } },
    { "DC[TF] PCLR", "Clear", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 298 } } },
    { "PCMP", "Compare Two Data", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 301 } } },
    { "DC[TF] PCOPY", "Copy with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 303 } } },
    { "DC[TF] PDEC", "Decrement by 1", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 307 } } },
    { "DC[TF] PDMSB", "Detect MSB with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 312 } } },
    { "DC[TF] PINC", "Increment by 1 with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 317 } } },
    { "DC[TF] PLDS", "Load System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 322 } } },
    { "PMULS", "Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 326 } } },
    { "DC[TF] PNEG", "Negate", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 329 } } },
    { "DC[TF] POR", "Logical OR", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 334 } } },
    { "PRND", "Rounding", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 338 } } },
    { "DC[TF] PSHA", "Shift Arithmetically with Condition", "DSP Arithmetic Shift Instruction", {}, { { SH1_2_DSP_DOC, 342 } } },
    { "DC[TF] PSHL", "Shift Logically with Condition", "DSP Logical Shift Instruction", {}, { { SH1_2_DSP_DOC, 350 } } },
    { "DC[TF] PSTS", "Store System Register", "DSP System Control Instruction", {}, { { SH1_2_DSP_DOC, 357 } } },
    { "DC[TF] PSUB", "Subtract with Condition", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 362 } } },
    { "PSUB PMULS", "Subtraction & Multiply Signed by Signed", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 367 } } },
    { "PSUBC", "Subtraction with Carry", "DSP Arithmetic Operation Instruction", {}, { { SH1_2_DSP_DOC, 372 } } },
    { "DC[TF] PXOR", "Logical Exclusive OR", "DSP Logical Operation Instruction", {}, { { SH1_2_DSP_DOC, 375 } } },
    { "OCBI", "Operand Cache Block Invalidate", "Data Transfer Instruction", {}, { { SH4A_DOC, 386 } } },
    { "OCBP", "Operand Cache Block Purge", "Data Transfer Instruction", {}, { { SH4A_DOC, 387 } } },
    { "OCBWB", "Operand Cache Block Write Back", "Data Transfer Instruction", {}, { { SH4A_DOC, 388 } } },
    { "FABS", "Floating-point Absolute Value", "Floating-Point Instruction", {}, { { SH4A_DOC, 467 } } },
    { "FADD", "Floating-point ADD", "Floating-Point Instruction", {}, { { SH4A_DOC, 468 } } },
    { "FCMP", "Floating-point Compare", "Floating-Point Instruction", {}, { { SH4A_DOC, 471 } } },
    { "FCNVDS", "Floating-point Convert Double to Single Precision", "Floating-Point Instruction", {}, { { SH4A_DOC, 475 } } },
    { "FCNVSD", "Floating-point Convert Single to Double Precision", "Floating-Point Instruction", {}, { { SH4A_DOC, 478 } } },
    { "FDIV", "Floating-point Divide", "Floating-Point Instruction", {}, { { SH4A_DOC, 480 } } },
    { "FIPR", "Floating-point Inner Product", "Floating-Point Instruction", {}, { { SH4A_DOC, 484 } } },
    { "FLDI0", "Floating-point Load Immediate 0.0", "Floating-Point Instruction", {}, { { SH4A_DOC, 486 } } },
    { "FLDI1", "Floating-point Load Immediate 1.0", "Floating-Point Instruction", {}, { { SH4A_DOC, 487 } } },
    { "FLDS", "Floating-point Load to System register", "Floating-Point Instruction", {}, { { SH4A_DOC, 488 } } },
    { "FLOAT", "Floating-point Convert from Integer", "Floating-Point Instruction", {}, { { SH4A_DOC, 489 } } },
    { "FMAC", "Floating-point Multiply and Accumulate", "Floating-Point Instruction", {}, { { SH4A_DOC, 491 } } },
    { "FMUL", "Floating-point Multiply", "Floating-Point Instruction", {}, { { SH4A_DOC, 504 } } },
    { "FNEG", "Floating-point Negate Value", "Floating-Point Instruction", {}, { { SH4A_DOC, 507 } } },
    { "FPCHG", "Pr-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 508 } } },
    { "FRCHG", "FR-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 509 } } },
    { "FSCA", "Floating Point Sine And Cosine Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 510 } } },
    { "FSCHG", "Sz-bit Change", "Floating-Point Instruction", {}, { { SH4A_DOC, 512 } } },
    { "FSQRT", "Floating-point Square Root", "Floating-Point Instruction", {}, { { SH4A_DOC, 513 } } },
    { "FSRRA", "Floating Point Square Reciprocal Approximate", "Floating-Point Instruction", {}, { { SH4A_DOC, 516 } } },
    { "FSTS", "Floating-point Store System Register", "Floating-Point Instruction", {}, { { SH4A_DOC, 518 } } },
    { "FSUB", "Floating-point Subtract", "Floating-Point Instruction", {}, { { SH4A_DOC, 519 } } },
    { "FTRC", "Floating-point Truncate and Convert to integer", "Floating-Point Instruction", {}, { { SH4A_DOC, 522 } } },
    { "FTRV", "Floating-point Transform Vector", "Floating-Point Instruction", {}, { { SH4A_DOC, 525 } } },
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
              n = { info.name };
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
    { replace_string(data, "<img src=", "<img alt=\""s + title + "\" class=\"image_filter\""s); };

  for(insns& block : insn_blocks)
  {
    for(insn& instruction : block)
    {
      fix_format(instruction.data<format>(), 10); // replace tabs with spaces
      fix_images(instruction.data<note>(), instruction.data<name>());
      fix_images(instruction.data<description>(), instruction.data<name>());

      replace_symbols(instruction.data<abstract>(), typeable_symbols);
      replace_patterns(instruction.data<abstract>(), typeable_patterns);
      replace_symbols(instruction.data<brief>(), unicode_symbols);
      replace_symbols(instruction.data<flags>(), typeable_symbols);

      replace_symbols(instruction.data<description>(), long_accronyms);
      replace_patterns(instruction.data<description>(), short_accronyms);

      replace_symbols(instruction.data<note>(), long_accronyms);
      replace_patterns(instruction.data<note>(), short_accronyms);
      format_assembly(instruction.data<example>());

      no_angle_brackets(instruction.data<operation>());


      /*
      replace_symbols(instruction.data<name>());
      replace_symbols(instruction.data<restriction>());
      replace_symbols(instruction.data<classification>());
      replace_symbols(instruction.data<mnemonic>());
      replace_symbols(instruction.data<mnemonic_origin>());
      replace_symbols(instruction.data<code>());
      replace_symbols(instruction.data<flags>());

      replace_symbols(instruction.data<note>());
      replace_symbols(instruction.data<operation>());
      replace_symbols(instruction.data<example>());
      replace_symbols(instruction.data<exceptions>());
      */
    }
  }

  return insn_blocks;
}
