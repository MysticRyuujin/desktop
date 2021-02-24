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

pragma Singleton
import QtQuick 2.0
import "../daemon"

// These are general messages that are used in multiple places throughout the
// app (so they're only translated once).
QtObject {
  // Alpha and beta pre-release image annotations
  //: Screen reader annotation for the "Alpha" banner shown in alpha prerelease
  //: builds
  readonly property string alphaPrereleaseImg: uiTranslate("HeaderBar", "Alpha pre-release")
  //: Screen reader annotation for the "Beta" banner shown in beta prerelease
  //: builds
  readonly property string betaPrereleaseImg: uiTranslate("HeaderBar", "Beta pre-release")
  // Screen reader annotation for the "Preview" badge shown on preview features.
  // This label and the Preview badge itself are not translated due to the risk
  // of a translation of "preview" being too different from the intended
  // meaning.  (Features using this label are very carefully positioned as
  // "preview" features.)
  readonly property string previewPrereleaseImg: "Preview"
  //: Help label used for the Help tray menu as well as help links on some
  //: settings
  readonly property string helpLabel: uiTranslate("TrayMenuBuilder", "Help")

  // Get display text for a location selection.  Returns one of the following
  // (localized):
  // * Specific selection: "US East"
  // * Auto, region known: "Auto (US East)"
  // * Auto, region unknown: "Auto"
  function displayLocation(location, isAuto) {
    if(!location)
      return uiTranslate("RegionModule", "Auto")
    if(isAuto)
      return uiTranslate("RegionModule", "Auto (%1)").arg(Daemon.getLocationName(location))
    return Daemon.getLocationName(location)
  }

  // Display a location selection based on chosen/auto locations
  function displayLocationSelection(chosenLocation, autoLocation) {
    if(chosenLocation)
      return displayLocation(chosenLocation, false)
    return displayLocation(autoLocation, true)
  }
}
