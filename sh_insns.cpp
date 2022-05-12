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
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <regex>

#include "build_instructions.h"

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


// ----------------------------------------------------------------------------

enum note_type
{
  note_normal,
  note_code
};

void print_note (std::string_view name, std::string_view val, note_type t)
{
  // skip leading line breaks in the input string.
  std::size_t pos = val.find_first_not_of('\n');

  if(pos != std::string_view::npos)
  {
    std::cout << "<span title=\"section\">" << name << "</span>" << std::endl << "<br />" << std::endl;
    switch(t)
    {
      case note_normal: std::cout << val.substr(pos) << "<br /><br />"; break;
      case note_code: std::cout << "<span title=\"code\">" << val.substr(pos) << "</span>"; break;
    }
    std::cout << std::endl;
  }
}


std::string print_list(const isa_property& prop, const std::string& newtext)
{
  std::string r;
  for(const auto& val : prop)
    if(!val.empty())
      r += std::regex_replace(std::string(val), std::regex(std::string(val)), newtext);
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
      return std::string("<p>").append(prop).append("</p>");
    return "<p></p>";
  });
}

std::string print_isa_compatibility (const insn& i)
{
  std::string r = print_isa_props(i, isa_name);
  if (i.privileged())
    r += "<br />Privileged";
  return r;
}

std::string colorize_code(std::string_view code)
{
  char prev = '\0';
  std::string r;
  for(auto& current : code)
  {
    if(current == prev ||
       ((prev == '0' || prev == '1') &&
        (current == '0' || current == '0')))
      r.push_back(current);
    else
    {
      if(!r.empty() && prev != ' ')
        r.append("</span>");
      if(current != ' ')
        r.append("<span title=\"");
      switch(current)
      {
        case '1':
        case '0': r.append("binary"); break;
        case '*': r.append("star"); break;
        default: r.push_back(current); break;
      }
      if(current != ' ')
        r.append("\">").push_back(current);
    }
    prev = current;
  }
  if(!r.empty())
    r.append("</span>");
  return r;
}

