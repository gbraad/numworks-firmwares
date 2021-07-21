#ifndef APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H
#define APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H

#include "probability/abstract/input_homogeneity_data_source.h"
#include "probability/abstract/table_view_controller.h"
#include "probability/gui/selectable_table_view_with_background.h"
#include "probability/models/statistic/homogeneity_statistic.h"

namespace Probability {

class HomogeneityTableViewController : public TableViewController {
public:
  HomogeneityTableViewController(Escher::Responder * parent,
                                 HomogeneityStatistic * statistic,
                                 InputEventHandlerDelegate * inputEventHandlerDelegate,
                                 DynamicTableViewDataSourceDelegate * dataSourceDelegate,
                                 Escher::SelectableTableViewDelegate * tableDelegate);

  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::TextField * textField,
                                 const char * text,
                                 Ion::Events::Event event) override;

  Escher::SelectableTableView * selectableTableView() override { return &m_table; }
  Escher::TableViewDataSource * tableViewDataSource() override { return &m_tableData; }

private:
  Escher::SelectableTableViewDataSource m_selectionDataSource;
  InputHomogeneityDataSource m_innerTableData;
  HomogeneityTableDataSource m_tableData;
  SelectableTableViewWithBackground m_table;
  HomogeneityStatistic * m_statistic;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_CONTROLLERS_HOMOGENEITY_TABLE_VIEW_CONTROLLER_H */