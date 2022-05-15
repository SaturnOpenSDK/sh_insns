/*
sh_insns - Renesas SH Instruction Set Summary

Copyright (C) 2013-2015 Oleg Endo

This is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file LICENSE.  If not see
<httdiv://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <iomanip>
#include <string_view>
#include <cstring>
#include <regex>
#include <algorithm>

#include "build_instructions.h"

using namespace std::literals;

// ----------------------------------------------------------------------------

static isa_property isa_name = isa_property
(
  SH1, "SH1",
  SH2, "SH2",
  SH2E, "SH2E",
  SH2A, "SH2A",
  SH3, "SH3",
  SH3E, "SH3E",
  SH4, "SH4",
  SH4A, "SH4A",
  SH_DSP, "DSP"
);

// ----------------------------------------------------------------------------

std::string fix_id(std::string id)
{
  std::replace_if(id.begin(), id.end(),[](unsigned char c) -> bool { return c == ' '; }, '_');
  return id;
}

std::string fix_html(std::string html)
{
  std::size_t pos = std::string::npos;
  while(pos = html.find('<'), pos != std::string::npos)
    html.replace(pos, 1, "&lt;");
  while(pos = html.find('>'), pos != std::string::npos)
    html.replace(pos, 1, "&gt;");
  return html;
}

void print_section_title(std::string_view title)
{
  std::cout << "<span title=\"section\">" << title << "</span>" << std::endl
            << "<br />" << std::endl;
}

bool skip_endlines(std::string& val)
{
  auto is_endline = [](unsigned char c) -> bool { return c == '\n'; };
  auto pos = std::find_if_not(val.begin(), val.end(), is_endline); // move past initial line break
  bool end = pos != val.end();
  if(end)
    val.erase(val.begin(), pos);
  return end;
}

// ----------------------------------------------------------------------------

void print_list_section (std::string_view title, std::string val)
{
  if(skip_endlines(val))
  {
    val.erase(val.find_last_of('\n')); // remove trailing newline
    val.insert(0, "  <var>").append("</var>");
    for(auto pos = val.find('\n'); pos != std::string::npos; pos = val.find('\n', pos + sizeof("</var>\n  <var>")))
      val.replace(pos, 1, "</var>\n  <var>");
    print_section_title(title);
    std::cout << "<span title=\"list\">" << std::endl
              << val << std::endl
              << "</span>" << std::endl;
  }
}

void print_code_section (std::string_view title, std::string val)
{
  if(skip_endlines(val))
  {
    print_section_title(title);
    std::cout << "<span title=\"code\">" << fix_html(val) << "</span>"<< std::endl;
  }
}

void print_note_section (std::string title, std::string val)
{
  if(skip_endlines(val))
  {
    print_section_title(title);
    std::cout << "<br />";
    auto target = "<img src="sv;
    if (auto it = std::search(val.begin(), val.end(), std::default_searcher(target.begin(), target.end()));
        it != val.end())
      std::cout << std::string(val.begin(), it) << "<img alt=\"" << title << "\" class=\"image_filter\" src=" << std::string(it + target.size(), val.end());
    else
      std::cout << val;
    std::cout << "<br /><br />" << std::endl;
  }
}

void print_assembly_section (std::string_view title, std::string val)
{
  static_assert ("#NOREFORMAT"sv.size() == 11, "oh snap");

  if(skip_endlines(val))
  {
    print_section_title(title);
    auto noreformat = "#NOREFORMAT"sv;

    if(!val.compare(0, noreformat.size(), noreformat))
      std::cout << "<span title=\"assembly\">"
                << fix_html(val.substr(noreformat.size()))
                << "</span>" << std::endl;
    else
    {

      auto is_endline = [](unsigned char c) -> bool { return c == '\n'; };
      auto to_lowercase = [](unsigned char c) -> unsigned char { return std::tolower(c); };

      std::cout << std::setfill(' ') << std::left;
      std::cout << "<span title=\"assembly\">";
      std::regex_replace(val.begin(), val.begin(), val.end(), std::regex("H'([[:xdigit:]]+)", std::regex_constants::extended), "0x\\1", std::regex_constants::format_sed);
      std::regex_replace(val.begin(), val.begin(), val.end(), std::regex("R([[:digit:]]{1,2})", std::regex_constants::extended), "r\\1", std::regex_constants::format_sed);
      auto pos = val.begin(); // chopped off part of the string, so start from the beginning

      auto is_mnemonic = [](unsigned char c) -> unsigned char{ return std::isalpha(c) || c == '.' ; };
      auto is_comment_or_EOL = [](unsigned char c) -> unsigned char{ return c == ';' || c == '\n' ; };
      do
      {
        if(*pos == '\n')
        {
          std::cout << std::endl;
        }
        else
        {
          auto next = std::find_if_not(pos, val.end(), is_mnemonic);
          std::transform(pos, next, pos, to_lowercase);
          std::cout << std::setw(6) << std::string(pos, next);

          pos = next;

          if(*pos != ';' && *pos != '\n')
          {
            next = std::find_if(pos, val.end(), is_comment_or_EOL);
            std::transform(pos, next, pos, to_lowercase);
            if(*next == ';')
              std::cout << std::setw(9) << std::string(pos, next) << "! ";
            else
              std::cout << std::string(pos, next);
            pos = next;
          }
          if(*pos == ';' && *pos != '\n')
          {
            ++pos;
            next = std::find_if(pos, val.end(), is_endline); // move past initial line break
            std::cout << std::string(pos, next);

            pos = next;
          }
          if(*pos == '\n')
            std::cout << std::endl;
        }
      } while(++pos != val.end());
      std::cout << "</span>" << std::endl;
    }
  }
}


std::string print_list(const isa_property& prop, const std::string& newtext)
{
  std::string r;
  for(const auto& val : prop)
    if(!val.empty())
      r += std::regex_replace(std::string(val), std::regex(val.begin(), val.end(), std::regex_constants::basic), newtext, std::regex_constants::format_sed);
  return r;
}


std::string for_all_isas (const insn& i, const isa_property& p, std::function<std::string(bool, const std::string_view&)> func)
{
  constexpr static const std::array<isa, 9> display_order = { SH1, SH2, SH2E,
                                                              SH3, SH3E, SH_DSP,
                                                              SH4, SH4A, SH2A, };
  std::string r;
  for(std::size_t pos = 0; pos < isa_name.size(); ++pos)
    r += func(i.is_isa(display_order[pos]), p[display_order[pos]]);
  return r;
}

std::string print_isa_list (const insn& i)
{
  return for_all_isas(i, isa_name,
  [](bool match, const std::string_view& prop) -> std::string
  {
    if(match)
      return std::string(" ").append(prop);
    return std::string();
  });
}

std::string print_isa_props (const insn& i, const isa_property& p)
{
  return for_all_isas(i, p,
  [](bool match, const std::string_view& prop) -> std::string
  {
    if(match && !prop.empty())
      return std::string("<var>").append(prop).append("</var>");
    return "<var></var>";
  });
}

std::string print_isa_compatibility (const insn& i)
{
  std::string r = print_isa_props(i, isa_name);
  for(auto g : i.data<environments>())
  {

  }
//  if (i.privileged())
//    r += "<br />Privileged";
  return r;
}

constexpr char operand_type(char c)
{
  if(c == '1') // make binary match
    return '0';
  return c;
}

std::string colorize_code(std::string_view code)
{
  std::string r;
  uint8_t count = 0, spaces = 0;
  char current = '\0';

  for(auto pos = code.begin(); pos != code.end(); ++pos)
  {
    r.push_back(*pos);
    ++count;

    if(*pos == ' ' && pos + 1 != code.end())
    {
      ++spaces;
      continue;
    }
    current = *pos;

    if(pos + 1 == code.end() ||
       operand_type(current) != operand_type(*(pos + 1)))
    {
      auto total = std::count(code.begin(), code.end(), current); // find the total number of occurances in the whole string
      std::string tag = "<span title=\"";
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

      r.insert(r.end() - count + spaces, tag.begin(), tag.end());
      r.append("</span>");
      count = 0;
      spaces = 0;
    }
  }

  return r;
}

int main (void)
{
  std::cout << std::unitbuf; // enable automatic flushing
  std::cerr << std::unitbuf; // enable automatic flushing

  std::cout <<
R"html(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8"/>
<title>Renesas SH Instruction Set Summary</title>

<style>

:root
{
  --table-width: 1490px;
  --details-width: 820px;
}

@media (prefers-color-scheme : light)
{
  .image_filter {} /* no filtering needed */

  :root
  {
    --system-background-color: #FFFFFF;
    --system-text-color: #000000;

    --table-background-color: #FFFFFF;
    --table-text-color: #000000;
    --table-background-hover-color: #F0F0F0;
    --table-image-hover-color: #F0F0F0;

    --header-background-color: #D0D0D0;
    --header-text-color: #000000;

    --grid-inactive-text-color: #C0C0C0;
    --grid-cell-background-color: #F4F4F4;

    --cpu-grid-active-text-color: #404040;
    --cycle-grid-active-text-color: #000000;
  }

  span[title='Ignored']                          { color: goldenrod; }
  span[title='Opcode Identifier']                { color: dodgerblue; }
  span[title='A']                                { color: darkgreen; }
  span[title='D']                                { color: darkmagenta; }
  span[title^='Unsigned Immediate Data']         { color: darkturquoise; }
  span[title^='Signed Immediate Data']           { color: darkturquoise; }
  span[title^='Source Register']                 { color: aquamarine; }
  span[title^='Destination Register']            { color: blueViolet; }
  span[title^='Displacement']                    { color: indigo; }
  span[title^='Multiplier Source Register 1']    { color: cyan; }
  span[title^='Multiplier Source Register 2']    { color: chartreuse; }
  span[title^='Multiplier Destination Register'] { color: darkcyan; }
  span[title^='ALU Source Register 1']           { color: darkmagenta; }
  span[title^='ALU Source Register 2']           { color: darkorange; }
  span[title^='ALU Destination Register']        { color: indianred; }
  span[title^='ALU Destination Register']        { color: forestgreen; }
}

