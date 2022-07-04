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
#include "post_processing.h"

using namespace std::literals;
using namespace std::string_view_literals;
// ----------------------------------------------------------------------------


static isa_property display_name = isa_property
{
  SH1, "SH1",   // SH7032 / 7034 --Operating frequency 20MHz
  SH2, "SH2",   // SH7604 --Operating frequency 28.7MHz
  SH2E, "SH2E", // SH7206 (No Fpu)- Dhrystone 480MIPS/200MHz
  SH2A, "SH2A", // SH7262 (with fpu) 345MIPS/144MHz
  SH3, "SH3",   // SH7702/7708 - Action frequency 60MHz
  SH3_FPU, "SH3E", // SH7718 - Action frequency 100MHz
  SH4, "SH4",
  SH4A, "SH4A",
  SH1_DSP, "DSP" // SH7410 - Action frequency 60MHz
};

// ----------------------------------------------------------------------------

std::string fix_id(std::string data)
{
  data = std::regex_replace(data, std::regex("<var[^>]+>([^<]+)</var>", std::regex_constants::extended),
                            "\\1", std::regex_constants::format_sed);
  return data;
}

// ----------------------------------------------------------------------------

std::string build_environments(std::list<environment_t> environments)
{
  std::string rval;
  for(const auto& env : environments)
  {

  }
  return rval;
}


std::string build_citations(std::list<citation_t> citations)
{
  std::string rval;
  for(const auto& cite : citations)
  {

  }
  return rval;
}

std::string build_span_section (std::string_view word_title, std::string tag_title, std::string val)
{
  std::string rval;
  if(!val.empty())
  {
    rval.append("<span title=\"section\">")
        .append(word_title)
        .append("</span>\n")
        .append("<span title=\"")
        .append(tag_title)
        .append("\">")
        .append(val)
        .append("</span>\n");
  }
  return rval;
}


std::string regex_property_list(const isa_property& prop, const std::string& newtext)
{
  std::string r;
  for(const auto& val : prop)
    if(!val.empty())
      r += std::regex_replace(std::string(val), std::regex(std::begin(val), std::end(val), std::regex_constants::basic), newtext, std::regex_constants::format_sed);
  return r;
}


std::string build_isa_list (const insn& i)
{
  static isa_property names = isa_property
  {
    SH1, "SH1",
    SH1_DSP, "SH1_DSP",
    SH2, "SH2",
    SH2_DSP, "SH2_DSP",
    SH2E, "SH2E",
    SH2A, "SH2A",
    SH2A_FPU, "SH2A_FPU",
    SH3, "SH3",
    SH3_FPU, "SH3_FPU",
    SH3_DSP, "SH3_DSP",
    SH4, "SH4",
    SH4A, "SH4A",
  };
  constexpr static const std::array<isa, 12> list = { SH1, SH1_DSP, SH2, SH2_DSP,
                                                      SH2E, SH2A, SH2A_FPU,
                                                      SH3, SH3_FPU, SH3_DSP,
                                                      SH4, SH4A };
  auto func = [](bool match, const std::string_view& prop) -> std::string
  {
    if(match)
      return std::string(" ").append(prop);
    return std::string();
  };
  std::string r;
  for(std::size_t pos = 0; pos < list.size(); ++pos)
    r += func(i.for_isa(list[pos]), names[list[pos]]);
  return r;
}

