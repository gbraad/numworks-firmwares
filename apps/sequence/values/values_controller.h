#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include "../sequence_store.h"
#include "../sequence_title_cell.h"
#include "../../shared/values_controller.h"
#include "interval_parameter_controller.h"

namespace Sequence {

class ValuesController : public Shared::ValuesController {
public:
  ValuesController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, Shared::Interval * interval, ButtonRowController * header);
  Button * buttonAtIndex(int index, ButtonRowController::Position position) const override {
    return const_cast<Button *>(&m_setIntervalButton);
  }
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  I18n::Message emptyMessage() override;
private:
  bool setDataAtLocation(double floatBody, int columnIndex, int rowIndex) override;
  int maxNumberOfCells() override;
  int maxNumberOfFunctions() override;
  constexpr static int k_maxNumberOfCells = 30;
  constexpr static int k_maxNumberOfSequences = 3;
  SequenceTitleCell m_sequenceTitleCells[k_maxNumberOfSequences];
  SequenceTitleCell * functionTitleCells(int j) override;
  EvenOddBufferTextCell m_floatCells[k_maxNumberOfCells];
  EvenOddBufferTextCell * floatCells(int j) override;
  SequenceStore * functionStore() const override { return static_cast<SequenceStore *>(Shared::ValuesController::functionStore()); }
  IntervalParameterController * intervalParameterController() override {
    return &m_intervalParameterController;
  }
  ViewController * functionParameterController() override;
  I18n::Message valuesParameterControllerPageTitle() const override;
#if COPY_COLUMN
  Shared::ValuesFunctionParameterController m_sequenceParameterController;
#endif
  IntervalParameterController m_intervalParameterController;
  Button m_setIntervalButton;
};

}

#endif