@media (prefers-color-scheme : dark)
{
  .image_filter /* invert colors */
  {
    filter: invert(100%);
  }

  :root
  {
    --system-background-color: #000000;
    --system-text-color: #FFFFFF;

    --table-background-color: #000000;
    --table-text-color: #FFFFFF;
    --table-background-hover-color: #404040;
    --table-image-hover-color: #C0C0C0;

    --header-background-color: #606060;
    --header-text-color: #FFFFFF;

    --grid-inactive-text-color: #707070;
    --grid-cell-background-color: #303030;

    --cpu-grid-active-text-color: #C0C0C0;
    --cycle-grid-active-text-color: #FFFFFF;
  }

  span[title='Ignored']                          { color: gold; }
  span[title='Opcode Identifier']                { color: #55ff55; }
  span[title='A']                                { color: lightseagreen; }
  span[title='D']                                { color: olive; }
  span[title^='Unsigned Immediate Data']         { color: #aaaaff; }
  span[title^='Signed Immediate Data']           { color: #aaaaff; }
  span[title^='Source Register']                 { color: #ffff55; }
  span[title^='Destination Register']            { color: #f92672; }
  span[title^='Displacement']                    { color: coral; }
  span[title^='Multiplier Source Register 1']    { color: cyan; }
  span[title^='Multiplier Source Register 2']    { color: chartreuse; }
  span[title^='Multiplier Destination Register'] { color: darkcyan; }
  span[title^='ALU Source Register 1']           { color: darkorange; }
  span[title^='ALU Source Register 2']           { color: deeppink; }
  span[title^='ALU Destination Register']        { color: darkmagenta; }
  span[title^='ALU Destination Register']        { color: forestgreen; }
}

/*
ALU Arithmetic Logic Unit
ASID Address Space Identifier
CPU Central Processing Unit
FPU Floating Point Unit
ITLB Instruction Translation Look aside Buffer
LRU Least Recently Used
LSB Least Significant Bit
MMU Memory Management Unit
MSB Most Significant Bit
PC Program Counter
PMB Privileged space Mapping Buffer
RISC Reduced Instruction Set Computer
TLB Translation Lookaside Buffer
UBC User Break Controller
UTLB Unified Translation Look aside Buffer



9 8 7 6 5 4 3 2 1 0
M Q I3 I2 I1 I0 S T


S bit: Used by the multiply/accumulate instruction.

Reserved bits: Always reads as 0, and should always be written with 0.

Bits I3–I0: Interrupt mask bits.

M and Q bits: Used by the DIV0U/S and DIV1 instructions.

Global base register (GBR):
Indicates the base address of the indirect
GBR addressing mode. The indirect GBR
addressing mode is used in data transfer
for on-chip peripheral module register
areas and in logic operations.

Vector base register (VBR):
Indicates the base address of the exception
processing vector area.

SR: Status register

T bit: The MOVT, CMP/cond, TAS, TST, BT (BT/S), BF (BF/S), SETT, and CLRT instructions use
       the T bit to indicate true (1) or false (0). The ADDV/C, SUBV/C, DIV0U/S, DIV1, NEGC,
       SHAR/L, SHLR/L, ROTR/L, and ROTCR/L instructions also use bit T to indicate carry/borrow
       or overflow/underflow




DSP status register (DSR)

GT = Signed greater than bit
Z = Zero value bit
N = Negative value bit
V = Overflow bit
CS = Condition select bits
DC = DSP condition bit



*/

body
{
  background-color: var(--system-background-color);
  color: var(--system-text-color);
  font-size: 15px;
  padding-left: calc(50% - var(--table-width) / 2); /* center everything */
  height: 25000px; /* for sticky header */
  margin: 0px;
}

#header
{
  background-color: var(--system-background-color);
  color: var(--system-text-color);
  width: var(--table-width);
}

#table_header
{
  position: sticky !important;
  top: 0px;
  height: 85px;
  z-index: 100;
  font-weight: bold;
  font-style: italic;
  background-color: var(--header-background-color);
  color: var(--header-text-color)
}



input[id="cb_SH1"  ]:checked ~ label.SH1,
input[id="cb_SH2"  ]:checked ~ label.SH2,
input[id="cb_SH2E" ]:checked ~ label.SH2E,
input[id="cb_SH3"  ]:checked ~ label.SH3,
input[id="cb_SH3E" ]:checked ~ label.SH3E,
input[id="cb_DSP"  ]:checked ~ label.DSP,
input[id="cb_SH4"  ]:checked ~ label.SH4,
input[id="cb_SH4A" ]:checked ~ label.SH4A,
input[id="cb_SH2A" ]:checked ~ label.SH2A  { display: inline-grid; }
label.summary { display: none; }

input[id^="cb_" ]
{
  position: sticky;
  z-index: 1000;
  padding-left: 30px;
}

input[id^="cb_" ]::after
{
  content:attr(name);
}

input[type='checkbox'][id^="row"] { display: none; }
input[type='checkbox'][id^="row"]:checked + label > .details
  { display: unset; }

.summary
{
  background-color: var(--table-background-color);
  color: var(--table-text-color);
  clear: both;
}

label.summary > .colorized > span
{
  font-family: monospace;
  display: contents;
}

label.summary > span:not(.details)
{
  padding-right: 20px;
  border-top-width: 1px;
  border-top-style: dotted;
  font-family: monospace;
  font-size: 13px;
}

.summary > span:not(.details):nth-child(2)
{
  white-space: pre-wrap;
}

.summary > span:not(.details):nth-child(1) { padding-left: 10px; }
.summary > span:not(.details):nth-child(3)
{
  white-space: pre-wrap;
  padding-right: 30px;
}


/* links hover events */
label.summary:hover > *,
.details:hover > *,
.details:hover + .summary
{ background-color: var(--table-background-hover-color); }

/* keep images readable */
.details:hover img
{ background-color: var(--table-image-hover-color) !important; }

.summary
{
  display: inline-grid;
  grid-template-columns: 150px 240px 470px 150px 130px 120px 120px 110px;
  grid-template-rows: 50px auto;
}

/* all columns */
.summary > span
{
  vertical-align: middle;
  display: inline-block;
}

.cpu_grid, .cycle_grid
{
  display: inline-grid !important;
  grid-gap: 2px;
  grid-template-columns: 33px 33px 33px;
  grid-template-rows: 13px 13px 13px;
  font-family: monospace;
  font-size: 11px;
  font-weight: unset;
  font-style: unset;
}

.cpu_grid > var, .cycle_grid > var
{
  color: var(--grid-inactive-text-color);
  background-color: var(--grid-cell-background-color);
  display: block;
  text-align: center;
  padding: 0px;
  margin: 0px;
  height: 13px;
}

.cpu_grid > var:nth-of-type(1):before { content:"SH1"  ; }
.cpu_grid > var:nth-of-type(2):before { content:"SH2"  ; }
.cpu_grid > var:nth-of-type(3):before { content:"SH2E" ; }
.cpu_grid > var:nth-of-type(4):before { content:"SH3"  ; }
.cpu_grid > var:nth-of-type(5):before { content:"SH3E" ; }
.cpu_grid > var:nth-of-type(6):before { content:"DSP"  ; }
.cpu_grid > var:nth-of-type(7):before { content:"SH4"  ; }
.cpu_grid > var:nth-of-type(8):before { content:"SH4A" ; }
.cpu_grid > var:nth-of-type(9):before { content:"SH2A" ; }

.summary .cpu_grid > var,
.summary .cycle_grid > var { color: transparent; }

input[id="cb_SH1"  ]:checked ~ .summary .cpu_grid > var:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary .cpu_grid > var:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary .cpu_grid > var:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary .cpu_grid > var:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary .cpu_grid > var:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary .cpu_grid > var:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary .cpu_grid > var:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary .cpu_grid > var:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary .cpu_grid > var:nth-of-type(9)
{ color: var(--grid-inactive-text-color); }

input[id="cb_SH1"  ]:checked ~ .summary.SH1  .cpu_grid > var:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary.SH2  .cpu_grid > var:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary.SH2E .cpu_grid > var:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary.SH3  .cpu_grid > var:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary.SH3E .cpu_grid > var:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary.DSP  .cpu_grid > var:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary.SH4  .cpu_grid > var:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary.SH4A .cpu_grid > var:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary.SH2A .cpu_grid > var:nth-of-type(9)
{ color: var(--cpu-grid-active-text-color); }

input[id="cb_SH1"  ]:checked ~ .summary.SH1  .cycle_grid > var:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary.SH2  .cycle_grid > var:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary.SH2E .cycle_grid > var:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary.SH3  .cycle_grid > var:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary.SH3E .cycle_grid > var:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary.DSP  .cycle_grid > var:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary.SH4  .cycle_grid > var:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary.SH4A .cycle_grid > var:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary.SH2A .cycle_grid > var:nth-of-type(9)
{ color: var(--cycle-grid-active-text-color); }

/* styling for details section */
.summary > .details
{
  border: unset;
  display: none;
  padding: 10px calc(var(--table-width) - var(--details-width) - 30px) 10px 30px;
  width: var(--details-width);
}

.summary > .details > span[title="code"],
.summary > .details > span[title="assembly"]
{
  display: block;
  font-size: 13px;
  font-family: monospace;
  white-space: pre;
  margin: 1em 0;
}

.summary > .details > span[title="section"]
{
  font-style: italic;
  font-weight: bold;
}

span[title='list']
{
  list-style-type: circle;
}
span[title='list'] > var
{
  font-style: unset;
  display: list-item;
}

</style>
</head>
<body>
  <div id="header">
    <span style="font-size:20px;font:bold">Renesas SH Instruction Set Summary</span>
    <div style="float:right">Last updated: )html" << __DATE__ << " " << __TIME__ << R"html(</div>
    <br />
    <div style="float:right">
      <a href="mailto:olegendo@gcc.gnu.org?Subject=Renesas%20SH%20Instruction%20Set%20Summary">Contact</a>
      <a href="https://github.com/shared-ptr/sh_insns">Page Source</a>
    </div>
  </div>)html"

  << print_list(isa_name, "\n  <input type=\"checkbox\" id=\"cb_&\" name=\"&\" checked /><label for=\"cb_&\">&</label>")

  << R"html(
    <span id="table_header" class="summary)html" << print_list(isa_name, " &") << R"html(">
    <span>Compatibilty</span>
    <span>Format</span>
    <span>Abstract</span>
    <span>Code</span>
    <span>Bit Flags</span>
    <span>Instruction Group
      <span class="cpu_grid"><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var></span>
    </span>
    <span>Issue Cycles
      <span class="cpu_grid"><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var></span>
    </span>
    <span>Latency Cycles
      <span class="cpu_grid"><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var></span>
    </span>
  </span>)html";

  std::list<insns> insn_blocks = build_insn_blocks ();

  try
  {
    int id = 0;
    for (const auto& block : insn_blocks)
    {
      std::cout << "<br/><br/><br/><b>" << block.section_title << "</b><br/><br/>" << std::endl;

      for (const auto& i : block)
      {
        std::cout << "<input type=\"checkbox\" id=\"row" << id << "\" />" << std::endl;
        std::cout
            << "<label class=\"summary" << print_isa_list(i) << "\" for=\"row" << id << "\">" << std::endl
            << "<span class=\"cpu_grid\"><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var></span>" << std::endl
            << "<span>" << fix_html(i.data<format>()) << "</span>" << std::endl
            << "<span>" << fix_html(i.data<abstract>()) << "</span>" << std::endl
            << "<span id=\"" << fix_id(i.data<code>()) << "\" class=\"colorized\">" << colorize_code(i.data<code>()) << "</span>" << std::endl
            << "<span>" << i.data<flags>() << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << print_isa_props (i, i.data<group>()) << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << print_isa_props (i, i.data<issue>()) << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << print_isa_props (i, i.data<latency>()) << "</span>" << std::endl
            << "<span class=\"details\">" << std::endl;

        print_note_section (i.data<name>(), i.data<description>());
        print_note_section ("Note", i.data<note>());
        print_code_section("Operation", i.data<operation>());
        print_assembly_section ("Example", i.data<example>());
        print_list_section ("Possible Exceptions", i.data<exceptions>());

        std::cout << "</span>" << std::endl // close "details"
                  << "</label>" << std::endl;
        ++id;
      }
//      break;
    }

    std::cout << "</body>" << std::endl
              << "</html>" << std::endl;
  }
  catch (std::string message)
  {
    std::cerr << "exception caught: " << message << std::endl;
  }
  catch (...)
  {
    std::cerr << "uncaught exception!" << std::endl;
  }


  return 0;
}

