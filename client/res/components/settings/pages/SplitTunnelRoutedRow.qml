// Copyright (c) 2021 Private Internet Access, Inc.
//
// This file is part of the Private Internet Access Desktop Client.
//
// The Private Internet Access Desktop Client is free software: you can
// redistribute it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// The Private Internet Access Desktop Client is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Private Internet Access Desktop Client.  If not, see
// <https://www.gnu.org/licenses/>.

import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3
import "../../core"
import "../../theme"
import "../../common"
import "../../client"
import "../../daemon"
import "../inputs"
import "../stores"
import Qt.labs.platform 1.1
import PIA.SplitTunnelManager 1.0
import PIA.NativeHelpers 1.0
import PIA.NativeAcc 1.0 as NativeAcc
import "qrc:/javascript/keyutil.js" as KeyUtil
import "qrc:/javascript/util.js" as Util

// This is the "Routed Packets" row of the split tunnel app list.
// Some of the properties here correspond to properties of SplitTunnelAppRow for
// keyboard nav and accessibility.
SplitTunnelRowBase {
  id: routedRow

  property bool showAppIcons: true

  function keyboardShowModePopup() {
    modeDropDown.showPopup()
  }

  // Select a cell in this row with the keyboard.
  keyboardSelect: function(keyboardColumn) {
    switch(keyboardColumn) {
      default:
        break // Nothing to do for these columns
      case keyColumns.mode:
        keyboardShowModePopup()
        break
    }
  }

  // Effective column (this row does not have a 'remove' cell)
  effectiveColumnFor: function(column) {
    return Math.min(keyColumns.mode, column)
  }

  // Screen reader row annotation
  accRow: NativeAcc.TableRow {
    name: displayName
    item: routedRow
    selected: false
    outlineExpanded: false
    outlineLevel: 0
  }

  // Screen reader cell annotations
  accAppCell: NativeAcc.TableCellText {
    name: displayName
    item: routedPacketsText
  }
  accPathCell: null
  accModeCell: NativeAcc.TableCellDropDownButton {
    name: modeDropDown.displayText
    item: modeDropDown
    onActivated: {
      routedRow.focusCell(keyColumns.mode)
      keyboardShowModePopup()
    }
  }
  accRemoveCell: null

  // Localized display name (used in list's accessibility table)
  readonly property string displayName: routedPacketsText.text

  implicitHeight: Math.max(50, appPathText.y + appPathText.height + 10)

  Text {
    id: routedPacketsText
    anchors.left: parent.left
    anchors.leftMargin: showAppIcons ? 40 : 5
    y: 4
    anchors.right: modeDropDown.left
    anchors.rightMargin: 5
    text: uiTr("Routed Packets")
    color: Theme.settings.inputListItemPrimaryTextColor
    font.pixelSize: 16
    elide: Text.ElideRight
  }

  Text{
    id: appPathText
    y: 25
    font.pixelSize: 11
    text: uiTr("Includes most containers and virtual machines")
    color: Theme.settings.inputListItemSecondaryTextColor
    elide: Text.ElideRight
    wrapMode: Text.WordWrap
    anchors.right: modeDropDown.left
    anchors.rightMargin: 5
    anchors.left: routedPacketsText.left
  }

  ThemedComboBox {
    id: modeDropDown
    width: 145
    height: 24
    model: otherAppModeChoices
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter
    anchors.rightMargin: 45

    // Not a tabstop, navigation occurs in table
    focusOnDismissFunc: function() { routedRow.focusCell(keyColumns.mode) }

    currentIndex: {
      // Hack in a dependency on 'model' - the dropdown forgets our bound value
      // for the current index if the model changes (like retranslation), and
      // this forces the binding to reapply
      var dep = model
      return Daemon.settings.routedPacketsOnVPN ? 1 : 0;
    }

    onActivated: {
      Daemon.applySettings({'routedPacketsOnVPN': currentIndex === 1});
    }
  }

  // Highlight cue for the mode drop-down
  HighlightCue {
    anchors.fill: modeDropDown
    visible: effectiveColumnFor(highlightColumn) === keyColumns.mode
  }

  HighlightCue {
    anchors.fill: parent
    visible: effectiveColumnFor(highlightColumn) === keyColumns.app
    inside: true
  }
}