int main (void)
{
  std::cout <<
R"html(<!DOCTYPE html>
<html lang="en">
<head><title>Renesas SH Instruction Set Summary</title>

<style type="text/css">

:root
{
  --table-width: 1320px;
  --details-width: 720px;
}

@media (prefers-color-scheme : light)
{
  :root
  {
    --system-background-color: #000000;
    --system-text-color: #FFFFFF;

    --table-background-color: #FFFFFF;
    --table-text-color: #000000;
    --table-background-hover-color: #F0F0F0;

    --header-background-color: #D0D0D0;
    --header-text-color: #000000;

    --grid-inactive-text-color: #C0C0C0;
    --grid-cell-background-color: #F4F4F4;

    --cpu-grid-active-text-color: #404040;
    --cycle-grid-active-text-color: #000000;
  }
}

@media (prefers-color-scheme : dark)
{
  :root
  {
    --system-background-color: #000000;
    --system-text-color: #FFFFFF;

    --table-background-color: #000000;
    --table-text-color: #FFFFFF;
    --table-background-hover-color: #404040;

    --header-background-color: #606060;
    --header-text-color: #FFFFFF;

    --grid-inactive-text-color: #707070;
    --grid-cell-background-color: #303030;

    --cpu-grid-active-text-color: #C0C0C0;
    --cycle-grid-active-text-color: #FFFFFF;
  }

  span[title='A']      { color: LightSeaGreen; }
  span[title='D']      { color: Olive; }
  span[title='i']      { color: AntiqueWhite; }
  span[title='m']      { color: Aquamarine; }
  span[title='n']      { color: BlueViolet; }
  span[title='d']      { color: Coral; }
  span[title='e']      { color: Cyan; }
  span[title='f']      { color: Chartreuse; }
  span[title='g']      { color: DarkCyan; }
  span[title='u']      { color: DarkMagenta; }
  span[title='x']      { color: DarkOrange; }
  span[title='y']      { color: DeepPink; }
  span[title='z']      { color: ForestGreen; }
  span[title='star']   { color: Gold; }
  span[title='binary'] { color: LightBlue; }
}

body
{
  background-color: var(--system-background-color);
  color: var(--system-text-color);
  font-size: 13px;
  padding-left: calc(50% - var(--table-width) / 2); /* center everything */
  height: 10000px; /* for sticky header */
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

label.summary > .colorized > div
{
  font-family: monospace;
  display: content;
}

label.summary > div:not(.details)
{
  padding-right: 20px;
  border-top-width: 1px;
  border-top-style: dotted;
  font-family: monospace;
  font-size: 11px;
}

.summary > div:not(.details):nth-child(2)
{
  white-space: pre-wrap;
}

.summary > div:not(.details):nth-child(1) { padding-left: 10px; }
.summary > div:not(.details):nth-child(3)
{
  white-space: pre-wrap;
  padding-right: 30px;
}


/* links hover events */
label.summary:hover > *,
.details:hover > *,
.details:hover + .summary
{ background-color: var(--table-background-hover-color); }

.summary
{
  display: inline-grid;
  grid-template-columns: 150px 240px 350px 150px 80px 120px 120px 110px;
  grid-template-rows: 50px auto;
}

/* all columns */
.summary > div
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
  font-size: 9px;
  font-weight: unset;
  font-style: unset;
}

.cpu_grid > p, .cycle_grid > p
{
  color: var(--grid-inactive-text-color);
  background-color: var(--grid-cell-background-color);
  display: block;
  text-align: center;
  padding: 0px;
  margin: 0px;
  height: 13px;
}

.cpu_grid > p:nth-of-type(1):before { content:"SH1"  ; }
.cpu_grid > p:nth-of-type(2):before { content:"SH2"  ; }
.cpu_grid > p:nth-of-type(3):before { content:"SH2E" ; }
.cpu_grid > p:nth-of-type(4):before { content:"SH3"  ; }
.cpu_grid > p:nth-of-type(5):before { content:"SH3E" ; }
.cpu_grid > p:nth-of-type(6):before { content:"DSP"  ; }
.cpu_grid > p:nth-of-type(7):before { content:"SH4"  ; }
.cpu_grid > p:nth-of-type(8):before { content:"SH4A" ; }
.cpu_grid > p:nth-of-type(9):before { content:"SH2A" ; }

.summary .cpu_grid > p,
.summary .cycle_grid > p { color: transparent; }

input[id="cb_SH1"  ]:checked ~ .summary .cpu_grid > p:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary .cpu_grid > p:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary .cpu_grid > p:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary .cpu_grid > p:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary .cpu_grid > p:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary .cpu_grid > p:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary .cpu_grid > p:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary .cpu_grid > p:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary .cpu_grid > p:nth-of-type(9)
{ color: var(--grid-inactive-text-color); }

input[id="cb_SH1"  ]:checked ~ .summary.SH1  .cpu_grid > p:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary.SH2  .cpu_grid > p:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary.SH2E .cpu_grid > p:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary.SH3  .cpu_grid > p:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary.SH3E .cpu_grid > p:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary.DSP  .cpu_grid > p:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary.SH4  .cpu_grid > p:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary.SH4A .cpu_grid > p:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary.SH2A .cpu_grid > p:nth-of-type(9)
{ color: var(--cpu-grid-active-text-color); }

input[id="cb_SH1"  ]:checked ~ .summary.SH1  .cycle_grid > p:nth-of-type(1),
input[id="cb_SH2"  ]:checked ~ .summary.SH2  .cycle_grid > p:nth-of-type(2),
input[id="cb_SH2E" ]:checked ~ .summary.SH2E .cycle_grid > p:nth-of-type(3),
input[id="cb_SH3"  ]:checked ~ .summary.SH3  .cycle_grid > p:nth-of-type(4),
input[id="cb_SH3E" ]:checked ~ .summary.SH3E .cycle_grid > p:nth-of-type(5),
input[id="cb_DSP"  ]:checked ~ .summary.DSP  .cycle_grid > p:nth-of-type(6),
input[id="cb_SH4"  ]:checked ~ .summary.SH4  .cycle_grid > p:nth-of-type(7),
input[id="cb_SH4A" ]:checked ~ .summary.SH4A .cycle_grid > p:nth-of-type(8),
input[id="cb_SH2A" ]:checked ~ .summary.SH2A .cycle_grid > p:nth-of-type(9)
{ color: var(--cycle-grid-active-text-color); }

/* styling for details section */
.summary > .details
{
  border: unset;
  display: none;
  padding: 10px calc(var(--table-width) - var(--details-width) - 30px) 10px 30px;
  width: var(--details-width);
}

.summary > .details > span[title="code"]
{
  display: block;
  font-size: 11px;
  font-family: monospace;
  white-space: pre;
  margin: 1em 0;
}

.summary > .details > span[title="section"]
{
  font-style: italic;
  font-weight: bold;
}

.summary > .details li
{
  margin-left: 15px;
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

  << print_list(isa_name, "\n  <input type=\"checkbox\" id=\"cb_$&\" name=\"$&\" checked /><label for=\"cb_$&\">$&</label>")

  << R"html(<div id="table_header" class="summary)html"
  << print_list(isa_name, " $&")
  << R"html(">
    <div>Compatibilty</div>
    <div>Format</div>
    <div>Abstract</div>
    <div>Code</div>
    <div>T Bit<br />DC Bit</div>
    <div>Instruction Group
      <div class="cpu_grid"><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p></div>
    </div>
    <div>Issue Cycles
      <div class="cpu_grid"><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p></div>
    </div>
    <div>Latency Cycles
      <div class="cpu_grid"><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p></div>
    </div>
  </div>)html";

  std::vector<insns> insn_blocks = build_insn_blocks ();

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
            << "<div class=\"cpu_grid\"><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p><p></p></div>" << std::endl
            << "<div>" << i.data<format>() << "</div>" << std::endl
            << "<div>" << i.data<abstract>() << "</div>" << std::endl
            << "<div class=\"colorized\">" << colorize_code(i.data<code>()) << "</div>" << std::endl
            << "<div>" << i.data<t_bit>() << "<br />" << i.data<dc_bit>() << "</div>" << std::endl
            << "<div class=\"cycle_grid\">" << print_isa_props (i, i.data<group>()) << "</div>" << std::endl
            << "<div class=\"cycle_grid\">" << print_isa_props (i, i.data<issue>()) << "</div>" << std::endl
            << "<div class=\"cycle_grid\">" << print_isa_props (i, i.data<latency>()) << "</div>" << std::endl
            << "<div class=\"details\">" << std::endl;

        print_note ("Description", i.data<description>(), note_normal);
        print_note ("Note", i.data<note>(), note_normal);
        print_note ("Operation", i.data<operation>(), note_code);
        print_note ("Example", i.data<example>(), note_code);
        print_note ("Possible Exceptions", i.data<exceptions>(), note_normal);

        std::cout << "</div>" << std::endl
                  << "</label>" << std::endl;
        ++id;
      }
    }

    std::cout << "</body>" << std::endl
              << "</html>" << std::endl;
  }
  catch (...)
  {
    std::cerr << "exception encountered" << std::endl;
  }


  return 0;
}

