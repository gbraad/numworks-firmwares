#include "storage_list_controller.h"
#include "../app.h"
#include "../../i18n.h"
#include <assert.h>
#include <escher/metric.h>

using namespace Shared;

namespace Graph {

StorageListController::StorageListController(Responder * parentResponder, StorageCartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer) :
  Shared::StorageFunctionListController(parentResponder, functionStore, header, footer, I18n::Message::AddFunction),
  m_functionTitleCells{},
  m_expressionCells{},
  m_parameterController(this, functionStore, I18n::Message::FunctionColor, I18n::Message::DeleteFunction)
{
  for (int i = 0; i < k_maxNumberOfDisplayableRows; i++) {
    m_expressionCells[i].setLeftMargin(k_expressionMargin);
  }
}

const char * StorageListController::title() {
  return I18n::translate(I18n::Message::FunctionTab);
}

StorageListParameterController * StorageListController::parameterController() {
  return &m_parameterController;
}

int StorageListController::maxNumberOfDisplayableRows() {
  return k_maxNumberOfDisplayableRows;
}

FunctionTitleCell * StorageListController::titleCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_functionTitleCells[index];
}

HighlightCell * StorageListController::expressionCells(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayableRows);
  return &m_expressionCells[index];
}

void StorageListController::willDisplayTitleCellAtIndex(HighlightCell * cell, int j) {
  Shared::BufferFunctionTitleCell * myFunctionCell = (Shared::BufferFunctionTitleCell *)cell;
  StorageFunction * function = m_functionStore->modelAtIndex(j);
  char bufferName[BufferTextView::k_maxNumberOfChar];
  const char * functionName = function->fullName();
  int index = 0;
  const char ofXSring[4] = {'(',  m_functionStore->symbol(),')', 0};
  size_t ofXSringSize = strlen(ofXSring);
  while (*functionName != Ion::Storage::k_dotChar
      && index < BufferTextView::k_maxNumberOfChar - ofXSringSize)
  {
    // We keep room to write the final "(x)" //TODO should we?
    assert(functionName < function->fullName() + strlen(function->fullName()));
    bufferName[index] = *functionName;
    functionName++;
    index++;
  }
  strlcpy(&bufferName[index], ofXSring, ofXSringSize+1);
  myFunctionCell->setText(bufferName);
  KDColor functionNameColor = function->isActive() ? function->color() : Palette::GreyDark;
  myFunctionCell->setColor(functionNameColor);
}

void StorageListController::willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) {
  Shared::StorageFunctionListController::willDisplayExpressionCellAtIndex(cell, j);
  FunctionExpressionCell * myCell = (FunctionExpressionCell *)cell;
  StorageFunction * f = m_functionStore->modelAtIndex(j);
  KDColor textColor = f->isActive() ? KDColorBlack : Palette::GreyDark;
  myCell->setTextColor(textColor);
}

}
