#ifndef SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H
#define SETTINGS_MESSAGE_TABLE_WITH_GAUGE_WITH_SEPARATOR_H

#include <escher/input_event_handler_delegate.h>
#include <escher/message_table_cell_with_gauge.h>
#include <escher/text_field_delegate.h>
#include "cell_with_separator.h"

namespace Settings {

class MessageTableCellWithGaugeWithSeparator : public CellWithSeparator {
public:
  MessageTableCellWithGaugeWithSeparator(I18n::Message message, const KDFont * font) :
    CellWithSeparator(),
    m_cell(message, font) {}
  Escher::View * accessoryView() const { return m_cell.accessoryView(); }
  void setMessage(I18n::Message message) { return m_cell.setMessage(message); }
private:
  Escher::HighlightCell * cell() override { return &m_cell; }
  Escher::MessageTableCellWithGauge m_cell;
};

}

#endif