std::string build_isa_tagged_property_list (const insn& i, const isa_property& p)
{
  constexpr static const std::array<isa, 9> list = {
                                                     SH1, SH2,
                                                     SH2E, SH2A | SH2A_FPU, SH3,
                                                     SH3_FPU, SH4, SH4A,
                                                     SH1_DSP | SH2_DSP | SH3_DSP
                                                   };
  auto func = [](bool match, const std::string_view& prop) -> std::string
  {
    if(match && !prop.empty())
      return std::string("<var>").append(prop).append("</var>");
    return "<var></var>";
  };
  std::string r;
  for(std::size_t pos = 0; pos < list.size(); ++pos)
    r += func(i.for_isa(list[pos]), p[list[pos]]);
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
<title>Hitachi SuperH Instruction Set Summary</title>
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

    --mnemonic-text-color: #FF0000;
  }

  var[title="Ignored"]                          { color: goldenrod; }
  var[title="Opcode Identifier"]                { color: dodgerblue; }
  var[title="A"]                                { color: darkgreen; }
  var[title="D"]                                { color: darkmagenta; }
  var[title^='Unsigned Immediate Data']         { color: darkturquoise; }
  var[title^='Signed Immediate Data']           { color: darkturquoise; }
  var[title^='Source Register']                 { color: aquamarine; }
  var[title^='Destination Register']            { color: blueViolet; }
  var[title^='Displacement']                    { color: indigo; }
  var[title^='Multiplier Source Register 1']    { color: cyan; }
  var[title^='Multiplier Source Register 2']    { color: chartreuse; }
  var[title^='Multiplier Destination Register'] { color: darkcyan; }
  var[title^='ALU Source Register 1']           { color: darkmagenta; }
  var[title^='ALU Source Register 2']           { color: darkorange; }
  var[title^='ALU Destination Register']        { color: indianred; }
  var[title^='ALU Destination Register']        { color: forestgreen; }
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

    --mnemonic-text-color: #FF8888;
  }

  var[title="Ignored"]                          { color: gold; }
  var[title="Opcode Identifier"]                { color: #55ff55; }
  var[title="A"]                                { color: lightseagreen; }
  var[title="D"]                                { color: olive; }
  var[title^='Unsigned Immediate Data']         { color: #aaaaff; }
  var[title^='Signed Immediate Data']           { color: #aaaaff; }
  var[title^='Source Register']                 { color: #ffff55; }
  var[title^='Destination Register']            { color: #f92672; }
  var[title^='Displacement']                    { color: coral; }
  var[title^='Multiplier Source Register 1']    { color: cyan; }
  var[title^='Multiplier Source Register 2']    { color: chartreuse; }
  var[title^='Multiplier Destination Register'] { color: darkcyan; }
  var[title^='ALU Source Register 1']           { color: darkorange; }
  var[title^='ALU Source Register 2']           { color: deeppink; }
  var[title^='ALU Destination Register']        { color: darkmagenta; }
  var[title^='ALU Destination Register']        { color: forestgreen; }
}

var, section, q, q::before, q::after
{ all: unset; }

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
  font-weight: 700;
  font-style: italic;
  background-color: var(--header-background-color);
  color: var(--header-text-color)
}

.section_title
{
  display: block;
  font-weight: 900;
  padding-bottom: 0.5em;
  padding-top: 2em;
}

input[id^="cb_" ]::after { content:attr(name); }

#cb_SH1:checked  ~ label.SH1,
#cb_SH2:checked  ~ label.SH2,
#cb_SH2E:checked ~ label.SH2E,
#cb_SH2A:checked ~ label.SH2A,
#cb_SH2A:checked ~ label.SH2A_FPU,
#cb_SH3:checked  ~ label.SH3,
#cb_SH3E:checked ~ label.SH3_FPU,
#cb_DSP:checked  ~ label.SH1_DSP,
#cb_DSP:checked  ~ label.SH1_DSP,
#cb_DSP:checked  ~ label.SH2_DSP,
#cb_SH4:checked  ~ label.SH4,
#cb_SH4A:checked ~ label.SH4A
{ display: inline-grid; }

label.summary { display: none; }

input[id^="cb_" ]
{
  position: sticky;
  z-index: 1000;
  padding-left: 30px;
}

input[id^="row"] { display: none; }
input[id^="row"]:checked + label > .details
  { display: unset; }

.summary
{
  background-color: var(--table-background-color);
  color: var(--table-text-color);
  clear: both;
}

label.summary > .colorized
{
  font-family: monospace;
  word-wrap: break-word;
}

label.summary > .colorized > var
  { display: contents; }

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
label.summary:hover img,
.details:hover img
{ background-color: var(--table-image-hover-color) !important; }

.summary
{
  display: inline-grid;
  grid-template-columns: 150px 240px 470px 150px 130px 120px 120px 110px;
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
  color: transparent;
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
.cpu_grid > var:nth-of-type(4):before { content:"SH2A" ; }
.cpu_grid > var:nth-of-type(5):before { content:"SH3"  ; }
.cpu_grid > var:nth-of-type(6):before { content:"SH3E" ; }
.cpu_grid > var:nth-of-type(7):before { content:"SH4"  ; }
.cpu_grid > var:nth-of-type(8):before { content:"SH4A" ; }
.cpu_grid > var:nth-of-type(9):before { content:"DSP"  ; }


#cb_SH1:checked  ~ .summary .cpu_grid > var:nth-of-type(1),
#cb_SH2:checked  ~ .summary .cpu_grid > var:nth-of-type(2),
#cb_SH2E:checked ~ .summary .cpu_grid > var:nth-of-type(3),
#cb_SH2A:checked ~ .summary .cpu_grid > var:nth-of-type(4),
#cb_SH3:checked  ~ .summary .cpu_grid > var:nth-of-type(5),
#cb_SH3E:checked ~ .summary .cpu_grid > var:nth-of-type(6),
#cb_SH4:checked  ~ .summary .cpu_grid > var:nth-of-type(7),
#cb_SH4A:checked ~ .summary .cpu_grid > var:nth-of-type(8),
#cb_DSP:checked  ~ .summary .cpu_grid > var:nth-of-type(9)
{ color: var(--grid-inactive-text-color); }

#cb_SH1:checked  ~ .summary.SH1       .cpu_grid > var:nth-of-type(1),
#cb_SH2:checked  ~ .summary.SH2       .cpu_grid > var:nth-of-type(2),
#cb_SH2E:checked ~ .summary.SH2E      .cpu_grid > var:nth-of-type(3),
#cb_SH2A:checked ~ .summary.SH2A      .cpu_grid > var:nth-of-type(4),
#cb_SH2A:checked ~ .summary.SH2A_FPU  .cpu_grid > var:nth-of-type(4), /* show FPU instructions too */
#cb_SH3:checked  ~ .summary.SH3       .cpu_grid > var:nth-of-type(5),
#cb_SH3E:checked ~ .summary.SH3       .cpu_grid > var:nth-of-type(6),
#cb_SH3E:checked ~ .summary.SH3_FPU   .cpu_grid > var:nth-of-type(6), /* show FPU instructions too */
#cb_SH4:checked  ~ .summary.SH4       .cpu_grid > var:nth-of-type(7),
#cb_SH4A:checked ~ .summary.SH4A      .cpu_grid > var:nth-of-type(8),
#cb_DSP:checked  ~ .summary.SH1_DSP   .cpu_grid > var:nth-of-type(9), /* show DSP instructions too */
#cb_DSP:checked  ~ .summary.SH2_DSP   .cpu_grid > var:nth-of-type(9),
#cb_DSP:checked  ~ .summary.SH3_DSP   .cpu_grid > var:nth-of-type(9)
{ color: var(--cpu-grid-active-text-color); }

#cb_SH1:checked  ~ .summary.SH1       .cycle_grid > var:nth-of-type(1),
#cb_SH2:checked  ~ .summary.SH2       .cycle_grid > var:nth-of-type(2),
#cb_SH2E:checked ~ .summary.SH2E      .cycle_grid > var:nth-of-type(3),
#cb_SH2A:checked ~ .summary.SH2A      .cycle_grid > var:nth-of-type(4),
#cb_SH2A:checked ~ .summary.SH2A_FPU  .cycle_grid > var:nth-of-type(4), /* show FPU instructions too */
#cb_SH3:checked  ~ .summary.SH3       .cycle_grid > var:nth-of-type(5),
#cb_SH3E:checked ~ .summary.SH3       .cycle_grid > var:nth-of-type(6),
#cb_SH3E:checked ~ .summary.SH3_FPU   .cycle_grid > var:nth-of-type(6), /* show FPU instructions too */
#cb_SH4:checked  ~ .summary.SH4       .cycle_grid > var:nth-of-type(7),
#cb_SH4A:checked ~ .summary.SH4A      .cycle_grid > var:nth-of-type(8),
#cb_DSP:checked  ~ .summary.SH1_DSP   .cycle_grid > var:nth-of-type(9), /* show DSP instructions too */
#cb_DSP:checked  ~ .summary.SH2_DSP   .cycle_grid > var:nth-of-type(9),
#cb_DSP:checked  ~ .summary.SH3_DSP   .cycle_grid > var:nth-of-type(9)
{ color: var(--cycle-grid-active-text-color); }

/* styling for details section */
.summary > .details
{
  border: unset;
  display: none;
  padding: 10px calc(var(--table-width) - var(--details-width) - 30px) 10px 30px;
  width: var(--details-width);
}

.summary > .details > span
  { display: block; }

.summary > .details > span:first-of-type
  { padding-top: 0em !important; }

.summary > .details > span[title="operation"],
.summary > .details > span[title="assembly"]
{
  font-size: 13px;
  font-family: monospace;
  white-space: pre;
}

.summary > .details > span[title="section"]
{
  padding-bottom: 0.5em;
  padding-top: 1em;
  font-style: italic;
  font-weight: 700;
}

.summary > .details > span[title="section"] > em
{ color: var(--mnemonic-text-color); }


span[title="list"]
{
  list-style-type: circle;
}

span[title="list"] > var
{
  font-style: unset;
  display: list-item;
}

input[id="radio_math"]:checked ~ var[title="for all"]::before { content: "∀"; }

input[id="radio_math"]:checked ~ .summary var[title="greater than or equal"]::before { content: "≤"; }
input[id="radio_math"]:checked ~ .summary var[title="less than or equal"]::before { content: "≥"; }
input[id="radio_math"]:checked ~ .summary var[title="equality"]::before { content: "="; }
input[id="radio_math"]:checked ~ .summary var[title="shift bits left"]::before { content: "«"; }
input[id="radio_math"]:checked ~ .summary var[title="shift bits right"]::before { content: "»"; }
input[id="radio_math"]:checked ~ .summary var[title="binary or"]::before { content: "∨"; }
input[id="radio_math"]:checked ~ .summary var[title="binary and"]::before { content: "∧"; }
input[id="radio_math"]:checked ~ .summary var[title="binary xor"]::before { content: "⊕"; }
input[id="radio_math"]:checked ~ .summary var[title="binary not"]::before { content: "¬"; }
input[id="radio_math"]:checked ~ .summary var[title="double prime"]::after { content: "″"; }
input[id="radio_math"]:checked ~ .summary var[title="prime"]::after { content: "′"; }
input[id="radio_math"]:checked ~ .summary var[title="subtract"]::before { content: "−"; }
input[id="radio_math"]:checked ~ .summary var[title="square root"]::before { content: "√("; }
input[id="radio_math"]:checked ~ .summary var[title="square root"]::after { content: ")"; }
/* input[id="radio_math"]:checked ~ .summary var[title="divide"]::before { content: "÷"; } */
input[id="radio_math"]:checked ~ .summary var[title="multiply"]::before { content: "×"; }
input[id="radio_math"]:checked ~ .summary var[title="absolute value"]::before { content: "|"; }
input[id="radio_math"]:checked ~ .summary var[title="absolute value"]::after { content: "|"; }


input[id="radio_C"]:checked ~ .summary var[title="greater than or equal"]::before { content: ">="; }
input[id="radio_C"]:checked ~ .summary var[title="less than or equal"]::before { content: "<="; }
input[id="radio_C"]:checked ~ .summary var[title="equality"]::before { content: "=="; }
input[id="radio_C"]:checked ~ .summary var[title="shift bits left"]::before { content: "<<"; }
input[id="radio_C"]:checked ~ .summary var[title="shift bits right"]::before { content: ">>"; }
input[id="radio_C"]:checked ~ .summary var[title="binary or"]::before { content: "|"; }
input[id="radio_C"]:checked ~ .summary var[title="binary and"]::before { content: "&"; }
input[id="radio_C"]:checked ~ .summary var[title="binary xor"]::before { content: "^"; }
input[id="radio_C"]:checked ~ .summary var[title="binary not"]::before { content: "~"; }
input[id="radio_C"]:checked ~ .summary var[title="double prime"]::after { content: "’’"; }
input[id="radio_C"]:checked ~ .summary var[title="prime"]::after { content: "’"; }
input[id="radio_C"]:checked ~ .summary var[title="subtract"]::before { content: "-"; }
input[id="radio_C"]:checked ~ .summary var[title="square root"]::before { content: "sqrt("; }
input[id="radio_C"]:checked ~ .summary var[title="square root"]::after { content: ")"; }
/* input[id="radio_C"]:checked ~ .summary var[title="divide"]::before { content: "/"; } */
input[id="radio_C"]:checked ~ .summary var[title="multiply"]::before { content: "*"; }
input[id="radio_C"]:checked ~ .summary var[title="absolute value"]::before { content: "abs("; }
input[id="radio_C"]:checked ~ .summary var[title="absolute value"]::after { content: ")"; }


var[title="divide"]::before { content: "/"; }
var[title="greater than"]::before { content: ">"; }
var[title="less than"]::before { content: "<"; }
var[title="store into (right)"]::before { content: "→"; }
var[title="store into (left)"]::before { content: "←"; }
var[title="add"]::before { content: "+"; }

</style>
</head>
<body>
  <div id="header">
    <span style="font-size:20px;font-weight:700">Hitachi SuperH Instruction Set Summary</span>
    <div style="float:right">Last updated: )html" << __DATE__ << " " << __TIME__ << R"html(</div>
    <br />
    <div style="float:right">
      <a href="https://github.com/SaturnOpenSDK/sh_insns">Page Source</a>
    </div>
  </div>
  <input type="radio" id="radio_C" name="symbols" checked /><label for="radio_C">C programming Symbols</label>
  <input type="radio" id="radio_math" name="symbols" /><label for="radio_math">Mathematic Symbols</label>
  <br />
  <select name="CPUtype">
    <option value="Grid">Show Grid</option>
    <option value="SH1">SuperH 1</option>
    <option value="SH2">SuperH 2</option>
    <option value="SHDSP">SuperH DSP</option>
    <option value="SH2E">SuperH 2E</option>
    <option value="SH2A">SuperH 2A without FPU</option>
    <option value="SH2A_FPU">SuperH 2A with FPU</option>

    <option value="SH3">SuperH 3</option>
    <option value="SH3_FPU">SuperH 3E (3 with FPU)</option>
    <option value="SH3_DSP">SuperH 3-DSP</option>
  </select>
  <br />)html"

  << regex_property_list(display_name, "\n  <input type=\"checkbox\" id=\"cb_&\" name=\"&\" checked /><label for=\"cb_&\">&</label>")

  << R"html(
    <span id="table_header" class="summary)html" << regex_property_list(display_name, " &") << R"html(">
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

  try
  {
    std::list<insns> insn_blocks;
    build_insn_blocks(insn_blocks);
    post_processing(insn_blocks);

    int id = 0;
    for (const auto& block : insn_blocks)
    {
      std::cout << "<span class=\"section_title\">" << block.section_title << "</span>" << std::endl;

      for (const auto& i : block)
      {
        std::cout << "<input name=\"instruction\" type=\"radio\" id=\"row" << id << "\" />" << std::endl;
        std::cout
            << "<label class=\"summary" << build_isa_list(i) << "\" for=\"row" << id << "\">" << std::endl
            << "<span class=\"cpu_grid\"><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var><var></var></span>" << std::endl
            << "<span>" << i.data<format>() << "</span>" << std::endl
            << "<span>" << i.data<abstract>() << "</span>" << std::endl
            << "<span id=\"" << fix_id(i.data<opcode>()) << "\" class=\"colorized\">" << i.data<opcode>() << "</span>" << std::endl
            << "<span>" << i.data<flags>() << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << build_isa_tagged_property_list (i, i.data<group>()) << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << build_isa_tagged_property_list (i, i.data<issue>()) << "</span>" << std::endl
            << "<span class=\"cycle_grid\">" << build_isa_tagged_property_list (i, i.data<latency>()) << "</span>" << std::endl
            << "<span class=\"details\">" << std::endl;

        std::cout << build_environments (i.data<environments>());
        std::cout << build_citations (i.data<citations>());
        std::cout << build_span_section (i.data<name>(), "note", i.data<description>());
        std::cout << build_span_section ("Note", "note", i.data<note>());
        std::cout << build_span_section ("Operation", "operation", i.data<operation>());
        std::cout << build_span_section ("Example", "assembly", i.data<example>());
        std::cout << build_span_section ("Possible Exceptions", "list", i.data<exceptions>());

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


  return 0;
}

