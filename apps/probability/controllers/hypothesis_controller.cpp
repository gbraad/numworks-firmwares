#include "hypothesis_controller.h"

#include <apps/i18n.h>
#include <escher/container.h>
#include <escher/input_event_handler_delegate.h>
#include <escher/invocation.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include "input_controller.h"

using namespace Probability;

HypothesisController::HypothesisController(Escher::StackViewController * parent, NormalInputController * inputController,
                                           InputEventHandlerDelegate * handler, TextFieldDelegate * textFieldDelegate)
    : SelectableListViewPage(parent),
      m_inputController(inputController),
      m_h0(&m_selectableTableView, handler, textFieldDelegate),
      m_ha(&m_selectableTableView, handler, textFieldDelegate),
      m_next(&m_selectableTableView, I18n::Message::Ok, buttonActionInvocation()) {
  m_h0.setMessage(I18n::Message::H0);
  m_h0.setAccessoryText("p=");
  m_ha.setMessage(I18n::Message::Ha);
  m_ha.setAccessoryText("p>");
}

HighlightCell * HypothesisController::reusableCell(int i, int type) {
  switch (i) {
    case k_indexOfH0:
      return &m_h0;
    case k_indexOfHa:
      return &m_ha;
    case k_indexOfNext:
      return &m_next;
    default:
      assert(false);
  }
}

void HypothesisController::didBecomeFirstResponder() {
  // TODO factor out
  if (selectedRow() == -1) {
    selectCellAtLocation(0, 0);
  } else {
    selectCellAtLocation(selectedColumn(), selectedRow());
  }
  Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
}

void HypothesisController::buttonAction() {
  openPage(m_inputController, false);
}