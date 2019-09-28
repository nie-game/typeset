// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'typeset' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIBTYPESET_PARSING_HORIZONTALMODE_H
#define LIBTYPESET_PARSING_HORIZONTALMODE_H

#include "tex/parsing/mode.h"

#include "tex/parsing/commands.h"
#include "tex/parsing/typesetting-machine.h"

#include "tex/fontmetrics.h"
#include "tex/hbox.h"
#include "tex/typeset.h"

namespace tex
{

namespace parsing
{

class LIBTYPESET_API HorizontalMode : public Mode
{
public:
  HorizontalMode(TypesettingMachine& m);
  ~HorizontalMode() = default;

  typedef RetCode(*Callback)(HorizontalMode&);

  TypesettingMachine& machine() const;
  FontMetrics metrics() const;

  std::map<std::string, Callback>& commands();
  void push(Callback cmd);
  void pop();

  RetCode advance() override;

  List& hlist();
  TextTypesetter& typesetter();

  void prepareTypesetter();

  void writeOutput();

  /* Callbacks */
  static RetCode main_callback(HorizontalMode&);
  static RetCode par_callback(HorizontalMode&);

private:
  TextTypesetter m_typesetter;
  std::vector<Callback> m_callbacks;
  std::map<std::string, Callback> m_commands;
  List m_hlist;
};

} // namespace parsing

} // namespace tex

namespace tex
{
namespace parsing
{

inline std::map<std::string, HorizontalMode::Callback>& HorizontalMode::commands()
{
  return m_commands;
}

inline TextTypesetter& HorizontalMode::typesetter()
{
  return m_typesetter;
}

} // namespace parsing
} // namespace tex

#endif // LIBTYPESET_PARSING_HORIZONTALMODE_H
