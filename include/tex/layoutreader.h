// Copyright (C) 2019 Vincent Chambrin
// This file is part of the 'typeset' project
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef LIBTYPESET_LAYOUTREADER_H
#define LIBTYPESET_LAYOUTREADER_H

#include "tex/glue.h"
#include "tex/hbox.h"
#include "tex/kern.h"
#include "tex/rule.h"
#include "tex/vbox.h"

#include <cassert>
#include <type_traits>

namespace tex
{

struct Pos
{
  float x;
  float y;
  Pos(float x, float y) {
    Pos::x = x;
    Pos::y = y;
  }
};

struct LIBTYPESET_API LayoutReader 
{ 
  void operator()(std::shared_ptr<tex::Box> box, const Pos & p);
};

struct LIBTYPESET_API PartialLayoutReader
{
  static const bool Done = true;
  static const bool Continue = false;

  bool operator()(std::shared_ptr<tex::Box> box, const Pos & p);
};


template<typename Reader>
void read_hbox_full(Reader && reader, const std::shared_ptr<HBox> & layout, Pos pos)
{
  reader(layout, pos);

  for (auto node : layout->list())
  {
    if (node->isBox())
    {
      auto box = std::static_pointer_cast<Box>(node);

      if (box->is<Rule>())
      {
        reader(std::static_pointer_cast<Rule>(box), pos);
      }
      else if (box->isListBox())
      {
        auto listbox = std::static_pointer_cast<ListBox>(box);
        const float shifted_baseline = pos.y + listbox->shiftAmount();
        if (listbox->isHBox())
        {
          read_hbox_full(reader, std::static_pointer_cast<HBox>(listbox), Pos{ pos.x, shifted_baseline });
        }
        else
        {
          assert(listbox->isVBox());
          read_vbox_full(reader, std::static_pointer_cast<VBox>(listbox), Pos{ pos.x, shifted_baseline });
        }
      }
      else
      {
        reader(box, pos);
      }

      pos.x += box->width();
    }
    else if (node->is<Kern>())
    {
      pos.x += std::static_pointer_cast<Kern>(node)->space();
    }
    else if (node->is<Glue>())
    {
      auto glue = std::static_pointer_cast<Glue>(node);

      pos.x += glue->space();

      if (layout->glueRatio() < 0.f)
      {
        if (layout->glueOrder() == glue->shrinkOrder())
          pos.x += layout->glueRatio() * glue->shrink();
      }
      else
      {
        if (layout->glueOrder() == glue->stretchOrder())
          pos.x += layout->glueRatio() * glue->stretch();
      }
    }
  }
}

template<typename Reader>
void read_vbox_full(Reader && reader, const std::shared_ptr<VBox> & layout, Pos pos)
{
  reader(layout, pos);

  pos.y -= layout->height();

  for (auto node : layout->list())
  {
    if (node->isBox())
    {
      auto box = std::static_pointer_cast<Box>(node);

      pos.y += box->height();

      if (box->is<Rule>())
      {
        reader(std::static_pointer_cast<Rule>(box), pos);
      }
      else if (box->isListBox())
      {
        auto listbox = std::static_pointer_cast<ListBox>(box);
        const float shift = listbox->shiftAmount();
        if (listbox->isHBox())
        {
          read_hbox_full(reader, std::static_pointer_cast<HBox>(listbox), Pos{ pos.x + shift, pos.y });
        }
        else
        {
          assert(listbox->isVBox());
          read_vbox_full(reader, std::static_pointer_cast<VBox>(listbox), Pos{ pos.x + shift, pos.y });
        }
      }
      else
      {
        reader(box, pos);
      }

      pos.y += box->depth();
    }
    else if (node->is<Kern>())
    {
      pos.y += std::static_pointer_cast<Kern>(node)->space();
    }
    else if (node->is<Glue>())
    {
      auto glue = std::static_pointer_cast<Glue>(node);

      pos.y += glue->space();

      if (layout->glueRatio() < 0.f)
      {
        if (layout->glueOrder() == glue->shrinkOrder())
          pos.y += layout->glueRatio() * glue->shrink();
      }
      else
      {
        if (layout->glueOrder() == glue->stretchOrder())
          pos.y += layout->glueRatio() * glue->stretch();
      }
    }
  }
}

template<typename Reader>
bool read_hbox_partial(Reader && reader, const std::shared_ptr<HBox> & layout, Pos pos)
{
  if(reader(layout, pos))
    return PartialLayoutReader::Done;

  for (auto node : layout->list())
  {
    if (node->isBox())
    {
      auto box = std::static_pointer_cast<Box>(node);

      if (box->is<Rule>())
      {
        if (reader(std::static_pointer_cast<Rule>(box), pos))
          return PartialLayoutReader::Done;
      }
      else if (box->isListBox())
      {
        auto listbox = std::static_pointer_cast<ListBox>(box);
        const float shifted_baseline = pos.y + listbox->shiftAmount();
        if (listbox->isHBox())
        {
          if(read_hbox_partial(reader, std::static_pointer_cast<HBox>(listbox), Pos{ pos.x, shifted_baseline }))
            return PartialLayoutReader::Done;
        }
        else
        {
          assert(listbox->isVBox());
          if(read_vbox_partial(reader, std::static_pointer_cast<VBox>(listbox), Pos{ pos.x, shifted_baseline }))
            return PartialLayoutReader::Done;
        }
      }
      else
      {
        if(reader(box, pos))
          return PartialLayoutReader::Done;
      }

      pos.x += box->width();
    }
    else if (node->is<Kern>())
    {
      pos.x += std::static_pointer_cast<Kern>(node)->space();
    }
    else if (node->is<Glue>())
    {
      auto glue = std::static_pointer_cast<Glue>(node);

      pos.x += glue->space();

      if (layout->glueRatio() < 0.f)
      {
        if (layout->glueOrder() == glue->shrinkOrder())
          pos.x += layout->glueRatio() * glue->shrink();
      }
      else
      {
        if (layout->glueOrder() == glue->stretchOrder())
          pos.x += layout->glueRatio() * glue->stretch();
      }
    }
  }

  return PartialLayoutReader::Continue;
}

template<typename Reader>
bool read_vbox_partial(Reader && reader, const std::shared_ptr<VBox> & layout, Pos pos)
{
  if (reader(layout, pos))
    return PartialLayoutReader::Done;

  pos.y -= layout->height();

  for (auto node : layout->list())
  {
    if (node->isBox())
    {
      auto box = std::static_pointer_cast<Box>(node);

      pos.y += box->height();

      if (box->is<Rule>())
      {
        if (reader(std::static_pointer_cast<Rule>(box), pos))
          return PartialLayoutReader::Done;
      }
      else if (box->isListBox())
      {
        auto listbox = std::static_pointer_cast<ListBox>(box);
        const float shift = listbox->shiftAmount();
        if (listbox->is<HBox>())
        {
          if (read_hbox_partial(reader, std::static_pointer_cast<HBox>(listbox), Pos{ pos.x + shift, pos.y }))
            return PartialLayoutReader::Done;
        }
        else
        {
          assert(listbox->is<VBox>());
          if(read_vbox_partial(reader, std::static_pointer_cast<VBox>(listbox), Pos{ pos.x + shift, pos.y }))
            return PartialLayoutReader::Done;
        }
      }
      else
      {
        if(reader(box, pos))
          return PartialLayoutReader::Done;
      }

      pos.y += box->depth();
    }
    else if (node->is<Kern>())
    {
      pos.y += std::static_pointer_cast<Kern>(node)->space();
    }
    else if (node->is<Glue>())
    {
      auto glue = std::static_pointer_cast<Glue>(node);

      pos.y += glue->space();

      if (layout->glueRatio() < 0.f)
      {
        if (layout->glueOrder() == glue->shrinkOrder())
          pos.y += layout->glueRatio() * glue->shrink();
      }
      else
      {
        if (layout->glueOrder() == glue->stretchOrder())
          pos.y += layout->glueRatio() * glue->stretch();
      }
    }
  }

  return PartialLayoutReader::Continue;
}


template<typename T>
struct layout_reader_impl;

template<>
struct layout_reader_impl<void>
{
  template<typename Reader>
  static void read(Reader && reader, const std::shared_ptr<Box> & layout, Pos pos)
  {
    if (layout->is<Rule>())
    {
      reader(std::static_pointer_cast<Rule>(layout), pos);
    }
    else if (layout->isListBox())
    {
      auto listbox = std::static_pointer_cast<ListBox>(layout);
      if (listbox->is<HBox>())
      {
        read_hbox_full(reader, std::static_pointer_cast<HBox>(listbox), pos);
      }
      else
      {
        assert(listbox->is<VBox>());
        read_vbox_full(reader, std::static_pointer_cast<VBox>(listbox), pos);
      }
    }
    else
    {
      reader(layout, pos);
    }
  }
};

template<>
struct layout_reader_impl<bool>
{
  template<typename Reader>
  static void read(Reader && reader, const std::shared_ptr<Box> & layout, Pos pos)
  {
    if (layout->is<Rule>())
    {
      reader(std::static_pointer_cast<Rule>(layout), pos);
    }
    else if (layout->isListBox())
    {
      auto listbox = std::static_pointer_cast<ListBox>(layout);
      if (listbox->isHBox())
      {
        read_hbox_partial(reader, std::static_pointer_cast<HBox>(listbox), pos);
      }
      else
      {
        assert(listbox->isVBox());
        read_vbox_partial(reader, std::static_pointer_cast<VBox>(listbox), pos);
      }
    }
    else
    {
      reader(layout, pos);
    }
  }
};

template<typename Reader>
void read(Reader && reader, const std::shared_ptr<Box> & layout)
{
  Pos pos = Pos(0, layout->height());
  layout_reader_impl< std::result_of_t<Reader(std::shared_ptr<Box>, Pos)> >::read(std::forward<Reader>(reader), layout, pos);
}

template<typename Reader>
void read(Reader && reader, const std::shared_ptr<Box> & layout, Pos pos)
{
  layout_reader_impl< std::result_of_t<Reader(std::shared_ptr<Box>, Pos)> >::read(std::forward<Reader>(reader), layout, pos);
}

} // namespace tex

#endif // LIBTYPESET_LAYOUTREADER_H
