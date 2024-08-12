// Copyright (c) 2024 Private Internet Access, Inc.
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
import "."
import "../../" // settings/ for SettingsMessages
import "../../inputs"
import "../../stores"
import "../../../core"
import "../../../client"
import "../../../common"
import "../../../daemon"
import "../../../theme"
import "../"
import PIA.NativeHelpers 1.0
import PIA.SplitTunnelManager 1.0
import PIA.BrandHelper 1.0

Page {
  readonly property bool splitTunnelEnabled: {
    // On any platform, disable if the split tunnel feature is not supported
    if(Daemon.state.splitTunnelSupportErrors.length)
      return false
    // On Windows, disable the split tunnel app list when the checkbox is
    // disabled, or if the driver hasn't been installed yet
    if(Qt.platform.os === 'windows')
      return appExclusionCheckbox.enabled && !appExclusionCheckbox.needsInstall && Daemon.settings.splitTunnelEnabled
    if(Qt.platform.os === 'osx')
        return Daemon.state.netExtensionState === "Installed" && Daemon.settings.splitTunnelEnabled
    // On Linux enable it all the time (there's no install)
    return Daemon.settings.splitTunnelEnabled
  }

  readonly property bool ruleListEmpty: {
    return Daemon.settings.splitTunnelRules.length === 0 && Daemon.settings.bypassSubnets.length === 0
  }

  GridLayout {
    anchors.fill: parent
    columns: 2
    columnSpacing: Theme.settings.controlGridDefaultColSpacing
    rowSpacing: Theme.settings.controlGridDefaultRowSpacing

    CheckboxInput {
      Layout.columnSpan: 2
      id: appExclusionCheckbox
      label: uiTranslate("NetworkPage", "Split Tunnel")
      // On Windows, if the driver isn't installed and the setting isn't
      // enabled, show the "needs install" description.
      readonly property bool needsInstall: {
        return Qt.platform.os === 'windows' &&
          Daemon.state.netExtensionState === "NotInstalled" &&
          !setting.daemonSetting.currentValue
      }
      readonly property bool canStartInstall: {
        // Can't install if the OS is not supported.
        if(Daemon.state.splitTunnelSupportErrors.length)
          return false
        // We can start an install if no installation is going on right now, and
        // no reboot is required.
        return NativeHelpers.reinstallWfpCalloutStatus === "" ||
           NativeHelpers.reinstallWfpCalloutStatus === "success" ||
           NativeHelpers.reinstallWfpCalloutStatus === "error" ||
           NativeHelpers.reinstallWfpCalloutStatus === "denied"
      }
      enabled: {
        if(setting.currentValue) {
          // Disabling is always allowed - this might be the only way to clear
          // warnings if the driver has somehow been removed and can't be
          // installed.
          return true
        }
        if(canStartInstall) {
          // No installation occurring, and no reboot required
          return true
        }
        // If we're installing right now or a reboot is required, this setting
        // can't be enabled right now.  'desc' sets an appropriate message.
        return false
      }
      desc: {
        // The status of an ongoing installation overrides anything else.
        // (If the user manually installs the extension on Win 7 RTM, and they
        // get the "reboot" state, show that rather than the OS version error.)
        switch(NativeHelpers.reinstallWfpCalloutStatus) {
          default:
          case "":
          case "success":
          case "error":
          case "denied":
            break
          case "reboot":
            return SettingsMessages.installReboot
          case "working":
            // Only report the installation if it was started here.
            // (We still disable the control and/or report Reboot status, etc.,
            // even if the installation was started from the Help page.)
            if(setting.installingDriver)
              return uiTranslate("NetworkPage", "Installing split tunnel filter...")
        }

        var splitTunnelException = Daemon.state.splitTunnelSupportErrors
        if(splitTunnelException.length)
          return SettingsMessages.getSplitTunnelErrorDesc(splitTunnelException)

        if(Daemon.state.netExtensionState === "NotInstalled") {
          if(!setting.currentValue)
            return uiTranslate("NetworkPage", "Enabling this feature will install the split tunnel filter.")
          else {
            if(Qt.platform.os === 'osx')
            {
                // Extension is not installed, checkbox has been enabled, the installation process is started
                return SettingsMessages.stMacOSNotInstalledMessage + " [[" +
            uiTranslate("NetworkPage", "Learn More") + "]]"
            }
            else if(Qt.platform.os === 'windows')
            {
                // This state normally shouldn't happen, but it is possible if the
                // driver was somehow uninstalled.
                // The daemon doesn't disable split tunnel if the driver is not
                // installed, because this state briefly occurs during a reinstall
                // too.
                return uiTranslate("NetworkPage", "The split tunnel filter is not installed.  Reinstall it on the Help page.")
            }
          }
        }
        // Driver is installed or there was an error monitoring it.
        return  uiTranslate("NetworkPage", "Choose which applications use the VPN.") + " [[" +
            uiTranslate("NetworkPage", "Learn More") + "]]"
      }
      embedLinkClicked: function(){Qt.openUrlExternally(BrandHelper.getBrandParam("appExclusionsHelpLink"))}
      setting: Setting {
        id: appExclusionSetting
        readonly property DaemonSetting daemonSetting: DaemonSetting { name: 'splitTunnelEnabled' }
        sourceValue: daemonSetting.sourceValue

        // This is enabled whenever we trigger a driver install.  The help page
        // can also trigger a driver (re)install, so this indicates that we
        // should handle the result here.
        property bool installingDriver: false
        property bool confirming: false

        onCurrentValueChanged: {
          if(currentValue === daemonSetting.currentValue || confirming)
            return

          // Disabling is always allowed
          if(!currentValue) {
            daemonSetting.currentValue = currentValue
            return
          }
          // Prompt the users to update to macOS 11 or newer
          // if they want to use the split tunnel feature
          if (Qt.platform.os === 'osx' && !NativeHelpers.macosSplitTunnelSupported) {
            confirming = true
            currentValue = false
            confirmDialog.show()
          } else {
            continueEnabling()
          }
        }

        function cancelEnabling() {
          confirming = false
        }
        function continueEnabling() {
          // This is really touchy due to the "bidirectional" nature of
          // Setting.currentValue - it's used both to indicate a selection made
          // by the user and the value we want to display.  Since this triggers
          // onCurrentValueChanged, which may then call continueEnabling again,
          // we have to be very careful.
          //
          // If currentValue is already true (any case where no confirmation was
          // necessary), we don't need to do anything.
          //
          // If currentValue had been set back to false to show a confirmation,
          // we need to set it back to true here.  In that case, 'confirming'
          // prevents infinite recursion.
          if(!currentValue)
            currentValue = true
          confirming = false
          // If we're already installing or a reboot is needed, we can't enable
          // (the control should be disabled in this case)
          if(!appExclusionCheckbox.canStartInstall) {
            currentValue = false  // Reject - should not have happened
            return
          }

          // Install the callout driver if needed when enabling this setting
          if(appExclusionCheckbox.needsInstall) {
            installingDriver = true
            NativeHelpers.installWfpCallout()
            // Don't apply the setting change yet, apply it when the install
            // completes.
          }
          else {
            // Otherwise, just apply the setting
            daemonSetting.currentValue = currentValue
          }
        }

        function checkInstallStatus() {
          if(!installingDriver)
            return
          switch(NativeHelpers.reinstallWfpCalloutStatus) {
            default:
            case "working":
              break
            case "success":
              installingDriver = false
              // Apply the setting change that triggered the install
              daemonSetting.currentValue = currentValue
              break
            case "reboot":
              installingDriver = false
              wSettings.alert(SettingsMessages.installReboot, uiTranslate("NetworkPage", "App Exclusions"), 'info')
              daemonSetting.currentValue = currentValue = false
              break
            case "error":
              installingDriver = false
              wSettings.alert(SettingsMessages.splitTunnelInstallError, uiTranslate("NetworkPage", "App Exclusions"), 'error')
              daemonSetting.currentValue = currentValue = false
              break
            case "denied":
              installingDriver = false
              daemonSetting.currentValue = currentValue = false
              break
          }
        }
      }

      Connections {
        target: NativeHelpers
        function onReinstallWfpCalloutStatusChanged() {
          appExclusionCheckbox.setting.checkInstallStatus()
        }
      }
    }

    RowLayout {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      Layout.topMargin: 10
      Text {
        Layout.fillWidth: true
        color: Theme.dashboard.textColor
        text: uiTr("Your Split Tunnel Rules")
        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
        wrapMode: Text.WordWrap
      }

      SettingsButton{
        text: uiTranslate("SplitTunnelAddAppRow", "Add Application")
        Layout.alignment: Qt.AlignRight
        enabled: splitTunnelEnabled
        onClicked: {
          addApplicationDialog.openDialog()
        }
        icon: "add"
      }
      SettingsButton {
        text: uiTranslate("SplitTunnelAddIpRow", "Add IP Address")
        Layout.alignment: Qt.AlignRight
        icon: "add"
        enabled: splitTunnelEnabled
        onClicked: {
          addIpDialog.showDialog()
        }
      }
    }

    SplitTunnelSettings {
      Layout.columnSpan: 2
      Layout.fillHeight: true
      Layout.fillWidth: true
      label: appExclusionCheckbox.label
      enabled: splitTunnelEnabled
    }

    // This is the text at the botton. This never changes since the text right below the checkbox will change
    // depending on the installation state (if an installation is required)
    StaticText {
      Layout.columnSpan: 2
      Layout.fillWidth: true
      wrapMode: Text.WordWrap
      color: Theme.settings.inputDescriptionColor
      text: uiTranslate("NetworkPage", "Apps may need to be restarted for changes to be applied.")
    }

    SplitTunnelAppDialog {
      id: addApplicationDialog
    }

    SplitTunnelAddIpDialog {
      id: addIpDialog
    }
    OverlayDialog {
      id: confirmDialog

      title: appExclusionCheckbox.label
      buttons: [Dialog.Ok]
      contentWidth: 300

      DialogMessage {
        width: parent.width
        icon: 'info'
        text: SettingsMessages.stUpdateOSMessage
        color: Theme.settings.inputLabelColor
      }

      function show() {
        visible = true
        focus = true
        open()
      }
      onAccepted: {
        appExclusionSetting.cancelEnabling();
      }
    }
  }

}
