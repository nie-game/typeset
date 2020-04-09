// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'typeset' project
// For conditions of distribution and use, see copyright notice in LICENSE

#include "tex/math/style.h"

namespace tex
{

namespace math
{

struct StyleConstructor
{
  static Style build(int id, int size, bool cramped)
  {
    return Style{ id, size, cramped };
  }
};

const Style Style::D = StyleConstructor::build(0, 0, false);
const Style Style::Dc = StyleConstructor::build(1, 0, true);
const Style Style::T = StyleConstructor::build(2, 1, false);
const Style Style::Tc = StyleConstructor::build(3, 1, true);
const Style Style::S = StyleConstructor::build(4, 2, false);
const Style Style::Sc = StyleConstructor::build(5, 2, true);
const Style Style::SS = StyleConstructor::build(6, 3, false);
const Style Style::SSc = StyleConstructor::build(7, 3, true);

namespace StyleTables
{

static const int D = 0;
static const int Dc = 1;
static const int T = 2;
static const int Tc = 3;
static const int S = 4;
static const int Sc = 5;
static const int SS = 6;
static const int SSc = 7;

static const int sup_table[] = { S, Sc, S, Sc, SS, SSc, SS, SSc };
static const int sub_table[] = { Sc, Sc, Sc, Sc, SSc, SSc, SSc, SSc };
static const int fracNum_table[] = { T, Tc, S, Sc, SS, SSc, SS, SSc };
static const int fracDen_table[] = { Tc, Tc, Sc, Sc, SSc, SSc, SSc, SSc };
static const int cramp_table[] = { Dc, Dc, Tc, Tc, Sc, Sc, SSc, SSc };
static const int text_table[] = { D, Dc, T, Tc, T, Tc, T, Tc };

static const Style style_table[] = {
  Style::D,
  Style::Dc,
  Style::T,
  Style::Tc,
  Style::S,
  Style::Sc,
  Style::SS,
  Style::SSc
};

} // namespace StyleTables


Style::Style(int id, int size, bool cramped)
  : mId(id)
  , mSize(size)
  , mCramped(cramped)
{

}

Style Style::sup() const
{
  return StyleTables::style_table[StyleTables::sup_table[id()]];
}

Style Style::sub() const
{
  return StyleTables::style_table[StyleTables::sub_table[id()]];
}


Style Style::fracNum() const
{
  return StyleTables::style_table[StyleTables::fracNum_table[id()]];
}

Style Style::fracDen() const
{
  return StyleTables::style_table[StyleTables::fracDen_table[id()]];
}

Style Style::cramp() const
{
  return StyleTables::style_table[StyleTables::cramp_table[id()]];
}

Style Style::text() const
{
  return StyleTables::style_table[StyleTables::text_table[id()]];
}


bool Style::isTight() const
{
  return mSize >= 2;
}

Style Style::fromId(int id)
{
  return StyleTables::style_table[id];
}

} // namespace math

} // namespace tex
